#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "util.h"
#include "../globVal/glob_val.h"

int processLine(char *, int, int, SymbolTable *);
int startFirstProcess(char *, char *);

int startFirstProcess(char *asmblerOpenFile, char *newFile)
{
    FILE *file = fopen(asmblerOpenFile, "r");
    // FILE *newFile = fopen(newFile, "w");
    char line[MAX_LINE_LENGTH];
    /* creaet counters for tracking the code and instruction */
    int DC = 0; /* Data counter */
    int IC = 0; /* Instruction counter */
    /* create symbol table */
    SymbolTable st;
    initSymbolTable(&st); /* Initialize the symbol table */

    /*check if the file is opened*/
    if (!file)
    {
        perror("Open .am file - after open macro => failed");
        return -1;
    }

    /*start processing line by line*/
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        processLine(line, DC, IC, &st);
    }

    printf("\nprint symbol table:::\n");
    printSymbols(&st);
    return 0;
}

/*
    This function process the line of the original Asembly code.
    there are 5 possible line
    1: empty: all lite is white spaces or tabs
    2: comment: the first char of the line is ";"
    3: directive: tell the asembler what to do.
        direcrive line -does not- preduce a machine instruction binary output
    4: instruction: line that preduce a binary for the machine
    5: defining constent: creating a const variable

    if there is an error - the function will return 1.
    that's how the program will know to not preduce the files at the end.
*/
int processLine(char *line, int DC, int IC, SymbolTable *st)
{
    enum Flag flag = START;
    size_t pLen; /* for checking the if it's a lable */

    int firstCharIndex;
    char *p;
    char delimiters[] = " \t\n";
    /*1: check if it is an empty line*/
    if (line[0] == '\0' || strspn(line, " \t\n") == strlen(line))
    {
        printf("Empty or whitespace-only line\n");
        return 0; /* Return immediately if it's an empty line */
    }

    /*2: check if the first char is ; meaning its a comment line */
    firstCharIndex = strspn(line, " \t\n");
    if (line[firstCharIndex] == ';')
    {
        printf("Comment line: %s\n", line);
        return 0; /* Return immediately if the line is a comment */
    }

    /* connect ',' to be w.o spaces between them, before:"6 , -9 , len" after:"6,-9,len" */
    normalizeString(line);
    printf("line >>> %s\n", line);
    /* get the fisrt word from line */
    p = strtok(line, delimiters);

    /* Tokenize the line */
    while (p != NULL)
    {
        pLen = strlen(p);

        /* Define check:
             if it's a define add it to the symbol table */
        if (strcmp(p, ".define") == 0)
        {
            return addDefine(p, st);
        }

        /* Lable check:
            if there is a lable in the first word,
            by see if the last char is ":" */
        else if (flag == START && *(p + pLen - 1) == ':' && pLen < MAX_LABLE_NAME)
        {
            char *label;
            /*if the first char of the lable isnt a char - raise an error/flag*/
            if (!isalpha(*p))
            {
                printf("Error while defining a lable - the first character of %s isn't 'alpha batic'\n", p);
                return -1;
            }

            label = (char *)malloc(pLen); /*Allocate memory for label, including space for null terminator*/
            if (label != NULL)
            { /*if the alloction is was successful*/
                strncpy(label, p, pLen - 1);
                printf("LABLE: %s. > ", label);
            }
            if (checkWord(label) == 0) /* if return 0 - it's valid lable name - add to lable matrix */
            {
                p = strtok(NULL, delimiters); /* Get the next word-token */
                /*check what is the next word and if it's code or data*/
                if (*p == '.' && (strcmp(p, ".data") == 0 || strcmp(p, ".string") == 0))
                {
                    addSymbol(st, label, "data", DC);
                }
                else if (isOpCode(p) == 0)
                {
                    addSymbol(st, label, "code", DC);
                }
            }
            else
            {
                return -1; /* there is an error with the lable name */
            }
            free(label);  /*free the lable memory*/
            flag = LABEL; /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
        }

        /* After checking the start word of the line, and add it to the symbol table
            if needed - this part of code will ganarate the machine code of the asmbler */
        /* if it's a directive line */
        if (*p == '.')
        {
            /* do we need falg ??? 
            if (flag == LABEL)
            {
                printf("data:: ");
            }*/
            printf("DIR: %s\n", p);
            /* check witch of the cases it is */
            if (strcmp(p, ".data") == 0)
            {
                int countData = 0;
                p = strtok(NULL, delimiters);
                /* use while loop to exstract all the data into array */
                while (p != NULL)
                {   
                    char *word = NULL;
                    /* converting each word to int and into 14 binaty */
                    if (isInteger(p)){
                        word = BinaryString14(atoi(p));
                    }
                    else {
                        int num = getSymbolValue(st, p); /* if there is symbol - retunrn it's index */
                        if (num == -1){
                            printf("Error: %s is invalid type of data = %d\n", p, num);
                            return -1;
                        }
                        
                        word = BinaryString14(st->symbols[num].val);
                    }
                    countData++;
                    /*get the next value*/
                    p = strtok(NULL, ", \n");
                }
                if (countData == 0)
                {
                    printf("ERROR: there is no data after the .data\n");
                    return -1;
                }
                DC += countData;
            }
            else if (strcmp(p, ".string") == 0)
            {
                p = strtok(NULL, delimiters);
                /* after a .string the next word need to be - "somting" */
                if (*p == '\"' && *(p + strlen(p) - 1) == '\"')
                {
                    /* exstract the word to */
                    printf("STRING: %s\n", p);
                }
                else
                {
                    printf("Error: no \" at the start or finish of .string for %s\"\n", p);
                    return -1;
                }
            }
            /* the asmbler need to ignore this line and will print worning massage */
            else if (strcmp(p, ".entry") == 0 || strcmp(p, ".extern") == 0)
            {
                printf("Worning: %s ", p);
                return 0;
            }
            /* if it is not a valid directive name -> error */
            else
            {
                printf("Error: this directive command doesn't exsist!\n");
            }
        }
        else if (isOpCode(p) == 0)
        {
            if (flag == LABEL)
            {
                printf("code:: ");
                /* enter it to the flag symbol */
            }
            /* add here the opcode and all the processing for the operands */
            printf("OP_CODE:%s\n", p);
            while (p != NULL)
            {
                p = strtok(NULL, delimiters);
                if (p != NULL)
                {
                    printf("OC: %s\n", p);
                }
            }
        }
        else
        {
            printf("Token: %s\n", p); /* Print each token */
        }

        p = strtok(NULL, delimiters); /* Get the next word-token */
        flag = NUM_OF_FLAG;
    }
    printf("\n");

    return 0;
}

int main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    char newFileWord[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/word.text";
    startFirstProcess(outputFileName, newFileWord);

    return 0;
}

