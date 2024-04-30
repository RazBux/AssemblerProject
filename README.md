
# Assembler Project Overview

This README provides an overview of the assembler project, detailing the processes and modules involved in translating assembly language into machine code.

## Components Overview

The project is structured into several key stages:

1. **Pre-Assembler**: Handles macro expansions by processing the input file to replace macro calls with their definitions.

2. **First Process (First Pass)**: Defines labels and calculates memory addresses, building a symbol table for subsequent translation.

3. **Second Process (Second Pass)**: Translates assembly instructions into machine code, resolving addresses using the previously built symbol table.

4. **Encryption and File Handling**: Applies additional processing such as encryption and formats output files for deployment.

5. **Utility Modules**: Provides essential services like string manipulation, file handling, and binary conversions.
    - `util.c/h`: Common utilities.
    - `printBinary.c/h`: Binary string conversion functions.
    - `dataCodeTable.c/h`: Manages data and code information tables.

## Build Instructions

Use the makefile provided to compile and link the source files:
```
make
```
To clean up generated files:
```
make clean
```

## Usage

To run the assembler on single or multiple files:
```
./asmbler file1 file2 ...
```
For all files in the `testFile` directory:
```
make run
```

## Additional Notes

The project includes robust error handling to manage common syntax and semantic errors in assembly code.

--- 
