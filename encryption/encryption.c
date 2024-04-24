#include <stdio.h>
#include <stdlib.h>

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
    char *encrypted = malloc(8);  // Allocate memory for 7 characters plus null terminator
    if (encrypted == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < 14; i += 2) {
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


int main() {
    const char *binarySequence = "00000000111000";
    char *encrypted = encryptBinary(binarySequence);
    int number = 100;
    printf("Formatted Number: %04d  ", number);
    if (encrypted != NULL) {
        printf("Encrypted: %s\n", encrypted);  
        free(encrypted);
    }
    return 0;
}
