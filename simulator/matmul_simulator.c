#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// RISC-V Matrix Extension Simulator
// Implements the MATMUL instruction for 2x2 matrix multiplication

typedef struct {
    int32_t m[2][2];
} matrix_2x2_t;

typedef struct {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t func3  : 3;
    uint32_t rs1    : 5;
    uint32_t rs2    : 5;
    uint32_t func7  : 7;
} r_type_inst_t;

typedef struct {
    uint32_t regs[32];
    uint8_t *memory;
    size_t memory_size;
} cpu_state_t;

// Constants for MATMUL instruction
#define OPCODE_CUSTOM_1  0x2B
#define FUNC3_MATMUL     0x7
#define FUNC7_MATMUL     0x1

// Initialize CPU state
cpu_state_t* init_cpu(size_t memory_size) {
    cpu_state_t *cpu = malloc(sizeof(cpu_state_t));
    if (!cpu) return NULL;
    
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->memory = calloc(memory_size, 1);
    cpu->memory_size = memory_size;
    
    if (!cpu->memory) {
        free(cpu);
        return NULL;
    }
    
    return cpu;
}

void free_cpu(cpu_state_t *cpu) {
    if (cpu) {
        free(cpu->memory);
        free(cpu);
    }
}

// Memory access functions
int32_t read_word(cpu_state_t *cpu, uint32_t addr) {
    if (addr + 3 >= cpu->memory_size) {
        printf("ERROR: Memory access out of bounds: 0x%x\n", addr);
        return 0;
    }
    return *(int32_t*)(cpu->memory + addr);
}

void write_word(cpu_state_t *cpu, uint32_t addr, int32_t value) {
    if (addr + 3 >= cpu->memory_size) {
        printf("ERROR: Memory write out of bounds: 0x%x\n", addr);
        return;
    }
    *(int32_t*)(cpu->memory + addr) = value;
}

// Matrix operations
matrix_2x2_t read_matrix_2x2(cpu_state_t *cpu, uint32_t addr) {
    matrix_2x2_t matrix;
    matrix.m[0][0] = read_word(cpu, addr + 0);
    matrix.m[0][1] = read_word(cpu, addr + 4);
    matrix.m[1][0] = read_word(cpu, addr + 8);
    matrix.m[1][1] = read_word(cpu, addr + 12);
    return matrix;
}

void write_matrix_2x2(cpu_state_t *cpu, uint32_t addr, matrix_2x2_t matrix) {
    write_word(cpu, addr + 0,  matrix.m[0][0]);
    write_word(cpu, addr + 4,  matrix.m[0][1]);
    write_word(cpu, addr + 8,  matrix.m[1][0]);
    write_word(cpu, addr + 12, matrix.m[1][1]);
}

matrix_2x2_t matrix_multiply_2x2(matrix_2x2_t a, matrix_2x2_t b) {
    matrix_2x2_t result;
    
    result.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0];
    result.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1];
    result.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0];
    result.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1];
    
    return result;
}

// Instruction decode
r_type_inst_t decode_r_type(uint32_t instruction) {
    r_type_inst_t inst;
    inst.opcode = instruction & 0x7F;
    inst.rd     = (instruction >> 7) & 0x1F;
    inst.func3  = (instruction >> 12) & 0x7;
    inst.rs1    = (instruction >> 15) & 0x1F;
    inst.rs2    = (instruction >> 20) & 0x1F;
    inst.func7  = (instruction >> 25) & 0x7F;
    return inst;
}

// MATMUL instruction implementation
int execute_matmul(cpu_state_t *cpu, r_type_inst_t inst) {
    printf("Executing MATMUL: rd=x%d, rs1=x%d, rs2=x%d\n", 
           inst.rd, inst.rs1, inst.rs2);
    
    // Get memory addresses from registers
    uint32_t addr_a = cpu->regs[inst.rs1];
    uint32_t addr_b = cpu->regs[inst.rs2];
    uint32_t addr_result = cpu->regs[inst.rd];
    
    printf("  Matrix A address: 0x%x\n", addr_a);
    printf("  Matrix B address: 0x%x\n", addr_b);
    printf("  Result address: 0x%x\n", addr_result);
    
    // Read matrices from memory
    matrix_2x2_t matrix_a = read_matrix_2x2(cpu, addr_a);
    matrix_2x2_t matrix_b = read_matrix_2x2(cpu, addr_b);
    
    printf("  Matrix A: [[%d, %d], [%d, %d]]\n",
           matrix_a.m[0][0], matrix_a.m[0][1],
           matrix_a.m[1][0], matrix_a.m[1][1]);
    printf("  Matrix B: [[%d, %d], [%d, %d]]\n",
           matrix_b.m[0][0], matrix_b.m[0][1],
           matrix_b.m[1][0], matrix_b.m[1][1]);
    
    // Perform matrix multiplication
    matrix_2x2_t result = matrix_multiply_2x2(matrix_a, matrix_b);
    
    printf("  Result:   [[%d, %d], [%d, %d]]\n",
           result.m[0][0], result.m[0][1],
           result.m[1][0], result.m[1][1]);
    
    // Write result to memory
    write_matrix_2x2(cpu, addr_result, result);
    
    return 0;
}

// Main instruction execution function
int execute_instruction(cpu_state_t *cpu, uint32_t instruction) {
    r_type_inst_t inst = decode_r_type(instruction);
    
    // Check if this is our custom MATMUL instruction
    if (inst.opcode == OPCODE_CUSTOM_1 && 
        inst.func3 == FUNC3_MATMUL && 
        inst.func7 == FUNC7_MATMUL) {
        return execute_matmul(cpu, inst);
    }
    
    printf("ERROR: Unknown instruction: 0x%08x\n", instruction);
    return -1;
}

// Utility function to print matrix from memory
void print_matrix_at_address(cpu_state_t *cpu, uint32_t addr, const char* name) {
    matrix_2x2_t matrix = read_matrix_2x2(cpu, addr);
    printf("%s at 0x%x: [[%d, %d], [%d, %d]]\n", name, addr,
           matrix.m[0][0], matrix.m[0][1],
           matrix.m[1][0], matrix.m[1][1]);
}

// Demo function
void run_matmul_demo(cpu_state_t *cpu) {
    printf("=== RISC-V MATMUL Instruction Demo ===\n\n");
    
    // Set up test matrices in memory
    uint32_t addr_a = 0x1000;
    uint32_t addr_b = 0x1020;
    uint32_t addr_result = 0x1040;
    
    // Matrix A = [[1, 2], [3, 4]]
    write_word(cpu, addr_a + 0,  1);
    write_word(cpu, addr_a + 4,  2);
    write_word(cpu, addr_a + 8,  3);
    write_word(cpu, addr_a + 12, 4);
    
    // Matrix B = [[5, 6], [7, 8]]
    write_word(cpu, addr_b + 0,  5);
    write_word(cpu, addr_b + 4,  6);
    write_word(cpu, addr_b + 8,  7);
    write_word(cpu, addr_b + 12, 8);
    
    // Set up registers (rd=x1, rs1=x2, rs2=x3)
    cpu->regs[1] = addr_result;  // rd
    cpu->regs[2] = addr_a;       // rs1
    cpu->regs[3] = addr_b;       // rs2
    
    printf("Initial setup:\n");
    print_matrix_at_address(cpu, addr_a, "Matrix A");
    print_matrix_at_address(cpu, addr_b, "Matrix B");
    printf("\n");
    
    // Create MATMUL instruction: func7=1, rs2=3, rs1=2, func3=7, rd=1, opcode=0x2B
    uint32_t matmul_inst = (FUNC7_MATMUL << 25) | (3 << 20) | (2 << 15) | 
                          (FUNC3_MATMUL << 12) | (1 << 7) | OPCODE_CUSTOM_1;
    
    printf("Instruction encoding: 0x%08x\n", matmul_inst);
    printf("Executing instruction...\n\n");
    
    // Execute the instruction
    if (execute_instruction(cpu, matmul_inst) == 0) {
        printf("\nResult after execution:\n");
        print_matrix_at_address(cpu, addr_result, "Result Matrix");
        printf("\nExpected: [[19, 22], [43, 50]] (1*5+2*7=19, 1*6+2*8=22, etc.)\n");
    }
}

int main() {
    cpu_state_t *cpu = init_cpu(64 * 1024);  // 64KB memory
    if (!cpu) {
        printf("Failed to initialize CPU\n");
        return 1;
    }
    
    run_matmul_demo(cpu);
    
    free_cpu(cpu);
    return 0;
}
