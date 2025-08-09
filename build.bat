@echo off
REM Windows Build Script for RISC-V Matrix Extension Demo
REM Replaces Makefile functionality for Windows systems

setlocal EnableDelayedExpansion

echo RISC-V Matrix Extension Demo - Windows Build Script
echo ====================================================

REM Create build directory
if not exist "build" mkdir build

REM Check for GCC compiler
where gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: GCC compiler not found. Please install MinGW-w64 or similar.
    echo Download from: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

echo Found GCC compiler
gcc --version | findstr "gcc"

REM Build simulator
echo.
echo Building simulator...
gcc -Wall -Wextra -std=c99 -O2 -g -o build\matmul_simulator.exe simulator\matmul_simulator.c
if errorlevel 1 (
    echo ERROR: Failed to build simulator
    pause
    exit /b 1
)
echo ✓ Simulator built successfully

REM Build test runner
echo.
echo Building test runner...
gcc -Wall -Wextra -std=c99 -O2 -g -o build\test_runner.exe tests\test_matmul.c
if errorlevel 1 (
    echo ERROR: Failed to build test runner
    pause
    exit /b 1
)
echo ✓ Test runner built successfully

echo.
echo ====================================================
echo Build complete! Available commands:
echo   build.bat demo     - Run demonstration
echo   build.bat test     - Run test suite
echo   build.bat translate - Show SAIL to CGEN translation
echo   build.bat encoding  - Show instruction encoding
echo   build.bat clean     - Clean build artifacts
echo   build.bat help      - Show this help
echo ====================================================

REM Handle command line arguments
if "%1"=="demo" goto demo
if "%1"=="test" goto test
if "%1"=="translate" goto translate
if "%1"=="encoding" goto encoding
if "%1"=="clean" goto clean
if "%1"=="help" goto help
if "%1"=="" goto end

echo Unknown command: %1
goto help

:demo
echo.
echo === Running RISC-V MATMUL Demo ===
build\matmul_simulator.exe
goto end

:test
echo.
echo === Running Test Suite ===
build\test_runner.exe
goto end

:translate
echo.
echo === SAIL to CGEN Translation Demo ===
echo Input SAIL specification:
type specs\matmul_sail.spec | more
echo.
echo Generated CGEN description:
type cgen\matmul_cgen.scm | more
goto end

:encoding
echo.
echo === RISC-V MATMUL Instruction Encoding ===
echo Instruction: matmul rd, rs1, rs2
echo Format: R-type
echo Encoding: func7[31:25] ^| rs2[24:20] ^| rs1[19:15] ^| func3[14:12] ^| rd[11:7] ^| opcode[6:0]
echo Values:   0000001     ^| rs2       ^| rs1       ^| 111       ^| rd      ^| 0101011
echo.
echo Example: matmul x1, x2, x3
echo Binary:  0000001 00011 00010 111 00001 0101011
echo Hex:     0x0062F85B
goto end

:clean
echo.
echo === Cleaning Build Artifacts ===
if exist "build" rmdir /s /q build
echo Clean complete
goto end

:help
echo.
echo === RISC-V Matrix Extension Demo - Help ===
echo.
echo Available commands:
echo   build.bat          - Build everything (default)
echo   build.bat demo     - Run the matrix multiplication demonstration
echo   build.bat test     - Run the test suite
echo   build.bat translate - Show SAIL to CGEN translation example
echo   build.bat encoding  - Display instruction encoding details
echo   build.bat clean     - Remove build artifacts
echo   build.bat help      - Show this help message
echo.
echo Example usage:
echo   build.bat ^&^& build.bat demo
goto end

:end
echo.
