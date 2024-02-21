#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 81
#define MAX_MACRO_NAME 31
#define INITIAL_MACRO_COUNT 10

/* Dynamic structure to hold macro names and texts */
typedef struct
{
    char **names;
    char **texts;
    int size;
    int count;
} MacroStorage;

// Initialize macro storage
void initMacroStorage(MacroStorage *storage)
{
    storage->size = INITIAL_MACRO_COUNT;
    storage->count = 0;
    storage->names = malloc(storage->size * sizeof(char *));
    storage->texts = malloc(storage->size * sizeof(char *));
}

// Function to print all stored macros
void printStoredMacros(const MacroStorage *storage)
{
    printf("Stored Macros:\n");
    for (int i = 0; i < storage->count; i++)
    {
        printf("No.%d: Macro Name: %s\nMacro Text:\n%s\n", i,storage->names[i], storage->texts[i]);
    }
}

// Add a macro to the storage
void addMacro(MacroStorage *storage, const char *name, const char *text)
{
    if (storage->count == storage->size)
    {
        storage->size *= 2;
        /* Increace the macro memory is needed and move all data to with realloc */
        storage->names = realloc(storage->names, storage->size * sizeof(char *));
        storage->texts = realloc(storage->texts, storage->size * sizeof(char *));
    }
    storage->names[storage->count] = strdup(name);
    storage->texts[storage->count] = strdup(text);
    storage->count++;
}

// Free macro storage
void freeMacroStorage(MacroStorage *storage)
{
    for (int i = 0; i < storage->count; i++)
    {
        free(storage->names[i]);
        free(storage->texts[i]);
    }
    free(storage->names);
    free(storage->texts);
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


void readMacrosFromFile(FILE *file, MacroStorage *storage, const char *outputFileName)
{
    char line[MAX_LINE_LENGTH];
    char name[MAX_MACRO_NAME] = {0};
    char text[MAX_LINE_LENGTH * 10] = {0};
    int readingMacro = 0;

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile)
    {
        perror("File opening failed");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strstr(line, " mcr ") != NULL)
        {
            /* get the macro name and copy to name*/
            char *mcrName = get_macro_name(line);
            strcpy(name,mcrName);
            printf("name:: %s\n",name);
            text[0] = '\0'; // Reset text buffer
            readingMacro = 1;

        }
        else if (strstr(line, " endmcr") != NULL && readingMacro)
        {
            printf("end of macro\n");
            addMacro(storage, name, text);
            name[0] = '\0'; //reset the mcaro name
            text[0] = '\0';
            readingMacro = 0;
        }
        else if (readingMacro)
        {
            if (strlen(text) + strlen(line) < sizeof(text) - 1)
            {
                /* Add to here: increase the memory if needed.. */
                strcat(text, line);
            }
            else
            {
                fprintf(stderr, "Macro text buffer overflow. Macro text may be truncated.\n");
            }
        }
        else
        {
            /* write the line to the file */
            fputs(line, outputFile);
        }
    }
}

/* Function to process the input file and replace macros */
void processFileAddMacro(const char *outputFileName, MacroStorage *storage)
{
    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile)
    {
        perror("File opening failed");
        return;
    }

    char line[MAX_LINE_LENGTH];

    // do it with token work...
    while (fgets(line, sizeof(line), outputFile) != NULL)
    {
        // check each name if it is a macro name and replace it with the macro
    }

    fclose(outputFile);
}

int main()
{
    char inputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/cprog/AsmblerProject/m.txt";
    char outputFileName[] = "ffff.txt";

    MacroStorage storage;
    initMacroStorage(&storage);

    // Assuming readMacrosFromFile is correctly implemented to fill 'storage'
    FILE *file = fopen(inputFileName, "r");
    if (file)
    {
        readMacrosFromFile(file, &storage, outputFileName);
        fclose(file);
    }
    else
    {
        perror("Failed to open input file for reading macros");
        return 1;
    }

    // Print all stored macros
    printStoredMacros(&storage);

    // Process the input file and write the result to the output file
    processFileAddMacro(outputFileName, &storage);

    freeMacroStorage(&storage);

    printf("Processing complete. Output written to %s\n", outputFileName);

    return 0;
}
