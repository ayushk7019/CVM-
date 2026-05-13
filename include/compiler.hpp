#pragma once
#include <vector>
#include <string>
#include "ast.hpp"
#include "chunk.hpp"

struct Local {
    std::string name;
    int depth;
};

class Compiler : public ExprVisitor, public StmtVisitor {
public:
    Compiler();
    Chunk compile(std::vector<StmtPtr>& stmts);
    bool hadError() const { return m_hadError; }

private:
    Chunk m_chunk;
    bool  m_hadError = false;
    std::vector<Local> m_locals;
    int m_scopeDepth = 0;

    void beginScope();
    void endScope(int line);

    void emit(OpCode op, int line);
    void emit(OpCode op, uint8_t operand, int line);
    int  emitJump(OpCode op, int line);
    void patchJump(int offset);
    void emitLoop(int loopStart, int line);
    int  addConstant(Value val, int line);

    int  resolveLocal(const std::string& name);
    void declareLocal(const std::string& name, int line);
    void error(const std::string& msg, int line);

    Value visitNumberLiteral(NumberLiteral&) override;
    Value visitStringLiteral(StringLiteral&) override;
    Value visitBoolLiteral(BoolLiteral&) override;
    Value visitNilLiteral(NilLiteral&) override;
    Value visitVariable(Variable&) override;
    Value visitAssign(Assign&) override;
    Value visitBinary(BinaryExpr&) override;
    Value visitUnary(UnaryExpr&) override;
    Value visitGrouping(GroupingExpr&) override;

    void visitPrintStmt(PrintStmt&) override;
    void visitInputStmt(InputStmt&) override;
    void visitExprStmt(ExprStmt&) override;
    void visitVarDecl(VarDecl&) override;
    void visitBlockStmt(BlockStmt&) override;
    void visitIfStmt(IfStmt&) override;
    void visitWhileStmt(WhileStmt&) override;
};
