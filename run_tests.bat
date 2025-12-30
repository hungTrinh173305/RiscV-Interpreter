@echo off
REM Script to run all test files for RISC-V Interpreter on Windows
REM Usage: run_tests.bat

echo ==========================================
echo RISC-V Interpreter Test Suite
echo ==========================================
echo.

REM Check if executable exists
if not exist "riscv_interpreter.exe" (
    echo Error: riscv_interpreter.exe not found!
    echo Please run 'make riscv_interpreter' first.
    exit /b 1
)

REM List of test files
set tests=test1.txt test_program.txt test_rtype.txt test_itype.txt test_lui.txt test_mem.txt test_sb.txt test_abi.txt test_edge_cases.txt test_comprehensive.txt test_nor.txt

REM Run each test
for %%f in (%tests%) do (
    if exist "%%f" (
        echo ----------------------------------------
        echo Running: %%f
        echo ----------------------------------------
        riscv_interpreter.exe ^< "%%f"
        echo.
    ) else (
        echo Warning: %%f not found, skipping...
        echo.
    )
)

echo ==========================================
echo All tests completed!
echo ==========================================

