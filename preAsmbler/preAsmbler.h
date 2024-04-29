#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include <stdio.h> 


/** Structure to store macros including arrays for macro names and texts */
typedef struct {
    char **names;    /* Array of macro names */
    char **texts;    /* Array of macro text replacements */
    size_t size;     /* Allocated size of arrays */
    size_t count;    /* Number of stored macros */
} MacroStorage;


/** 
 * Initializes the macro storage with initial capacity.
 * @param storage Pointer to the MacroStorage structure.
 */
void init_macro_storage(MacroStorage *storage);


/** 
 * Cleans input text by removing unnecessary whitespace.
 * @param src Pointer to the original text.
 * @return Pointer to the cleaned text. The caller is responsible for freeing this memory.
 */
char *clean_text(const char *src);


/** 
 * Starts the pre-assembler process, reading and processing macros from the input file.
 * @param inputFileName Path to the input assembler file.
 * @param outputFileName Path to the output file where the processed text will be stored.
 * @return Integer indicating the success or failure of the process (0 for success, non-zero for failure).
 */
int startPreAsmbler(char *inputFileName, char *outputFileName);

#endif /* PRE_ASSEMBLER_H */
