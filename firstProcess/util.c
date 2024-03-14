#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../globVal/glob_val.h"
#include "util.h"

void initSymbolTable(SymbolTable *symbolArray, size_t symbolCount)
{
    symbolArray = NULL;
    symbolCount = 0;
}

void addSymbol(SymbolTable *symbolArray, size_t symbolCount, char* symbol, char* prop, int val) {
    /* Resize the symbolArray to accommodate one more SymbolTable */ 
    SymbolTable *newArray = (SymbolTable*)realloc(symbolArray, (symbolCount + 1) * sizeof(SymbolTable));
    if (newArray == NULL) {
        /* Handle realloc failure */ 
        printf("Error reallocating memory\n");
        return;
    }
    symbolArray = newArray;

    /* Allocate memory and copy the strings to ensure the data is owned by the array */
    symbolArray[symbolCount].symbol = strdup(symbol);
    symbolArray[symbolCount].prop = strdup(prop);
    symbolArray[symbolCount].val = val;

    symbolCount++;
}

void printSymbols(const SymbolTable *symbolArray, size_t symbolCount) {
    size_t i;
    for (i = 0; i < symbolCount; i++) {
        printf("Symbol: %s, Prop: %s, Val: %d\n", symbolArray[i].symbol, symbolArray[i].prop, symbolArray[i].val);
    }
}

int hasSymbol(SymbolTable *symbolArray, size_t symbolCount, char* name) {
    size_t i;
    for (i = 0; i < symbolCount; i++) {
        if (strcmp(symbolArray[i].symbol, name) == 0) {
            return 1;
        }
    }
    return 0;
}

void freeSymbolTable(SymbolTable **symbolArray, size_t *symbolCount) {
    size_t i;
    for (i = 0; i < *symbolCount; i++) {
        free((*symbolArray)[i].symbol);
        free((*symbolArray)[i].prop);
    }
    free(*symbolArray);
    *symbolArray = NULL;
    *symbolCount = 0;
}

void writeSymbolTableToFile(const SymbolTable *symbolArray, size_t symbolCount, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    size_t i;
    for (i = 0; i < symbolCount; i++) {
        fprintf(file, "Symbol: %s, Prop: %s, Val: %d\n", symbolArray[i].symbol, symbolArray[i].prop, symbolArray[i].val);
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

