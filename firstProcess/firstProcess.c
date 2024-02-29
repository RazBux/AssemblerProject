#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../globVal/glob_val.h"

int processLine(char *);
int startFirstProcess(char *);
int checkForSameLableName(char *);

int startFirstProcess(char *asmblerOpenFile)
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

        processLine(line);
    }

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
*/
int processLine(char *line)
{
    int labelFlag = 0; /*maybe using enum ? for multiful flags ?*/
    enum Flag flag = START;
    size_t pLen; /* for checking the if it's a lable */

    int firstCharIndex;
    char *p;
    char delimiters[] = " \t\n";
    /*1: check if it is an empty line*/
    if (line[0] == '\0' || strspn(line, " \t\n") == strlen(line))
    {
        printf("Empty or whitespace-only line\n");
        return 0; /* Return immediately if the line is empty */
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
        printf("Token: %s\n", p); /* Print each token */
        pLen = strlen(p);

        /* Lable check:
            if there is a lable in the first word,
            by see if the last char is ":" - is so, check */
        if (START && *(p + pLen - 1) == ':' && pLen < MAX_LABLE_NAME)
        {
            /*if the first char of the lable isnt a char - raise an error/flag*/
            if (!isalpha(*(p))){
                printf("Error while defining a lable - the first character of %s isn't 'alpha batic'\n", p);
                return -1;
            }
                
            char *label = (char *)malloc(pLen - 1); /*Allocate memory for label, including space for null terminator*/
            if (label != NULL)
            { /*if the alloction is was successful*/
                strncpy(label, p, pLen - 1);
                printf("there is a lable here> %s\n", label);
            }
            if (checkForSameLableName(label) == 0) /* if return 0 - it's valid lable name - add to lable matrix */
            {
                printf("Valid lable name -> add it to the lable matrix\n");
                /*
                    addLableToMetrix(label);
                */
            }
            free(label); /*free the lable memory*/
            flag = LABEL; /* the next work should be or 'op_code' or instruction like '.data, .string, .entry, .extern' */
        }   
        


        p = strtok(NULL, delimiters); /* Get the next token */
    }
    printf("\n");

    return 0;
}

/* Function to check if the new lable is already assige before. if so, it is an error*/
int checkForSameLableName(char *newLableName)
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

int main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    startFirstProcess(outputFileName);

    return 0;
}
