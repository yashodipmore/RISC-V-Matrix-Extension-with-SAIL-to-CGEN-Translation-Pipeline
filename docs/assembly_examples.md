# Assembly Examples for RISC-V Matrix Extension

## Basic Matrix Multiplication Example

```assembly
# RISC-V Assembly example using MATMUL instruction
# Multiply two 2x2 matrices stored in memory

.data
# Matrix A: [[1, 2], [3, 4]]
matrix_a:
    .word 1, 2
    .word 3, 4

# Matrix B: [[5, 6], [7, 8]]  
matrix_b:
    .word 5, 6
    .word 7, 8

# Result matrix C: [[0, 0], [0, 0]] (will be computed)
matrix_c:
    .word 0, 0
    .word 0, 0

.text
.globl _start

_start:
    # Load addresses into registers
    la x1, matrix_a      # x1 = address of matrix A
    la x2, matrix_b      # x2 = address of matrix B  
    la x3, matrix_c      # x3 = address of result matrix C
    
    # Execute matrix multiplication
    # matmul rd, rs1, rs2 -> matmul x3, x1, x2
    # This multiplies matrices at x1 and x2, stores result at x3
    .word 0x0020F1AB     # matmul x3, x1, x2 (encoded instruction)
    
    # Expected result in matrix_c: [[19, 22], [43, 50]]
    # 19 = 1*5 + 2*7, 22 = 1*6 + 2*8
    # 43 = 3*5 + 4*7, 50 = 3*6 + 4*8
    
    # Exit program
    li a7, 93           # sys_exit
    li a0, 0            # exit code
    ecall
```

## Function Call Example

```assembly
# Matrix multiplication as a function call
# void matmul_2x2(int32_t *result, int32_t *a, int32_t *b)

.text
.globl matmul_2x2

matmul_2x2:
    # Arguments:
    # a0 = result matrix address
    # a1 = matrix A address
    # a2 = matrix B address
    
    # Save return address
    addi sp, sp, -8
    sd ra, 0(sp)
    
    # Copy addresses to appropriate registers for MATMUL
    mv x3, a0           # result address -> x3
    mv x1, a1           # matrix A address -> x1
    mv x2, a2           # matrix B address -> x2
    
    # Execute matrix multiplication
    .word 0x0020F1AB    # matmul x3, x1, x2
    
    # Restore return address and return
    ld ra, 0(sp)
    addi sp, sp, 8
    ret
```

## Optimized Loop Example

```assembly
# Matrix multiplication with multiple matrices
# for (int i = 0; i < n; i++) {
#     C[i] = A[i] * B[i];
# }

.text
.globl batch_matmul

batch_matmul:
    # Arguments:
    # a0 = number of matrices (n)
    # a1 = array of result matrices
    # a2 = array of A matrices
    # a3 = array of B matrices
    
    mv t0, zero         # i = 0
    mv t1, a0           # n
    mv t2, a1           # result array
    mv t3, a2           # A array
    mv t4, a3           # B array
    
loop:
    beq t0, t1, done    # if i == n, exit loop
    
    # Calculate addresses for current iteration
    slli t5, t0, 4      # i * 16 (16 bytes per 2x2 matrix)
    add x1, t3, t5      # A[i] address
    add x2, t4, t5      # B[i] address
    add x3, t2, t5      # C[i] address
    
    # Execute matrix multiplication
    .word 0x0020F1AB    # matmul x3, x1, x2
    
    # Increment loop counter
    addi t0, t0, 1      # i++
    j loop
    
done:
    ret
```

## Error Handling Example

```assembly
# Matrix multiplication with bounds checking
# int safe_matmul(uint32_t result_addr, uint32_t a_addr, uint32_t b_addr)

.text
.globl safe_matmul

safe_matmul:
    # Arguments:
    # a0 = result address
    # a1 = matrix A address  
    # a2 = matrix B address
    
    # Check for null pointers
    beqz a0, error      # if result_addr == 0, error
    beqz a1, error      # if a_addr == 0, error
    beqz a2, error      # if b_addr == 0, error
    
    # Check alignment (addresses must be 4-byte aligned)
    andi t0, a0, 3
    bnez t0, error      # if result_addr not aligned, error
    andi t0, a1, 3
    bnez t0, error      # if a_addr not aligned, error
    andi t0, a2, 3
    bnez t0, error      # if b_addr not aligned, error
    
    # Setup registers for MATMUL
    mv x3, a0           # result address
    mv x1, a1           # matrix A address
    mv x2, a2           # matrix B address
    
    # Execute matrix multiplication
    .word 0x0020F1AB    # matmul x3, x1, x2
    
    # Return success
    li a0, 0            # return 0 (success)
    ret
    
error:
    li a0, -1           # return -1 (error)
    ret
```

## Instruction Encoding Reference

```assembly
# MATMUL instruction encoding breakdown
# Format: matmul rd, rs1, rs2
# 
# 31    25|24  20|19  15|14 12|11  7|6    0
# func7   | rs2  | rs1  |func3| rd  |opcode
# 0000001 | rs2  | rs1  | 111 | rd  |0101011
#
# Examples:
# matmul x1, x2, x3  -> .word 0x0032F8AB
# matmul x4, x5, x6  -> .word 0x006F5AAB  
# matmul x7, x8, x9  -> .word 0x00943BAB

.text
examples:
    .word 0x0032F8AB    # matmul x1, x2, x3
    .word 0x006F5AAB    # matmul x4, x5, x6
    .word 0x00943BAB    # matmul x7, x8, x9
```

## Performance Benchmark Assembly

```assembly
# Benchmark matrix multiplication performance
# Measures cycles for 1000 matrix operations

.data
benchmark_matrices:
    # 1000 pairs of test matrices
    .space 32000        # 1000 * 2 * 16 bytes

result_matrices:
    .space 16000        # 1000 * 16 bytes

.text
.globl benchmark_matmul

benchmark_matmul:
    # Setup
    la t0, benchmark_matrices
    la t1, result_matrices
    li t2, 1000         # iteration count
    
    # Read cycle counter start
    rdcycle t3          # start cycles
    
benchmark_loop:
    beqz t2, benchmark_done
    
    # Setup matrix addresses
    mv x1, t0           # matrix A
    addi x2, t0, 16     # matrix B (16 bytes after A)
    mv x3, t1           # result
    
    # Execute MATMUL
    .word 0x0020F1AB    # matmul x3, x1, x2
    
    # Advance pointers
    addi t0, t0, 32     # next pair of input matrices
    addi t1, t1, 16     # next result matrix
    addi t2, t2, -1     # decrement counter
    
    j benchmark_loop
    
benchmark_done:
    # Read cycle counter end
    rdcycle t4          # end cycles
    sub a0, t4, t3      # return cycle count
    ret
```
