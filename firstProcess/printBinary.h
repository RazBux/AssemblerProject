#ifndef PRINT_BINARY_H
#define PRINT_BINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct First_word {
    unsigned int not_in_use : 4;
    unsigned int op_code : 4;
    unsigned int src_op_addr : 2;
    unsigned int dest_op_addr : 2;
    unsigned int ARE : 2;
} First_word;

/*format the number divsion in binary*/
typedef struct Number
{
    /* 14 bytes, ARE_bytes are 00 */
    unsigned int number : 12; /*The number will be display as 12 digint binary*/
    unsigned int ARE : 2;
} Number;

char* intToBinaryString(unsigned int num, int bits);

char* getFirstWordBinary(const First_word *word);

char* BinaryString14(int number);

#endif