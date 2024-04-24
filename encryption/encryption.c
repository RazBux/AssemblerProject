#include <stdio.h>
#include <stdlib.h>
#include "../process/dataCodeTable.h"
#include "../process/util.h"

char* encryptBinary(const char *binary);
void encryptionReverse(const Node *node, int *E);
void printEncryptionReverse(const WordList *list, int *E);
void encryptionToFile(const Node *node, FILE *file, int *E);

/**
 * Converts a 14-digit binary sequence into a 7-character encrypted string.
 * Each pair of binary digits is converted to a specific character:
 * '00' -> '*', '01' -> '#', '10' -> '%', '11' -> '!'.
 * Dynamically allocates memory for the encrypted string, which must be freed by the caller.
 *
 * @param binary A string representing the 14-digit binary input.
 * @return A dynamically allocated string containing the 7-character encrypted output.
 */
char* encryptBinary(const char *binary) {
    char *encrypted = malloc(8);  /* Allocate memory for 7 characters plus null terminator */ 
    int i;
    if (encrypted == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (i = 0; i < 14; i += 2) {
        switch ((binary[i] - '0') * 2 + (binary[i + 1] - '0')) {
            case 0: encrypted[i / 2] = '*'; break;  /*'00'*/ 
            case 1: encrypted[i / 2] = '#'; break;  /*'01'*/ 
            case 2: encrypted[i / 2] = '%'; break;  /*'10'*/ 
            case 3: encrypted[i / 2] = '!'; break;  /*'11'*/ 
        }
    }
    encrypted[7] = '\0';  /*Null-terminate the string*/ 

    return encrypted;
}

/* Recursive helper function to print nodes in reverse */
void encryptionReverse(const Node *node, int *E) {
    if (node != NULL) {
        encryptionReverse(node->next, E);  /*Traverse to the end of the list*/ 
        
        printf("%04d  %s\n", *E, encryptBinary(node->word));     /*Print the word on the way back*/ 
        *E+=1;

    }
}

/* Function to print all words in the linked list from end to start */
void printEncryptionReverse(const WordList *list, int *E) {
    encryptionReverse(list->head, E);
}

/**/

/**
 * Writes the words in a linked list in reverse order with recursion to a given file.
 * @param list1 First word list to process.
 * @param list2 Second word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeEncryptionToFile(const WordList *IC_table, const WordList *DC_table, const char *fileName) {
    int E = 100;  /* Starting value for E */
    FILE *file = fopen(fileName, "w");  /* Open file for writing */

    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    
    fprintf(file, "  %d  %d\n", IC_table->count, DC_table->count);
    
    /* Process each list in reverse and write to file */
    encryptionToFile(IC_table->head, file, &E);
    encryptionToFile(DC_table->head, file, &E);

    fclose(file);  /* Close the file */
}

/**
 * Helper function to recursively write nodes in reverse to a file.
 * @param node Current node in the linked list.
 * @param file File pointer to write data.
 * @param E Pointer to the encryption count, which is incremented for each node.
 */
void encryptionToFile(const Node *node, FILE *file, int *E) {
    if (node != NULL) {
        encryptionToFile(node->next, file, E);  /* Traverse to the end of the list */
        fprintf(file, "%04d  %s\n", *E, encryptBinary(node->word));  /* Write the word on the way back */
        *E += 1;
    }
}


/**
 * Writes the words in a linked list to a file.
 * @param list The word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeWordListToFile(const WordList *list, const char *fileName) {
    FILE *file = fopen(fileName, "w");  /* Open file for writing */
    Node *current;

    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    current = list->head;
    while (current != NULL) {
        fprintf(file, "%s\n", current->word);  /* Write the current word to the file */
        current = current->next;  /* Move to the next node */
    }

    fclose(file);  /* Close the file */
}






