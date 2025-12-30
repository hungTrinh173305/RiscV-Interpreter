#!/bin/bash

# Simple compile script for RISC-V Interpreter
# This script compiles all necessary files

echo "=========================================="
echo "Compiling RISC-V Interpreter"
echo "=========================================="
echo ""

# Check if gcc is available
if ! command -v gcc &> /dev/null; then
    echo "ERROR: gcc not found!"
    echo ""
    echo "Please install gcc:"
    echo "  Ubuntu/Debian: sudo apt-get install gcc make"
    echo "  macOS: xcode-select --install"
    echo "  Or use your package manager"
    exit 1
fi

echo "Found gcc compiler!"
echo ""

# Compile flags
CFLAGS="-std=c99 -D_BSD_SOURCE -Wall -g"

echo "Compiling linkedlist.c..."
gcc -c $CFLAGS linkedlist.c -o linkedlist.o
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile linkedlist.c"
    exit 1
fi

echo "Compiling hashtable.c..."
gcc -c $CFLAGS hashtable.c -o hashtable.o
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile hashtable.c"
    exit 1
fi

echo "Compiling riscv.c..."
gcc -c $CFLAGS riscv.c -o riscv.o
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile riscv.c"
    exit 1
fi

echo "Compiling riscv_interpreter.c..."
gcc -c $CFLAGS riscv_interpreter.c -o riscv_interpreter.o
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile riscv_interpreter.c"
    exit 1
fi

echo "Linking riscv_interpreter..."
gcc $CFLAGS -Werror -o riscv_interpreter linkedlist.o hashtable.o riscv.o riscv_interpreter.o -fsanitize=address
if [ $? -ne 0 ]; then
    echo "WARNING: Failed with -fsanitize=address, trying without..."
    gcc $CFLAGS -Werror -o riscv_interpreter linkedlist.o hashtable.o riscv.o riscv_interpreter.o
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to link riscv_interpreter"
        exit 1
    fi
fi

echo ""
echo "=========================================="
echo "Compilation successful!"
echo "=========================================="
echo ""
echo "You can now run: ./riscv_interpreter < test1.txt"
echo ""

