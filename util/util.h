/* utils.h */

#ifndef INSTRUCTION_UTILS_H
#define INSTRUCTION_UTILS_H

#include <stdio.h>
#include <string.h>

/* Include global values if needed */
#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME 30
#define MAX_LABLE_NAME 31
#define MEMORY_SIZE 4096
#define INITIAL_MACRO_COUNT 10
#define WORD_LEN 14



/* Defines the codes for the supported operations */
typedef enum Op_code
{
    mov,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    hlt
} op_code;

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

typedef struct {
    const char *name;
    op_code code;
} CommandMap;

/* Functions prototypes */

/* function for init the symbol table */
void initSymbolTable(SymbolTable *st);

/* Adds a new symbol to the symbol table array */
void addSymbol(SymbolTable *st, char *symbol, char *prop, int val);

/* Check if there the name passed to the function will be uniqe in the SymbolTable 
    @param symbolTable - the data structue use to store the ST 
    @param symbolCount - 
    @param name - pass to see if it's will be uniqe  
    @return 0 if the name will be uniqe to ST and 1 if there is alredy name in the ST equal to it
    */
int hasSymbol(const SymbolTable *st, const char* name);

int hasSymbol_exen(const SymbolTable *st, const char *name, char* ex_prop);

/* Prints all symbols in the symbol table array */
void printSymbols(const SymbolTable *st);

void icdcSymbolTable(const SymbolTable *st, int IC);

/* Free the memory of the SymbolTable array we allocate during both Process */
void freeSymbolTable(SymbolTable **st);

/* Write all the symbol to file after finish to collect them and there is no error during the second run */
void writeSymbolTableToFile(const SymbolTable *st, const char *filename);

/* addDefine*/
int addDefine(char *line, SymbolTable *st);

/* Check if the word is a saved word */
int checkWord(const char *word);

int isInteger(const char *str);

int getSymbolIndex(const SymbolTable *st, const char *name);
int getSymbolIndex_dc(const SymbolTable *st, const char *name);

int isOpCode(char *word);

void normalizeString(char *input);

op_code getOpCode(const char *command);

int isValidLable(char* lable);

char* combineIntStr(const char *str, int num);

char* createExtendedFileName(const char* fileName, const char* extension);

#endif /* INSTRUCTION_UTILS_H */
