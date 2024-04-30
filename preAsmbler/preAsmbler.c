#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preAsmbler.h"
#include "../utils/util.h"

/**
 * Initializes the macro storage with initial capacity.
 * @param storage Pointer to the MacroStorage structure.
 */
void init_macro_storage(MacroStorage *storage) {
    storage->head = NULL;
    storage->count = 0;

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
void print_stored_macros(const MacroStorage *storage) {
    MacroNode *current = storage->head;
    int i = 0;

    if (current == NULL) {
        printf("No macros stored.\n");
        return;
    }

    printf("Stored Macros:\n");
    while (current != NULL) {
        printf("\n%d:\nMacro Name: %s\nMacro Text:\n%s\n", i++, current->name, current->text);
        current = current->next;
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
void add_macro(MacroStorage *storage, const char *name, const char *text) {
    MacroNode *newNode = (MacroNode *)malloc(sizeof(MacroNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    /* Manually allocate memory for name and text */ 
    newNode->name = malloc(strlen(name) + 1);  /* +1 for the null terminator */ 
    if (newNode->name == NULL) {
        printf("Memory allocation for name failed.\n");
        free(newNode);  /* Don't forget to clean up already allocated memory */ 
        exit(1);
    }
    strcpy(newNode->name, name);  /* Copy the name into the newly allocated space */ 

    newNode->text = malloc(strlen(text) + 1); 
    if (newNode->text == NULL) {
        printf("Memory allocation for text failed.\n");
        free(newNode->name);
        free(newNode);
        exit(1);
    }
    strcpy(newNode->text, text);  /* Copy the text into the newly allocated space */ 

    /* Link the new node at the beginning of the list */ 
    newNode->next = storage->head;
    storage->head = newNode;
    storage->count++;
}


/**
 * Frees all allocated memory associated with the macro storage.
 * @param storage Pointer to the MacroStorage to be freed.
 */
void free_macro_storage(MacroStorage *storage) {
    MacroNode *current = storage->head;
    MacroNode *next;

    while (current != NULL) {
        next = current->next;
        free(current->name);
        free(current->text);
        free(current);
        current = next;
    }

    storage->head = NULL;
    storage->count = 0;
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
    printf("preAs >> %s", line);
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
        if (*line != ';')
        {
            if (strstr(line, "mcr ") != NULL)
            {
                /* get the macro name and copy to name*/
                char *mcrName = get_macro_name(line);
                if (mcrName == NULL)
                {
                    printf("Error: fail to alocate memory");
                    exit(1);
                }

                strcpy(name, mcrName);
                free(mcrName);

                text[0] = '\0'; /* Reset text buffer */
                readingMacro = 1;
            }
            else if (strstr(line, "endmcr") != NULL && readingMacro)
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
        int lineModified = 0;
        MacroNode *current = storage->head;
        while (current != NULL && !lineModified)
        {
            char *found = strstr(line, current->name);
            if (found)
            {
                /* Calculate indentation */
                int indentation = found - line;
                char indentStr[MAX_LINE_LENGTH] = {0};

                /* Split the macro text into lines and indent each line */
                char *macroText = current->text;
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
            current = current->next;
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
