#include "disassembler.hpp"
#include <iostream>
#include <iomanip>

void Disassembler::disassemble(const Chunk& chunk, const std::string& name) {
    std::cout << "=== " << name << " ===\n";
    std::cout << std::left
              << std::setw(6)  << "OFFSET"
              << std::setw(6)  << "LINE"
              << std::setw(22) << "OPCODE"
              << "OPERAND\n";
    std::cout << std::string(50, '-') << "\n";

    int offset = 0;
    while (offset < (int)chunk.code.size()) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int Disassembler::disassembleInstruction(const Chunk& chunk, int offset) {
    // Print offset
    std::cout << std::setw(4) << std::setfill('0') << offset << "  "
              << std::setfill(' ');

    // Print line number
    int line = chunk.getLine(offset);
    std::cout << std::setw(4) << line << "  ";

    OpCode op = static_cast<OpCode>(chunk.code[offset]);
    std::cout << std::setw(22) << std::left << opcodeName(op);

    switch (op) {
        // 1-byte operand: constant index
        case OpCode::OP_CONSTANT: {
            uint8_t idx = chunk.code[offset + 1];
            std::cout << "idx=" << (int)idx << "  value=";
            printValue(chunk.constants[idx]);
            std::cout << "\n";
            return offset + 2;
        }
        // 1-byte operand: name index
        case OpCode::OP_DEFINE_GLOBAL:
        case OpCode::OP_GET_GLOBAL:
        case OpCode::OP_SET_GLOBAL: {
            uint8_t idx = chunk.code[offset + 1];
            std::cout << "name='" << chunk.names[idx] << "'\n";
            return offset + 2;
        }
        // 1-byte operand: stack slot
        case OpCode::OP_GET_LOCAL:
        case OpCode::OP_SET_LOCAL: {
            uint8_t slot = chunk.code[offset + 1];
            std::cout << "slot=" << (int)slot << "\n";
            return offset + 2;
        }
        // 2-byte operand: jump offset
        case OpCode::OP_JUMP:
        case OpCode::OP_JUMP_IF_FALSE: {
            uint16_t jump = (chunk.code[offset+1] << 8) | chunk.code[offset+2];
            int target = offset + 3 + jump;
            std::cout << "offset=" << jump << "  -> " << target << "\n";
            return offset + 3;
        }
        case OpCode::OP_LOOP: {
            uint16_t jump = (chunk.code[offset+1] << 8) | chunk.code[offset+2];
            int target = offset + 3 - jump;
            std::cout << "offset=" << jump << "  -> " << target << "\n";
            return offset + 3;
        }
        // No operand
        default:
            std::cout << "\n";
            return offset + 1;
    }
}
