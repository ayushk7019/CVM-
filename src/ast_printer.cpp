#include "ast_printer.hpp"
#include <iostream>

void AstPrinter::printIndent() {
    for (int i = 0; i < m_indent; i++) std::cout << "  ";
}

void AstPrinter::print(std::vector<StmtPtr>& stmts) {
    for (auto& stmt : stmts) stmt->accept(*this);
}

void AstPrinter::visitPrintStmt(PrintStmt& n) {
    printIndent(); std::cout << "PrintStmt\n";
    m_indent++;
    n.expression->accept(*this);
    m_indent--;
}

void AstPrinter::visitInputStmt(InputStmt& n) {
    printIndent(); std::cout << "InputStmt -> " << n.varName << "\n";
}

void AstPrinter::visitExprStmt(ExprStmt& n) {
    printIndent(); std::cout << "ExprStmt\n";
    m_indent++;
    n.expression->accept(*this);
    m_indent--;
}

void AstPrinter::visitVarDecl(VarDecl& n) {
    printIndent(); std::cout << "VarDecl -> " << n.name << "\n";
    m_indent++;
    n.initializer->accept(*this);
    m_indent--;
}

void AstPrinter::visitBlockStmt(BlockStmt& n) {
    printIndent(); std::cout << "Block\n";
    m_indent++;
    for (auto& s : n.statements) s->accept(*this);
    m_indent--;
}

void AstPrinter::visitIfStmt(IfStmt& n) {
    printIndent(); std::cout << "IfStmt\n";
    m_indent++;
    printIndent(); std::cout << "condition:\n";
    m_indent++; n.condition->accept(*this); m_indent--;
    printIndent(); std::cout << "then:\n";
    m_indent++; n.thenBranch->accept(*this); m_indent--;
    if (n.elseBranch) {
        printIndent(); std::cout << "else:\n";
        m_indent++; n.elseBranch->accept(*this); m_indent--;
    }
    m_indent--;
}

void AstPrinter::visitWhileStmt(WhileStmt& n) {
    printIndent(); std::cout << "WhileStmt\n";
    m_indent++;
    printIndent(); std::cout << "condition:\n";
    m_indent++; n.condition->accept(*this); m_indent--;
    printIndent(); std::cout << "body:\n";
    m_indent++; n.body->accept(*this); m_indent--;
    m_indent--;
}

Value AstPrinter::visitNumberLiteral(NumberLiteral& n) {
    printIndent(); std::cout << "Number(" << n.value << ")\n";
    return std::monostate{};
}

Value AstPrinter::visitStringLiteral(StringLiteral& n) {
    printIndent(); std::cout << "String(\"" << n.value << "\")\n";
    return std::monostate{};
}

Value AstPrinter::visitBoolLiteral(BoolLiteral& n) {
    printIndent(); std::cout << "Bool(" << (n.value ? "true" : "false") << ")\n";
    return std::monostate{};
}

Value AstPrinter::visitNilLiteral(NilLiteral&) {
    printIndent(); std::cout << "Nil\n";
    return std::monostate{};
}

Value AstPrinter::visitVariable(Variable& n) {
    printIndent(); std::cout << "Variable(" << n.name << ")\n";
    return std::monostate{};
}

Value AstPrinter::visitAssign(Assign& n) {
    printIndent(); std::cout << "Assign -> " << n.name << "\n";
    m_indent++; n.value->accept(*this); m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitBinary(BinaryExpr& n) {
    printIndent(); std::cout << "Binary(" << n.op << ")\n";
    m_indent++;
    n.left->accept(*this);
    n.right->accept(*this);
    m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitUnary(UnaryExpr& n) {
    printIndent(); std::cout << "Unary(" << n.op << ")\n";
    m_indent++; n.right->accept(*this); m_indent--;
    return std::monostate{};
}

Value AstPrinter::visitGrouping(GroupingExpr& n) {
    printIndent(); std::cout << "Grouping\n";
    m_indent++; n.expression->accept(*this); m_indent--;
    return std::monostate{};
}
