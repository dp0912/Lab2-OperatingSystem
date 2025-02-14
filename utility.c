#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myshell.h"

// Prints the current working directory
void print_working_directory() {
    char cwd[1024];  // Buffer to hold current directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);  // Print the current directory
    } else {
        perror("getcwd failed");  // Print error if getcwd fails
    }
}

// Changes the current directory
void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("chdir failed");  // Print error if chdir fails
    }
}
