#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preAsmbler.h"
#include "../utils/util.h"


/** 
 * Initializes the macro storage with initial capacity.
 * @param storage Pointer to the MacroStorage structure.
 */
void init_macro_storage(MacroStorage *storage)
{
    storage->size = 10;
    storage->count = 0;
    storage->names = malloc(storage->size * sizeof(char *));
    storage->texts = malloc(storage->size * sizeof(char *));
}


/** 
 * Cleans input text by removing unnecessary whitespace.
 * @param src Pointer to the original text.
 * @return Pointer to the cleaned text. The caller is responsible for freeing this memory.
 */
char *clean_text(const char *src)
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


/**
 * Prints all stored macros from the macro storage. Each macro's name and text are displayed.
 *
 * @param storage Pointer to the MacroStorage structure containing the macros to be printed.
 */
void print_stored_macros(const MacroStorage *storage)
{
    int i;
    if (storage->count > 0)
    {
        printf("Stored Macros:\n");
        for (i = 0; i < storage->count; i++)
        {
            printf("%d:\nMacro Name: \n%s\nMacro Text:\n%s\n", i, storage->names[i], storage->texts[i]);
        }
    }
    else {
        printf("There is no macro in this file\n");
    }
}


/**
 * Adds a new macro definition to the macro storage, including its name and text. If the current storage capacity is reached, it doubles the storage size.
 * Text is cleaned of unnecessary whitespace before storage.
 *
 * @param storage Pointer to the MacroStorage structure where the macro is to be added.
 * @param name Pointer to the string containing the macro's name.
 * @param text Pointer to the string containing the macro's text.
 */
void add_macro(MacroStorage *storage, const char *name, char *text)
{
    /* Clean the text from spaces and others */
    char *cleanedText = clean_text(text);
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

    storage->names[storage->count] = malloc(strlen(name) + 1);
    storage->texts[storage->count] = malloc(strlen(text) + 1);

    if (storage->names[storage->count] == NULL || storage->texts[storage->count] == NULL){
    	printf("Fail to allocate memory");
    	exit(1);
    }
    
    strcpy(storage->names[storage->count],name);
    strcpy(storage->texts[storage->count],text);

    storage->count++;
}


/** 
 * Frees all allocated memory associated with the macro storage.
 * @param storage Pointer to the MacroStorage to be freed.
 */
void free_macro_storage(MacroStorage *storage)
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


/**
 * Extracts the macro name from a given line of text if it contains a macro declaration.
 * Assumes the format 'mcr [macro_name]' to identify and extract the macro name.
 *
 * @param line The input string from which the macro name is to be extracted.
 * @return A pointer to a newly allocated string containing the macro name, or NULL if no macro name is found or memory allocation fails. The caller is responsible for freeing this memory.
 */
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


/** This function read the Assmbler program file and take all the macro from it.  
 * @param file Name of the file to process for macro expansion.
 * @param storage Pointer to the MacroStorage containing all defined macros. 
 * @param outputFileName Name of the ".am" file for writing the new parse macro content.
 */
void read_macros_from_file(FILE *file, MacroStorage *storage, const char *outputFileName)
{
    char line[MAX_LINE_LENGTH];
    char name[MAX_LEN_NAME] = {0};
    char text[MAX_LINE_LENGTH * 10] = {0};
    int readingMacro = 0;

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile)
    {
        perror("File opening failed");
        return;
    }

    /*maybe need to change to MAX_LINE_LENGTH the sizeof(line)*/
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strstr(line, " mcr ") != NULL)
        {
            /* get the macro name and copy to name*/
            char *mcrName = get_macro_name(line);
            strcpy(name, mcrName);
            text[0] = '\0'; /* Reset text buffer */
            readingMacro = 1;
            free(mcrName);
        }
        else if (strstr(line, " endmcr") != NULL && readingMacro)
        {
            add_macro(storage, name, text);
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


/** 
 * Processes the text in the output file, replacing macro calls with the appropriate macro text.
 * for exsample: if there is a macroName: "mm", so in all the palces there is "mm"
 * the name will be removed and the macroText of "mm" will be displayed
 * @param outputFileName Name of the file to process for macro expansion.
 * @param storage Pointer to the MacroStorage containing all defined macros. 
 */
void add_macro_to_file(const char *outputFileName, MacroStorage *storage)
{
    FILE *outputFile = fopen(outputFileName, "r");
    FILE *tempFile = fopen("tempfile.txt", "w");
    char line[MAX_LINE_LENGTH];

    if (!outputFile)
    {
        perror("File opening failed");
        exit(1);
    }

    if (!tempFile)
    {
        perror("Temporary file opening failed");
        fclose(outputFile);
        exit(1);
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
        if (!lineModified && strlen(line) > 1) /* To avoid writing empty lines */
        {
            fputs(line, tempFile); /* Write the line as it is if not modified */
        }
    }

    fclose(outputFile);
    fclose(tempFile);

    /* Replace the original file with the updated one */
    remove(outputFileName);
    rename("tempfile.txt", outputFileName);
}


/** 
 * Main function to start the pre-assembly process, coordinating file reading, macro processing, and output file generation.
 * @param inputFileName Name of the assembler file to process.
 * @param outputFileName Name of the file to output processed text.
 * @return Integer status code indicating the success or failure of the operation.
 */
int startPreAsmbler(char *inputFileName, char *outputFileName)
{
    /* change this to recives the files name. and them chagne the name to
        ".am" at the end and return the file path for the firstProcess */
    /*
    char inputFileName[] = "./textFiles/m.txt";
    char outputFileName[] = "./textFiles/m.am";
    */
    FILE *file = fopen(inputFileName, "r");

    /* Create the MacroStorage in memory*/
    MacroStorage storage;
    init_macro_storage(&storage);

    if (file)
    {
        printf("\n--- START PRE-ASSMBLER ---\n");
        read_macros_from_file(file, &storage, outputFileName);
        fclose(file);
    }
    else
    {
        printf("Error: Failed to open \"%s\" file.\n", inputFileName);
        return -1;
    }

    /* Print all stored macros - Optional */
    print_stored_macros(&storage);

    /* Process the input file and write the result to the output file */
    add_macro_to_file(outputFileName, &storage);

    free_macro_storage(&storage);

    printf("Output written to: %s\n", outputFileName); 
    printf("\n--- PRE-ASSMBLER COMPLETE! ---\n");

    return 0;
}
