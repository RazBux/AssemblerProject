#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "util.h"

/* Define a constant array of saved words */
const char *saved_words[] = {
    /* G:1 commands - 2 operands */
    "mov", "cmp", "add", "sub", "lea",
    /* G:2 commands - 1 operand */
    "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    /* G:3 commands - no operands */
    "rts", "hlt",
    /* Register names */
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    NULL /* Sentinel to mark the end of the array */
};


/**
 * Checks if a given word is one of the saved words, which include commands and register names.
 * Saved words are defined in a constant array.
 *
 * @param word The word to check against the saved words.
 * @return Returns -1 if the word is found (indicating an error due to reserved word usage),
 *         0 otherwise.
 */
int checkWord(const char *word)
{
    int i = 0;
    while (saved_words[i] != NULL)
    { /* Iterate over the array until the sentinel NULL is found */
        if (strcmp(saved_words[i], word) == 0)
        {
            printf("Error: %s is a saved word, cannot use it when creating a labal", word);
            return -1; /* Word is found in the array */
        }
        i++;
    }
    return 0; /* Word is not found */
}


/**
 * Adds a new symbol to the symbol table, dynamically resizing the symbol array if necessary.
 * The symbol, its properties, and its value are stored in the table.
 *
 * @param st Pointer to the SymbolTable to add the symbol.
 * @param symbol The symbol name as a string.
 * @param prop The property associated with the symbol as a string.
 * @param val The integer value associated with the symbol.
 */
void addSymbol(SymbolTable *st, char *symbol, char *prop, int val) {
    SymbolNode *newNode = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (newNode == NULL) {
        printf("Error allocating memory for symbol node\n");
        return;
    }

    /* Allocate memory and copy the symbol */ 
    newNode->symbol = (char *)malloc(strlen(symbol) + 1); /* +1 for null terminator */ 
    if (newNode->symbol == NULL) {
        printf("Error allocating memory for symbol name\n");
        free(newNode);  /* Clean up already allocated memory for the node */ 
        return;
    }
    strcpy(newNode->symbol, symbol);

    /* Allocate memory and copy the property */ 
    newNode->prop = (char *)malloc(strlen(prop) + 1); /* +1 for null terminator */ 
    if (newNode->prop == NULL) {
        printf("Error allocating memory for property\n");
        free(newNode->symbol); /* Clean up symbol allocation */ 
        free(newNode);         /* Clean up node allocation */ 
        return;
    }
    strcpy(newNode->prop, prop);

    newNode->val = val;
    newNode->next = NULL;

    if (st->head == NULL) {
        st->head = newNode; /* First node in the list */ 
    } else {
        SymbolNode *current = st->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode; /* Append at the end of the list */ 
    }
    st->symbolCount++; /* Increment the count of symbols in the table */ 
}


/**
 * Initializes a SymbolTable by setting the symbols pointer to NULL
 * and the symbol count to 0, indicating an empty table.
 *
 * @param st Pointer to the SymbolTable to initialize.
 */
void initSymbolTable(SymbolTable *st) {
    st->head = NULL;
    st->symbolCount = 0;
}



/**
 * Prints all symbols stored in the SymbolTable along with their properties and values.
 *
 * @param st Pointer to the SymbolTable containing the symbols to be printed.
 */
void printSymbols(const SymbolTable *st)
{
    SymbolNode *current = st->head;
    size_t i;
    int symbolWidth = 10;
    int propWidth = 10;
    int valueWidth = 10;
    /* Print header */
    int len = printf("%-*s %-*s %-*s\n", symbolWidth, "Symbol", propWidth, "Property", valueWidth, "Value");

    /* Print dashes '-' for the length of the printed line */
    for (i = 0; i < len - 5; i++)
    {
        printf("-");
    }
    printf("\n");

    /* Print each symbol in the table */
    while(current != NULL)
    {
        printf("%-*s %-*s %-*d\n",
               symbolWidth, current->symbol,
               propWidth, current->prop,
               valueWidth, current->val);
        current = current->next;
    }
    
    printf("\n");
}


/**
 * Add proper value for 'code' or 'data'.
 * for data - add the IC (counter for instruction code) + 100 (starting place of the program)
 * for code - add 100 for starting place of the program.
 * 
 * @param st A pointer to the SymbolTable to print.
 * @param IC The counter of the instruction code.
 */
void icdcSymbolTable(const SymbolTable *st, int IC)
{
    SymbolNode *current = st->head;
    while (current != NULL)
    {
        if (strcmp(current->prop,"code") == 0){
            current->val += 100;
        }
        else if (strcmp(current->prop,"data") == 0){
            current->val += 100 + IC;
        }
        current = current->next;
    }
}


/**
 * Checks if a symbol exists in the SymbolTable.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return 1 if the symbol exists, 0 otherwise.
 */
int hasSymbol(const SymbolTable *st, const char* name) {
    SymbolNode *current = st->head;
    while (current != NULL) {
        if (strcmp(current->symbol, name) == 0) {
            printf("Error: %s is already in the symbol table\n", name);
            return 1;
        }
        current = current->next;
    }
    return 0;
}


/**
 * Checks if a symbol with specific properties already exists in the symbol table.
 *
 * @param st A pointer to the symbol table where symbols are stored.
 * @param name A pointer to the string representing the name of the symbol to check.
 * @param ex_prop A pointer to the string representing the expected property of the symbol.
 * @return 1 if the symbol with the specified property exists, 0 otherwise.
 */
int hasSymbol_exen(const SymbolTable *st, const char *name, char* ex_prop)
{
    SymbolNode *current = st->head;
    while (current != NULL)
    {
        if (strcmp(current->symbol, name) == 0 && 
            (strcmp(current->prop, ex_prop) == 0 ))
        {
            printf("Error: %s is already in the symbol table", name);
            return 1;
        }
        current = current->next;
    }
    return 0;
}


/**
 * Checks if a symbol exists in the SymbolTable and returns a pointer to its value.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return Pointer to the value of the symbol if it exists, NULL otherwise.
 */
SymbolNode* getSymbol(const SymbolTable *st, const char *name) {
    SymbolNode *current = st->head;
    while (current != NULL) {
        if (strcmp(current->symbol, name) == 0) {
            return current; /* Return the pointer to the found symbol node */ 
        }
        current = current->next;
    }
    return NULL; /* Return NULL if the symbol is not found */ 
}


/**
 * Checks if a symbol exists in the SymbolTable and have code or data prop
 * and returns a pointer to its value.
 * 
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return Pointer to the value of the symbol if it exists, NULL otherwise.
 */
SymbolNode* getSymbolIndex_dc(const SymbolTable *st, const char *name)
{
    SymbolNode *current = st->head;
    while (current != NULL) {
        if (strcmp(current->symbol, name) == 0 &&
            (strcmp(current->prop, "code") == 0 || strcmp(current->prop, "data") == 0)) {
            return current; /* Return the pointer to the found symbol node with the specified properties */ 
        }
        current = current->next;
    }
    return NULL; /* Return NULL if the symbol is not found or does not have the specified properties */ 
}


/**
 * Frees all resources associated with a SymbolTable. After calling this function,
 * all symbols and their associated properties are deallocated, and the symbol table's
 * pointer is set to NULL to prevent dangling references.
 *
 * @param st A pointer to the SymbolTable to free.
 */
void freeSymbolTable(SymbolTable *st) {
    SymbolNode *current = st->head;
    SymbolNode *next;

    while (current != NULL) {
        next = current->next;
        free(current->symbol);
        free(current->prop);
        free(current);
        current = next;
    }
    st->head = NULL;
    st->symbolCount = 0;
}


/**
 * Writes the contents of a SymbolTable to a file.
 * 
 * @param st A pointer to the SymbolTable.
 * @param filename The name of the file to write to.
 */
void writeSymbolTableToFile(const SymbolTable *st, const char *filename)
{
    SymbolNode *current = st->head;
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }
    while (current != NULL)
    {
        fprintf(file, "Symbol: %s, Prop: %s, Val: %d\n",
                current->symbol, current->prop, current->val);
        current = current->next;
    }
    fclose(file);
}


/**
 * Processes a line assumed to define a symbol with a constant value,
 * checking for format correctness and adding the symbol to a symbol table
 * if it does not already exist.
 *
 * @param line The line of text to process.
 * @param delimiters The delimiter characters used to tokenize the line.
 * @param st Pointer to the SymbolTable where the symbol will be added.
 */
int addDefine(char *p, SymbolTable *st)
{
    int number;       /* To store converted number from string */
    char *defineName; /* To store the symbol name dynamically */
    char delimiters[] = " \t\n";

    /* Get the first token which should be the symbol name */
    p = strtok(NULL, delimiters);

    /* Check if the name is a valid identifier */
    if (checkWord(p) == 0)
    {
        /* Allocate memory for the name of the define */
        defineName = (char *)malloc(strlen(p) + 1); /* +1 for null terminator */
        if (defineName == NULL)
        {
            printf("Error: Memory allocation failed\n");
            return -1;
        }
        strcpy(defineName, p);

        /* Expect '=' as the next token */
        p = strtok(NULL, delimiters);
        if (!(p && strlen(p) == 1 && *p == '='))
        {
            printf("Error: Expected '=', found '%s'\n", p);
            free(defineName);
            return -1;
        }

        /* Next token should be a numeric value */
        p = strtok(NULL, delimiters);
        if (!p || sscanf(p, "%d", &number) != 1)
        {
            printf("Error: Expected integer, found '%s'\n", p);
            free(defineName);
            return -1;
        }

        /* Output for debugging or confirmation */
        printf("mdefine: %s = %d\n", defineName, number);

        /* Add symbol to table if it does not already exist */
        if (!hasSymbol(st, defineName))
        {
            addSymbol(st, defineName, "mdefine", number);
        }
        else
        {
            printf("Error: Symbol '%s' already exists in the symbol table\n", defineName);
            return -1;
        }

        /* Free allocated memory for the symbol name */
        free(defineName);
        return 0;
    }
    else
    {
        printf("Error: Invalid symbol name, saved word arn't allowed.'%s'\n", p);
        return -1;
    }
}


/**
 * Checks if a given word is an opcode saved in the list of known opcodes.
 *
 * @param word A pointer to the string representing the word to check.
 * @return 0 if the word is a known opcode, 1 otherwise.
 */
int isOpCode(char *word)
{
    int i;
    const char *saved_words[] = {
        /*G:1 command - 2 operands*/
        "mov", "cmp", "add", "sub", "lea",
        /*G:2 command - 1 operands*/
        "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
        /*G:3 no operands */
        "rts", "hlt"
    };

    int numWords = sizeof(saved_words) / sizeof(saved_words[0]);
    for (i = 0; i < numWords; i++)
    {
        if (strcmp(saved_words[i], word) == 0)
        {
            return 0; /* match found*/
        }
    }
    return 1; /* no match found */
}


/**
 * Retrieves the opcode for a given command string.
 *
 * @param command A pointer to the string representing the command.
 * @return The corresponding opcode if found, -1 otherwise.
 */
op_code getOpCode(const char *command)
{
    /*---try to make this static for reuseable*/
    CommandMap commands[] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"not", not}, {"clr", clr}, {"lea", lea}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"hlt", hlt}
    };
    
    int i;
    for (i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        if (strcmp(commands[i].name, command) == 0)
        {
            return commands[i].code; /* return the number value of this */
        }
    }
    return -1; /* if it's not opcode */
}


/**
 * Checks if a string represents a valid integer. The function accounts for leading whitespace,
 * an optional plus or minus sign, and numeric characters.
 *
 * @param str A pointer to a null-terminated string to be checked.
 * @return Returns 1 if the string is a valid integer representation, 0 otherwise.
 */
int isInteger(const char *str)
{
    /* Skip leading whitespace */
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    /* Optional leading + or - sign */
    if (*str == '+' || *str == '-')
    {
        str++;
    }

    /* Check for empty string or just a sign without digits */
    if (*str == '\0')
    {
        return 0;
    }

    /* Check for numeric characters */
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return 0; /* Non-digit character found */
        }
        str++;
    }

    return 1; /* Only digits (possibly with leading sign and whitespace) were found */
}


/**
 * Normalizes a given string in-place by removing all spaces directly before and after commas.
 * This function ensures that no spaces surround any commas in the string, while maintaining
 * other parts of the string unchanged.
 *
 * @param input The modifiable input string that may contain extra spaces around commas.
 */
void normalizeString(char *input)
{
    int i = 0, j = 0;      /* Indexes for input and 'output' within the same string */
    int lastNonSpace = -1; /* Track the last non-space character position */

    while (input[i] != '\0')
    {
        if (input[i] == ' ')
        {
            /* Only add space to output if it is not potentially before a comma */
            if (input[i + 1] != ',' && (i == 0 || input[i - 1] != ','))
            {
                input[j++] = input[i];
            }
            i++;
        }
        else if (input[i] == ',')
        {
            /* Adjust j to remove any spaces before the comma */
            if (lastNonSpace >= 0 && lastNonSpace < j - 1)
            {
                j = lastNonSpace + 1;
            }
            input[j++] = input[i]; /* Add the comma to the 'output' */
            i++;
            /* Skip all spaces following the comma */
            while (input[i] == ' ')
            {
                i++;
            }
        }
        else
        {
            input[j++] = input[i]; /* Copy the character to the 'output' */
            lastNonSpace = j - 1;  /* Update last non-space character position */
            i++;
        }
    }

    input[j] = '\0'; /* Null-terminate the modified string */
}


/**
 * Checks if a given string is a valid label.
 * 
 * A valid label must start with an alphabetical character, followed by
 * any combination of alphanumeric characters.
 * 
 * @param lable A pointer to the string to be checked.
 * @return 1 if the string is a valid label, -1 otherwise.
 */
int isValidLable(char* lable){
    int i;
    /* the first char of a LABLE need to be alphaBatic */
    if (!isalpha(*lable)){
        return -1;
    }

    /* check all the character if they valid */
    for (i = 1; i < strlen(lable); i++){
        char c = *(lable +i);
        if (!isalpha(c) && !isdigit(c)){
            return -1;
        }
    }
    return 1;
}


/**
 * Combines an integer and a string into a formatted string.
 * The integer is formatted to four digits with leading zeros and combined with the string with three spaces in between.
 *
 * @param str The string part of the input.
 * @param num The integer to format and combine with the string.
 * @return A dynamically allocated string containing the formatted output. 
 *         The caller is responsible for freeing this memory.
 */
char* combineIntStr(const char *str, int num) {
    /* Allocate memory for the resulting string */ 
    /* Integer (4 digits + '\0') + 2 spaces + original string + '\0' */ 
    char *result = malloc(5 + 2 + strlen(str) + 1);
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    /* Use sprintf to format and concatenate the string directly into the allocated buffer */ 
    sprintf(result, "%s \t %04d", str, num);

    return result;
}


/**
 * Concatenates a given filename with a specified extension and returns the new string.
 * The caller is responsible for freeing the returned string.
 *
 * @param fileName The base name of the file.
 * @param extension The extension to append to the filename, including the dot (e.g., ".txt").
 * @return A new dynamically allocated string containing the original filename with the specified extension appended.
 */
char* createExtendedFileName(const char* fileName, const char* extension) { 
    /* Calculate needed length for the new string */ 
    int length = strlen(fileName) + strlen(extension) + 1; /* +1 for the null terminator */ 
    char* outputFileName = (char*)malloc(length);
    if (outputFileName == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Copy the original filename and concatenate the extension */ 
    strcpy(outputFileName, fileName);
    strcat(outputFileName, extension);

    return outputFileName;
}
