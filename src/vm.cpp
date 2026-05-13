#include "vm.hpp"
#include <iostream>
#include <sstream>

VM::VM() {
    m_opCounts.fill(0);
}

void VM::push(Value val) {
    if (m_stackTop >= STACK_MAX) {
        std::cerr << "Stack overflow.\n";
        return;
    }
    m_stack[m_stackTop++] = val;
}

Value VM::pop() {
    return m_stack[--m_stackTop];
}

Value& VM::peek(int distance) {
    return m_stack[m_stackTop - 1 - distance];
}

uint8_t VM::readByte() {
    return m_chunk->code[m_ip++];
}

uint16_t VM::readShort() {
    uint16_t hi = readByte();
    uint16_t lo = readByte();
    return (hi << 8) | lo;
}

Value VM::readConstant() {
    uint8_t idx = readByte();
    return m_chunk->constants[idx];
}

std::string VM::readName() {
    uint8_t idx = readByte();
    return m_chunk->names[idx];
}

InterpretResult VM::runtimeError(const std::string& msg) {
    int line = m_chunk->getLine(m_ip - 1);
    std::cerr << "[line " << line << "] Runtime error: " << msg << "\n";
    return InterpretResult::RUNTIME_ERROR;
}

void VM::printStack() {
    std::cout << "          stack: [ ";
    for (int i = 0; i < m_stackTop; i++) {
        printValue(m_stack[i]);
        if (i < m_stackTop - 1) std::cout << " | ";
    }
    std::cout << " ]\n";
}

InterpretResult VM::run(Chunk& chunk, bool trace) {
    m_chunk = &chunk;
    m_ip    = 0;

    while (true) {
        if (trace) {
            printStack();
            int offset = m_ip;
            OpCode op = static_cast<OpCode>(chunk.code[offset]);
            std::cout << "[" << offset << "] " << opcodeName(op) << "\n";
        }

        uint8_t rawOp = readByte();
        OpCode instruction = static_cast<OpCode>(rawOp);
        m_opCounts[rawOp]++;

        switch (instruction) {

            case OpCode::OP_CONSTANT: {
                Value val = readConstant();
                push(val);
                break;
            }
            case OpCode::OP_TRUE:  push(true);             break;
            case OpCode::OP_FALSE: push(false);            break;
            case OpCode::OP_NIL:   push(std::monostate{}); break;
            case OpCode::OP_POP:   pop();                  break;

            case OpCode::OP_ADD: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) + std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '+'.");
                break;
            }
            case OpCode::OP_SUB: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) - std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '-'.");
                break;
            }
            case OpCode::OP_MUL: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) * std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '*'.");
                break;
            }
            case OpCode::OP_DIV: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b)) {
                    if (std::get<int64_t>(b) == 0) return runtimeError("Division by zero.");
                    push(std::get<int64_t>(a) / std::get<int64_t>(b));
                } else return runtimeError("Operands must be integers for '/'.");
                break;
            }
            case OpCode::OP_NEGATE: {
                Value a = pop();
                if (std::holds_alternative<int64_t>(a))
                    push(-std::get<int64_t>(a));
                else return runtimeError("Operand must be an integer for negation.");
                break;
            }

            case OpCode::OP_EQUAL: {
                Value b = pop(); Value a = pop();
                push(isEqual(a, b));
                break;
            }
            case OpCode::OP_NOT_EQUAL: {
                Value b = pop(); Value a = pop();
                push(!isTruthy(Value(isEqual(a, b))));
                break;
            }
            case OpCode::OP_LESS: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) < std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '<'.");
                break;
            }
            case OpCode::OP_LESS_EQUAL: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) <= std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '<='.");
                break;
            }
            case OpCode::OP_GREATER: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) > std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '>'.");
                break;
            }
            case OpCode::OP_GREATER_EQUAL: {
                Value b = pop(); Value a = pop();
                if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b))
                    push(std::get<int64_t>(a) >= std::get<int64_t>(b));
                else return runtimeError("Operands must be integers for '>='.");
                break;
            }
            case OpCode::OP_NOT: {
                Value a = pop();
                push(!isTruthy(a));
                break;
            }

            case OpCode::OP_DEFINE_GLOBAL: {
                std::string name = readName();
                m_globals[name] = pop();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                std::string name = readName();
                auto it = m_globals.find(name);
                if (it == m_globals.end())
                    return runtimeError("Undefined variable '" + name + "'.");
                push(it->second);
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                std::string name = readName();
                if (m_globals.find(name) == m_globals.end())
                    return runtimeError("Undefined variable '" + name + "'.");
                m_globals[name] = peek(0);
                break;
            }
            case OpCode::OP_GET_LOCAL: {
                uint8_t slot = readByte();
                push(m_stack[slot]);
                break;
            }
            case OpCode::OP_SET_LOCAL: {
                uint8_t slot = readByte();
                m_stack[slot] = peek(0);
                break;
            }

            case OpCode::OP_JUMP: {
                uint16_t offset = readShort();
                m_ip += offset;
                break;
            }
            case OpCode::OP_JUMP_IF_FALSE: {
                uint16_t offset = readShort();
                if (!isTruthy(peek(0))) m_ip += offset;
                break;
            }
            case OpCode::OP_LOOP: {
                uint16_t offset = readShort();
                m_ip -= offset;
                break;
            }

            case OpCode::OP_PRINT: {
                printValue(pop());
                std::cout << "\n";
                break;
            }
            case OpCode::OP_INPUT: {
                std::string line;
                std::getline(std::cin, line);
                try {
                    push((int64_t)std::stoll(line));
                } catch (...) {
                    push(std::monostate{});
                }
                break;
            }

            case OpCode::OP_RETURN: {
                return InterpretResult::OK;
            }

            default:
                return runtimeError("Unknown opcode.");
        }
    }
}
