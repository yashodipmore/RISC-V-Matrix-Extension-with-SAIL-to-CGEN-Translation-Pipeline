/* SAIL-style Specification for RISC-V Matrix Multiply Extension
 * This demonstrates the formal specification approach used in SAIL
 */

// Custom instruction format for MATMUL
// Format: matmul rd, rs1, rs2
// Encoding: R-type with custom func7 and func3

struct matmul_instruction {
    opcode: bits(7) = 0b0110011,  // R-type opcode
    rd: regidx,                   // destination register
    func3: bits(3) = 0b111,       // custom func3 for matmul
    rs1: regidx,                  // source register 1
    rs2: regidx,                  // source register 2  
    func7: bits(7) = 0b0000001    // custom func7 for matrix ops
}

// Instruction semantics
function execute_matmul(rd: regidx, rs1: regidx, rs2: regidx) -> unit = {
    // Read 2x2 matrices from memory pointed by rs1 and rs2
    let matrix_a = read_matrix_2x2(X(rs1));
    let matrix_b = read_matrix_2x2(X(rs2));
    
    // Perform matrix multiplication
    let result = matrix_multiply_2x2(matrix_a, matrix_b);
    
    // Store result matrix at memory location pointed by rd
    write_matrix_2x2(X(rd), result);
}

// Matrix operations helper functions
function read_matrix_2x2(addr: xlenbits) -> matrix_2x2 = {
    let m00 = mem_read(addr + 0,  4, false, false, false);
    let m01 = mem_read(addr + 4,  4, false, false, false);
    let m10 = mem_read(addr + 8,  4, false, false, false);
    let m11 = mem_read(addr + 12, 4, false, false, false);
    Matrix2x2(m00, m01, m10, m11)
}

function write_matrix_2x2(addr: xlenbits, matrix: matrix_2x2) -> unit = {
    mem_write(addr + 0,  4, matrix.m00, false, false, false);
    mem_write(addr + 4,  4, matrix.m01, false, false, false);
    mem_write(addr + 8,  4, matrix.m10, false, false, false);
    mem_write(addr + 12, 4, matrix.m11, false, false, false);
}

function matrix_multiply_2x2(a: matrix_2x2, b: matrix_2x2) -> matrix_2x2 = {
    let c00 = a.m00 * b.m00 + a.m01 * b.m10;
    let c01 = a.m00 * b.m01 + a.m01 * b.m11;
    let c10 = a.m10 * b.m00 + a.m11 * b.m10;
    let c11 = a.m10 * b.m01 + a.m11 * b.m11;
    Matrix2x2(c00, c01, c10, c11)
}

// Type definitions
union matrix_2x2 = Matrix2x2 : (bits(32), bits(32), bits(32), bits(32))

// Instruction mapping
mapping clause encdec = MATMUL(rd, rs1, rs2) 
  <-> 0b0000001 @ rs2 @ rs1 @ 0b111 @ rd @ 0b0110011

mapping clause assembly = MATMUL(rd, rs1, rs2) 
  <-> "matmul" ^ spc() ^ reg_name(rd) ^ sep() ^ reg_name(rs1) ^ sep() ^ reg_name(rs2)

mapping clause execute = MATMUL(rd, rs1, rs2) 
  <-> execute_matmul(rd, rs1, rs2)
