#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataCodeTable.h"

/* Function prototypes */
void addWord(WordList *list, const char *word);
void changeWord(WordList *list, int position, const char *newWord);
void printWordList(const WordList *list);
Node* createNode(const char *word);

/*
int main() {
    WordList list = {NULL, 0};

    addWord(&list, "Hello");
    addWord(&list, "World");
    addWord(&list, "ANSI");
    addWord(&list, "C");

    printWordList(&list);

    changeWord(&list, 2, "Standard");
    printWordList(&list);

    return 0;
}
*/


/* Function to add a word to the linked list */
void addWord(WordList *list, const char *word) {
    Node *newNode = createNode(word); /* Create a new node with the given word */
    if (newNode == NULL) return; /* Memory allocation failed */

    printf("AddToList - %s\n", word);

    newNode->next = list->head; /* Insert at the beginning */
    list->head = newNode;
    list->count++; /* Increment the count of words */
}

/* Function to change a word in the linked list by position */
void changeWord(WordList *list, int position, const char *newWord) {
    int i;
    Node *current;
    if (position < 1 || position > list->count) {
        printf("Invalid position!\n");
        return;
    }

    current = list->head;
    for (i = 1; i < position; i++) {
        current = current->next;
    }

    free(current->word); /* Free the old word */
    current->word = strdup(newWord); /* Allocate memory and copy the new word */
}

/* Function to print all words in the linked list */
void printWordList(const WordList *list) {
    
    Node *current = list->head;
    printf("Word List: \n");
    while (current != NULL) {
        printf("%s\n", current->word);
        current = current->next;
    }
    printf("\n");
}

/* Helper function to create a new node */
Node* createNode(const char *word) {
    Node *newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL) {
        return NULL; /* Memory allocation failed */
    }

    newNode->word = strdup(word); /* Duplicate the word for the node */
    if (newNode->word == NULL) {
        free(newNode);
        return NULL; /* Memory allocation failed */
    }

    newNode->next = NULL;
    return newNode;
}

