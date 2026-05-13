#pragma once
#include <string>
#include <vector>
#include <unordered_map>
// #include "token.hpp"
#include "C:\Users\HP\Downloads\cvm_new\include\token.hpp"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();
    bool hadError() const { return m_hadError; }

private:
    std::string m_source;
    std::vector<Token> m_tokens;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;
    int m_lineStart = 0;
    bool m_hadError = false;

    static const std::unordered_map<std::string, TokenType> s_keywords;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, Value literal);

    void string();
    void number();
    void identifier();
    void skipLineComment();

    int currentColumn() const { return m_current - m_lineStart; }

    void errorAt(int line, int col, const std::string& msg);
};
