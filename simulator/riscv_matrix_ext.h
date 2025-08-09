/**
 * RISC-V Matrix Extension Header
 * Defines types and constants for custom matrix instructions
 */

#ifndef RISCV_MATRIX_EXT_H
#define RISCV_MATRIX_EXT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Matrix dimension constants
#define MATRIX_DIM 2
#define MATRIX_SIZE (MATRIX_DIM * MATRIX_DIM)
#define MATRIX_BYTES (MATRIX_SIZE * sizeof(int32_t))

// Instruction encoding constants
#define OPCODE_CUSTOM_1  0x2B
#define FUNC3_MATMUL     0x7
#define FUNC7_MATMUL     0x1

// Register file size
#define NUM_REGISTERS 32
#define REGISTER_BITS 5

// Memory configuration
#define DEFAULT_MEMORY_SIZE (64 * 1024)  // 64KB
#define MEMORY_ALIGNMENT 4

// Matrix data structure
typedef struct {
    int32_t m[MATRIX_DIM][MATRIX_DIM];
} matrix_2x2_t;

// RISC-V instruction formats
typedef union {
    uint32_t raw;
    struct {
        uint32_t opcode : 7;
        uint32_t rd     : 5;
        uint32_t func3  : 3;
        uint32_t rs1    : 5;
        uint32_t rs2    : 5;
        uint32_t func7  : 7;
    } r_type;
} riscv_instruction_t;

// CPU state representation
typedef struct {
    uint32_t regs[NUM_REGISTERS];
    uint32_t pc;
    uint8_t *memory;
    size_t memory_size;
    bool debug_enabled;
} cpu_state_t;

// Function prototypes

// CPU management
cpu_state_t* riscv_cpu_init(size_t memory_size);
void riscv_cpu_free(cpu_state_t *cpu);
void riscv_cpu_reset(cpu_state_t *cpu);

// Memory operations
int32_t riscv_mem_read_word(cpu_state_t *cpu, uint32_t addr);
void riscv_mem_write_word(cpu_state_t *cpu, uint32_t addr, int32_t value);
bool riscv_mem_check_bounds(cpu_state_t *cpu, uint32_t addr, size_t size);

// Matrix operations
matrix_2x2_t riscv_matrix_read(cpu_state_t *cpu, uint32_t addr);
void riscv_matrix_write(cpu_state_t *cpu, uint32_t addr, matrix_2x2_t matrix);
matrix_2x2_t riscv_matrix_multiply(matrix_2x2_t a, matrix_2x2_t b);
void riscv_matrix_print(matrix_2x2_t matrix, const char* name);

// Instruction execution
int riscv_execute_matmul(cpu_state_t *cpu, uint32_t rd, uint32_t rs1, uint32_t rs2);
int riscv_execute_instruction(cpu_state_t *cpu, uint32_t instruction);

// Instruction encoding/decoding
riscv_instruction_t riscv_decode_instruction(uint32_t raw);
uint32_t riscv_encode_matmul(uint32_t rd, uint32_t rs1, uint32_t rs2);

// Debugging and tracing
void riscv_debug_enable(cpu_state_t *cpu, bool enable);
void riscv_debug_print_state(cpu_state_t *cpu);
void riscv_debug_print_instruction(uint32_t instruction);

// Error codes
#define RISCV_SUCCESS           0
#define RISCV_ERROR_MEMORY     -1
#define RISCV_ERROR_ALIGNMENT  -2
#define RISCV_ERROR_BOUNDS     -3
#define RISCV_ERROR_INSTRUCTION -4

// Utility macros
#define RISCV_ALIGN_4(addr) (((addr) + 3) & ~3)
#define RISCV_IS_ALIGNED_4(addr) (((addr) & 3) == 0)

// Register name constants for debugging
extern const char* riscv_register_names[NUM_REGISTERS];

// SAIL specification compliance markers
#define SAIL_INSTRUCTION_FORMAT  r_type
#define SAIL_MEMORY_MODEL        word_addressed
#define SAIL_REGISTER_FILE       x0_x31

// CGEN integration markers
#define CGEN_INSTRUCTION_CLASS   "matrix_ops"
#define CGEN_SEMANTIC_FUNCTION   "matrix_multiply_2x2"
#define CGEN_OPTIMIZATION_LEVEL  3

#ifdef __cplusplus
}
#endif

#endif /* RISCV_MATRIX_EXT_H */
