#include "compiler.hpp"
#include <iostream>
#include <stdexcept>

Compiler::Compiler() {}

Chunk Compiler::compile(std::vector<StmtPtr>& stmts) {
    for (auto& stmt : stmts) {
        stmt->accept(*this);
    }
    emit(OpCode::OP_RETURN, 0);
    return std::move(m_chunk);
}

// ─── Scope ────────────────────────────────────────────────────

void Compiler::beginScope() {
    m_scopeDepth++;
}

void Compiler::endScope(int line) {
    m_scopeDepth--;
    // Pop all locals declared in this scope
    while (!m_locals.empty() && m_locals.back().depth > m_scopeDepth) {
        emit(OpCode::OP_POP, line);
        m_locals.pop_back();
    }
}

// ─── Emit helpers ─────────────────────────────────────────────

void Compiler::emit(OpCode op, int line) {
    m_chunk.writeOp(op, line);
}

void Compiler::emit(OpCode op, uint8_t operand, int line) {
    m_chunk.writeOp(op, line);
    m_chunk.write(operand, line);
}

// Emit a jump instruction with placeholder offset
// Returns the offset of the placeholder so it can be patched later
int Compiler::emitJump(OpCode op, int line) {
    m_chunk.writeOp(op, line);
    m_chunk.write(0xFF, line);  // placeholder high byte
    m_chunk.write(0xFF, line);  // placeholder low byte
    return m_chunk.size() - 2;  // return position of placeholder
}

// Go back and fill in the jump offset
void Compiler::patchJump(int offset) {
    int jump = m_chunk.size() - offset - 2;
    if (jump > 0xFFFF) {
        std::cerr << "Jump offset too large.\n";
        m_hadError = true;
        return;
    }
    m_chunk.code[offset]     = (jump >> 8) & 0xFF;  // high byte
    m_chunk.code[offset + 1] = jump & 0xFF;          // low byte
}

// Emit a loop instruction that jumps backward
void Compiler::emitLoop(int loopStart, int line) {
    m_chunk.writeOp(OpCode::OP_LOOP, line);
    int offset = m_chunk.size() - loopStart + 2;
    if (offset > 0xFFFF) {
        std::cerr << "Loop offset too large.\n";
        m_hadError = true;
    }
    m_chunk.write((offset >> 8) & 0xFF, line);
    m_chunk.write(offset & 0xFF, line);
}

int Compiler::addConstant(Value val, int line) {
    int idx = m_chunk.addConstant(val);
    if (idx > 255) {
        error("Too many constants in one chunk.", line);
        return 0;
    }
    return idx;
}

// ─── Variable helpers ─────────────────────────────────────────

// Returns slot index if local found, -1 if not
int Compiler::resolveLocal(const std::string& name) {
    for (int i = (int)m_locals.size() - 1; i >= 0; i--) {
        if (m_locals[i].name == name) return i;
    }
    return -1;
}

void Compiler::declareLocal(const std::string& name, int line) {
    // Check for duplicate in same scope
    for (auto& local : m_locals) {
        if (local.depth == m_scopeDepth && local.name == name) {
            error("Variable '" + name + "' already declared in this scope.", line);
            return;
        }
    }
    m_locals.push_back({name, m_scopeDepth});
}

void Compiler::error(const std::string& msg, int line) {
    std::cerr << "[line " << line << "] Compile error: " << msg << "\n";
    m_hadError = true;
}

// ─── Statements ───────────────────────────────────────────────

void Compiler::visitVarDecl(VarDecl& n) {
    // Compile the initializer — puts value on stack
    n.initializer->accept(*this);

    if (m_scopeDepth > 0) {
        // Local variable — value stays on stack at its slot
        declareLocal(n.name, n.line);
    } else {
        // Global variable — store by name
        int idx = m_chunk.addName(n.name);
        emit(OpCode::OP_DEFINE_GLOBAL, (uint8_t)idx, n.line);
    }
}

void Compiler::visitBlockStmt(BlockStmt& n) {
    beginScope();
    for (auto& stmt : n.statements) {
        stmt->accept(*this);
    }
    endScope(n.line);
}

void Compiler::visitPrintStmt(PrintStmt& n) {
    n.expression->accept(*this);
    emit(OpCode::OP_PRINT, n.line);
}

void Compiler::visitInputStmt(InputStmt& n) {
    emit(OpCode::OP_INPUT, n.line);
    // Store result into variable
    int slot = resolveLocal(n.varName);
    if (slot != -1) {
        emit(OpCode::OP_SET_LOCAL, (uint8_t)slot, n.line);
    } else {
        int idx = m_chunk.addName(n.varName);
        emit(OpCode::OP_SET_GLOBAL, (uint8_t)idx, n.line);
    }
    emit(OpCode::OP_POP, n.line);
}

void Compiler::visitExprStmt(ExprStmt& n) {
    n.expression->accept(*this);
    emit(OpCode::OP_POP, n.line);
}

void Compiler::visitIfStmt(IfStmt& n) {
    // Compile condition
    n.condition->accept(*this);

    // Jump over then-branch if false
    int thenJump = emitJump(OpCode::OP_JUMP_IF_FALSE, n.line);
    emit(OpCode::OP_POP, n.line);  // pop condition

    // Compile then-branch
    n.thenBranch->accept(*this);

    // Jump over else-branch
    int elseJump = emitJump(OpCode::OP_JUMP, n.line);

    // Patch the then-jump to here
    patchJump(thenJump);
    emit(OpCode::OP_POP, n.line);  // pop condition (false case)

    // Compile else-branch if present
    if (n.elseBranch) {
        n.elseBranch->accept(*this);
    }

    patchJump(elseJump);
}

void Compiler::visitWhileStmt(WhileStmt& n) {
    int loopStart = m_chunk.size();

    // Compile condition
    n.condition->accept(*this);

    // Jump out if false
    int exitJump = emitJump(OpCode::OP_JUMP_IF_FALSE, n.line);
    emit(OpCode::OP_POP, n.line);  // pop condition

    // Compile body
    n.body->accept(*this);

    // Loop back to start
    emitLoop(loopStart, n.line);

    // Patch exit jump
    patchJump(exitJump);
    emit(OpCode::OP_POP, n.line);  // pop condition (false case)
}

// ─── Expressions ──────────────────────────────────────────────

Value Compiler::visitNumberLiteral(NumberLiteral& n) {
    int idx = addConstant(n.value, n.line);
    emit(OpCode::OP_CONSTANT, (uint8_t)idx, n.line);
    return std::monostate{};
}

Value Compiler::visitBoolLiteral(BoolLiteral& n) {
    emit(n.value ? OpCode::OP_TRUE : OpCode::OP_FALSE, n.line);
    return std::monostate{};
}

Value Compiler::visitNilLiteral(NilLiteral& n) {
    emit(OpCode::OP_NIL, n.line);
    return std::monostate{};
}

Value Compiler::visitVariable(Variable& n) {
    int slot = resolveLocal(n.name);
    if (slot != -1) {
        emit(OpCode::OP_GET_LOCAL, (uint8_t)slot, n.line);
    } else {
        int idx = m_chunk.addName(n.name);
        emit(OpCode::OP_GET_GLOBAL, (uint8_t)idx, n.line);
    }
    return std::monostate{};
}

Value Compiler::visitAssign(Assign& n) {
    n.value->accept(*this);
    int slot = resolveLocal(n.name);
    if (slot != -1) {
        emit(OpCode::OP_SET_LOCAL, (uint8_t)slot, n.line);
    } else {
        int idx = m_chunk.addName(n.name);
        emit(OpCode::OP_SET_GLOBAL, (uint8_t)idx, n.line);
    }
    return std::monostate{};
}

Value Compiler::visitBinary(BinaryExpr& n) {
    // Short-circuit for logical operators
    if (n.op == "and") {
        n.left->accept(*this);
        int jump = emitJump(OpCode::OP_JUMP_IF_FALSE, n.line);
        emit(OpCode::OP_POP, n.line);
        n.right->accept(*this);
        patchJump(jump);
        return std::monostate{};
    }
    if (n.op == "or") {
        n.left->accept(*this);
        int elseJump = emitJump(OpCode::OP_JUMP_IF_FALSE, n.line);
        int endJump  = emitJump(OpCode::OP_JUMP, n.line);
        patchJump(elseJump);
        emit(OpCode::OP_POP, n.line);
        n.right->accept(*this);
        patchJump(endJump);
        return std::monostate{};
    }

    // Compile both operands
    n.left->accept(*this);
    n.right->accept(*this);

    if      (n.op == "+")  emit(OpCode::OP_ADD,           n.line);
    else if (n.op == "-")  emit(OpCode::OP_SUB,           n.line);
    else if (n.op == "*")  emit(OpCode::OP_MUL,           n.line);
    else if (n.op == "/")  emit(OpCode::OP_DIV,           n.line);
    else if (n.op == "==") emit(OpCode::OP_EQUAL,         n.line);
    else if (n.op == "!=") emit(OpCode::OP_NOT_EQUAL,     n.line);
    else if (n.op == "<")  emit(OpCode::OP_LESS,          n.line);
    else if (n.op == "<=") emit(OpCode::OP_LESS_EQUAL,    n.line);
    else if (n.op == ">")  emit(OpCode::OP_GREATER,       n.line);
    else if (n.op == ">=") emit(OpCode::OP_GREATER_EQUAL, n.line);
    else error("Unknown operator: " + n.op, n.line);

    return std::monostate{};
}

Value Compiler::visitUnary(UnaryExpr& n) {
    n.right->accept(*this);
    if      (n.op == "-") emit(OpCode::OP_NEGATE, n.line);
    else if (n.op == "!") emit(OpCode::OP_NOT,    n.line);
    else error("Unknown unary operator: " + n.op, n.line);
    return std::monostate{};
}

Value Compiler::visitGrouping(GroupingExpr& n) {
    n.expression->accept(*this);
    return std::monostate{};
}

Value Compiler::visitStringLiteral(StringLiteral& n) {
    int idx = addConstant(n.value, n.line);
    emit(OpCode::OP_CONSTANT, (uint8_t)idx, n.line);
    return std::monostate{};
}