#include <stdlib.h>
#include <string.h>
#include "secondProcess.h"
#include "../utils/util.h"
#include "../utils/printBinary.h"

int searchWordLL(char *, WordList *);

/**
 * "Second process" handling the conversion and linking of
 * labels (entries and externals) to their binary and address representations.
 * It manages symbol resolution and prepares the data for the .ent and .ext files.
 *
 * @param DC_table Pointer to the WordList for data code (DC) table.
 * @param IC_table Pointer to the WordList for instruction code (IC) table.
 * @param entF Pointer to the WordList where entries will be stored for the .ent file.
 * @param extF Pointer to the WordList where externals will be stored for the .ext file.
 * @param st Pointer to the SymbolTable containing symbols to be resolved.
 * @param Flag Pointer to an integer flag used for error handling; modified if errors occur.
 *
 * @return Returns 0 upon completion, irrespective of errors encountered during the process.
 *
 * The function iterates over the symbol table to classify symbols as 'external' or 'entry',
 * adding them to separate lists. It then scans the instruction code table to update symbol
 * references with their binary codes and ARE (Absolute, Relocatable, External) attributes.
 * Errors during the process affect the Flag, potentially preventing file creation.
 */
void startSecondProcess(WordList *DC_table, WordList *IC_table, WordList *entF, WordList *extF, SymbolTable *st, int *Flag)
{
    /* creating linkedList for both entries and externes that will help with the ARE bytes */
    WordList externes = {NULL, 0};
    WordList enteries = {NULL, 0};
    SymbolNode *stCurrent = st->head;

    Node *current;
    int trackIC = IC_table->count + 100 - 1;

    printf("\n--- START SECOND PROCESS ---\n");

    /* adding the entries and externs into thier lists */
    while (stCurrent != NULL)
    {
        if (strcmp(stCurrent->prop, "external") == 0)
        {
            addWord(&externes, stCurrent->symbol);
        }
        else if (strcmp(stCurrent->prop, "entry") == 0)
        {
            addWord(&enteries, stCurrent->symbol);
        }
        stCurrent = stCurrent->next;
    }

    /*
    printf("\nExterns_WordList >> ");
    printWordListReverse(&externes);
    */

    /*for each symbol in the IC_Code -> change the word to bin*/
    current = IC_table->head;
    while (current != NULL)
    {
        /*cheack for symboles - if the starting char isn't 0 or 1 -> it's a LABLE*/
        if (!(*current->word == '0' || *current->word == '1'))
        {
            Number labNum = {0};
            /* printf("line >> %s\n", current->word); */
            /* change the LABLE to bin & add the ARE. >> EXTERN: 01 {1} ENTRY:  10 {2} */
            /*if the value is extern*/
            if (searchWordLL(current->word, &externes))
            {
                /* add extern word for the .ext file */
                char *combinW = combineIntStr(current->word, trackIC);
                addWord(extF, combinW);
                free(combinW);

                labNum.ARE = 1;
                /*printf("change the number from:: %s => %s\n", current->word, getNumberBinary(&labNum));*/
                current->word = getNumberBinary(&labNum); /*change the lable to bin*/
            }
            else if (getSymbolIndex_dc(st, current->word) != NULL)
            {

                SymbolNode *symbolNode = getSymbolIndex_dc(st, current->word);

                /*if the word in the enteries list - add it for the .ent file */
                if (searchWordLL(current->word, &enteries))
                {
                    char *combinW = combineIntStr(symbolNode->symbol, symbolNode->val);
                    addWord(entF, combinW);
                    free(combinW);
                }

                /* change the LABLE to bin numbers */
                labNum.ARE = 2;
                labNum.number = symbolNode->val;

                /* printf("change the number from:: %s => %s\n", current->word, getNumberBinary(&labNum)); */
                current->word = getNumberBinary(&labNum);
            }
            else
            {
                printf("Error: there is no %s in the Symbol Table\n", current->word);
                /* raise a flag so the files won't be created */
                *Flag -= 1;
            }
        }
        current = current->next;
        trackIC -= 1;
    }

    printf("\n--- SECOND PROCESS COMPLETE ---\n");
}

/**
 * Compares each word in the linked list with a given string.
 * If a match is found, returns 1. Otherwise, returns 0.
 *
 * @param list A pointer to the WordList to traverse.
 * @param compareWord The string to compare against each word in the list.
 * @return 1 if a match is found, 0 otherwise.
 */
int searchWordLL(char *LABLE, WordList *wl)
{
    Node *current = wl->head;
    while (current != NULL)
    {
        if (strcmp(current->word, LABLE) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}
