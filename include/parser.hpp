#pragma once
#include <vector>
#include <memory>
#include <string>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::vector<StmtPtr> parse();
    bool hadError() const { return m_hadError; }

private:
    std::vector<Token> m_tokens;
    int m_current = 0;
    bool m_hadError = false;

    // Statement parsers
    StmtPtr statement();
    StmtPtr varDeclaration();
    StmtPtr printStatement();
    StmtPtr inputStatement();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr block();
    StmtPtr expressionStatement();

    // Expression parsers (precedence climbing)
    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr orExpr();
    ExprPtr andExpr();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();

    // Helpers
    Token& peek();
    Token& previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& msg);
    void synchronize();
    void error(const std::string& msg);
};
