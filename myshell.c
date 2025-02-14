#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024  // Maximum size of input string
#define MAX_ARG_SIZE 100    // Maximum number of arguments

// Function declarations for internal command functions and others
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

// Main shell loop
int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];  // Holds the input from the user
    char *args[MAX_ARG_SIZE];    // Array to store arguments from input

    // Set shell environment variable for the current shell path
    char shell_path[MAX_INPUT_SIZE];
    getcwd(shell_path, sizeof(shell_path));  // Get the current working directory
    strcat(shell_path, "/myshell");  // Append the shell executable name
    setenv("shell", shell_path, 1);  // Set the "shell" environment variable

    // Check if running in batch mode (if a file is provided as an argument)
    if (argc == 2) {
        FILE *batch_file = fopen(argv[1], "r");  // Open batch file
        if (!batch_file) {
            perror("Batch file error");
            return 1;  // Return error if batch file can't be opened
        }
        batch_mode(batch_file);  // Process the batch file commands
        fclose(batch_file);  // Close the batch file
        return 0;
    }

    // Interactive mode (for command line input)
    while (1) {
        printf("myshell> ");
        if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
            break;  // Exit the shell if EOF is encountered
        }
        
        input[strcspn(input, "\n")] = 0;  // Remove newline character from input

        // Tokenize input into arguments
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARG_SIZE - 1) {
            args[i++] = token;  // Add token to the args array
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the arguments list

        // If a command is entered, execute it
        if (args[0]) {
            execute_command(args);  // Execute the command
        }
    }
    return 0;
}

// Function to execute commands
void execute_command(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        internal_cd(args[1]);  // Change directory
    } else if (strcmp(args[0], "clr") == 0) {
        internal_clr();  // Clear the screen
    } else if (strcmp(args[0], "dir") == 0) {
        internal_dir(args[1] ? args[1] : ".");  // List directory contents
    } else if (strcmp(args[0], "environ") == 0) {
        internal_environ();  // List environment variables
    } else if (strcmp(args[0], "echo") == 0) {
        internal_echo(args);  // Echo message
    } else if (strcmp(args[0], "help") == 0) {
        internal_help();  // Display help menu
    } else if (strcmp(args[0], "pause") == 0) {
        internal_pause();  // Wait for user input
    } else if (strcmp(args[0], "quit") == 0) {
        internal_quit();  // Exit the shell
    } else {
        execute_external(args);  // Execute external commands
    }
}

// Internal command implementations

// Change the current directory
void internal_cd(char *path) {
    if (!path) {
        printf("Current directory: %s\n", getenv("PWD"));  // Print current directory if no path is provided
        return;
    }
    if (chdir(path) == 0) {
        setenv("PWD", path, 1);  // Update the PWD environment variable
    } else {
        perror("cd failed");  // Print error if chdir fails
    }
}

// Clear the terminal screen
void internal_clr() {
    if (system("clear") == -1) {
        perror("clr failed");  // Print error if the command fails
    }
}

// List contents of a directory
void internal_dir(char *path) {
    DIR *dir = opendir(path);  // Open directory
    if (!dir) {
        perror("dir failed");  // Print error if directory can't be opened
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s  ", entry->d_name);  // Print each directory entry
    }
    printf("\n");
    closedir(dir);  // Close the directory
}

// Print environment variables
void internal_environ() {
    extern char **environ;  // Get environment variables
    for (char **env = environ; *env; env++) {
        printf("%s\n", *env);  // Print each environment variable
    }
}

// Echo a message
void internal_echo(char *args[]) {
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);  // Print each argument
    }
    printf("\n");
}

// Show help information
void internal_help() {
    if (system("more help.txt") == -1) {
        perror("help failed");  // Print error if the help command fails
    }
}

// Pause execution and wait for user to press Enter
void internal_pause() {
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');  // Wait until Enter is pressed
}

// Quit the shell
void internal_quit() {
    printf("Exiting shell...\n");
    exit(0);  // Exit the program
}

// Function to execute external commands
void execute_external(char *args[]) {
    pid_t pid = fork();  // Create a new process
    if (pid == 0) {
        // Child process
        setenv("parent", getenv("shell"), 1);  // Set the parent shell environment variable
        execvp(args[0], args);  // Execute the external command
        perror("Command execution failed");  // If execvp fails, print error
        exit(1);  // Exit child process
    } else if (pid > 0) {
        // Parent process
        wait(NULL);  // Wait for the child process to complete
    } else {
        perror("Fork failed");  // Print error if fork fails
    }
}

// Batch mode function to process commands from a file
void batch_mode(FILE *file) {
    char line[MAX_INPUT_SIZE];  // Line buffer to hold commands from file
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline from the end
        char *args[MAX_ARG_SIZE];  // Array to store arguments
        int i = 0;
        char *token = strtok(line, " ");  // Tokenize the line into arguments
        while (token != NULL && i < MAX_ARG_SIZE - 1) {
            args[i++] = token;  // Add token to the args array
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the arguments list
        if (args[0]) {
            execute_command(args);  // Execute the command
        }
    }
}
