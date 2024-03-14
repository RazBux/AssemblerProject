#include <stdio.h>
#include <string.h>
#include "../globVal/glob_val.h"
#include "util.h"

/* Function to check if a word is in saved_words */
int is_op_code(char* word){
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

