#pragma once
#include <vector>
#include <string>
#include "opcode.hpp"
#include "value.hpp"

struct Chunk {
    std::vector<uint8_t> code;        // raw bytecode
    std::vector<Value>   constants;   // constant pool
    std::vector<int>     lines;       // line number per byte
    std::vector<std::string> names;   // variable name pool

    // Write one byte of bytecode
    void write(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }

    // Write an opcode
    void writeOp(OpCode op, int line) {
        write(static_cast<uint8_t>(op), line);
    }

    // Add a constant to the pool, return its index
    int addConstant(Value val) {
        constants.push_back(val);
        return (int)constants.size() - 1;
    }

    // Add a variable name to the name pool, return its index
    int addName(const std::string& name) {
        // Reuse existing name if present
        for (int i = 0; i < (int)names.size(); i++) {
            if (names[i] == name) return i;
        }
        names.push_back(name);
        return (int)names.size() - 1;
    }

    // Get line number for a bytecode offset
    int getLine(int offset) const {
        if (offset < 0 || offset >= (int)lines.size()) return -1;
        return lines[offset];
    }

    int size() const { return (int)code.size(); }
};
