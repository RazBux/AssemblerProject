#ifndef _GLOB_VAL_H
#define _GLOB_VAL_H

#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME 30
#define MAX_LABLE_NAME 31
#define MEMORY_SIZE 4096
#define WORD_LEN 14

/* Use for managing the state when processing the line from the Asmbly file */
enum Flag{
    START, 
    LABEL, /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
    OP_CODE, /* operands name sparating by ',' */
    G1_OPERAND,
    G2_OPERAND,
    G3_OPERAND,
    DEFINE, /* name of the  */
    NUM_OF_FLAG
};

/* Defines a register type - r0 to r7 & not a register */
typedef enum Registers
{
    r0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    NOT_REG = -1 /*there is use for this one ?*/
} reg;

/* Defines the codes for the supported operations */
typedef enum Op_code
{
    mov,
    cmp,
    add,
    sub,
    lea,
    clr,
    not,
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


/* division for bytes for the "First word" in an instruction */
typedef struct First_word
{
    /*14 bytes total*/
    unsigned int ARE : 2;
    unsigned int dest_op_addr : 2;
    unsigned int src_op_addr : 2;
    unsigned int op_code : 4;
    unsigned int not_in_use : 4;
} first_word;

/*format the number divsion in binary*/
typedef struct Number
{
    /* 14 bytes, ARE_bytes are 00 */
    unsigned int ARE : 2;
    unsigned int number : 12; /*The number will be display as 12 digint binary*/
} number;

#endif
