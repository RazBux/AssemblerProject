#ifndef INSTRUCTION_UTILS_H
#define INSTRUCTION_UTILS_H

#include <stdio.h>

/* Include global values if needed */
#define MAX_LINE_LENGTH 81
#define MAX_LEN_NAME 31
#define MEMORY_SIZE 4096


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


/**
 * Initializes a SymbolTable by setting the symbols pointer to NULL
 * and the symbol count to 0, indicating an empty table.
 *
 * @param st Pointer to the SymbolTable to initialize.
 */
void initSymbolTable(SymbolTable *st);


/**
 * Adds a new symbol to the symbol table, dynamically resizing the symbol array if necessary.
 * The symbol, its properties, and its value are stored in the table.
 *
 * @param st Pointer to the SymbolTable to add the symbol.
 * @param symbol The symbol name as a string.
 * @param prop The property associated with the symbol as a string.
 * @param val The integer value associated with the symbol.
 */
void addSymbol(SymbolTable *st, char *symbol, char *prop, int val);


/**
 * Checks if a symbol exists in the SymbolTable.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return 1 if the symbol exists, 0 otherwise.
 */
int hasSymbol(const SymbolTable *st, const char* name);


/**
 * Checks if a symbol with specific properties already exists in the symbol table.
 *
 * @param st A pointer to the symbol table where symbols are stored.
 * @param name A pointer to the string representing the name of the symbol to check.
 * @param ex_prop A pointer to the string representing the expected property of the symbol.
 * @return 1 if the symbol with the specified property exists, 0 otherwise.
 */
int hasSymbol_exen(const SymbolTable *st, const char *name, char* ex_prop);


/**
 * Prints all symbols stored in the SymbolTable along with their properties and values.
 *
 * @param st Pointer to the SymbolTable containing the symbols to be printed.
 */
void printSymbols(const SymbolTable *st);


/**
 * Add proper value for 'code' or 'data'.
 * for data - add the IC (counter for instruction code) + 100 (starting place of the program)
 * for code - add 100 for starting place of the program.
 * 
 * @param st A pointer to the SymbolTable to print.
 * @param IC The counter of the instruction code.
 */
void icdcSymbolTable(const SymbolTable *st, int IC);


/**
 * Frees all resources associated with a SymbolTable. After calling this function,
 * all symbols and their associated properties are deallocated, and the symbol table's
 * pointer is set to NULL to prevent dangling references.
 *
 * @param st A pointer to the SymbolTable to free.
 */
void freeSymbolTable(SymbolTable *st);


/**
 * Writes the contents of a SymbolTable to a file.
 * 
 * @param st A pointer to the SymbolTable.
 * @param filename The name of the file to write to.
 */
void writeSymbolTableToFile(const SymbolTable *st, const char *filename);


/**
 * Processes a line assumed to define a symbol with a constant value,
 * checking for format correctness and adding the symbol to a symbol table
 * if it does not already exist.
 *
 * @param line The line of text to process.
 * @param delimiters The delimiter characters used to tokenize the line.
 * @param st Pointer to the SymbolTable where the symbol will be added.
 */
int addDefine(char *line, SymbolTable *st);


/**
 * Checks if a given word is one of the saved words, which include commands and register names.
 * Saved words are defined in a constant array.
 *
 * @param word The word to check against the saved words.
 * @return Returns -1 if the word is found (indicating an error due to reserved word usage),
 *         0 otherwise.
 */
int checkWord(const char *word);


/**
 * Checks if a string represents a valid integer. The function accounts for leading whitespace,
 * an optional plus or minus sign, and numeric characters.
 *
 * @param str A pointer to a null-terminated string to be checked.
 * @return Returns 1 if the string is a valid integer representation, 0 otherwise.
 */
int isInteger(const char *str);


/**
 * Checks if a symbol exists in the SymbolTable and returns a pointer to its value.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return Pointer to the value of the symbol if it exists, NULL otherwise.
 */
int getSymbolIndex(const SymbolTable *st, const char *name);


/**
 * Checks if a symbol exists in the SymbolTable and have code or data prop
 * and returns a pointer to its value.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return Pointer to the value of the symbol if it exists, NULL otherwise.
 */
int getSymbolIndex_dc(const SymbolTable *st, const char *name);


/**
 * Checks if a given word is an opcode saved in the list of known opcodes.
 *
 * @param word A pointer to the string representing the word to check.
 * @return 0 if the word is a known opcode, 1 otherwise.
 */
int isOpCode(char *word);


/**
 * Normalizes a given string in-place by removing all spaces directly before and after commas.
 * This function ensures that no spaces surround any commas in the string, while maintaining
 * other parts of the string unchanged.
 *
 * @param input The modifiable input string that may contain extra spaces around commas.
 */
void normalizeString(char *input);


/**
 * Retrieves the opcode for a given command string.
 *
 * @param command A pointer to the string representing the command.
 * @return The corresponding opcode if found, -1 otherwise.
 */
op_code getOpCode(const char *command);


/**
 * Checks if a given string is a valid label.
 * 
 * A valid label must start with an alphabetical character, followed by
 * any combination of alphanumeric characters.
 * 
 * @param lable A pointer to the string to be checked.
 * @return 1 if the string is a valid label, -1 otherwise.
 */
int isValidLable(char* lable);


/**
 * Combines an integer and a string into a formatted string.
 * The integer is formatted to four digits with leading zeros and combined with the string with three spaces in between.
 *
 * @param str The string part of the input.
 * @param num The integer to format and combine with the string.
 * @return A dynamically allocated string containing the formatted output. 
 *         The caller is responsible for freeing this memory.
 */
char* combineIntStr(const char *str, int num);


/**
 * Concatenates a given filename with a specified extension and returns the new string.
 * The caller is responsible for freeing the returned string.
 *
 * @param fileName The base name of the file.
 * @param extension The extension to append to the filename, including the dot (e.g., ".txt").
 * @return A new dynamically allocated string containing the original filename with the specified extension appended.
 */
char* createExtendedFileName(const char* fileName, const char* extension);


#endif /* INSTRUCTION_UTILS_H */
