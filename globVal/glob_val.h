#ifndef _GLOB_VAL_H
#define _GLOB_VAL_H

#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME 30
#define MAX_LABLE_NAME 31
#define MEMORY_SIZE 4096
#define INITIAL_MACRO_COUNT 10
#define WORD_LEN 14

/* Dynamic structure to hold macro names and texts */
typedef struct
{
    char **names;
    char **texts;
    int size;
    int count;
} MacroStorage;

/* Use for managing the state when processing the line from the Asmbly file */
enum Flag
{
    START,
    LABEL,   /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
    OP_CODE, /* operands name sparating by ',' */
    G1_OPERAND,
    G2_OPERAND,
    G3_OPERAND,
    DEFINE,
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
    not,
    clr,
    lea,
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


typedef struct {
    const char *name;
    op_code code;
} CommandMap;

/*
CommandMap commands[] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"not", not}, {"clr", clr}, {"lea", lea}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"hlt", hlt}
};
*/

#endif
