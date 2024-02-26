# Define compiler and flags
CC=gcc
CFLAGS=-ansi -Wall -pedantic -g

# Target to build the executable directly from source files
firstProcess: firstProcess.c util.c
	$(CC) $(CFLAGS) firstProcess.c util.c -o firstProcess

# Target to run the program
run: firstProcess
	./firstProcess

# Target to clean the build files
clean:
	rm -f firstProcess
