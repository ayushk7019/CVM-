#pragma once
#include <memory>
#include <vector>
#include <string>
#include "value.hpp"

struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

struct NumberLiteral;
struct StringLiteral;
struct BoolLiteral;
struct NilLiteral;
struct Variable;
struct Assign;
struct BinaryExpr;
struct UnaryExpr;
struct GroupingExpr;

struct ExprVisitor {
    virtual Value visitNumberLiteral(NumberLiteral&) = 0;
    virtual Value visitStringLiteral(StringLiteral&) = 0;
    virtual Value visitBoolLiteral(BoolLiteral&) = 0;
    virtual Value visitNilLiteral(NilLiteral&) = 0;
    virtual Value visitVariable(Variable&) = 0;
    virtual Value visitAssign(Assign&) = 0;
    virtual Value visitBinary(BinaryExpr&) = 0;
    virtual Value visitUnary(UnaryExpr&) = 0;
    virtual Value visitGrouping(GroupingExpr&) = 0;
    virtual ~ExprVisitor() = default;
};

struct Expr {
    int line = 0;
    virtual Value accept(ExprVisitor&) = 0;
    virtual ~Expr() = default;
};

struct NumberLiteral : Expr {
    int64_t value;
    NumberLiteral(int64_t v, int line) : value(v) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitNumberLiteral(*this); }
};

struct StringLiteral : Expr {
    std::string value;
    StringLiteral(std::string v, int line) : value(std::move(v)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitStringLiteral(*this); }
};

struct BoolLiteral : Expr {
    bool value;
    BoolLiteral(bool v, int line) : value(v) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitBoolLiteral(*this); }
};

struct NilLiteral : Expr {
    NilLiteral(int line) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitNilLiteral(*this); }
};

struct Variable : Expr {
    std::string name;
    Variable(std::string n, int line) : name(std::move(n)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitVariable(*this); }
};

struct Assign : Expr {
    std::string name;
    ExprPtr value;
    Assign(std::string n, ExprPtr val, int line)
        : name(std::move(n)), value(std::move(val)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitAssign(*this); }
};

struct BinaryExpr : Expr {
    ExprPtr left;
    std::string op;
    ExprPtr right;
    BinaryExpr(ExprPtr l, std::string op, ExprPtr r, int line)
        : left(std::move(l)), op(std::move(op)), right(std::move(r)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitBinary(*this); }
};

struct UnaryExpr : Expr {
    std::string op;
    ExprPtr right;
    UnaryExpr(std::string op, ExprPtr r, int line)
        : op(std::move(op)), right(std::move(r)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitUnary(*this); }
};

struct GroupingExpr : Expr {
    ExprPtr expression;
    GroupingExpr(ExprPtr e, int line) : expression(std::move(e)) { this->line = line; }
    Value accept(ExprVisitor& v) override { return v.visitGrouping(*this); }
};

struct PrintStmt;
struct InputStmt;
struct ExprStmt;
struct VarDecl;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;

struct StmtVisitor {
    virtual void visitPrintStmt(PrintStmt&) = 0;
    virtual void visitInputStmt(InputStmt&) = 0;
    virtual void visitExprStmt(ExprStmt&) = 0;
    virtual void visitVarDecl(VarDecl&) = 0;
    virtual void visitBlockStmt(BlockStmt&) = 0;
    virtual void visitIfStmt(IfStmt&) = 0;
    virtual void visitWhileStmt(WhileStmt&) = 0;
    virtual ~StmtVisitor() = default;
};

struct Stmt {
    int line = 0;
    virtual void accept(StmtVisitor&) = 0;
    virtual ~Stmt() = default;
};

struct PrintStmt : Stmt {
    ExprPtr expression;
    PrintStmt(ExprPtr e, int line) : expression(std::move(e)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitPrintStmt(*this); }
};

struct InputStmt : Stmt {
    std::string varName;
    InputStmt(std::string name, int line) : varName(std::move(name)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitInputStmt(*this); }
};

struct ExprStmt : Stmt {
    ExprPtr expression;
    ExprStmt(ExprPtr e, int line) : expression(std::move(e)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitExprStmt(*this); }
};

struct VarDecl : Stmt {
    std::string name;
    ExprPtr initializer;
    VarDecl(std::string n, ExprPtr init, int line)
        : name(std::move(n)), initializer(std::move(init)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitVarDecl(*this); }
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> stmts, int line)
        : statements(std::move(stmts)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitBlockStmt(*this); }
};

struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
    IfStmt(ExprPtr cond, StmtPtr then, StmtPtr els, int line)
        : condition(std::move(cond)), thenBranch(std::move(then)),
          elseBranch(std::move(els)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitIfStmt(*this); }
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr cond, StmtPtr body, int line)
        : condition(std::move(cond)), body(std::move(body)) { this->line = line; }
    void accept(StmtVisitor& v) override { v.visitWhileStmt(*this); }
};
