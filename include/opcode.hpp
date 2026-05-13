#pragma once
#include <cstdint>

// Each opcode is one byte.
// Comments show stack effect: [before] -> [after]
enum class OpCode : uint8_t {

    // Constants & literals
    OP_CONSTANT,      // [  ] -> [val]   operand: 1-byte index into constant pool
    OP_TRUE,          // [  ] -> [true]
    OP_FALSE,         // [  ] -> [false]
    OP_NIL,           // [  ] -> [nil]

    // Stack ops
    OP_POP,           // [val] -> []

    // Arithmetic
    OP_ADD,           // [a, b] -> [a+b]
    OP_SUB,           // [a, b] -> [a-b]
    OP_MUL,           // [a, b] -> [a*b]
    OP_DIV,           // [a, b] -> [a/b]
    OP_NEGATE,        // [a]    -> [-a]

    // Comparison
    OP_EQUAL,         // [a, b] -> [a==b]
    OP_NOT_EQUAL,     // [a, b] -> [a!=b]
    OP_LESS,          // [a, b] -> [a<b]
    OP_LESS_EQUAL,    // [a, b] -> [a<=b]
    OP_GREATER,       // [a, b] -> [a>b]
    OP_GREATER_EQUAL, // [a, b] -> [a>=b]

    // Logical
    OP_NOT,           // [a] -> [!a]

    // Variables - globals (by name index)
    OP_DEFINE_GLOBAL, // [val] -> []      operand: 1-byte name index
    OP_GET_GLOBAL,    // [  ]  -> [val]   operand: 1-byte name index
    OP_SET_GLOBAL,    // [val] -> [val]   operand: 1-byte name index

    // Variables - locals (by stack slot)
    OP_GET_LOCAL,     // [  ]  -> [val]   operand: 1-byte slot index
    OP_SET_LOCAL,     // [val] -> [val]   operand: 1-byte slot index

    // Control flow
    OP_JUMP,          // [  ] -> []       operand: 2-byte forward offset
    OP_JUMP_IF_FALSE, // [cond] -> [cond] operand: 2-byte forward offset
    OP_LOOP,          // [  ] -> []       operand: 2-byte backward offset

    // I/O
    OP_PRINT,         // [val] -> []
    OP_INPUT,         // [  ]  -> [val]

    // Sentinel
    OP_RETURN,        // [val] -> []

    // Must be last - used for array sizing
    OP_COUNT
};

// Convert opcode to string for disassembler
inline const char* opcodeName(OpCode op) {
    switch (op) {
        case OpCode::OP_CONSTANT:      return "OP_CONSTANT";
        case OpCode::OP_TRUE:          return "OP_TRUE";
        case OpCode::OP_FALSE:         return "OP_FALSE";
        case OpCode::OP_NIL:           return "OP_NIL";
        case OpCode::OP_POP:           return "OP_POP";
        case OpCode::OP_ADD:           return "OP_ADD";
        case OpCode::OP_SUB:           return "OP_SUB";
        case OpCode::OP_MUL:           return "OP_MUL";
        case OpCode::OP_DIV:           return "OP_DIV";
        case OpCode::OP_NEGATE:        return "OP_NEGATE";
        case OpCode::OP_EQUAL:         return "OP_EQUAL";
        case OpCode::OP_NOT_EQUAL:     return "OP_NOT_EQUAL";
        case OpCode::OP_LESS:          return "OP_LESS";
        case OpCode::OP_LESS_EQUAL:    return "OP_LESS_EQUAL";
        case OpCode::OP_GREATER:       return "OP_GREATER";
        case OpCode::OP_GREATER_EQUAL: return "OP_GREATER_EQUAL";
        case OpCode::OP_NOT:           return "OP_NOT";
        case OpCode::OP_DEFINE_GLOBAL: return "OP_DEFINE_GLOBAL";
        case OpCode::OP_GET_GLOBAL:    return "OP_GET_GLOBAL";
        case OpCode::OP_SET_GLOBAL:    return "OP_SET_GLOBAL";
        case OpCode::OP_GET_LOCAL:     return "OP_GET_LOCAL";
        case OpCode::OP_SET_LOCAL:     return "OP_SET_LOCAL";
        case OpCode::OP_JUMP:          return "OP_JUMP";
        case OpCode::OP_JUMP_IF_FALSE: return "OP_JUMP_IF_FALSE";
        case OpCode::OP_LOOP:          return "OP_LOOP";
        case OpCode::OP_PRINT:         return "OP_PRINT";
        case OpCode::OP_INPUT:         return "OP_INPUT";
        case OpCode::OP_RETURN:        return "OP_RETURN";
        default:                       return "OP_UNKNOWN";
    }
}
