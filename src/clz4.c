#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h> // For dirname and basename

#define PATH_BUFFER_SIZE 1024

// Function to remove the trailing slash from a directory path.
// This is crucial because basename and dirname might behave unexpectedly
// or modify the input string if it has a trailing slash.
// Consistent input (no trailing slash) is needed for reliable behavior.
void remove_trailing_slash(char *path) {
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] == '/') {
        path[len - 1] = '\0';
    }
}

// Function to execute an external command and wait for it.
// Returns the command's exit code or -1 in case of an error in process execution.
int execute_command(char *const argv[], const char* error_msg_prefix) {
    pid_t pid = fork();
    if (pid == 0) { // Child process
        execvp(argv[0], argv);
        // perror() will append the system error message to the prefix
        perror(error_msg_prefix); // Only executes if execvp fails
        exit(1); // Exit with error if execvp fails
    } else if (pid < 0) { // Error creating child process
        perror("Error creating child process"); // Changed from "Erro ao criar processo filho"
        return -1; // Return -1 to indicate failure in process creation
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); // Return the command's exit code
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Command '%s' terminated abnormally (signal %d)\n", argv[0], WTERMSIG(status)); // Translated
            return -1; // Return -1 to indicate abnormal termination
        } else {
            fprintf(stderr, "Command '%s' ended abnormally for an unknown reason.\n", argv[0]); // Added for clarity
            return -1;
        }
    }
}

// Function to compact a directory using tar and lz4, then optionally remove the original.
void compact_and_remove(char *original_directory) {
    char directory_path_copy_for_basename[PATH_BUFFER_SIZE];
    char directory_path_copy_for_dirname[PATH_BUFFER_SIZE];

    // Prepare copy for basename (it might modify the string)
    strncpy(directory_path_copy_for_basename, original_directory, sizeof(directory_path_copy_for_basename) - 1);
    directory_path_copy_for_basename[sizeof(directory_path_copy_for_basename) - 1] = '\0';
    // Remove trailing slash for consistent behavior with basename/dirname
    remove_trailing_slash(directory_path_copy_for_basename);

    char *base_name = basename(directory_path_copy_for_basename); // E.g., "TEST"

    // Prepare copy for dirname (it might also modify the string)
    strncpy(directory_path_copy_for_dirname, original_directory, sizeof(directory_path_copy_for_dirname) - 1);
    directory_path_copy_for_dirname[sizeof(directory_path_copy_for_dirname) - 1] = '\0';
    // Remove trailing slash for consistent behavior with basename/dirname
    remove_trailing_slash(directory_path_copy_for_dirname);

    char *parent_dir = dirname(directory_path_copy_for_dirname); // E.g., "." or "path/to"

    char tar_file[PATH_BUFFER_SIZE];
    char lz4_file[PATH_BUFFER_SIZE];
    int n;

    // Construct the .tar and .lz4 file names in the parent directory, using the base name of the original directory
    n = snprintf(tar_file, sizeof(tar_file), "%s/%s.tar", parent_dir, base_name);
    if (n < 0 || (size_t)n >= sizeof(tar_file)) {
        fprintf(stderr, "Error: Constructed file path for .tar is too long or snprintf error.\n");
        return;
    }

    n = snprintf(lz4_file, sizeof(lz4_file), "%s/%s.tar.lz4", parent_dir, base_name);
    if (n < 0 || (size_t)n >= sizeof(lz4_file)) {
        fprintf(stderr, "Error: Constructed file path for .lz4 is too long or snprintf error.\n");
        return;
    }

    printf("Starting compression of '%s'...\n", original_directory); // Translated

    // 1. Create .tar file
    // Use 'tar -C <parent_directory> -cvf <full_tar_path> <base_name_of_directory_to_archive>'
    // This ensures tar operates in the parent directory and doesn't try to archive the tar file itself.
    char *tar_argv[] = {"tar", "-C", parent_dir, "-cvf", tar_file, base_name, NULL}; // Alterado: "cvf" para "-cvf"
    printf("Creating tar file: '%s' from '%s' (in directory '%s')...\n", tar_file, base_name, parent_dir); // Translated
    if (execute_command(tar_argv, "Error creating tar file") != 0) { // Translated prefix
        fprintf(stderr, "Failed to create tar file '%s'. Aborting.\n", tar_file); // Translated
        remove(tar_file); // Try to clean up partial tar file if it exists
        return;
    }
    printf("Tar file created successfully.\n"); // Translated

    // 2. Compress .tar file with lz4
    char *lz4_argv[] = {"lz4", tar_file, lz4_file, NULL};
    printf("Compressing tar file with lz4: '%s' to '%s'...\n", tar_file, lz4_file); // Translated
    if (execute_command(lz4_argv, "Error creating lz4 file") != 0) { // Translated prefix
        fprintf(stderr, "Failed to compress with lz4. The original tar file '%s' has been kept.\n", tar_file); // Translated
        remove(lz4_file); // Remove partial lz4 file
        return; // Do not remove original directory or tar, as compression failed
    }
    printf("lz4 file created successfully.\n"); // Translated

    // 3. Confirm removal and remove originals
    char confirm_char;
    printf("\n!!! WARNING: The following operations are DESTRUCTIVE !!!\n"); // Translated
    printf("Are you sure you want to REMOVE the original directory '%s'\n", original_directory); // Translated
    printf("and the intermediate tar file '%s' AFTER successful compression?\n", tar_file); // Translated
    printf("Enter 'y' for yes, any other key to cancel: "); // Translated, 's' to 'y' for convention
    scanf(" %c", &confirm_char); // Space before %c consumes any pending whitespace

    if (confirm_char == 'y' || confirm_char == 'Y') { // Changed from 's' to 'y'
        printf("Removing original files...\n"); // Translated
        // Removal now uses the base name (TEST) and the full path of the tar file
        char *rm_argv[] = {"rm", "-rf", tar_file, original_directory, NULL}; // Use original_directory for rm
        if (execute_command(rm_argv, "Error removing directory and tar file") != 0) { // Translated prefix
            fprintf(stderr, "Failed to remove directory and tar file. Please check manually: '%s' and '%s'\n", original_directory, tar_file); // Translated
            return;
        }
        printf("Original directory '%s' and tar file '%s' removed successfully.\n", original_directory, tar_file); // Translated
        printf("Operation completed. Final file: '%s'\n", lz4_file); // Translated
    } else {
        printf("Removal cancelled. The original directory '%s' and tar file '%s' have been kept.\n", original_directory, tar_file); // Translated
        printf("Final compressed file: '%s'\n", lz4_file); // Translated
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]); // Translated
        return 1;
    }

    char *input_directory = argv[1];

    struct stat st;
    // Use stat() to check the directory, even if it ends with '/', as we want the actual path.
    if (stat(input_directory, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: Directory '%s' not found or not a valid directory.\n", input_directory); // Translated
        return 1;
    }

    compact_and_remove(input_directory);

    return 0;
}
