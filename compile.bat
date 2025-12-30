@echo off
REM Simple compile script for RISC-V Interpreter
REM This script compiles all necessary files

echo ==========================================
echo Compiling RISC-V Interpreter
echo ==========================================
echo.

REM Check if gcc is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: gcc not found!
    echo.
    echo Please install one of the following:
    echo   1. MinGW-w64: https://www.mingw-w64.org/
    echo   2. MSYS2: https://www.msys2.org/
    echo   3. WSL (Windows Subsystem for Linux)
    echo.
    echo Or use an online C compiler if needed.
    pause
    exit /b 1
)

echo Found gcc compiler!
echo.

REM Compile flags
set CFLAGS=-std=c99 -D_BSD_SOURCE -Wall -g

echo Compiling linkedlist.c...
gcc -c %CFLAGS% linkedlist.c -o linkedlist.o
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile linkedlist.c
    pause
    exit /b 1
)

echo Compiling hashtable.c...
gcc -c %CFLAGS% hashtable.c -o hashtable.o
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile hashtable.c
    pause
    exit /b 1
)

echo Compiling riscv.c...
gcc -c %CFLAGS% riscv.c -o riscv.o
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile riscv.c
    pause
    exit /b 1
)

echo Compiling riscv_interpreter.c...
gcc -c %CFLAGS% riscv_interpreter.c -o riscv_interpreter.o
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile riscv_interpreter.c
    pause
    exit /b 1
)

echo Linking riscv_interpreter...
gcc %CFLAGS% -Werror -o riscv_interpreter.exe linkedlist.o hashtable.o riscv.o riscv_interpreter.o -fsanitize=address
if %errorlevel% neq 0 (
    echo ERROR: Failed to link riscv_interpreter
    echo Trying without -fsanitize=address...
    gcc %CFLAGS% -Werror -o riscv_interpreter.exe linkedlist.o hashtable.o riscv.o riscv_interpreter.o
    if %errorlevel% neq 0 (
        echo ERROR: Failed to link riscv_interpreter
        pause
        exit /b 1
    )
)

echo.
echo ==========================================
echo Compilation successful!
echo ==========================================
echo.
echo You can now run: riscv_interpreter.exe ^< test1.txt
echo.

