#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE 

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>


#define MAX_INPUT_SIZE 1024  // Maximum size of input string
#define MAX_ARG_SIZE 100     // Maximum number of arguments

// Function declarations
void execute_command(char *args[]);
void internal_cd(char *path);
void internal_clr();
void internal_dir(char *path);
void internal_environ();
void internal_echo(char *args[]);
void internal_help();
void internal_pause();
void internal_quit();
void execute_external(char *args[]);
void batch_mode(FILE *file);

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];  // Input buffer
    char *args[MAX_ARG_SIZE];    // Arguments array

    // Set the SHELL environment variable
    char shell_path[MAX_INPUT_SIZE];
    char *result = realpath("/proc/self/exe", shell_path);
    if (result != NULL) {
        setenv("shell", shell_path, 1);  // Set the shell's full path in the environment
    } else {
        perror("myshell: Unable to set SHELL environment variable");
    }

    // Check for batch mode
    if (argc == 2) {
        FILE *batch_file = fopen(argv[1], "r");
        if (!batch_file) {
            perror("myshell: Unable to open batch file");
            return 1;
        }
        batch_mode(batch_file);
        fclose(batch_file);
        return 0;
    }

    // Interactive mode
    while (1) {
        printf("myshell> ");
        if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
            break;
        }
        input[strcspn(input, "\n")] = 0;  // Remove newline character

        // Tokenize input into arguments
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARG_SIZE - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the argument list

        // Execute the command
        if (args[0]) {
            execute_command(args);
        }
    }
    return 0;
}

// Execute commands
void execute_command(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        printf("myshell: Executing internal command: cd\n");
        internal_cd(args[1]);
    } else if (strcmp(args[0], "clr") == 0) {
        printf("myshell: Executing internal command: clr\n");
        internal_clr();
    } else if (strcmp(args[0], "dir") == 0) {
        printf("myshell: Executing internal command: dir\n");
        internal_dir(args[1] ? args[1] : ".");
    } else if (strcmp(args[0], "environ") == 0) {
        printf("myshell: Executing internal command: environ\n");
        internal_environ();
    } else if (strcmp(args[0], "echo") == 0) {
        printf("myshell: Executing internal command: echo\n");
        internal_echo(args);
    } else if (strcmp(args[0], "help") == 0) {
        printf("myshell: Executing internal command: help\n");
        internal_help();
    } else if (strcmp(args[0], "pause") == 0) {
        printf("myshell: Executing internal command: pause\n");
        internal_pause();
    } else if (strcmp(args[0], "quit") == 0) {
        printf("myshell: Executing internal command: quit\n");
        internal_quit();
    } else {
        printf("myshell: Executing external command: %s\n", args[0]);
        execute_external(args);
    }
}

// Internal command implementations
void internal_cd(char *path) {
    if (!path) {
        printf("Current directory: %s\n", getenv("PWD"));
        return;
    }
    if (chdir(path) == 0) {
        setenv("PWD", path, 1);  // Update PWD environment variable
    } else {
        perror("myshell: cd failed");
    }
}

void internal_clr() {
    if (system("clear") == -1) {
        perror("myshell: clr failed");
    }
}

void internal_dir(char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("myshell: dir failed");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(dir);
}

void internal_environ() {
    extern char **environ;
    for (char **env = environ; *env; env++) {
        printf("%s\n", *env);
    }
}

void internal_echo(char *args[]) {
    if (!args[1]) {
        printf("myshell: No message provided.\n");
        return;
    }
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

void internal_help() {
    if (system("more help.txt") == -1) {
        perror("myshell: help failed");
    }
}

void internal_pause() {
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');  // Wait until Enter is pressed
}

void internal_quit() {
    printf("Exiting shell...\n");
    exit(0);
}

// Execute external commands
void execute_external(char *args[]) {
    pid_t pid = fork();  // Create a child process
    if (pid == 0) {
        // In child process
        printf("myshell: Fork successful. In child process (PID: %d).\n", getpid());
        setenv("parent", getenv("shell"), 1);  // Set the "parent" environment variable

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("myshell: Command execution failed");
        }
        exit(EXIT_FAILURE);  // Exit child if execvp fails
    } else if (pid > 0) {
        // In parent process
        int status;
        printf("myshell: In parent process (PID: %d). Waiting for child (PID: %d).\n", getpid(), pid);

        // Wait for the child process to complete
        waitpid(pid, &status, 0);

        // Check child's exit status
        if (WIFEXITED(status)) {
            printf("myshell: Child exited with code %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("myshell: Child terminated by signal %d.\n", WTERMSIG(status));
        }
    } else {
        perror("myshell: Fork failed");
    }
}

// Batch mode function
void batch_mode(FILE *file) {
    char line[MAX_INPUT_SIZE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
        if (line[0] == '\0' || line[0] == '#') continue;  // Skip empty lines or comments

        char *args[MAX_ARG_SIZE];
        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL && i < MAX_ARG_SIZE - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        if (args[0]) {
            execute_command(args);
        }
    }
}
