#!/bin/bash

# Script to run all test files for RISC-V Interpreter
# Usage: ./run_tests.sh

echo "=========================================="
echo "RISC-V Interpreter Test Suite"
echo "=========================================="
echo ""

# Check if executable exists
if [ ! -f "./riscv_interpreter" ]; then
    echo "Error: riscv_interpreter not found!"
    echo "Please run 'make riscv_interpreter' first."
    exit 1
fi

# List of test files
tests=(
    "test1.txt"
    "test_program.txt"
    "test_rtype.txt"
    "test_itype.txt"
    "test_lui.txt"
    "test_mem.txt"
    "test_sb.txt"
    "test_abi.txt"
    "test_edge_cases.txt"
    "test_comprehensive.txt"
    "test_nor.txt"
)

# Run each test
for test in "${tests[@]}"; do
    if [ -f "$test" ]; then
        echo "----------------------------------------"
        echo "Running: $test"
        echo "----------------------------------------"
        ./riscv_interpreter < "$test"
        echo ""
    else
        echo "Warning: $test not found, skipping..."
        echo ""
    fi
done

echo "=========================================="
echo "All tests completed!"
echo "=========================================="

