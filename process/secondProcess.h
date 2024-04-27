#ifndef secondProcess_H
#define secondProcess_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "../util/util.h"
#include "printBinary.h"
#include "dataCodeTable.h"


/**
 * "Second process" handling the conversion and linking of
 * labels (entries and externals) to their binary and address representations.
 * It manages symbol resolution and prepares the data for the .ent and .ext files.
 * 
 * The function iterates over the symbol table to classify symbols as 'external' or 'entry',
 * adding them to separate lists. It then scans the instruction code table to update symbol
 * references with their binary codes and ARE (Absolute, Relocatable, External) attributes.
 * Errors during the process affect the Flag, potentially preventing file creation.
 * 
 * @param DC_table Pointer to the WordList for data code (DC) table.
 * @param IC_table Pointer to the WordList for instruction code (IC) table.
 * @param entF Pointer to the WordList where entries will be stored for the .ent file.
 * @param extF Pointer to the WordList where externals will be stored for the .ext file.
 * @param st Pointer to the SymbolTable containing symbols to be resolved.
 * @param Flag Pointer to an integer flag used for error handling; modified if errors occur.
 */
void startSecondProcess(WordList *DC_table, WordList *IC_table, WordList *entF, WordList *extF, SymbolTable *st, int *Flag);


/**
 * Compares each word in the linked list with a given string.
 * If a match is found, returns 1. Otherwise, returns 0.
 *
 * @param list A pointer to the WordList to traverse.
 * @param compareWord The string to compare against each word in the list.
 * @return 1 if a match is found, 0 otherwise.
 */
int searchWordLL(char *LABLE, WordList *wl);


#endif
