# MyShell

**MyShell** is a simple shell program written in C that simulates the basic functionality of a Unix-like shell. It supports a variety of built-in commands, external command execution, and batch processing.

---

## Features

- **Built-in Commands**:
  - `cd <directory>`: Changes the current working directory.
  - `clr`: Clears the terminal screen.
  - `dir <directory>`: Lists the files in a specified directory.
  - `environ`: Displays all environment variables.
  - `echo <message>`: Prints a message to the screen.
  - `help`: Shows the help menu.
  - `pause`: Pauses execution and waits for the user to press Enter.
  - `quit`: Exits the shell.

- **External Command Execution**: Supports executing system commands through `execvp()`.

- **Batch Mode**: Allows reading and executing commands from a batch file.

- **Customizable Environment Variables**: Shell sets the `PWD` and `SHELL` environment variables.

---

## Requirements

- **C Compiler**: GCC (or any compatible C compiler).
- **Linux-like Environment**: The shell works on systems like Linux, and has been tested on WSL (Windows Subsystem for Linux) and PowerShell.
- **Make**: Used to compile the project.

---

## Compilation

### Using `make`
1. Clone the repository or download the project files.
2. Navigate to the project directory.
3. Use the `make` command to compile the project.

```bash
make
