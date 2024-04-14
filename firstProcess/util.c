#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../globVal/glob_val.h"
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
 * Checks if a given word is in the array of saved words.
 *
 * @param word The word to check against the saved words.
 * @return Returns 1 if the word is found, 0 otherwise.
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
 * Adds a new symbol along with its properties and value to the SymbolTable.
 * This function dynamically resizes the symbol array within the SymbolTable to accommodate the new symbol.
 * @param st A pointer to the SymbolTable to add the symbol to.
 * @param symbol The symbol name as a string.
 * @param prop The property associated with the symbol as a string.
 * @param val The integer value associated with the symbol.
 */
void addSymbol(SymbolTable *st, char *symbol, char *prop, int val)
{
    /* if the symbol is new - create it */
    /* Resize the symbols array to accommodate one more Symbol */
    Symbol *newArray = (Symbol *)realloc(st->symbols, (st->symbolCount + 1) * sizeof(Symbol));
    printf("Added to ST >> %s, %s, %d\n", symbol,prop, val);
    if (newArray == NULL)
    {
        /* Handle realloc failure */
        printf("Error reallocating memory\n\n");
        return;
    }
    st->symbols = newArray;

    /* Allocate memory and copy the strings to ensure the data is owned by the array */
    st->symbols[st->symbolCount].symbol = strdup(symbol);
    st->symbols[st->symbolCount].prop = strdup(prop);
    st->symbols[st->symbolCount].val = val;

    st->symbolCount++; /* Increment the count of symbols */
}

/**
 * Initializes a SymbolTable by setting the symbols pointer to NULL
 * and symbolCount to 0, indicating an empty table.
 * @param st A pointer to the SymbolTable to initialize.
 */
void initSymbolTable(SymbolTable *st)
{
    st->symbols = NULL;  /* Start with no symbols */
    st->symbolCount = 0; /* No symbols initially */
}

/**
 * Prints all symbols in the SymbolTable.
 * @param st A pointer to the SymbolTable to print.
 */
void printSymbols(const SymbolTable *st)
{
    size_t i;
    int symbolWidth = 10;   
    int propWidth = 10;     
    int valueWidth = 10;    
    /* Print header */
    int len = printf("%-*s %-*s %-*s\n", symbolWidth, "Symbol", propWidth, "Property", valueWidth, "Value");
    
    /* Print dashes '-' for the length of the printed line */
    for (i = 0; i < len - 5; i++) {
        printf("-");
    }
    printf("\n");

    /* Print each symbol in the table */ 
    for (i = 0; i < st->symbolCount; i++)
    {
        printf("%-*s %-*s %-*d\n",
               symbolWidth, st->symbols[i].symbol,
               propWidth, st->symbols[i].prop,
               valueWidth, st->symbols[i].val);
    }
}
/* 
void printSymbols(const SymbolTable *st)
{
    size_t i;
    for (i = 0; i < st->symbolCount; i++)
    {
        printf("Symbol: %s, Property: %s, Value: %d\n",
               st->symbols[i].symbol, st->symbols[i].prop, st->symbols[i].val);
    }
}
*/



/**
 * Checks if a symbol exists in the SymbolTable.
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return 1 if the symbol exists, 0 otherwise.
 */
int hasSymbol(const SymbolTable *st, const char *name)
{
    size_t i;
    for (i = 0; i < st->symbolCount; i++)
    {
        if (strcmp(st->symbols[i].symbol, name) == 0)
        {
            printf("Error: %s is already in the symbol table", name);
            return 1;
        }
    }
    return 0;
}

/**
 * Frees all resources associated with a SymbolTable.
 * @param st A pointer to a pointer of the SymbolTable to free.
 */
void freeSymbolTable(SymbolTable **st)
{
    size_t i;
    for (i = 0; i < (*st)->symbolCount; i++)
    {
        free((*st)->symbols[i].symbol);
        free((*st)->symbols[i].prop);
    }
    free((*st)->symbols);
    (*st)->symbols = NULL;
    (*st)->symbolCount = 0;
}

/**
 * Writes the contents of a SymbolTable to a file.
 * @param st A pointer to the SymbolTable.
 * @param filename The name of the file to write to.
 */
void writeSymbolTableToFile(const SymbolTable *st, const char *filename)
{
    size_t i;
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }
    for (i = 0; i < st->symbolCount; i++)
    {
        fprintf(file, "Symbol: %s, Prop: %s, Val: %d\n",
                st->symbols[i].symbol, st->symbols[i].prop, st->symbols[i].val);
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

/* Function to check if a word is in saved_words */
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

/* Function to print the binary representation of a number's
    leave the 2 last bit 00 as wanted in the project */
void printBinary14(unsigned int number)
{
    unsigned int mask = 1 << 11; /*Start with the most significant bit of the 12-bit portion*/
    int i;
    for (i = 0; i < 12; i++)
    { /*Iterate through the first 12 bits*/
        if (number & mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
        mask >>= 1; /*Move the mask to the next bit*/
    }

    /*Print the last two bits as 0*/
    printf("00");
}

