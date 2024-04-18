# # Define compiler and flags
# CC=gcc
# CFLAGS=-ansi -Wall -pedantic -g

# # Target to build the executable directly from source files
# firstProcess: firstProcess.c util.c printBinary.c
# 	$(CC) $(CFLAGS) -v firstProcess.c util.c printBinary.c -o firstProcess

# # Target to run the program
# run: firstProcess
# 	./firstProcess


# Define compiler and flags
CC=gcc
CFLAGS=-ansi -Wall -pedantic -g

# Define object files
OBJ = firstProcess.o util.o printBinary.o

# Executable target depends on object files
firstProcess: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Rule to create object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to run the program
run: firstProcess
	./firstProcess

# Clean target to remove compiled files
clean:
	rm -f $(OBJ) firstProcess
