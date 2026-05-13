#include "lexer.hpp"
#include <iostream>
#include <cstdint>

const std::unordered_map<std::string, TokenType> Lexer::s_keywords = {
    {"true",   TokenType::TRUE},
    {"false",  TokenType::FALSE},
    {"nil",    TokenType::NIL},
    {"let",    TokenType::LET},
    {"if",     TokenType::IF},
    {"else",   TokenType::ELSE},
    {"while",  TokenType::WHILE},
    {"print",  TokenType::PRINT},
    {"input",  TokenType::INPUT},
    {"fn",     TokenType::FN},
    {"return", TokenType::RETURN},
    {"and",    TokenType::AND},
    {"or",     TokenType::OR},
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        m_start = m_current;
        scanToken();
    }
    m_tokens.emplace_back(TokenType::EOF_TOKEN, "", std::monostate{}, m_line, currentColumn());
    return m_tokens;
}

bool Lexer::isAtEnd() const {
    return m_current >= (int)m_source.size();
}

char Lexer::advance() {
    return m_source[m_current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::peekNext() const {
    if (m_current + 1 >= (int)m_source.size()) return '\0';
    return m_source[m_current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (m_source[m_current] != expected) return false;
    m_current++;
    return true;
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Lexer::addToken(TokenType type, Value literal) {
    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.emplace_back(type, text, literal, m_line, m_start - m_lineStart);
}

void Lexer::errorAt(int line, int col, const std::string& msg) {
    std::cerr << "[line " << line << ", col " << col << "] Lexer error: " << msg << "\n";
    m_hadError = true;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LPAREN);    break;
        case ')': addToken(TokenType::RPAREN);    break;
        case '{': addToken(TokenType::LBRACE);    break;
        case '}': addToken(TokenType::RBRACE);    break;
        case ',': addToken(TokenType::COMMA);     break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '+': addToken(TokenType::PLUS);      break;
        case '-': addToken(TokenType::MINUS);     break;
        case '*': addToken(TokenType::STAR);      break;
        case '/':
            if (match('/')) skipLineComment();
            else addToken(TokenType::SLASH);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case ' ': case '\r': case '\t': break;
        case '\n':
            m_line++;
            m_lineStart = m_current;
            break;
        case '"': string(); break;
        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                errorAt(m_line, currentColumn(),
                        std::string("Unexpected character: ") + c);
            }
            break;
    }
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && !isAtEnd()) advance();
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { m_line++; m_lineStart = m_current + 1; }
        advance();
    }
    if (isAtEnd()) {
        errorAt(m_line, currentColumn(), "Unterminated string.");
        return;
    }
    advance(); // closing "
    // Store the string value (without quotes)
    std::string val = m_source.substr(m_start + 1, m_current - m_start - 2);
    addToken(TokenType::STRING, val);
}

void Lexer::number() {
    while (isdigit(peek())) advance();
    int64_t val = std::stoll(m_source.substr(m_start, m_current - m_start));
    addToken(TokenType::NUMBER, val);
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = m_source.substr(m_start, m_current - m_start);
    auto it = s_keywords.find(text);
    if (it != s_keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}
