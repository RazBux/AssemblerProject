#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataCodeTable.h"

/* Function prototypes */
void addWord(WordList *list, const char *word);
void changeWord(WordList *list, int position, const char *newWord);
void printWordList(const WordList *list);
void printWordsReverse(const Node *node);
void printWordListReverse(const WordList *list);
Node* createNode(const char *word);

/* Function to add a word to the linked list */
void addWord(WordList *list, const char *word) {
    Node *newNode = createNode(word); /* Create a new node with the given word */
    if (newNode == NULL) return; /* Memory allocation failed */

    /* printf("AddToList - %s\n", word); */

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

    /*current->word = strdup(newWord);*/ 

    current->word = malloc(strlen(newWord) + 1);
    if (current-> word == NULL){
        printf("Fail to allocate memory");
        exit(1);
    }
    strcpy(current->word, newWord);

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

/* Recursive helper function to print nodes in reverse */
void printWordsReverse(const Node *node) {
    if (node != NULL) {
        printWordsReverse(node->next);  /*Traverse to the end of the list*/ 
        printf("%s\n", node->word);     /*Print the word on the way back*/ 
    }
}

/* Function to print all words in the linked list from end to start */
void printWordListReverse(const WordList *list) {
    printf("Word List (Reversed): \n");
    printWordsReverse(list->head);
    printf("\n");
}

/* Helper function to create a new node */
Node* createNode(const char *word) {
    Node *newNode = (Node*) malloc(sizeof(Node));
    char *newWord;
    if (newNode == NULL) {
        printf("Error: memory allocation failed for node");
        exit(1);  /* Exit if the node allocation fails */
    }

    newWord = (char *)malloc(strlen(word) + 1);
    if (newWord == NULL) {
        printf("Error: memory allocation failed for word");
        free(newNode);  /* Free the previously allocated node */
        exit(1);  /* Exit if the word allocation fails */
    }

    strcpy(newWord, word);  /* Copy the word into the newly allocated memory */
    newNode->word = newWord;  /* Assign the wocat ard to the node */
    newNode->next = NULL;     /* Initialize the next pointer to NULL */

    return newNode;  /* Return the newly created node */
}


/* Function to free all memory of the WordList */
void freeWordList(WordList *list) {
    Node *current = list->head;   /* Start with the head of the list */
    Node *next;

    while (current != NULL) {
        next = current->next;     /* Save the pointer to the next node */
        free(current->word);      /* Free the word stored in the current node */
        free(current);            /* Free the current node itself */
        current = next;           /* Move to the next node */
    }

    list->head = NULL;            /* After freeing all nodes, set the head to NULL */
    list->count = 0;              /* Reset the word count to 0 */
}



