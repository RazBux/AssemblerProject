/* utils.h */

#ifndef INSTRUCTION_UTILS_H
#define INSTRUCTION_UTILS_H

#include <stdio.h>
#include <string.h>

/* Include global values if needed */
#include "../globVal/glob_val.h"

/* Function prototypes */
int is_op_code(char* word);
void printBinary14(unsigned int number);

#endif /* INSTRUCTION_UTILS_H */
