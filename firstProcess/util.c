#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../globVal/glob_val.h"
#include "util.h"

/**
 * Adds a new symbol along with its properties and value to the SymbolTable.
 * This function dynamically resizes the symbol array within the SymbolTable to accommodate the new symbol.
 * @param st A pointer to the SymbolTable to add the symbol to.
 * @param symbol The symbol name as a string.
 * @param prop The property associated with the symbol as a string.
 * @param val The integer value associated with the symbol.
 */
void addSymbol(SymbolTable *st, char *symbol, char *prop, int val) {
    /* Resize the symbols array to accommodate one more Symbol */
    Symbol *newArray = (Symbol *)realloc(st->symbols, (st->symbolCount + 1) * sizeof(Symbol));
    if (newArray == NULL) {
        /* Handle realloc failure */
        printf("Error reallocating memory\n");
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
void initSymbolTable(SymbolTable *st) {
    st->symbols = NULL; /* Start with no symbols */
    st->symbolCount = 0; /* No symbols initially */
}

/**
 * Prints all symbols in the SymbolTable.
 * @param st A pointer to the SymbolTable to print.
 */
void printSymbols(const SymbolTable *st) {
    size_t i;
    for (i = 0; i < st->symbolCount; i++) {
        printf("Symbol: %s, Property: %s, Value: %d\n",
               st->symbols[i].symbol, st->symbols[i].prop, st->symbols[i].val);
    }
}

/**
 * Checks if a symbol exists in the SymbolTable.
 * @param st A pointer to the SymbolTable.
 * @param name The name of the symbol to check.
 * @return 1 if the symbol exists, 0 otherwise.
 */
int hasSymbol(const SymbolTable *st, const char* name) {
    size_t i;
    for (i = 0; i < st->symbolCount; i++) {
        if (strcmp(st->symbols[i].symbol, name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Frees all resources associated with a SymbolTable.
 * @param st A pointer to a pointer of the SymbolTable to free.
 */
void freeSymbolTable(SymbolTable **st) {
    size_t i;
    for (i = 0; i < (*st)->symbolCount; i++) {
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
void writeSymbolTableToFile(const SymbolTable *st, const char *filename) {
    size_t i;
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    for (i = 0; i < st->symbolCount; i++) {
        fprintf(file, "Symbol: %s, Prop: %s, Val: %d\n",
                st->symbols[i].symbol, st->symbols[i].prop, st->symbols[i].val);
    }
    fclose(file);
}

/* Function to check if a word is in saved_words */
int isOpCode(char* word){
    int i;
    const char* saved_words[] = {
    /*G:1 command - 2 operands*/ 
    "mov", "cmp", "add", "sub", "lea",  
    /*G:2 command - 1 operands*/ 
    "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"  
    /*G:3 no operands */  
    , "rts", "hlt"

    /* Register names maybe not need to be include. maybe only for checks */
    /*,"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"*/
    };

    int numWords = sizeof(saved_words) / sizeof(saved_words[0]);
    for (i = 0; i < numWords; i++) {
        if (strcmp(saved_words[i], word) == 0) {
            return 0; /* match found*/ 
        }
    }
    return 1; /* no match found */
}

/* Function to print the binary representation of a number's 
    leave the 2 last bit 00 as wanted in the project */
void printBinary14(unsigned int number) {
    unsigned int mask = 1 << 11; /*Start with the most significant bit of the 12-bit portion*/ 
    int i;
    for (i = 0; i < 12; i++) { /*Iterate through the first 12 bits*/ 
        if (number & mask) {
            printf("1");
        } else {
            printf("0");
        }
        mask >>= 1; /*Move the mask to the next bit*/ 
    }

    /*Print the last two bits as 0*/  
    printf("00");
}

