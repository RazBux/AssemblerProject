/*
#ifndef _GLOB_VAL_H
#define _GLOB_VAL_H

#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME 30
#define MAX_LABLE_NAME 31
#define MEMORY_SIZE 4096
#define INITIAL_MACRO_COUNT 10
#define WORD_LEN 14

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
    NOT_REG = -1
} reg;

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

CommandMap commands[] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"not", not}, {"clr", clr}, {"lea", lea}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"hlt", hlt}
};


#endif
*/

