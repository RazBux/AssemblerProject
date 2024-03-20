# Assembler Project

This assembler is designed to translate assembly language programs into machine code. It's comprised of three key components: the Preassembler, the Assembler processor (with two distinct processes), and the Encryption module.

## Overview

- **Preassembler**: Handles macros, generating an intermediary file for the Assembler processor.
- **First Process**: Constructs the symbol table, defines constants, and initiates the bit file creation.
- **Second Process**: Finalizes the bit file for the assembler.
- **Encryption**: Secures the bit file data through encryption.

## How It Works

The assembler processes files in a sequential manner:

1. **Preassembler Stage**: Expands macros defined in the assembly code.
2. **First Process Stage**: Generates a symbol table and begins creating machine code.
3. **Second Process Stage**: Completes the machine code generation using the symbol table.
4. **Encryption Stage**: Applies encryption to the generated machine code for security.

### Prerequisites

- GCC compiler
- Basic command-line knowledge
