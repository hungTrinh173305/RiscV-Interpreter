#!/bin/bash

# Script to commit changes to branch hungTrinh
# Usage: ./commit_to_branch.sh

echo "=========================================="
echo "Committing to branch: hungTrinh"
echo "=========================================="
echo ""

# Check if git is available
if ! command -v git &> /dev/null; then
    echo "ERROR: git not found!"
    echo ""
    echo "Please install git:"
    echo "  Ubuntu/Debian: sudo apt-get install git"
    echo "  macOS: xcode-select --install"
    exit 1
fi

# Check if this is a git repository
if [ ! -d ".git" ]; then
    echo "Initializing git repository..."
    git init
    echo ""
fi

echo "Checking current branch..."
git branch

echo ""
echo "Creating/checking out branch: hungTrinh"
git checkout -b hungTrinh 2>/dev/null || git checkout hungTrinh

echo ""
echo "Adding changed files..."
git add riscv.c hashtable.c

echo ""
echo "Files to be committed:"
git status --short

echo ""
echo "Committing changes..."
git commit -m "Fix U-type parsing, add NOR support, fix hashtable NULL check

- Remove debug printf statements from parse() and parse_saveload()
- Add parse_utype() function for LUI instruction (2 operands)
- Add NOR instruction support (nor rd, rs1, rs2 = ~(rs1 | rs2))
- Move register 0 reset to end of step() function
- Add NULL check in ht_get() to prevent crashes"

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "Commit successful!"
    echo "=========================================="
    echo ""
    echo "Current branch: $(git branch --show-current)"
    echo ""
    echo "Latest commit:"
    git log -1 --oneline
else
    echo ""
    echo "ERROR: Commit failed!"
    echo "Check the error message above."
fi

