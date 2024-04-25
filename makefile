# Define compiler and flags
CC=gcc
CFLAGS=-ansi -Wall -pedantic -g

# Define paths to additional source files
PROCESS_PATH=./process
ENCRYPTION_PATH=./encryption
PRE_ASMB_PATH=./preAsmbler
UTIL_PATH=./util

# Define source files
SOURCES=asmbler.c $(UTIL_PATH)/util.c $(PROCESS_PATH)/dataCodeTable.c $(PROCESS_PATH)/firstProcess.c $(PROCESS_PATH)/secondProcess.c $(PROCESS_PATH)/printBinary.c $(ENCRYPTION_PATH)/encryption.c $(PRE_ASMB_PATH)/preAsmbler.c 

# Define the output executable name
EXECUTABLE=asmbler

# Target to build the executable directly from source files
$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

# Target to run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE) m

# Target to clean the project
clean:
	rm -f $(EXECUTABLE)
	rm -f *.ob *.ext *.ent *.am

.PHONY: run clean
