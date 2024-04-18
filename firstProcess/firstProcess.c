#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "util.h"
#include "printBinary.h"
#include "../globVal/glob_val.h"

int checkAddressType(char *operand, SymbolTable *st);
int processLine(char *, int, int, SymbolTable *, FILE *);
int startFirstProcess(char *, char *);

int startFirstProcess(char *asmblerOpenFile, char *newFile)
{
    FILE *file = fopen(asmblerOpenFile, "r");
    FILE *outputFile = fopen(newFile, "w");
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
    if (!outputFile)
    {
        perror("Failed to open output file");
        fclose(file); /* close the input file before returning */
        return -1;
    }

    /*start processing line by line*/
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        processLine(line, DC, IC, &st, outputFile);
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
int processLine(char *line, int DC, int IC, SymbolTable *st, FILE *outputFile)
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
    printf("line >>> %s", line);
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

            label = (char *)malloc(pLen); /*Allocate memory for label, including space for null terminator*/
            if (label != NULL)
            { /*if the alloction is was successful*/
                strncpy(label, p, pLen - 1);
                printf("LABLE: %s > ", label);
            }
            /* check for valid lable name */
            if (!checkLable(label))
            {
                printf("Error, %c isn't valid for the LABLE roles\n", *label);
                return -1;
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
                else if (*p == '.' && (strcmp(p, ".entry") == 0 || strcmp(p, ".extern") == 0))
                {
                    printf("New E => %s\n", p);
                    /*add it's to the table*/
                    return 1; /*use this to add it to the symbol table and the TableData*/
                }
                else
                {
                    printf("%s is't valid word after LABLE.\n", p);
                    return -1;
                }
            }
            else
            {
                return -1; /* there is an error with the lable name */
            }
            free(label);  /*free the lable memory*/
            flag = LABEL; /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
        }

        /* After checking if there is LABEL and add it to the symbol table
            this part of code will ganarate the machine code of the asmbler */
        /* if it's a directive line */
        if (*p == '.')
        {
            /* do we need falg ???
            if (flag == LABEL)
            {
                printf("data:: ");
            }*/
            /* printf("DIR: %s\n", p); */

            /* check witch of the cases it is */
            if (strcmp(p, ".data") == 0)
            {
                int countData = 0;
                p = strtok(NULL, ", \n");
                /* use while loop to exstract all the data into array */
                while (p != NULL)
                {
                    char *word = NULL;
                    /* converting each word to int and into 14 binaty */
                    if (isInteger(p))
                    {
                        word = BinaryString14(atoi(p));
                    }
                    else
                    {
                        int num = getSymbolValue(st, p); /* if there is symbol - retunrn it's index */
                        if (num == -1)
                        {
                            printf("Error: %s is invalid type of data = %d\n", p, num);
                            return -1;
                        }

                        word = BinaryString14(st->symbols[num].val);
                    }
                    fprintf(outputFile, "%s\n", word); /* write the word to File */
                    countData++;
                    p = strtok(NULL, ", \n"); /*get the next value*/
                }
                if (countData == 0)
                {
                    printf("ERROR: there is no data after the .data\n");
                    return -1;
                }
                DC += countData;
                printf("DC %d", DC);
            }
            else if (strcmp(p, ".string") == 0)
            {
                p = strtok(NULL, delimiters);
                printf("STRING: %s\n", p);
                /* after a .string the next word need to be - "somting" */
                if (*p == '\"' && *(p + strlen(p) - 1) == '\"')
                {
                    size_t len = strlen(p);
                    size_t s;
                    /* Iterate from the second character to the second-to-last character */
                    for (s = 1; s < len - 1; s++)
                    {
                        // BinaryString14(p[s]);
                        fprintf(outputFile, "%s\n", BinaryString14(p[s]));
                    }
                    // BinaryString14('\0');
                    fprintf(outputFile, "%s\n", BinaryString14('\0'));
                    DC += len - 1; /* add 1 number for each charachter and 1 for the null */
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
        else if (getOpCode(p) != -1)
        {
            int opC = getOpCode(p); /*get the number of op code*/
            char *word;
            First_word fw = {0}; /*fw to fill the word*/

            /* G:3 no operands */
            if (strcmp("hlt", p) == 0 || strcmp("rts", p) == 0)
            {
                fw.op_code = opC;
                word = printFirstWordBinary(&fw);
                printf("fw binay: %s\n", word);
                p = strtok(NULL, delimiters);
                /*there is no word after the operand*/
                if (p == NULL)
                {
                    IC += 1;
                    return 0;
                }
            }

            /* G:2 commands - 1 operand */
            // "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
            else if (strcmp("clr", p) == 0 || strcmp("not", p) == 0 || strcmp("inc", p) == 0 || strcmp("jmp", p) == 0 || strcmp("bne", p) == 0 || strcmp("jsr", p) == 0 || strcmp("prn", p) == 0 || strcmp("red", p) == 0 || strcmp("dec", p) == 0)
            {
                p = strtok(NULL, delimiters); /*get the operande*/

                /*check the number of words in it's -> and also see what the method of addressing*/
                fw.op_code = opC;
                fw.dest_op_addr = checkAddressType(p, st); /*get the type of the addressing*/

                /* check the addressing type and if it's valied */
                /* "prn" --> 0,1,2,3*/

                /* "not, clr inc, dec, red" --> 1,2,3 */

                /* "jmp, bne, jsr" --> 1,3 */

                word = printFirstWordBinary(&fw);
                /* keep printing the words for the operand */

                /* count the number of code instruction */
                // CI += ?;

                /* check there is nothing after the word */
                p = strtok(NULL, delimiters);
                if (p == NULL)
                {
                    /*IC += ?; add to here the count needed...*/
                    return 0;
                }
                else
                {
                    printf("Error: %s operade allow only 1 after it.", p);
                }
            }

            /* G:1 commands - 2 operands */
            // "mov", "cmp", "add", "sub", "lea",
            else if (strcmp("hlt", p) == 0 || strcmp("rts", p) == 0 || strcmp("hlt", p) == 0 || strcmp("rts", p) == 0)
            {
            }
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
            printf("Error: %s isn't a valid word to use\n", p); /* Print each token */
            return -1;
        }

        p = strtok(NULL, delimiters); /* Get the next word-token */
        flag = NUM_OF_FLAG;
    }
    printf("\n");

    return 0;
}

/* this method check what kind of type address the operand have */
int checkAddressType(char *operand, SymbolTable *st)
{
    /* Addressing no 0: '00' - with # . it's can be number after of define value*/
    if (*operand == '#')
    {
        char *numberPart = operand + 1; // Skip the '#' to point to the next part
        printf("number = %s\n", numberPart);
        if (isInteger(numberPart))
        {
            printf("Is an integer ! addressing 0 \n");
            return 0;
        }
        /* check if it's an mdefine */
        else
        {
            int i = getSymbolValue(st, numberPart);
            if (i != -1 && strcmp(st->symbols[i].prop, "mdefine") == 0)
            {
                printf("It's an mdefine ! addressing 0 \n");
                return 0;
            }
        }
    }
    /* Addressing no 3: '11' - register addressing */
    else if (*operand == 'r' && isdigit(*(operand + 1)) && *(operand + 2) == '\0')
    {
        printf("Register operand: %s\n", operand);
        return 3;
    }
    /* Addressing no 2: '10' -  */
    else if (*(operand + strlen(operand) - 1) == ']')
    {

        const char *found = strchr(operand, '[');
        printf("Found: %c\n", *found);
        if (found != NULL)
        {
            int lableLen = found - operand;               /*index of the [*/
            char *label = (char *)malloc((lableLen) + 1); /*pointer of [*/
            int indexLen;
            char *index;

            strncpy(label, operand, lableLen);
            // label[lableLen] = '\0';

            if (label == NULL)
            {
                perror("Failed to allocate memory");
                return -1;
            }

            if (!checkLable(label)) /*check if it's an lable name*/
            {
                free(label);
                return -1; /* there is problem with the lables... */
            }

            indexLen = strlen(operand) - lableLen - 2;
            /* check the next word if it's a valid word */
            index = (char *)malloc(indexLen);

            if (index == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                return -1;
            }
            strncpy(index, found + 1, indexLen);
            printf("Index:::::: %s\n", index);
            if (!isdigit(index) || !strcmp(st->symbols[hasSymbol(st, index)].prop, "mdefine"))
            {
                printf("Error: %s ins't a digit or an .define", index);
                return -1;
            }
            
            /* check the index for being define or integer */
            /* checkes if it's an .define */
            int symbolIndex = hasSymbol(st, index);
            if (symbolIndex > -1 && !strcmp(st->symbols[symbolIndex].prop, "mdefine"))
            {
                printf("Error: %s isn't a .define", index);
                return -1;
            }
            else if (!isInteger(index))
            {
                printf("Error: %s isn't an integer", index);
                return -1;
            }

            free(label);
            free(index);

            return 2;
        }
    }
    // /* Addressing no 1: '01' - getting the from data LABLE .string, .data, .extern */
    // else if (){

    // }

    return -1; // Not starting with '#'
}

int main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    char newFileWord[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/word.text";
    startFirstProcess(outputFileName, newFileWord);

    return 0;
}
