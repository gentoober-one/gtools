/*
"clz4" is a C program that takes a directory as a command-line argument and performs the following operations: it archives the directory into a tar file, then compresses the tar file using the lz4 utility and, if both steps succeed, IT REMOVES THE ORIGINAL DIRECTORY AND THE TAR FILE. The goal is to archive and compress the directories and then REMOVE THE ORIGINALS, saving disk space. If any errors occur during the archiving, compression, or removal process, the program will print a corresponding error message.

!!! WARNING !!!!

There's a small "bug" here that you should BE AWARE of:

When specifying the directory that will be archived (and compressed with LZ4), YOU SHOULD OMIT THE "/" at the end of the directory. If you don't, YOU'LL LOSE YOUR DATA.

So, the POSSIBILE (CORRECT) USAGE IS "clz4 directory", NOT "clz4 directory/".
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

void compact_and_remove(char *directory) {
    char tar_file[256];
    char lz4_file[256];

    snprintf(tar_file, sizeof(tar_file), "%s.tar", directory);
    snprintf(lz4_file, sizeof(lz4_file), "%s.tar.lz4", directory);

    pid_t pid = fork();

    if (pid == 0) {
        execlp("tar", "tar", "cvf", tar_file, directory, NULL);
        perror("Erro ao criar arquivo tar");
        exit(1);
    } else if (pid < 0) {
        perror("Erro ao criar processo filho");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            pid_t lz4_pid = fork();

            if (lz4_pid == 0) {
                execlp("lz4", "lz4", tar_file, lz4_file, NULL);
                perror("Erro ao criar arquivo lz4");
                exit(1);
            } else if (lz4_pid < 0) {
                perror("Erro ao criar processo filho para lz4");
                exit(1);
            } else {
                int lz4_status;
                waitpid(lz4_pid, &lz4_status, 0);

                if (WIFEXITED(lz4_status) && WEXITSTATUS(lz4_status) == 0) {
                    printf("Compactado e removido: %s\n", directory);

                    execlp("rm", "rm", "-rf", tar_file, directory, NULL);
                    perror("Erro ao remover diretório e arquivo tar");
                    exit(1);
                } else {
                    printf("Falha na compactação: %s\n", directory);
                }
            }
        } else {
            printf("Falha na compactação: %s\n", directory);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <DIR>\n", argv[0]);
        return 1;
    }

    char *root_directory = argv[1];

    struct stat st;
    if (stat(root_directory, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("Diretório não encontrado ou não é um diretório válido: %s\n", root_directory);
        return 1;
    }

    compact_and_remove(root_directory);

    return 0;
}
