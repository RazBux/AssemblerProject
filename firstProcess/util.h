/* utils.h */

#ifndef INSTRUCTION_UTILS_H
#define INSTRUCTION_UTILS_H

#include <stdio.h>
#include <string.h>

/* Include global values if needed */
#include "../globVal/glob_val.h"

/* struct for holding symbols */
typedef struct {
    char *symbol;
    char *prop;
    int val;
} Symbol;

typedef struct {
    Symbol *symbols;  /* Dynamic array of symbols */
    size_t symbolCount;  /* Number of symbols */ 
} SymbolTable;

/* Functions prototypes */

/* function for init the symbol table */
void initSymbolTable(SymbolTable *st);

/* Adds a new symbol to the symbol table array */
void addSymbol(SymbolTable *symbolArray, size_t symbolCount, char* symbol, char* prop, int val);

/* Check if there the name passed to the function will be uniqe in the SymbolTable 
    @param symbolTable - the data structue use to store the ST 
    @param symbolCount - 
    @param name - pass to see if it's will be uniqe  
    @return 0 if the name will be uniqe to ST and 1 if there is alredy name in the ST equal to it
    */
int hasSymbol(SymbolTable *symbolArray, size_t symbolCount, char* name);

/* Prints all symbols in the symbol table array */
void printSymbols(const SymbolTable *symbolArray, size_t symbolCount);

/* Free the memory of the SymbolTable array we allocate during both Process */
void freeSymbolTable(SymbolTable **symbolArray, size_t *symbolCount);

/* Write all the symbol to file after finish to collect them and there is no error during the second run */
void writeSymbolTableToFile(const SymbolTable *symbolArray, size_t symbolCount, const char *filename);

/* Check if the word is an opCode */
int isOpCode(char* word);

/* Print the binary represention of a number, the number is spreding on 12 bytes. the last 2 bytes are 00 */
void printBinary14(unsigned int number);

#endif /* INSTRUCTION_UTILS_H */
