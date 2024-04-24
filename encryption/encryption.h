#ifndef encryption_H
#define encryption_H

#include "../process/dataCodeTable.h"

char* encryptBinary(const char *binary);
void encryptionReverse(const Node *node, int *E);
void printEncryptionReverse(const WordList *list, int *E);
void writeEncryptionToFile(const WordList *list1, const WordList *list2, const char *fileName);
void encryptionToFile(const Node *node, FILE *file, int *E);
void writeWordListToFile(const WordList *list, const char *fileName);

#endif
