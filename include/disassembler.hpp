#pragma once
#include "chunk.hpp"

class Disassembler {
public:
    static void disassemble(const Chunk& chunk, const std::string& name);
    static int  disassembleInstruction(const Chunk& chunk, int offset);
};
