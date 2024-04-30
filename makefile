# Define compiler and flags
CC=gcc
CFLAGS=-ansi -Wall -pedantic -g

# Define paths to additional source files
FIRST_PROCESS_PATH=./firstProcess
ENCRYPTION_PATH=./encryption
PRE_ASMB_PATH=./preAsmbler
UTIL_PATH=./utils
SECOND_PROCESS_PATH=./secondProcess

# Define source files
SOURCES=asmbler.c $(UTIL_PATH)/util.c $(UTIL_PATH)/dataCodeTable.c $(FIRST_PROCESS_PATH)/firstProcess.c $(SECOND_PROCESS_PATH)/secondProcess.c $(UTIL_PATH)/printBinary.c $(ENCRYPTION_PATH)/encryption.c $(PRE_ASMB_PATH)/preAsmbler.c 

# Define the output executable name
EXECUTABLE=asmbler

# Target to build the executable directly from source files
$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

# Target to run the program
run: $(EXECUTABLE)
	@for file in $$(find testFiles/ -type f -name '*.as'); do \
		trimmed=$$(echo "$$file" | sed 's/\.as$$//' | sed 's://*:/:g'); \
		./$(EXECUTABLE) "$$trimmed"; \
	done

.PHONY: run
#./$(EXECUTABLE) testFiles/

# Target to clean the project
clean:
	rm -f $(EXECUTABLE)
	find testFiles/ -type f \( -name '*.ob' -o -name '*.ext' -o -name '*.ent' -o -name '*.am' \) -delete
# rm -f testFiles/*.ob testFiles/*.ext testFiles/*.ent testFiles/*.am


.PHONY: run clean
