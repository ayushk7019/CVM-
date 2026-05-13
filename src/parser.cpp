#include "parser.hpp"
#include <iostream>
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    while (!isAtEnd()) {
        try {
            if (match({TokenType::LET})) {
                statements.push_back(varDeclaration());
            } else {
                statements.push_back(statement());
            }
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << "\n";
            synchronize();
        }
    }
    return statements;
}

StmtPtr Parser::statement() {
    if (match({TokenType::PRINT}))  return printStatement();
    if (match({TokenType::INPUT}))  return inputStatement();
    if (match({TokenType::IF}))     return ifStatement();
    if (match({TokenType::WHILE}))  return whileStatement();
    if (match({TokenType::LBRACE})) return block();
    return expressionStatement();
}

StmtPtr Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");
    consume(TokenType::EQUAL, "Expected '=' after variable name.");
    ExprPtr init = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_unique<VarDecl>(name.lexeme, std::move(init), name.line);
}

StmtPtr Parser::printStatement() {
    int line = previous().line;
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after print value.");
    return std::make_unique<PrintStmt>(std::move(value), line);
}

StmtPtr Parser::inputStatement() {
    int line = previous().line;
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name after 'input'.");
    consume(TokenType::SEMICOLON, "Expected ';' after input statement.");
    return std::make_unique<InputStmt>(name.lexeme, line);
}

StmtPtr Parser::ifStatement() {
    int line = previous().line;
    ExprPtr condition = expression();
    consume(TokenType::LBRACE, "Expected '{' after if condition.");
    m_current--;
    StmtPtr thenBranch = block();
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        consume(TokenType::LBRACE, "Expected '{' after else.");
        m_current--;
        elseBranch = block();
    }
    return std::make_unique<IfStmt>(std::move(condition),
                                    std::move(thenBranch),
                                    std::move(elseBranch), line);
}

StmtPtr Parser::whileStatement() {
    int line = previous().line;
    ExprPtr condition = expression();
    consume(TokenType::LBRACE, "Expected '{' after while condition.");
    m_current--;
    StmtPtr body = block();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body), line);
}

StmtPtr Parser::block() {
    int line = peek().line;
    consume(TokenType::LBRACE, "Expected '{'.");
    std::vector<StmtPtr> stmts;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (match({TokenType::LET})) {
            stmts.push_back(varDeclaration());
        } else {
            stmts.push_back(statement());
        }
    }
    consume(TokenType::RBRACE, "Expected '}' after block.");
    return std::make_unique<BlockStmt>(std::move(stmts), line);
}

StmtPtr Parser::expressionStatement() {
    int line = peek().line;
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return std::make_unique<ExprStmt>(std::move(expr), line);
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    ExprPtr expr = orExpr();
    if (match({TokenType::EQUAL})) {
        int line = previous().line;
        ExprPtr value = assignment();
        if (auto* var = dynamic_cast<Variable*>(expr.get())) {
            return std::make_unique<Assign>(var->name, std::move(value), line);
        }
        error("Invalid assignment target.");
    }
    return expr;
}

ExprPtr Parser::orExpr() {
    ExprPtr left = andExpr();
    while (match({TokenType::OR})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = andExpr();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::andExpr() {
    ExprPtr left = equality();
    while (match({TokenType::AND})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = equality();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::equality() {
    ExprPtr left = comparison();
    while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = comparison();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::comparison() {
    ExprPtr left = term();
    while (match({TokenType::LESS, TokenType::LESS_EQUAL,
                  TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = term();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::term() {
    ExprPtr left = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = factor();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::factor() {
    ExprPtr left = unary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = unary();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right), line);
    }
    return left;
}

ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        int line = previous().line;
        std::string op = previous().lexeme;
        ExprPtr right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right), line);
    }
    return primary();
}

ExprPtr Parser::primary() {
    if (match({TokenType::NUMBER})) {
        int64_t val = std::get<int64_t>(previous().literal);
        return std::make_unique<NumberLiteral>(val, previous().line);
    }
    if (match({TokenType::STRING})) {
        std::string val = std::get<std::string>(previous().literal);
        return std::make_unique<StringLiteral>(val, previous().line);
    }
    if (match({TokenType::TRUE}))
        return std::make_unique<BoolLiteral>(true, previous().line);
    if (match({TokenType::FALSE}))
        return std::make_unique<BoolLiteral>(false, previous().line);
    if (match({TokenType::NIL}))
        return std::make_unique<NilLiteral>(previous().line);
    if (match({TokenType::IDENTIFIER}))
        return std::make_unique<Variable>(previous().lexeme, previous().line);
    if (match({TokenType::LPAREN})) {
        int line = previous().line;
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr), line);
    }
    error("Expected expression, got '" + peek().lexeme + "'.");
    return nullptr;
}

Token& Parser::peek() { return m_tokens[m_current]; }
Token& Parser::previous() { return m_tokens[m_current - 1]; }
bool Parser::isAtEnd() { return peek().type == TokenType::EOF_TOKEN; }

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) { m_current++; return true; }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& msg) {
    if (check(type)) { m_current++; return previous(); }
    error(msg);
    return previous();
}

void Parser::error(const std::string& msg) {
    Token& t = peek();
    std::string err = "[line " + std::to_string(t.line) +
                      ", col " + std::to_string(t.column) +
                      "] Parse error: " + msg;
    m_hadError = true;
    throw std::runtime_error(err);
}

void Parser::synchronize() {
    m_current++;
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default: break;
        }
        m_current++;
    }
}
