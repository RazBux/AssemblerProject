#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "util.h"
#include "../globVal/glob_val.h"

int processLine(char *, int, int, SymbolTable *);
int startFirstProcess(char *);
int checkIfValidName(char *);
int addToSymbolTable(char*, char*, int);

int startFirstProcess(char *asmblerOpenFile)
{
    FILE *file = fopen(asmblerOpenFile, "r");
    char line[MAX_LINE_LENGTH];
    /* creaet counters for tracking the code and instruction */
    int DC = 0; /* Data counter */
    int IC = 0; /* Instruction counter */
    /* create symbol table */ 
    SymbolTable st;
    initSymbolTable(&st);  /* Initialize the symbol table */ 

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


    p = strtok(line, delimiters);

    /* Tokenize the line */
    while (p != NULL)
    {
        pLen = strlen(p);

        /* Lable check:
            if there is a lable in the first word,
            by see if the last char is ":" */
        if (flag == START && *(p + pLen - 1) == ':' && pLen < MAX_LABLE_NAME)
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
                printf("LABLE: %s\n", label);
            }
            if (checkIfValidName(label) == 0) /* if return 0 - it's valid lable name - add to lable matrix */
            {
                printf("Valid lable name -> add \"%s\" to lable matrix\n", label);
                /*
                    addLableToMetrix(label);
                */
            }
            free(label);  /*free the lable memory*/
            flag = LABEL; /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
        }

        /* if it's a directive line */
        else if (*p == '.')
        {
            if (flag == LABEL){
                printf("data:: ");
            }
            printf("DIR: %s\n", p);
            /* check witch of the cases it is */
            if (strcmp(p, ".data") == 0)
            {
                int countData = 0;
                /* use while loop to exstract all the data into array */
                while (p != NULL)
                {
                    p = strtok(NULL, delimiters);
                    if (p == NULL) /*meaning there is no other word after the .data*/
                    {
                        if (countData == 0)
                        {
                            printf("ERROR: there is no data after the .data\n");
                        }
                    }
                    else
                    {
                        printf("DATA: %s\n", p);
                        countData++;
                    }
                }

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
                }
            }
            /* the asmbler need to ignore this line and will print worning massage */
            else if (strcmp(p, ".entry") == 0 || strcmp(p, ".extern") == 0)
            {
                printf("Worning: %s ", p);
                return 0;
            }
            else if (strcmp(p, ".define") == 0){
                /* in case of define -> should be 'name' = 'number' and the name need to be in the sembol table */
                int number;
                char* defineName; 
                
                p = strtok(NULL, delimiters);
                
                /*check if the name is a uniqe name*/
                if(checkIfValidName(p) == 0){
                    /*allocate memory to the name of the define*/
                    defineName = (char *)malloc(strlen(p));
                    strcpy(defineName, p);
                    
                    /*see if the next value is '='*/
                    p = strtok(NULL, delimiters);
                    if (!(strlen(p) == 1 && *p == '=')){
                        printf("%s isn't a '=' as sould be when defining a constent\n", p);
                    }

                    /*check if it's a number*/
                    p = strtok(NULL, delimiters);
                    number = atoi(p); /*converting string to int number*/
                    if(!(number >= INT_MIN && number <= INT_MAX)){
                        printf("%s isn't int, pls use number when defining a constent\n",p);
                    }

                    printf("mdefine: %s = %d\n", defineName, number);
                    addSymbol(st,defineName,"mdefine",number);
                    free(defineName);
                }
                else{
                    printf("'%s' is alredy defined pls use another name insted\n", p);
                }
            }
            /* if it is not a valid directive name -> error */
            else
            {
                printf("Error: this directive command doesn't exsist!\n");
            }
        }
        else if (isOpCode(p) == 0)
        {
            if (flag == LABEL){
                printf("code:: ");
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
        /*add here in cases where there is lables or defining that used.*/
        /*else if(){

        }*/
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

/* Function to check if the new lable is already assige before. if so, it is an error*/
int checkIfValidName(char *newLableName)
{
    /*check if it's no a define or lable - p39*/
    return 0;
    /*add here the checking*/
    if (newLableName == NULL)
    {

        printf("error: There is all ready lable with this name");
    }

    return 0;
}

int addToSymbolTable(char* defineName,char* prop, int number){
    /*add the define the the table of symbols*/

    return 0;
}


int main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    startFirstProcess(outputFileName);

    return 0;
}
