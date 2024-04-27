
#ifndef dataCodeTable_H
#define dataCodeTable_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


/**
 * Adds a word to the start of a linked list.
 * 
 * @param list Pointer to the WordList to add the word to.
 * @param word The word to add to the list.
 */
void addWord(WordList *list, const char *word);


/**
 * Adds a word to the start of a linked list.
 * 
 * @param list Pointer to the WordList to add the word to.
 * @param word The word to add to the list.
 */
void changeWord(WordList *list, int position, const char *newWord);


/**
 * Prints all words in the linked list.
 * 
 * @param list Pointer to the WordList to be printed.
 */
void printWordList(const WordList *list);


/**
 * Prints all words in the linked list from end to start.
 * 
 * @param list Pointer to the WordList to be printed in reverse.
 */
void printWordListReverse(const WordList *list);


/**
 * Creates a new node for a word and initializes it.
 * 
 * @param word The word to store in the node.
 * @return A pointer to the newly created node, or NULL if memory allocation fails.
 */
Node* createNode(const char *word);


/**
 * Frees all memory allocated for the WordList.
 * 
 * @param list Pointer to the WordList to be freed.
 */
void freeWordList(WordList *list);


#endif
