#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "util.h"
#include "printBinary.h"
#include "dataCodeTable.h"

/*
#include "secondProcess.h"
#include "../encryption/encryption.h"
#include "../preAsmbler/preAsmbler.h"
*/

int checkAddressType(char *operand, SymbolTable *st);
int processLine(char *, WordList *, WordList *, SymbolTable *, int *, int *);
int startFirstProcess(char *, WordList *, WordList *, SymbolTable *, int , int, int *);
char *extract_brackets(const char *, int);
char *addressToBinatry(int addressType, char *p, SymbolTable *st, char addressC);

int startFirstProcess(char *asmblerOpenFile, WordList *DC_table, WordList *IC_table, SymbolTable *st, int DC, int IC, int *Flag)
{
    FILE *file = fopen(asmblerOpenFile, "r");
    char line[MAX_LINE_LENGTH];

    /*check if the file is opened*/
    if (!file)
    {
        perror("Open .am file - after open macro => failed");
        return -1;
    }

    /*start processing line by line*/
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        *Flag += processLine(line, DC_table, IC_table, st, &DC, &IC);
    }

    /* go over the SymbolTable and add 100 to each 'code' and IC+100 to 'data' */
    icdcSymbolTable(st, IC);

    printf("\nprint symbol table:::\n");
    printSymbols(st);
    
    /* status of IC and DC */
    printf("IC:: %d\n", IC);
    printf("DC:: %d\n", DC);

    fclose(file);

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
int processLine(char *line, WordList *DC_table, WordList *IC_table, SymbolTable *st, int *DC, int *IC)
{
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
        else if (*(p + pLen - 1) == ':' && pLen < MAX_LABLE_NAME)
        {
            char *label;

            label = (char *)malloc(pLen); /*Allocate memory for label, including space for null terminator*/
            if (label != NULL)
            { /*if the alloction is was successful*/
                strncpy(label, p, pLen - 1);
                printf("LABLE: %s > ", label);
            }
            /* check for valid lable name */
            if (!isValidLable(label))
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
                    addSymbol(st, label, "data", *DC);
                }
                else if (isOpCode(p) == 0)
                {
                    addSymbol(st, label, "code", *IC);
                }
                /* the entry and extern can come after lable ?? */
                else if (*p == '.' && (strcmp(p, ".entry") == 0 || strcmp(p, ".extern") == 0))
                {
                    printf("Error: it's cannot be...");
                    return -1;
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
            free(label); /*free the lable memory*/
        }

        /* After checking if there is LABEL and add it to the symbol table
            this part of code will ganarate the machine code of the asmbler */
        /* if it's a directive line */
        if (*p == '.')
        {
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
                        int num = getSymbolIndex(st, p); /* if there is symbol - retunrn it's index */
                        if (num == -1)
                        {
                            printf("Error: %s is invalid type of data = %d\n", p, num);
                            return -1;
                        }

                        word = BinaryString14(st->symbols[num].val);
                    }

                    addWord(DC_table, word); /*data table adding words*/
                    *DC += 1;

                    countData++;
                    p = strtok(NULL, ", \n"); /*get the next value*/
                }
                if (countData == 0)
                {
                    printf("ERROR: there is no data after the .data\n");
                    return -1;
                }
            }
            else if (strcmp(p, ".string") == 0)
            {
                p = strtok(NULL, delimiters);
                printf("STRING: %s\n", p);
                /* after a .string the next word need to be - "somting" */
                if (*p == '\"' && *(p + strlen(p) - 1) == '\"')
                {
                    size_t s;
                    /* Iterate from the second character to the second-to-last character */
                    for (s = 1; s < strlen(p) - 1; s++)
                    {
                        printf("%c, ", p[s]);
                        addWord(DC_table, BinaryString14(p[s]));
                        *DC += 1;
                    }
                    addWord(DC_table, BinaryString14('\0'));
                    *DC += 1; /* add 1 number for each charachter and 1 for the null */

                    p = strtok(NULL, delimiters);
                    if (p == NULL)
                        return 0;
                    else
                    {
                        printf("Error: no word should be after \"x\" of the string");
                        return -1;
                    }
                }
                else
                {
                    printf("Error: no \" at the start or finish of .string for %s\"\n", p);
                    return -1;
                }
            }
            /* the asmbler need to ignore this line and will print worning massage */
            else if (strcmp(p, ".extern") == 0 || strcmp(p, ".entry") == 0)
            {
                char* e = strcmp(p, ".extern") == 0 ? "external" : "entry";
                p = strtok(NULL, ", \n");
                if (p == NULL)
                {
                    printf("Error: no argument after .entry");
                    return -1;
                }

                while (p != NULL)
                {
                    if (!hasSymbol_exen(st, p, e))
                    {
                        printf("New %s => %s. ",e ,p);
                        addSymbol(st, p, e, 0);
                    }
                    else return -1;
                    p = strtok(NULL, ", \n");
                }

                return 0; /*use this to add it to the symbol table and the TableData*/
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
                word = getFirstWordBinary(&fw);
                printf("fw binay: %s\n", word);
                addWord(IC_table, word);
                *IC += 1;

                p = strtok(NULL, delimiters);
                /*there is no word after the operand*/
                if (p == NULL)
                    return 0;
                else
                {
                    printf("Error: invalid words after operand");
                    return -1;
                }
            }

            /* G:2 commands - 1 operand */
            /* "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", */ 
            else if (strcmp("clr", p) == 0 || strcmp("not", p) == 0 || strcmp("inc", p) == 0 || strcmp("jmp", p) == 0 || strcmp("bne", p) == 0 || strcmp("jsr", p) == 0 || strcmp("prn", p) == 0 || strcmp("red", p) == 0 || strcmp("dec", p) == 0)
            {
                int addressType;
                char *opName = (char *)malloc(strlen(p) + 1);
                strcpy(opName, p);

                p = strtok(NULL, delimiters); /*get the operande*/

                addressType = checkAddressType(p, st);
                if (addressType == -1)
                {
                    printf("Error: exit line because of error in address type\n");
                    return -1;
                }
                printf("operand %s addressType: %d\n\n", p, addressType);

                /* check the addressing type and if it's valied */
                /* "prn" --> 0,1,2,3 : need no check because it's can be any thing */

                /* "not, clr inc, dec, red" --> 1,2,3 */
                if ((strcmp("not", opName) == 0 || strcmp("clr", opName) == 0 || strcmp("inc", opName) == 0 || strcmp("dec", opName) == 0 || strcmp("red", opName) == 0) && addressType == 0)
                {
                    printf("Error: the address type of %d isn't valid for this op code: %s\n", addressType, opName);
                    return -1;
                }
                /* "jmp, bne, jsr" --> 1,3 */
                else if ((strcmp("jmp", opName) == 0 || strcmp("bne", opName) == 0 || strcmp("jsr", opName) == 0) && (addressType == 0 || addressType == 2))
                {
                    printf("Error: the address type of %d isn't valid for this op code: %s\n", addressType, opName);
                    return -1;
                }

                fw.op_code = opC;
                fw.dest_op_addr = addressType;
                word = getFirstWordBinary(&fw);
                addWord(IC_table, word); /* dont forget to add the addressing type for those words*/
                *IC += 1;
                printf("code counter == %d\n", *IC);

                if (addressType == 2) /*type of y[x]*/
                {
                    char *first_arg = extract_brackets(p, 1);
                    char *second_arg = extract_brackets(p, 2);
                    Number sNum = {0}; /*Using this to extract the number of the second arg*/
                    int sVal;

                    *IC += 2;

                    printf("F1:%s, S2:%s\n", first_arg, second_arg);

                    /*for the secode arg -> check if it's a number or mdefine and convert to binary*/
                    sVal = isInteger(second_arg) ? atoi(second_arg) : st->symbols[getSymbolIndex(st, second_arg)].val;
                    sNum.number = sVal;

                    /*add the word to IC_table*/
                    addWord(IC_table, first_arg);
                    addWord(IC_table, getNumberBinary(&sNum));

                    free(first_arg);
                    free(second_arg);
                }
                else
                { /*if it 0,1,3 addressType*/
                    char *word_operand = addressToBinatry(addressType, p, st, 'd');
                    addWord(IC_table, word_operand);
                    *IC += 1;
                }

                /* check there is nothing after the word */
                p = strtok(NULL, delimiters);
                if (p == NULL)
                    return 0;
                else
                {
                    printf("Error: %s operade allow only 1 after it.", p);
                    return -1;
                }

                free(opName);
            }

            /* G:1 commands - 2 operands */
            /* "mov", "cmp", "add", "sub", "lea" */
            else if (strcmp("mov", p) == 0 || strcmp("cmp", p) == 0 || strcmp("add", p) == 0 || strcmp("sub", p) == 0 || strcmp("lea", p) == 0)
            {
                int destAddressType;
                int sourceAddressType;
                char *firstOp;
                char *secondOp;
                char *opName = (char *)malloc(strlen(p) + 1);

                strcpy(opName, p);

                p = strtok(NULL, ", \n"); /*get the first operande*/
                firstOp = (char *)malloc(strlen(p) + 1);
                strcpy(firstOp, p);

                p = strtok(NULL, ", \n"); /*get the second operande*/
                secondOp = (char *)malloc(strlen(p) + 1);
                strcpy(secondOp, p);

                /* check for address types */
                sourceAddressType = checkAddressType(firstOp, st);
                destAddressType = checkAddressType(secondOp, st);
                if (destAddressType == -1 || sourceAddressType == -1)
                {
                    printf("Error: exit line because of error in address type\n");
                    return -1;
                }

                printf("firstOp %s addressType: %d\n", firstOp, destAddressType);
                printf("secondOp %s addressType: %d\n", secondOp, sourceAddressType);

                /* check for valid addressing type S == surce, D == dest */
                /* cmp - S&D: 0,1,2,3 there is no need to check */

                /* add,sub,mov - S: 0,1,2,3 | D: 1,2,3  .check if dest if 0 -> invalid*/
                if ((strcmp("add", opName) == 0 || strcmp("sub", opName) == 0 || strcmp("mov", opName) == 0) && destAddressType == 0)
                {
                    printf("Error: the address type of %d for 'dest' isn't valid for this op code: %s\n", destAddressType, opName);
                    return -1;
                }
                /* lea - S: 1,2 | D: 1,2,3 */
                else if (strcmp("lea", opName) == 0 && (sourceAddressType == 0 || sourceAddressType == 3 || destAddressType == 0))
                {
                    printf("Error: address type of isn't valid for this op code: %s\n", opName);
                    return -1;
                }

                /*add the firstWord to IC_table*/
                fw.op_code = opC;
                fw.dest_op_addr = destAddressType;
                fw.src_op_addr = sourceAddressType;
                word = getFirstWordBinary(&fw);
                addWord(IC_table, word);
                *IC += 1;
                if (destAddressType == 3 && sourceAddressType == 3)
                {
                    /*if both registers addressType - it's only 1 world
                     * from 2-4 the surce register and from 5-7 the dest register
                     */
                    RegNumber reg_num = {0};
                    /*convert registers to integer*/
                    int firstReg = atoi(firstOp + 1);
                    int secondReg = atoi(secondOp + 1);

                    reg_num.source_reg = firstReg;
                    reg_num.dest_reg = secondReg;

                    addWord(IC_table, getRegNumberBinary(&reg_num));
                    *IC += 1;
                }
                else
                {
                    if (sourceAddressType == 2)
                    {
                        char *first_arg = extract_brackets(firstOp, 1);
                        char *second_arg = extract_brackets(firstOp, 2);
                        char *bin_second = addressToBinatry(0, second_arg, st, '.');

                        *IC += 2;
                        printf("F1:%s, S2:%s\n", first_arg, second_arg);

                        /*add the word to IC_table*/
                        addWord(IC_table, first_arg);
                        addWord(IC_table, bin_second);
                        free(first_arg);
                        free(second_arg);
                    }
                    else
                    {
                        char *fWord_bin = addressToBinatry(sourceAddressType, firstOp, st, 's');
                        addWord(IC_table, fWord_bin);
                        *IC += 1;
                    }

                    if (destAddressType == 2)
                    {
                        char *first_arg = extract_brackets(secondOp, 1);
                        char *second_arg = extract_brackets(secondOp, 2);
                        char *bin_second = addressToBinatry(0, second_arg, st, '.');

                        *IC += 2;
                        printf("F1:%s, S2:%s\n", first_arg, second_arg);

                        /*add the word to IC_table*/
                        addWord(IC_table, first_arg);
                        addWord(IC_table, bin_second);
                        free(first_arg);
                        free(second_arg);
                    }
                    else
                    {
                        char *sWord_bin = addressToBinatry(destAddressType, secondOp, st, 'd');
                        addWord(IC_table, sWord_bin);
                        *IC += 1;
                    }
                }

                /* check there is nothing after the word */
                p = strtok(NULL, delimiters);
                if (p == NULL)
                    return 0;
                else
                {
                    printf("Error: %s operade allow only 1 after it.", p);
                    return -1;
                }
            }
        }
        else
        {
            printf("Error: %s isn't a valid word to use\n", p); /* Print each token */
            return -1;
        }
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
        char *numberPart = operand + 1; /* Skip the '#' to point to the next part */ 
        /*printf("number = %s\n", numberPart);*/
        if (isInteger(numberPart))
        {
            /*printf("Is an integer ! addressing 0 \n");*/
            return 0;
        }
        /* check if it's an mdefine */
        else
        {
            int i = getSymbolIndex(st, numberPart);
            if (i != -1 && strcmp(st->symbols[i].prop, "mdefine") == 0)
            {
                /*printf("It's an mdefine ! addressing 0 \n");*/
                return 0;
            }
        }
    }
    /* Addressing no 3: '11' - register addressing */
    else if (*operand == 'r' && isdigit(*(operand + 1)) && *(operand + 2) == '\0')
    {
        /*passing the 'r'*/
        int reg = atoi(operand + 1); /*convert the register number to integer*/
        if (reg < 8 && reg >= 0)     /*check if the register number is valid*/
            return 3;
        else
        {
            printf("Error registers are allowed only from 0 - 8");
            return -1;
        }
    }
    /* Addressing no 2: '10' - with lable[int/define]
        ---> in the seconde run we schold check if the symbol exsist */
    else if (*(operand + strlen(operand) - 1) == ']')
    {

        const char *found = strchr(operand, '[');
        if (found != NULL)
        {
            int lableLen = found - operand;               /*index of the [*/
            char *label = (char *)malloc((lableLen) + 1); /*pointer of [*/
            char *index;
            int indexLen;
            int checkStDefine;

            strncpy(label, operand, lableLen);
            /* label[lableLen] = '\0'; */ 

            if (label == NULL)
            {
                perror("Failed to allocate memory");
                exit(1);
            }

            if (!isValidLable(label)) /*check if it's an lable name*/
            {
                free(label);
                printf("Error: invalid LABLE");
                return -1; /* there is problem with the lables... */
            }

            indexLen = strlen(operand) - lableLen - 2;
            /* check the next word if it's a valid word */
            index = (char *)malloc(indexLen);

            if (index == NULL)
            {
                printf("Memory allocation failed\n");
                exit(1);
            }
            strncpy(index, found + 1, indexLen);

            checkStDefine = getSymbolIndex(st, index);
            if (checkStDefine > -1)
            {
                if (strcmp(st->symbols[checkStDefine].prop, "mdefine") != 0)
                {
                    printf("Error: %s isn't a .define\n", index);
                    return -1;
                }
            }
            else if (!isInteger(index))
            {
                printf("Error: %s isn't an integer\n", index);
                return -1;
            }

            free(label);
            free(index);

            return 2;
        }
    }
    /* Addressing no 1: '01' - getting the from data LABLE .string, .data, .extern */
    /* soft check if it's can be a valid lable */
    else if (isValidLable(operand))
    {
        printf("%s can be a valid lable\n", operand);
        return 1;
    }

    /* if it not one of the addressing type - return -1 = invalid type*/
    printf("Error: invalid type of addressing\n");
    return -1; /* invalid addressing type */ 
}

/*
 * Function to extract contents based on part:
 * part = 1 for the content inside the first level of brackets.
 * part = 2 for the content inside the second level of brackets.
 * Returns a dynamically allocated string that must be freed by the caller.
 * Need to free after use..
 */
char *extract_brackets(const char *input, int part)
{
    const char *start, *end;
    char *output = NULL;

    /* Find the first opening bracket */
    start = strchr(input, '[');
    if (start == NULL)
    {
        printf("No opening bracket found.\n");
        return NULL;
    }

    /* Find the corresponding closing bracket */
    end = strchr(start, ']');
    if (end == NULL)
    {
        printf("No closing bracket found.\n");
        return NULL;
    }

    if (part == 1)
    {
        /* Allocate memory to extract everything before the first '[' */
        output = (char *)malloc(start - input + 1);
        if (output != NULL)
        {
            strncpy(output, input, start - input);
            output[start - input] = '\0'; /* Null-terminate the result */
        }
        else
        {
            printf("Failed to allocate memory\n");
            exit(1);
        }
    }
    else if (part == 2)
    {
        /* Allocate memory and extract everything up to the first ']' after the '[' */
        output = (char *)malloc(end - start);
        if (output != NULL)
        {
            strncpy(output, start + 1, end - start - 1);
            output[end - start - 1] = '\0'; /* Null-terminate the result */
        }
        else
        {
            printf("Failed to allocate memory\n");
            exit(1);
        }
    }

    return output; /* Return the dynamically allocated substring */
}

/*get the address binary from world with addressType*/
char *addressToBinatry(int addressType, char *p, SymbolTable *st, char addressC)
{
    if (addressType == 0) /* meaning the arg starts with #*/
    {
        int sVal;
        Number sNum = {0};
        if (*p == '#')
        {
            p++; /*passing on #*/
        }

        sVal = isInteger(p) ? atoi(p) : st->symbols[getSymbolIndex(st, p)].val;
        sNum.number = sVal;
        return getNumberBinary(&sNum);
    }
    else if (addressType == 3) /*if it's a register*/
    {
        RegNumber regNum = {0}; /*using the first word to get the 'dest' holding the register number*/
        int rVal = atoi(p + 1);
        if (addressC == 'd')
        {
            regNum.dest_reg = rVal;
        }
        else if (addressC == 's')
        {
            regNum.source_reg = rVal;
        }

        return getRegNumberBinary(&regNum);
    }
    else if (addressType == 1)
    {
        return p;
    }
    else
    {
        printf("!!! Only 0, 1 or 3 address type is valid !!!");
        return NULL;
    }

}

/*
int checkInHouse_main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    

    WordList IC_table = {NULL, 0};
    WordList DC_table = {NULL, 0};
    

    WordList extF = {NULL, 0};
    WordList entF = {NULL, 0};

    int DC = 0; 
    int IC = 0; 
    int Flag = 0; 
    int E = 100; 

    SymbolTable st;
    initSymbolTable(&st);

    startFirstProcess(outputFileName, &DC_table, &IC_table, &st, DC, IC, &Flag);

    printf("DC_WordList >> ");
    printWordListReverse(&DC_table);

    printf("\nIC_WordList >> ");
    printWordListReverse(&IC_table);

    startSecondProcess(&DC_table, &IC_table, &entF, &extF, &st, &Flag);
    
    printf("DC >> ");
    printWordListReverse(&DC_table);

    printf("\nIC >> ");
    printWordListReverse(&IC_table);

    printf("\nExtern >> ");
    printWordList(&extF);

    printf("\nEntry >> ");
    printWordList(&entF);
    
    if (Flag != 0){
        printf("There were %d number of error >> the program won't create the files", Flag);
    }
    else {
        printf("Complete processing >> creating files now.\n");

        printf("  %d  %d\n", IC_table.count, DC_table.count);
        printEncryptionReverse(&IC_table, &E);
        printEncryptionReverse(&DC_table, &E);
        
    }


    return 0;
}
*/
