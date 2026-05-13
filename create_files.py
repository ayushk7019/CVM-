def write(path, content):
    open(path, 'w', encoding='utf-8').write(content)
    print(f'Created: {path}')

# Update ast_printer to handle StringLiteral
write('src/ast_printer.cpp', '''#include "ast_printer.hpp"
#include <iostream>

void AstPrinter::printIndent() {
    for (int i = 0; i < m_indent; i++) std::cout << "  ";
}

void AstPrinter::print(std::vector<StmtPtr>& stmts) {
    for (auto& stmt : stmts) stmt->accept(*this);
}

void AstPrinter::visitPrintStmt(PrintStmt& n) {
    printIndent(); std::cout << "PrintStmt\\n";
    m_indent++;
    n.expression->accept(*this);
    m_indent--;
}

void AstPrinter::visitInputStmt(InputStmt& n) {
    printIndent(); std::cout << "InputStmt -> " << n.varName << "\\n";
}

void AstPrinter::visitExprStmt(ExprStmt& n) {
    printIndent(); std::cout << "ExprStmt\\n";
    m_indent++;
    n.expression->accept(*this);
    m_indent--;
}

void AstPrinter::visitVarDecl(VarDecl& n) {
    printIndent(); std::cout << "VarDecl -> " << n.name << "\\n";
    m_indent++;
    n.initializer->accept(*this);
    m_indent--;
}

void AstPrinter::visitBlockStmt(BlockStmt& n) {
    printIndent(); std::cout << "Block\\n";
    m_indent++;
    for (auto& s : n.statements) s->accept(*this);
    m_indent--;
}

void AstPrinter::visitIfStmt(IfStmt& n) {
    printIndent(); std::cout << "IfStmt\\n";
    m_indent++;
    printIndent(); std::cout << "condition:\\n";
    m_indent++; n.condition->accept(*this); m_indent--;
    printIndent(); std::cout << "then:\\n";
    m_indent++; n.thenBranch->accept(*this); m_indent--;
    if (n.elseBranch) {
        printIndent(); std::cout << "else:\\n";
        m_indent++; n.elseBranch->accept(*this); m_indent--;
    }
    m_indent--;
}

void AstPrinter::visitWhileStmt(WhileStmt& n) {
    printIndent(); std::cout << "WhileStmt\\n";
    m_indent++;
    printIndent(); std::cout << "condition:\\n";
    m_indent++; n.condition->accept(*this); m_indent--;
    printIndent(); std::cout << "body:\\n";
    m_indent++; n.body->accept(*this); m_indent--;
    m_indent--;
}

Value AstPrinter::visitNumberLiteral(NumberLiteral& n) {
    printIndent(); std::cout << "Number(" << n.value << ")\\n";
    return std::monostate{};
}

Value AstPrinter::visitStringLiteral(StringLiteral& n) {
    printIndent(); std::cout << "String(\\"" << n.value << "\\")\\n";
    return std::monostate{};
}

Value AstPrinter::visitBoolLiteral(BoolLiteral& n) {
    printIndent(); std::cout << "Bool(" << (n.value ? "true" : "false") << ")\\n";
    return std::monostate{};
}

Value AstPrinter::visitNilLiteral(NilLiteral&) {
    printIndent(); std::cout << "Nil\\n";
    return std::monostate{};
}

Value AstPrinter::visitVariable(Variable& n) {
    printIndent(); std::cout << "Variable(" << n.name << ")\\n";
    return std::monostate{};
}

Value AstPrinter::visitAssign(Assign& n) {
    printIndent(); std::cout << "Assign -> " << n.name << "\\n";
    m_indent++; n.value->accept(*this); m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitBinary(BinaryExpr& n) {
    printIndent(); std::cout << "Binary(" << n.op << ")\\n";
    m_indent++;
    n.left->accept(*this);
    n.right->accept(*this);
    m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitUnary(UnaryExpr& n) {
    printIndent(); std::cout << "Unary(" << n.op << ")\\n";
    m_indent++; n.right->accept(*this); m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitGrouping(GroupingExpr& n) {
    printIndent(); std::cout << "Grouping\\n";
    m_indent++; n.expression->accept(*this); m_indent--;
    return std::monostate{};
}
''')

# Update ast_printer.hpp to add visitStringLiteral
write('include/ast_printer.hpp', '''#pragma once
#include <string>
#include <vector>
#include "ast.hpp"

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    void print(std::vector<StmtPtr>& stmts);

private:
    int m_indent = 0;
    void printIndent();

    Value visitNumberLiteral(NumberLiteral& n) override;
    Value visitStringLiteral(StringLiteral& n) override;
    Value visitBoolLiteral(BoolLiteral& n) override;
    Value visitNilLiteral(NilLiteral& n) override;
    Value visitVariable(Variable& n) override;
    Value visitAssign(Assign& n) override;
    Value visitBinary(BinaryExpr& n) override;
    Value visitUnary(UnaryExpr& n) override;
    Value visitGrouping(GroupingExpr& n) override;

    void visitPrintStmt(PrintStmt& n) override;
    void visitInputStmt(InputStmt& n) override;
    void visitExprStmt(ExprStmt& n) override;
    void visitVarDecl(VarDecl& n) override;
    void visitBlockStmt(BlockStmt& n) override;
    void visitIfStmt(IfStmt& n) override;
    void visitWhileStmt(WhileStmt& n) override;
};
''')

# Update compiler to handle StringLiteral
write('include/compiler.hpp', '''#pragma once
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
''')