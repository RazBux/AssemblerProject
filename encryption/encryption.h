#ifndef encryption_H
#define encryption_H

#include "../utils/dataCodeTable.h" 

/**
 * Writes the words in a linked list in reverse order with recursion to a given file.
 * the recuresion needed due to the insertion of the words to get the FIFO ordered.
 * @param list1 First word list to process.
 * @param list2 Second word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeEncryptionToFile(const WordList *list1, const WordList *list2, const char *fileName);


/**
 * Writes the words in a linked list to a file.
 * @param list The word list to process.
 * @param fileName The name of the file where to write the content.
 */
void writeWordListToFile(const WordList *list, const char *fileName);


#endif
