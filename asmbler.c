
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util/util.h"
#include "preAsmbler/preAsmbler.h"
#include "process/dataCodeTable.h"
#include "process/firstProcess.h"
#include "process/secondProcess.h"
#include "process/printBinary.h"
#include "encryption/encryption.h"

int main(int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc; i++)
    {
        /********************************************************
         ********************CREATE VARIABLES********************
         ********************************************************/
        char *asInputFile = createExtendedFileName(argv[i], ".as");
        char *asOutputFile = createExtendedFileName(argv[i], ".am");

        /* create lists for storing the code image */
        WordList IC_table = {NULL, 0};
        WordList DC_table = {NULL, 0};

        /* lists for .ent and .ext files */
        WordList extWL = {NULL, 0};
        WordList entWL = {NULL, 0};

        /* counters for tracking the code and instruction */
        int DC = 0;   /* Data counter */
        int IC = 0;   /* Instruction counter */
        int Flag = 0; /* Flag for errors */

        /* create and initialize the symbol table */
        SymbolTable st;
        initSymbolTable(&st);

        /********************************************************
         ********************START PROCESSING********************
         ********************************************************/
        printf("\n##### %5sASMBLER FILE %s.as %5s ####\n", "", argv[i], "");

        /* PreAsembler - after finish the .am file will be writen */
        Flag += startPreAsmbler(asInputFile, asOutputFile);

        if (Flag == 0)
        {
            /* first process fill the symbol-table & create code image in the IC_DC tables */
            startFirstProcess(asOutputFile, &DC_table, &IC_table, &st, DC, IC, &Flag);

            /* second process change the lables in IC_table to binary code */
            startSecondProcess(&DC_table, &IC_table, &entWL, &extWL, &st, &Flag);
        }
        /****************************************************
         ********************FILW WRITING********************
         ****************************************************/
        /* if there were no errors create the files and encrypt the machine code */
        if (Flag != 0)
        {
            printf("There were %d number of error >> the program won't create the files\n", abs(Flag));
        }
        else
        {
            char *objFile = createExtendedFileName(argv[i], ".ob");

            printf("Complete processing >> creating files now.\n");

            /* encrypt and create the code files */
            writeEncryptionToFile(&IC_table, &DC_table, objFile);

            if (extWL.count > 0)
            {
                char *extFile = createExtendedFileName(argv[i], ".ext");
                writeWordListToFile(&extWL, extFile);

                printf("\nExtern >> ");
                printWordList(&extWL);
            }

            if (entWL.count > 0)
            {
                char *entFile = createExtendedFileName(argv[i], ".ent");
                writeWordListToFile(&entWL, entFile);

                printf("\nEntry >> ");
                printWordList(&entWL);
            }
        }

        freeSymbolTable(&st);
        freeWordList(&IC_table);
        freeWordList(&DC_table);
        freeWordList(&entWL);
        freeWordList(&extWL);

        printf("##### %5sEND OF %s.as %5s #### \n", "", argv[i], "");
    }

    return 0;
}
