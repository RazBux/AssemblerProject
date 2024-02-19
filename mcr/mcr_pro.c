#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 81
#define MAX_MACRO_NAME 31
#define INITIAL_MACRO_COUNT 10

// Dynamic structure to hold macro names and texts
typedef struct {
    char **names;
    char **texts;
    int size;
    int count;
} MacroStorage;

// Initialize macro storage
void initMacroStorage(MacroStorage *storage) {
    storage->size = INITIAL_MACRO_COUNT;
    storage->count = 0;
    storage->names = malloc(storage->size * sizeof(char *));
    storage->texts = malloc(storage->size * sizeof(char *));
}

// Function to print all stored macros
void printStoredMacros(const MacroStorage *storage) {
    printf("Stored Macros:\n");
    for (int i = 0; i < storage->count; i++) {
        printf("Macro Name: %s\nMacro Text:\n%s\n", storage->names[i], storage->texts[i]);
    }
}

// Add a macro to the storage
void addMacro(MacroStorage *storage, const char *name, const char *text) {
    if (storage->count == storage->size) {
        storage->size *= 2;
        storage->names = realloc(storage->names, storage->size * sizeof(char *));
        storage->texts = realloc(storage->texts, storage->size * sizeof(char *));
    }
    storage->names[storage->count] = strdup(name);
    storage->texts[storage->count] = strdup(text);
    storage->count++;
}

// Free macro storage
void freeMacroStorage(MacroStorage *storage) {
    for (int i = 0; i < storage->count; i++) {
        free(storage->names[i]);
        free(storage->texts[i]);
    }
    free(storage->names);
    free(storage->texts);
}

void readMacrosFromFile(FILE *file, MacroStorage *storage) {
    char line[MAX_LINE_LENGTH];
    char name[MAX_MACRO_NAME] = {0};
    char text[MAX_LINE_LENGTH * 10] = {0}; // Large buffer for macro text
    int readingMacro = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character at the end of the line
        // line[strcspn(line, "\n")] = 0; 

        if (strstr(line, "mcr ") != NULL) {
            printf("%s",line);

            // Extract macro name more reliably
            char *start = strchr(line, ' ');
            if (start) {
                start++; // Move past the space
                strcpy(name, start);
                printf("name:: %s",name);
                text[0] = '\0'; // Reset text buffer
                readingMacro = 1;
            }
        } else if (strstr(line, "endmcr ") != NULL && readingMacro) {
            addMacro(storage, name, text);
            readingMacro = 0;
        } else if (readingMacro) {
            if (strlen(text) + strlen(line) < sizeof(text) - 1) {
                strcat(text, line);
            } else {
                fprintf(stderr, "Macro text buffer overflow. Macro text may be truncated.\n");
            }
        }
    }

}

// Function to process the input file and replace macros
void processAndWriteFile(const char *inputFileName, const char *outputFileName, MacroStorage *storage) {
    FILE *inputFile = fopen(inputFileName, "r");
    FILE *outputFile = fopen(outputFileName, "w");
    if (!inputFile || !outputFile) {
        perror("File opening failed");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int insideMacroDefinition = 0;

    while (fgets(line, sizeof(line), inputFile) != NULL) {
        // Check for macro definitions and skip them
        if (strncmp(line, "mcr", 3) == 0) {
            insideMacroDefinition = 1;
            continue; // Skip writing this line
        }
        if (strncmp(line, "endmcr", 6) == 0) {
            insideMacroDefinition = 0;
            continue; // Skip writing this line
        }

        if (!insideMacroDefinition) {
            // Process line to replace macros
            char processedLine[MAX_LINE_LENGTH * 10]; // Buffer for processed line
            strcpy(processedLine, line); // Start with the original line

            for (int i = 0; i < storage->count; i++) {
                // Replace each occurrence of macro names with their texts
                // Note: This simplistic approach may need refinement for multiple occurrences or overlapping macros
                if (strstr(processedLine, storage->names[i])) {
                    // Replace macro name with its text
                    // For simplicity, demonstrating with a direct replacement for the first occurrence
                    // You would need a more sophisticated method for multiple or overlapping replacements
                    strcpy(processedLine, storage->texts[i]);
                }
            }

            fputs(processedLine, outputFile); // Write the processed (or original) line
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}

int main() {
    char inputFileName[] = "/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/cprog/AsmblerProject/m.txt";
    char outputFileName[] = "ffff.txt";

    MacroStorage storage;
    initMacroStorage(&storage);

    // Assuming readMacrosFromFile is correctly implemented to fill 'storage'
    FILE *file = fopen(inputFileName, "r");
    if (file) {
        readMacrosFromFile(file, &storage);
        fclose(file);
    } else {
        perror("Failed to open input file for reading macros");
        return 1;
    }

    // Print all stored macros
    printStoredMacros(&storage);

    // Process the input file and write the result to the output file
    processAndWriteFile(inputFileName, outputFileName, &storage);

    freeMacroStorage(&storage);

    printf("Processing complete. Output written to %s\n", outputFileName);

    return 0;
}
