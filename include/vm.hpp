#pragma once
#include <array>
#include <unordered_map>
#include <string>
#include "chunk.hpp"
#include "value.hpp"

#define STACK_MAX 256

enum class InterpretResult {
    OK,
    RUNTIME_ERROR
};

class VM {
public:
    VM();
    InterpretResult run(Chunk& chunk, bool trace = false);

private:
    // Stack
    std::array<Value, STACK_MAX> m_stack;
    int m_stackTop = 0;

    // Globals
    std::unordered_map<std::string, Value> m_globals;

    // Execution state
    Chunk*   m_chunk = nullptr;
    int      m_ip    = 0;

    // Opcode profiler
    std::array<uint64_t, static_cast<int>(OpCode::OP_COUNT)> m_opCounts;

    // Stack operations
    void        push(Value val);
    Value       pop();
    Value&      peek(int distance = 0);
    bool        stackOverflow();

    // Instruction reading
    uint8_t  readByte();
    uint16_t readShort();
    Value    readConstant();
    std::string readName();

    // Error reporting
    InterpretResult runtimeError(const std::string& msg);

    // Print the stack for trace mode
    void printStack();
};
