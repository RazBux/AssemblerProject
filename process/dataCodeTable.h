#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef dataCodeTable_H
#define dataCodeTable_H

/* Define a struct for the linked list nodes */
typedef struct Node {
    char *word;         /* Pointer to the word stored in this node */
    struct Node *next;  /* Pointer to the next node in the list */
} Node;

/* Define a struct for the linked list */
typedef struct {
    Node *head;         /* Pointer to the first node in the list */
    int count;          /* Counter for the number of words in the list */
} WordList;

/* Function prototypes */
void addWord(WordList *list, const char *word);
void changeWord(WordList *list, int position, const char *newWord);
void printWordList(const WordList *list);
void printWordListReverse(const WordList *list);
Node* createNode(const char *word);
void freeWordList(WordList *list);


#endif
