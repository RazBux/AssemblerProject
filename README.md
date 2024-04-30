
# Assembler Project Overview

This document provides a comprehensive overview of the assembler project, detailing the processes and modules involved.

## Components Overview

The assembler project consists of several components that work together to translate assembly language into machine code. The process is divided into multiple stages:

1. **Pre-Assembler**
2. **First Process (First Pass)**
3. **Second Process (Second Pass)**
4. **Encryption and File Handling**
5. **Utility Modules**

### 1. Pre-Assembler

The pre-assembler handles macro definitions and prepares the assembly file by expanding macros. It processes the input file to replace macro calls with their definitions to simplify the assembly process.

### 2. First Process (First Pass)

The first pass scans the assembly code to define labels and calculate their memory addresses. It builds a symbol table that maps each label to a specific address in memory, preparing for the actual code translation.

### 3. Second Process (Second Pass)

During the second pass, the assembler translates assembly instructions into machine code using the symbol table created in the first pass. It resolves addresses and handles instruction encoding based on the operation codes and operands specified.

### 4. Encryption and File Handling

After the assembly process, additional steps such as encryption or formatting may be performed to prepare the output files for deployment or further processing.

### 5. Utility Modules

Utility functions are crucial for the operation of the assembler. These include:
- `util.c/h`: Common utility functions for string manipulation, file handling, and other generic tasks.
- `printBinary.c/h`: Functions to convert numerical data into binary strings suitable for output.
- `dataCodeTable.c/h`: Manages tables storing data and code information during the assembly process.

## Build Instructions

To build the project, use the provided makefile with commands:
```
make all
```
This compiles all necessary source files and links them into the final executable.

## Usage

To run the assembler, use the following command:
```
./assembler inputfile.asm
```
Replace `inputfile.asm` with the path to your assembly file.

## Additional Notes

The project includes detailed error checking and handling to ensure the assembly process is robust against common syntax and semantic errors found in assembly code.

---

This README aims to guide users through the setup, operation, and understanding of the assembler project.
