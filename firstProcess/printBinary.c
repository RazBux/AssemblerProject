#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "printBinary.h"

/**
 * Prints the 14 bits of an integer or charachter in binary.
 * @param number The integer whose binary representation is to be printed.
 */
char *BinaryString14(int number)
{
    int i;
    static char binaryStr[14]; /* 14 bits plus null terminator */
    int mask = 1 << 13;        /* Start with the most significant bit of the 14-bit portion */

    for (i = 0; i < 14; i++)
    {
        binaryStr[i] = (number & mask) ? '1' : '0';
        mask >>= 1; /* Move the mask to the next bit */
    }
    printf("No.%d ,Binary == %s\n", number, binaryStr);
    return binaryStr;
}

/**
 * Convert an integer to a binary string with a fixed number of bits.
 * @param num The unsigned integer to convert.
 * @param bits The number of bits in the resulting binary string.
 * @return A pointer to the dynamically allocated binary string.
 */
char *intToBinaryString(unsigned int num, int bits)
{
    int i;
    char *binaryStr = malloc(bits + 1);
    if (binaryStr == NULL)
    {
        return NULL; /* Memory allocation failed */ 
    }
    binaryStr[bits] = '\0';
    for (i = bits - 1; i >= 0; i--)
    {
        binaryStr[i] = (num & 1) + '0';
        num >>= 1;
    }
    return binaryStr;
}

/**
 * Creates a binary string representation of a First_word struct.
 * @param word Pointer to the First_word struct.
 * @return A pointer to the dynamically allocated binary string of the number.
 */
char *getFirstWordBinary(const First_word *word)
{
    char *binaryStr = malloc(15); /* Allocate for 14 bits plus null terminator */ 
    char *temp;
    if (binaryStr == NULL)
    {
        printf("Failed to allocate memory for binary string.\n");
        return NULL;
    }
    binaryStr[0] = '\0'; /* Start with an empty string */ 

    /* Concatenate all bit fields into one binary string in the correct order */ 
    
    temp = intToBinaryString(word->not_in_use, 4);
    strcpy(binaryStr, temp);
    free(temp);

    temp = intToBinaryString(word->op_code, 4);
    strcat(binaryStr, temp);
    free(temp);

    temp = intToBinaryString(word->src_op_addr, 2);
    strcat(binaryStr, temp);
    free(temp);

    temp = intToBinaryString(word->dest_op_addr, 2);
    strcat(binaryStr, temp);
    free(temp);

    temp = intToBinaryString(word->ARE, 2);
    strcat(binaryStr, temp);
    free(temp);

    /*printf("Complete First Word (Binary): %s\n", binaryStr);*/
    return binaryStr;
}

/**
 * Creates a binary string representation of a number struct.
 * @param numStruct Pointer to the Number struct.
 * @return A pointer to the dynamically allocated binary string of the number.
 */
char *printNumberInBinary(const Number *numStruct)
{
    char *binaryStr = malloc(15); /* Allocate for 14 bits plus null terminator */ 
    char *temp;
    if (binaryStr == NULL)
    {
        printf("Failed to allocate memory for binary string.\n");
        return NULL;
    }
    binaryStr[0] = '\0'; /* Start with an empty string */ 

    /* Concatenate all bit fields into one binary string in the correct order */ 
    
    if (numStruct->number > 4095) {
        printf("Error: the number %u is too big - cannot fit in 12 bits\n", numStruct->number);
        return NULL;
    }
    temp = intToBinaryString(numStruct->number, 12);
    strcpy(binaryStr, temp);
    free(temp);

    temp = intToBinaryString(numStruct->ARE, 2);
    strcat(binaryStr, temp);
    free(temp);

    printf("Complete First Word (Binary): %s\n", binaryStr);
    /* free(binaryStr); */ 
    return binaryStr;
}

/*
int main()
{
    First_word word = {.op_code = 5, .ARE = 3};
    char *bin = printFirstWordBinary(&word);
    printf("bin %s\n", bin);

    Number num = {.ARE = 1, .number = 4095};
    char *binaryNumber = printNumberInBinary(&num);
    if (binaryNumber)
    {
        printf("Binary representation: %s\n", binaryNumber);
        free(binaryNumber);
    }
    free(bin);
    return 0;
}
*/

