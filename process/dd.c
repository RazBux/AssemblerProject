#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Combines an integer and a string into a formatted string.
 * The integer is formatted to four digits with leading zeros and combined with the string with three spaces in between.
 *
 * @param str The string part of the input.
 * @param num The integer to format and combine with the string.
 * @return A dynamically allocated string containing the formatted output. 
 *         The caller is responsible for freeing this memory.
 */
char* combineIntStr(const char *str, int num) {
    // Allocate memory for the resulting string
    // Integer (4 digits + '\0') + 2 spaces + original string + '\0'
    char *result = malloc(5 + 2 + strlen(str) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL; // Return NULL on allocation failure
    }

    // Use sprintf to format and concatenate the string directly into the allocated buffer
    sprintf(result, "%04d  %s", num, str);

    return result;
}

int main() {
    // Example usage
    char *result = combineIntStr("099003935", 199);
    if (result != NULL) {
        printf("Combined String: '%s'\n", result);
        free(result); // Free the dynamically allocated memory
    }

    result = combineIntStr("chage", 2);
    if (result != NULL) {
        printf("Combined String: '%s'\n", result);
        free(result); // Remember to free the dynamically allocated memory
    }

    return 0;
}
