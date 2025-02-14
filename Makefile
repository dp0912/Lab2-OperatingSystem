CC = gcc  # Compiler set to gcc
CFLAGS = -Wall -Wextra -std=c99  # Compilation flags to enable warnings and use C99 standard
OBJ = myshell.o utility.o  # Object files for the program

# Main target to compile the shell program
myshell: $(OBJ)
	$(CC) $(CFLAGS) -o myshell $(OBJ)  # Link object files into the executable myshell

# Target to compile myshell.o object file from myshell.c
myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c  # Compile myshell.c to myshell.o

# Target to compile utility.o object file from utility.c
utility.o: utility.c myshell.h
	$(CC) $(CFLAGS) -c utility.c  # Compile utility.c to utility.o

# Clean target to remove object files and executable
clean:
	rm -f myshell $(OBJ)  # Delete the executable and object files
