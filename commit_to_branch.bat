@echo off
REM Script to commit changes to branch hungTrinh
REM Usage: commit_to_branch.bat

echo ==========================================
echo Committing to branch: hungTrinh
echo ==========================================
echo.

REM Check if git is available
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: git not found!
    echo.
    echo Please install Git for Windows: https://git-scm.com/download/win
    echo Or add git to your PATH.
    pause
    exit /b 1
)

REM Check if this is a git repository
if not exist ".git" (
    echo Initializing git repository...
    git init
    echo.
)

echo Checking current branch...
git branch

echo.
echo Creating/checking out branch: hungTrinh
git checkout -b hungTrinh 2>nul
if %errorlevel% neq 0 (
    echo Branch already exists, switching to it...
    git checkout hungTrinh
)

echo.
echo Adding changed files...
git add riscv.c hashtable.c

echo.
echo Files to be committed:
git status --short

echo.
echo Committing changes...
git commit -m "Fix U-type parsing, add NOR support, fix hashtable NULL check

- Remove debug printf statements from parse() and parse_saveload()
- Add parse_utype() function for LUI instruction (2 operands)
- Add NOR instruction support (nor rd, rs1, rs2 = ~(rs1 | rs2))
- Move register 0 reset to end of step() function
- Add NULL check in ht_get() to prevent crashes"

if %errorlevel% equ 0 (
    echo.
    echo ==========================================
    echo Commit successful!
    echo ==========================================
    echo.
    echo Current branch: hungTrinh
    git branch --show-current
    echo.
    echo Latest commit:
    git log -1 --oneline
) else (
    echo.
    echo ERROR: Commit failed!
    echo Check the error message above.
)

pause

