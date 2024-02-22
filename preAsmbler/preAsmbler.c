#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

/* Initialize macro storage */
void initMacroStorage(MacroStorage *storage)
{
    storage->size = INITIAL_MACRO_COUNT;
    storage->count = 0;
    storage->names = malloc(storage->size * sizeof(char *));
    storage->texts = malloc(storage->size * sizeof(char *));
}

/* Method to clean the macroText from lots of tabs and unnessecry white spaces */
char *cleanText(const char *src)
{
    char *cleanedText;
    char *dst;
    int inWhitespace, started;

    cleanedText = (char *)malloc(strlen(src) + 1); /* Allocate memory for the cleaned text */
    if (cleanedText == NULL)
    {
        return NULL; /* Failed to allocate memory */
    }

    dst = cleanedText;

    /* Initially not in a whitespace sequence */
    inWhitespace = 0;

    /* Have not started copying non-whitespace characters */
    started = 0;

    while (*src)
    {
        if (*src == '\n')
        {
            /* Directly copy newline character */
            *dst++ = *src;    

            /* Reset whitespace sequence tracking */
            inWhitespace = 1; 

            /* Mark as started copying to allow spaces after this point */
            started = 1;      
        }
        else if (isspace((unsigned char)*src))
        {
            if (!inWhitespace && started)
            {
                *dst++ = ' '; /* Insert a single space for the first whitespace */
            }
            inWhitespace = 1; /* We're in a whitespace sequence */
        }
        else
        {
            *dst++ = *src;    /* Copy non-whitespace character */
            inWhitespace = 0; /* No longer in a whitespace sequence */
            started = 1;      /* Mark as started copying */
        }
        src++;
    }
    *dst = '\0'; /* Null-terminate the cleaned text */

    return cleanedText; /* Return the dynamically allocated cleaned text */
}

/* Function to print all stored macros */
void printStoredMacros(const MacroStorage *storage)
{
    int i;
    printf("Stored Macros:\n");
    for (i = 0; i < storage->count; i++)
    {
        printf("%d:\nMacro Name: \n%s\nMacro Text:\n%s\n", i, storage->names[i], storage->texts[i]);
    }
}

/* Function that add a macro to the storage. both the name and the text of the macro */
void addMacro(MacroStorage *storage, const char *name, char *text)
{
    /* Clean the text from spaces and others */
    char *cleanedText = cleanText(text);
    if (cleanedText != NULL)
    {
        strcpy(text, cleanedText);
        free(cleanedText); /* Free the allocated memory */
    }
    else
    {
        printf("Memory allocation failed.\n");
    }

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

/* Free macro storage */
void freeMacroStorage(MacroStorage *storage)
{
    int i;
    for (i = 0; i < storage->count; i++)
    {
        free(storage->names[i]);
        free(storage->texts[i]);
    }
    free(storage->names);
    free(storage->texts);
}

/* Function to get the macro name */
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
                size_t len = strlen(p);
                macro_name = malloc(len + 1);
                /* macro_name = malloc(strlen(p) + 1); */
                if (macro_name == NULL)
                {
                    return NULL;
                }
                strcpy(macro_name, p);

                /* Trim newline and other special characters from the end */
                while (len > 0 && (macro_name[len - 1] == '\n' || isspace(macro_name[len - 1]) || !isprint(macro_name[len - 1])))
                {
                    /* Remove the character and terminate the string earlier */
                    macro_name[--len] = '\0';
                }

                return macro_name;
            }
            break;
        }
        p = strtok(NULL, delimiter);
    }

    return NULL;
}

/* This function read the Assmbler program file and take all the macro from it. */
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
            strcpy(name, mcrName);
            text[0] = '\0'; /* Reset text buffer */
            readingMacro = 1;
        }
        else if (strstr(line, " endmcr") != NULL && readingMacro)
        {
            addMacro(storage, name, text);
            name[0] = '\0'; /* reset the mcaro name */
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

    fclose(outputFile);
}

/* 
    This function add all the MacroText where it's find their coresponding MacroName
    for exsample: if there is a macroName: "mm", so in all the palces there is "mm" 
    the name will be removed and the macroText of "mm" will be displayed 
*/
void AddMacroToFile(const char *outputFileName, MacroStorage *storage)
{
    FILE *outputFile = fopen(outputFileName, "r");
    FILE *tempFile = fopen("tempfile.txt", "w");
    char line[MAX_LINE_LENGTH];

    if (!outputFile)
    {
        perror("File opening failed");
        return;
    }

    if (!tempFile)
    {
        perror("Temporary file opening failed");
        fclose(outputFile);
        return;
    }

    while (fgets(line, sizeof(line), outputFile) != NULL)
    {
        int i, lineModified = 0;
        for (i = 0; i < storage->count && !lineModified; i++)
        {
            char *found = strstr(line, storage->names[i]);
            if (found)
            {
                /* Calculate indentation */
                int indentation = found - line;
                char indentStr[MAX_LINE_LENGTH] = {0};

                /* Split the macro text into lines and indent each line */
                char *macroText = storage->texts[i];
                char *lineStart = macroText;
                char *lineEnd;

                strncpy(indentStr, line, indentation);
                while ((lineEnd = strchr(lineStart, '\n')) != NULL)
                {
                    if (lineStart == macroText)
                    {                                       /* For the first line of macro text */
                        fprintf(tempFile, "%s", indentStr); /* Print indentation */
                    }
                    fwrite(lineStart, 1, lineEnd - lineStart + 1, tempFile); /* Print the macro line */
                    if (lineEnd[1] != '\0')
                    {                                       /* If not the end of macro text */
                        fprintf(tempFile, "%s", indentStr); /* Print indentation for the next line */
                    }
                    lineStart = lineEnd + 1;
                }
                if (*lineStart)
                {                                                      /* If there's text after the last newline */
                    fprintf(tempFile, "%s%s\n", indentStr, lineStart); /* Print it with indentation and add a newline */
                }
                lineModified = 1;
            }
        }
        if (!lineModified && strlen(line) > 1)
        {                          /* To avoid writing empty lines */
            fputs(line, tempFile); /* Write the line as it is if not modified */
        }
    }

    fclose(outputFile);
    fclose(tempFile);

    /* Replace the original file with the updated one */
    remove(outputFileName);
    rename("tempfile.txt", outputFileName);
}

int main(void)
{
    char inputFileName[] = "./textFiles/m.txt";
    char outputFileName[] = "./macro_output.txt";
    FILE *file = fopen(inputFileName, "r");

    /* Create the MacroStorage in memory*/
    MacroStorage storage;
    initMacroStorage(&storage);

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

    /* Print all stored macros - Optional */
    printStoredMacros(&storage);

    /* Process the input file and write the result to the output file */
    AddMacroToFile(outputFileName, &storage);

    freeMacroStorage(&storage);

    printf("Processing complete!\nOutput written to: %s\n", outputFileName);

    return 0;
}
