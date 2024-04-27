#ifndef encryption_H
#define encryption_H


#include "../process/dataCodeTable.h"


/**
 * Converts a 14-digit binary sequence into a 7-character encrypted string.
 * Each pair of binary digits is converted to a specific character:
 * '00' -> '*', '01' -> '#', '10' -> '%', '11' -> '!'.
 * Dynamically allocates memory for the encrypted string, which must be freed by the caller.
 *
 * @param binary A string representing the 14-digit binary input.
 * @return A dynamically allocated string containing the 7-character encrypted output.
 */
char* encryptBinary(const char *binary);


/** 
 * Recursive helper function to print that 
 * reverses the encryption process using a node structure.
 * 
 * @param node Pointer to the node structure representing the data to be reversed.
 * @param E Pointer to an integer array used in the decryption process.
 */
void encryptionReverse(const Node *node, int *E);


/**
 * Prints the reversed encryption from a list.
 * 
 * @param list Pointer to a WordList containing encrypted data.
 * @param E Pointer to an integer array used in the decryption process.
 */
void printEncryptionReverse(const WordList *list, int *E);


/**
 * Writes the words in a linked list in reverse order with recursion to a given file.
 * @param list1 First word list to process.
 * @param list2 Second word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeEncryptionToFile(const WordList *list1, const WordList *list2, const char *fileName);


/**
 * Helper function to recursively write nodes in reverse to a file.
 * @param node Current node in the linked list.
 * @param file File pointer to write data.
 * @param E Pointer to the encryption count, which is incremented for each node.
 */
void encryptionToFile(const Node *node, FILE *file, int *E);


/**
 * Writes the words in a linked list to a file.
 * @param list The word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeWordListToFile(const WordList *list, const char *fileName);

#endif
