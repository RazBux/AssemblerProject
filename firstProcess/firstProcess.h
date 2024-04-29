#ifndef FIRST_PROCESS_H
#define FIRST_PROCESS_H

#include "../utils/dataCodeTable.h"
#include "../utils/util.h"

/**
 * Starts the first process of assembly translation.
 * @param asmblerOpenFile Filename of the assembler input.
 * @param DC_table Data code table.
 * @param IC_table Instruction code table.
 * @param st Symbol table.
 * @param DC Data counter initial value.
 * @param IC Instruction counter initial value.
 * @param Flag Pointer to a flag indicating process status.
 * @return Integer status, 0 if successful, -1 if failed.
 */
void startFirstProcess(char *asmblerOpenFile, WordList *DC_table, WordList *IC_table, SymbolTable *st, int DC, int IC, int *Flag);


#endif /* FIRST_PROCESS_H */
