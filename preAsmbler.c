#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 80

/* Function prototypes */
char *extract_macro_name(const char *line);
void free_memory(char **array, size_t count);

int main(void) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char **macro_names = NULL;
    char *macro_content = NULL;
    size_t macro_count = 0;
    size_t i;
    int in_macro = 0;
    char *new_content;

    /*dont forget to change this to a < input.txt at the end..*/
    file = fopen("/Users/razbuxboim/Desktop/University/Open University semesters/2024/2024 a/מעבדה בתכנות מערכות/cprog/AsmblerProject/m.txt", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        /* Remove newline character */
        line[strcspn(line, "\n")] = '\0';

        if (strstr(line, "mcr ") == line) {  /* Check if line starts with "mcr" */
            in_macro = 1;
            macro_names = (char **)realloc(macro_names, (macro_count + 1) * sizeof(char *));
            if (macro_names == NULL) {
                printf("No memory\n");
                free_memory(macro_names, macro_count);
                free(macro_content);
                fclose(file);
                return -1;
            }
            macro_names[macro_count++] = extract_macro_name(line);
        } else if (strcmp(line, "endmcr") == 0) {
            in_macro = 0;
        } else if (in_macro) {
            /* Append line to macro content */
            new_content = (char *)realloc(macro_content, (macro_content ? strlen(macro_content) : 0) + strlen(line) + 2);
            if (new_content == NULL) {
                printf("No memory\n");
                free_memory(macro_names, macro_count);
                free(macro_content);
                fclose(file);
                return -1;
            }
            macro_content = new_content;
            strcat(macro_content, line);
            strcat(macro_content, "\n");
        }
    }

    fclose(file);

    /* Print macro names and content */
    printf("\nMacro names:\n");
    for (i = 0; i < macro_count; ++i) {
        printf("%s\n", macro_names[i]);
    }
    printf("\nMacro content:\n%s", macro_content);

    /* Free allocated memory */
    free_memory(macro_names, macro_count);
    free(macro_content);

    return 0;
}

char *extract_macro_name(const char *line) {
    const char *start = line + 4;  /* Skip "mcr " */
    char *name = (char *)malloc(strlen(start) + 1);
    if (name != NULL) {
        strcpy(name, start);
    }
    return name;
}

void free_memory(char **array, size_t count) {
    size_t i;
    for (i = 0; i < count; ++i) {
        free(array[i]);
    }
    free(array);
}
