#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum State
{
    Read,
    In_macro,
    End_macro,
    Finish
};

char *get_macro_name(char line[]);
int check_state(char *line, int state);

int main(void)
{
    enum State state = Read;

    /*using dynamic allocation for macro_name and macro_data*/
    char **macro_names = NULL;
    char **macro_data = NULL; /* see if need 2 starts */
    char *new_name = NULL;
    size_t macro_count = 0;
    int i;

    char line[80] = {0}; /*dont forget to check if the size of the line is bigger then 80*/

    FILE *file;
    /*dont forget to change this to a < input.txt at the end..*/
    file = fopen("/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/cprog/AsmblerProject/m.txt", "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    /*for reading from a < input..*/
    while (fgets(line, sizeof(line), file) != NULL)
    {
        state = check_state(line, state);
        switch (state)
        {
        case Read:
            printf("%s", line);
            break;

        case In_macro:
            if (strstr(line, "mcr ") != NULL)
            {
                new_name = get_macro_name(line);
                if (new_name == NULL)
                {
                    printf("No memory");
                    return -1;
                }
                macro_names = realloc(macro_names, (macro_count + 1) * sizeof(char *));
                if (macro_names == NULL)
                {
                    printf("No memory");
                    free(new_name);
                    return -1;
                }
                macro_names[macro_count] = new_name;
                macro_count++;
            }
            /*If the line we read contains the end macro signature*/
            if (strstr(line, " endmcr") != NULL)
            {
                state = End_macro;
            }

            if (macro_data == NULL)
            {
                macro_data = (char *)malloc(strlen(line + 1));
                if (macro_data == NULL)
                {
                    printf("No memory");
                    return -1;
                }
                strcpy(*macro_data, line);
            }
            else
            {
                *macro_data = (char *)realloc(macro_data, strlen(*macro_data) + strlen(line) + 1);
                if (macro_data == NULL)
                {
                    printf("No memory");
                    return -1;
                }
                strcat(*macro_data, line);
            }

            break;

        case End_macro:

            break;
        default:
            break;
        }

        free(new_name);
        free(macro_names);
        free(macro_data);
    }

    printf("\nMacro data:::\n");
    printf("%s", *macro_data);

    printf("\nMacro names:::\n");
    for (i = 0; i < macro_count; i++)
    {

        printf("%s", *macro_names);
    }

    return 0;
}

/* This function */
int check_state(char *line, int state)
{
    /*check if it is a start of a macro*/
    if (state == Read && strstr(line, "mcr ") != NULL)
    {
        return In_macro;
    }

    /*If in macro and see the "endmcr"*/
    if (state == In_macro)
    {
        return In_macro;
    }
    /*if it isn't one of them - still in read*/
    return Read;
}

/* To get the macro name */
char *get_macro_name(char *line)
{
    char *p;
    char delimiter[] = " ";
    char *macro_name;

    p = strtok(line, delimiter);
    while (p != NULL)
    {
        if (strcmp(p, "mcr") == 0)
        {
            p = strtok(NULL, delimiter);
            if (p != NULL)
            {
                macro_name = malloc(strlen(p) + 1);
                if (macro_name == NULL)
                {
                    return NULL;
                }
                strcpy(macro_name, p);
                return macro_name;
            }
            break;
        }
        p = strtok(NULL, delimiter);
    }

    return NULL;
}

