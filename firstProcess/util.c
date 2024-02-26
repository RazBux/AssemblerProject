#include <stdio.h>
#include <string.h>

/* Define boolean values */
#define true 1
#define false 0
typedef int bool;


#define SAVED_WORDS \
    /*G:1 command - 2 operands*/ \
    "mov", "cmp", "add", "sub", "lea",  \
    /*G:2 command - 1 operands*/ \
    "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"  \
    /*G:3 no operands */  \
    , "rts", "hlt" 

/* Declare an array of pointers to char, initializing it with the macro */
const char* saved_words[] = { SAVED_WORDS };
const int numWords = sizeof(saved_words) / sizeof(saved_words[0]);



/* Function to check if a word is in saved_words */
bool is_valid_save_word(char* word){
    int i;
    const char* saved_words[] = {
    /*G:1 command - 2 operands*/ 
    "mov", "cmp", "add", "sub", "lea",  
    /*G:2 command - 1 operands*/ 
    "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"  
    /*G:3 no operands */  
    , "rts", "hlt"
    /*Register names*/
    ,"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };

    for (i = 0; i < numWords; i++) {
        if (strcmp(saved_words[i], word) == 0) {
            return 1; /*True, match found*/ 
        }
    }
    return 0; /* False, no match found */
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

int main(void) {
    int i; 
    for (i = 0; i < numWords; i++) {
        printf("%s, no: %d,  bytes: ", saved_words[i], i);
        printBinary14(i);
        printf("\n");
    }

    printBinary14(-5);
    return 0;
}

