#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../globVal/glob_val.h"

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
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL )
    {
        char *p;
        char delimiters[] = " \t\n";

        p = strtok(line, delimiters);
        /* Tokenize the line */
        while (p != NULL )
        {   
            printf("Token: %s\n", p);     /* Print each token */
            p = strtok(NULL, delimiters); /* Get the next token */
        }
        printf("\n");
    }

    return 0;
}

int main(void)
{
    char outputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/AsmblerProject/preAsmbler/textFiles/m.am";
    startFirstProcess(outputFileName);

    return 0;
}

