#ifndef FIRST_PROCESS_H
#define FIRST_PROCESS_H

/** Including necessary libraries and custom header files */
#include <stdio.h> /* Standard input/output definitions */
#include <stdlib.h> /* Standard library definitions */
#include <string.h> /* String handling functions */
#include <ctype.h> /* Character type functions */
#include <limits.h> /* Definitions of common variable type characteristics */

#include "util.h" /* Utility functions and definitions */
#include "printBinary.h" /* Binary print function definitions */
#include "dataCodeTable.h" /* Data Code Table manipulations */
#include "secondProcess.h" /* Second processing stage functions */
#include "../encryption/encryption.h" /* Encryption related functions */
#include "../preAsmbler/preAsmbler.h" /* Pre-assembler processing functions */

/** Function prototypes for handling the assembly process. */

/**
 * Checks the type of address for the given operand.
 * @param operand The operand string to analyze.
 * @param st Pointer to the symbol table.
 * @return Integer representing the address type.
 */
int checkAddressType(char *operand, SymbolTable *st);

/**
 * Processes a single line of assembler code.
 * @param line String containing the line to process.
 * @param DC_table Pointer to data code table for data definitions.
 * @param IC_table Pointer to instruction code table for instruction definitions.
 * @param st Pointer to the symbol table.
 * @param DC Pointer to the data counter.
 * @param IC Pointer to the instruction counter.
 * @return Integer status, 0 if successful, 1 if there is an error.
 */
int processLine(char *line, WordList *DC_table, WordList *IC_table, SymbolTable *st, int *DC, int *IC);

/**
 * Starts the first process of assembly translation.
 * @param asmblerOpenFile Filename of the assembler input.
 * @param DC_table Data code table.
 * @param IC_table Instruction code table.
 * @param st Symbol table.
 * @param DC Data counter initial value.
 * @param IC Instruction counter initial value.
 * @param Flag Pointer to a flag indicating process status.
 * @return Integer status, 0 if successful, -1 if failed.
 */
int startFirstProcess(char *asmblerOpenFile, WordList *DC_table, WordList *IC_table, SymbolTable *st, int DC, int IC, int *Flag);

/**
 * Extracts content enclosed by brackets based on the specified part.
 * @param input The string input containing brackets.
 * @param part The part of the bracket content to extract (1 or 2).
 * @return Dynamically allocated string of the extracted content.
 */
char *extract_brackets(const char *input, int part);

/**
 * Converts an address to a binary representation based on the address type.
 * @param addressType Type of address (0, 1, 2, or 3).
 * @param p Pointer to the operand string.
 * @param st Pointer to the symbol table.
 * @param addressC Character indicating the address code ('s' for source, 'd' for destination).
 * @return Dynamically allocated string representing the binary code.
 */
char *addressToBinatry(int addressType, char *p, SymbolTable *st, char addressC);

#endif /* FIRST_PROCESS_H */

