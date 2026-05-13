#pragma once
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
