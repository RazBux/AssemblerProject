#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "firstProcess.h"
#include "../utils/printBinary.h"
#include "../utils/util.h"

int checkAddressType(char *operand, SymbolTable *st);
int processLine(char *line, WordList *DC_table, WordList *IC_table, SymbolTable *st, int *DC, int *IC);
char *extract_brackets(const char *input, int part);
char *addressToBinatry(int addressType, char *p, SymbolTable *st, char addressC);

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
void startFirstProcess(char *asmblerOpenFile, WordList *DC_table, WordList *IC_table, SymbolTable *st, int DC, int IC, int *Flag)
{
    FILE *file = fopen(asmblerOpenFile, "r");
    char line[MAX_LINE_LENGTH];

    /*check if the file is opened*/
    if (!file)
    {
        perror("Open .am file - after open macro => failed");
        *Flag -= 1;
        return;
    }

    printf("\n--- STRT FIRST PROCESS ---\n");

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
    /*
    printf("\nIC:: %d\n", IC);
    printf("DC:: %d\n\n", DC);
    */

    fclose(file);

    printf("\n--- COMPLETE FIRST PROCESS ---\n\n");
}

/**
 * Processes a single line of assembler code.
 * @param line String containing the line to process.
 * @param DC_table Pointer to data code table for data definitions.
 * @param IC_table Pointer to instruction code table for instruction definitions.
 * @param st Pointer to the symbol table.
 * @param DC Pointer to the data counter.
 * @param IC Pointer to the instruction counter.
 * @return Integer status, 0 if successful, 1 if there is an error.
 *
 * This function process the line of the original Asembly code.
 * there are 5 possible line
 * 1: empty: all lite is white spaces or tabs
 * 2: comment: the first char of the line is ";"
 * 3: directive: tell the asembler what to do.
 *    direcrive line -does not- preduce a machine instruction binary output
 * 4: instruction: line that preduce a binary for the machine
 * 5: defining constent: creating a const variable

 * if there is an error - the function will return 1.
 * that's how the program will know to not preduce the files at the end.
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
        printf("Comment line: %s", line);
        return 0; /* Return immediately if the line is a comment */
    }

    /* connect ',' to be w.o spaces between them, before:"6 , -9 , len" after:"6,-9,len" */
    normalizeString(line);
    printf("\nline >>> %s", line);
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
        else if (*(p + pLen - 1) == ':' && pLen < MAX_LEN_NAME)
        {
            char *label;

            label = (char *)malloc(pLen); /*Allocate memory for label, including space for null terminator*/
            if (label != NULL)
            { /*if the alloction is was successful*/
                /*strncpy(label, p, pLen - 1);*/
                strcpy(label, p);
                label[strlen(p) - 1] = '\0';
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
                if (p == NULL)
                {
                    printf("Error: there is no word after lable\n");
                    return -1;
                }

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
                        SymbolNode *sn = getSymbol(st,p); 
                        int num = sn->val; /* if there is symbol - retunrn it's index */
                        if (num == -1)
                        {
                            printf("Error: %s is invalid type of data = %d\n", p, num);
                            return -1;
                        }

                        word = BinaryString14(sn->val);
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
                char *e = strcmp(p, ".extern") == 0 ? "external" : "entry";
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
                        printf("New %s => %s. ", e, p);
                        addSymbol(st, p, e, 0);
                    }
                    else
                        return -1;
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
                /*printf("fw binay: %s\n", word);*/
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
                    /* printf("Error: exit line because of error in address type\n"); */
                    return -1;
                }
                printf("operand %s addressType: %d\n", p, addressType);

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
                /* printf("code counter == %d\n", *IC); */

                if (addressType == 2) /*type of y[x]*/
                {
                    char *first_arg = extract_brackets(p, 1);
                    char *second_arg = extract_brackets(p, 2);
                    Number sNum = {0}; /*Using this to extract the number of the second arg*/
                    /*SymbolNode *sn = getSymbol(st,second_arg);*/
                    
                    int sVal;

                    *IC += 2;

                    /* printf("F1:%s, S2:%s\n", first_arg, second_arg); */

                    /*for the secode arg -> check if it's a number or mdefine and convert to binary*/
                    sVal = isInteger(second_arg) ? atoi(second_arg) : getSymbol(st,second_arg)->val;
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

                /* check if the memoty was allocat */
                if (opName == NULL)
                {
                    printf("Memoty allocatuin fail");
                    exit(1);
                }
                strcpy(opName, p);

                /* --- first operand --- */
                p = strtok(NULL, ","); /*get the first operande*/
                if (p == NULL)
                {
                    printf("Error: Operand missing or no comma separator found\n");
                    return -1;
                }
                firstOp = (char *)malloc(strlen(p) + 1);
                if (firstOp == NULL)
                {
                    printf("Error: Memory allocation failed for first operand.\n");
                    free(opName);
                    exit(1);
                }
                strcpy(firstOp, p);

                /* --- second operand --- */
                p = strtok(NULL, " \n"); /*get the second operande*/
                if (p == NULL)
                {
                    printf("Error: Operand missing or no comma separator found\n");
                    return -1;
                }

                secondOp = (char *)malloc(strlen(p) + 1);
                if (secondOp == NULL)
                {
                    printf("Error: Memory allocation failed for first operand.\n");
                    free(opName);
                    free(firstOp);
                    exit(1);
                }
                strcpy(secondOp, p);

                /* check for address types */
                sourceAddressType = checkAddressType(firstOp, st);
                destAddressType = checkAddressType(secondOp, st);
                if (destAddressType == -1 || sourceAddressType == -1)
                {
                    /* printf("Error: exit line because of error in address type\n"); */
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
                        /* printf("F1:%s, S2:%s\n", first_arg, second_arg); */

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
                        /* printf("F1:%s, S2:%s\n", first_arg, second_arg); */

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

                free(opName);
                free(firstOp);
                free(secondOp);
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

/**
 * Checks the type of address for the given operand.
 * @param operand The operand string to analyze.
 * @param st Pointer to the symbol table.
 * @return Integer representing the address type.
 *
 * '00' -> direct addressing # with number of define: #(x)
 * '01' -> valid lable name: LABLE
 * '10' -> index with number or define: y[x]
 * '11' -> register types: r(x)
 */
int checkAddressType(char *operand, SymbolTable *st)
{
    /* Addressing no 0: '00' - with #. It can be a number or a defined value */
    if (*operand == '#')
    {
        char *numberPart = operand + 1; /* Skip the '#' to point to the next part */ 
        if (isInteger(numberPart))
        {
            return 0; /* It's a number, direct addressing */ 
        }
        else
        {
            SymbolNode *node = getSymbol(st, numberPart);
            if (node != NULL && strcmp(node->prop, "mdefine") == 0)
            {
                return 0; /* It's a defined macro value */
            }
        }
    }

    /* Addressing no 3: '11' - register addressing */
    else if (*operand == 'r' && isdigit(*(operand + 1)) && *(operand + 2) == '\0')
    {
        int reg = atoi(operand + 1); /* Convert the register number to integer */ 
        if (reg >= 0 && reg < 8)
        {
            return 3; /* Valid register addressing */ 
        }
        else
        {
            printf("Error: Register numbers are allowed only from 0 to 7.\n");
            return -1;
        }
    }

    /* Addressing no 2: '10' - with label[index/define] 
        --> in the seconde run we schold check if the symbol exsist*/
    else if (*(operand + strlen(operand) - 1) == ']')
    {
        const char *firstBrack = strchr(operand, '[');
        if (firstBrack != NULL)
        {
            const char *secondBrack = strchr(operand, ']');

            int firstBracketIdx = firstBrack - operand;
            int secondBracketIdx = secondBrack - operand;

            char *lab_name = (char *)malloc(firstBracketIdx + 1);
            char *lab_number = (char *)malloc(secondBracketIdx - firstBracketIdx);

            if (secondBrack == NULL)
            {
                printf("Error: Mismatched brackets\n");
                return -1;
            }

            if (lab_name == NULL || lab_number == NULL)
            {
                perror("Failed to allocate memory");
                exit(1);
            }

            strncpy(lab_name, operand, firstBracketIdx);
            lab_name[firstBracketIdx] = '\0'; /*Null-terminate the string*/
            /*printf("LAB.NAME >> %s", lab_name);*/

            strncpy(lab_number, firstBrack + 1, secondBracketIdx - firstBracketIdx - 1);
            lab_number[secondBracketIdx - firstBracketIdx - 1] = '\0'; /*Null-terminate the string*/
            /* printf("  LAB.NUMBER >> %s\n", lab_number); */

            if (!isValidLable(lab_name)) /*Check if it's a valid label name*/
            {
                free(lab_name);
                free(lab_number);
                printf("Error: %s invalid a LABEL\n", lab_name);
                return -1;
            }

            if (getSymbol(st, lab_number) != NULL)
            {
                if (strcmp(getSymbol(st, lab_number)->prop, "mdefine") != 0)
                {
                    printf("Error: %s isn't a .define\n", lab_number);
                    free(lab_name);
                    free(lab_number);
                    return -1;
                }
            }
            else if (!isInteger(lab_number))
            {
                printf("Error: %s isn't an integer\n", lab_number);
                free(lab_name);
                free(lab_number);
                return -1;
            }

            free(lab_name);
            free(lab_number);

            return 2;
        }
        else
        {
            printf("Error: No closing bracket were found\n");
            return -1;
        }
    }

    /* Addressing no 1: '01' - direct label addressing */
    else if (isValidLable(operand))
    {
        return 1; /* Label exists in the symbol table */ 
        /*if (getSymbol(st, operand) != NULL)
        {
        }
        */
    }

    printf("Error: Invalid type of addressing for operand '%s'.\n", operand);
    return -1; /* invalid addressing type */
}

/**
 * Extracts content enclosed by brackets based on the specified part.
 * @param input The string input containing brackets.
 * @param part The part of the bracket content to extract (1 or 2).
 * @return Dynamically allocated string of the extracted content.
 *
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

/**
 * Converts an address to a binary representation based on the address type.
 * @param addressType Type of address (0, 1, 2, or 3).
 * @param p Pointer to the operand string.
 * @param st Pointer to the symbol table.
 * @param addressC Character indicating the address code ('s' for source, 'd' for destination).
 * @return Dynamically allocated string representing the binary code.
 */
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

        sVal = isInteger(p) ? atoi(p) : getSymbol(st,p)->val;
        
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
