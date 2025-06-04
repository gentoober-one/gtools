# Linux Utility Kit

A collection of scripts and C programs designed for common Linux system administration and development tasks. This kit provides tools for archiving, compression, system backup, ZFS snapshot management, and quick command cheatsheets.

## Features

*   **Archiving & Compression:** Efficiently archive and compress directories (`clz4`), and decompress them (`dlz4`).
*   **System Backup:** Create comprehensive system backups (`mkstage4`).
*   **ZFS Snapshot Management:** Automate ZFS snapshot creation and rotation (`snapcycle`).
*   **Quick Cheatsheets:** Access command-line cheatsheets directly from your terminal (`cheat`).
*   **Standard Extraction Utilities:** Simple wrappers for `tar` and `tar.gz` extraction with optional cleanup (`untar`, `untargz`).
*   **Build System:** Includes a `Makefile` for easy compilation of C programs and local installation.

## Repository Structure

*   `src/`: Contains C source code for compiled utilities (e.g., `clz4`).
*   `scripts/`: Contains various shell scripts.
*   `build/`: (Created by Makefile) Stores object files and the compiled `clz4` binary.
*   `dist/`: (Created by `make install`) Stores the distributable/installable versions of the utilities.
    *   `dist/bin/`: Contains compiled binaries like `clz4`.
    *   `dist/scripts/`: Contains the shell scripts.
*   `Makefile`: For compiling and installing utilities.
*   `README.md`: This file.

## Prerequisites

Ensure the following dependencies are installed on your system:

*   **For `clz4` (compilation and execution):**
    *   `gcc` (C compiler)
    *   `make` (Build utility)
    *   `libgen-dev` or equivalent (for `basename`/`dirname` - often part of `libc-dev` or `glibc-devel`)
    *   `tar` utility
    *   `lz4` utility
*   **For `cheat` script:**
    *   `curl`
*   **For `dlz4` script:**
    *   `lz4` utility
    *   `tar` utility
*   **For `mkstage4` script:**
    *   `tar` utility
    *   `lz4` utility
    *   `tput` (for colored output, optional)
*   **For `snapcycle` script:**
    *   `zfsutils` or `zfs` (ZFS command-line tools)
    *   `tput` (for colored output, optional)
*   **For `untar`, `untargz` scripts:**
    *   `tar` utility

## Building and Installation

### 1. Build `clz4`

To compile the `clz4` C program, navigate to the root of the repository and run:

```bash
make
```
This will create the executable at `build/clz4`.

### 2. Local Installation (to `dist/` directory)

To prepare the utilities for use and copy them into the `dist/` directory within the repository:

```bash
make install
```
This will:
*   Compile `clz4` (if not already built) and copy it to `dist/bin/clz4`.
*   Copy all scripts from `scripts/` to `dist/scripts/`.

After running `make install`, you can add `dist/bin` and `dist/scripts` to your system's PATH environment variable, or copy the individual files to a directory already in your PATH.

### 3. System-Wide Installation (Example)

To install the utilities for all users (requires `sudo`):

```bash
sudo cp dist/bin/clz4 /usr/local/bin/
sudo cp dist/scripts/* /usr/local/bin/
```
Ensure `/usr/local/bin/` is in your system's PATH.

### Cleaning

To remove build artifacts and the `dist` directory:
```bash
make clean
```
To remove only the files from the `dist` directory (as installed by `make install`):
```bash
make uninstall
```

## Utilities Provided

Below is a list of the utilities included in this kit.

---

### 1. `clz4` (C Program)

*   **Description:** Archives a specified directory into a `.tar.lz4` file. The compressed archive is saved in the parent directory of the target directory. It then prompts the user for confirmation before deleting the original directory and the intermediate `.tar` file.
*   **Build:** Compiled from `src/clz4.c` using `make`.
*   **Usage:**
    ```
    clz4 <DIRECTORY_PATH>
    ```
*   **Example:**
    ```bash
    # If clz4 is in your PATH after 'make install' and copying:
    clz4 /path/to/my_project
    # Or run from dist/bin:
    # ./dist/bin/clz4 /path/to/my_project
    ```
*   **Warning:** This utility performs destructive operations (deletes the original directory) after user confirmation. Ensure you understand this before confirming.

---

### 2. `cheat` (Shell Script)

*   **Description:** Provides quick access to command-line cheatsheets by fetching them from `cheat.sh`.
*   **Location:** `scripts/cheat` (installed to `dist/scripts/cheat`)
*   **Usage:**
    ```
    cheat [-h|--help] <command>
    ```
*   **Example:**
    ```bash
    cheat tar
    cheat find
    ```

---

### 3. `dlz4` (Shell Script)

*   **Description:** Decompresses `.tar.lz4` files (typically those created by `clz4`).
*   **Location:** `scripts/dlz4` (installed to `dist/scripts/dlz4`)
*   **Usage:**
    ```
    dlz4 [options] <file.tar.lz4>
    ```
    **Options:**
    *   `-r, --remove`: Remove original `.tar.lz4` and intermediate `.tar` file after successful decompression.
    *   `-h, --help`: Display help message.
*   **Example:**
    ```bash
    dlz4 my_archive.tar.lz4
    dlz4 -r project_backup.tar.lz4
    ```

---

### 4. `mkstage4` (Shell Script)

*   **Description:** Interactively creates a "Stage4" backup of the current Linux system. It archives the root filesystem (excluding specified system and temporary directories) into a `.tar.lz4` file.
*   **Location:** `scripts/mkstage4` (installed to `dist/scripts/mkstage4`)
*   **Usage:**
    ```
    mkstage4 [-h|--help] [BACKUP_DIRECTORY]
    ```
    *   `BACKUP_DIRECTORY`: Optional. Directory where the backup will be stored. Defaults to `/mnt/backups/`.
*   **Example:**
    ```bash
    sudo mkstage4 /srv/system_backups
    sudo mkstage4 # Uses default /mnt/backups/
    ```
*   **Notes:**
    *   This script typically requires `sudo` to read all system files and write to privileged backup locations.
    *   It includes interactive confirmation steps before proceeding with the backup.

---

### 5. `snapcycle` (Shell Script)

*   **Description:** Manages ZFS snapshots. It creates a new snapshot with the current timestamp and then deletes older snapshots, keeping a specified number of recent ones.
*   **Location:** `scripts/snapcycle` (installed to `dist/scripts/snapcycle`)
*   **Usage:**
    ```
    snapcycle [-h|--help] [ZFS_POOL_PATH] [KEEP_COUNT]
    ```
    *   `ZFS_POOL_PATH`: Optional. The ZFS dataset path (e.g., `rpool/ROOT/gentoo`). Defaults to `rpool/ROOT/gentoo`.
    *   `KEEP_COUNT`: Optional. Number of recent snapshots to keep. Defaults to `5`.
*   **Example:**
    ```bash
    sudo snapcycle tank/myZFSdata 7  # Keep 7 latest snapshots for tank/myZFSdata
    sudo snapcycle                  # Use defaults (rpool/ROOT/gentoo, keep 5)
    ```
*   **Notes:** Requires ZFS tools to be installed and typically needs `sudo` permissions.

---

### 6. `untar` (Shell Script)

*   **Description:** Extracts `.tar` archives.
*   **Location:** `scripts/untar` (installed to `dist/scripts/untar`)
*   **Usage:**
    ```
    untar [options] <file.tar>
    ```
    **Options:**
    *   `-r, --remove`: Remove the original `.tar` file after successful extraction.
    *   `-h, --help`: Display help message.
*   **Example:**
    ```bash
    untar mydata.tar
    untar -r old_archive.tar
    ```

---

### 7. `untargz` (Shell Script)

*   **Description:** Extracts `.tar.gz` or `.tgz` archives.
*   **Location:** `scripts/untargz` (installed to `dist/scripts/untargz`)
*   **Usage:**
    ```
    untargz [options] <file.tar.gz>
    ```
    **Options:**
    *   `-r, --remove`: Remove the original `.tar.gz` file after successful extraction.
    *   `-h, --help`: Display help message.
*   **Example:**
    ```bash
    untargz software.tar.gz
    untargz -r package.tgz
    ```

---

## Contributing

Suggestions and improvements are welcome! Please feel free to fork the repository, make changes, and submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
