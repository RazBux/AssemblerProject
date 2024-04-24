
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "util.h"
#include "printBinary.h"
#include "dataCodeTable.h"


#ifndef secondProcess_H
#define secondProcess_H


int startSecondProcess(WordList *DC_table, WordList *IC_table, WordList *entF, WordList *extF, SymbolTable *st, int *Flag);
char* combineIntStr(const char *str, int num);
int searchWordLL(char *LABLE, WordList *wl);

#endif

