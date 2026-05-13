#pragma once
#include <string>
#include <variant>
#include "value.hpp"

enum class TokenType {
    // Literals
    NUMBER, STRING, TRUE, FALSE, NIL,

    // Identifiers
    IDENTIFIER,

    // Arithmetic operators
    PLUS, MINUS, STAR, SLASH,

    // Comparison operators
    EQUAL_EQUAL, BANG_EQUAL,
    LESS, LESS_EQUAL,
    GREATER, GREATER_EQUAL,

    // Assignment
    EQUAL,

    // Logical
    AND, OR, BANG,

    // Delimiters
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    SEMICOLON, COMMA,

    // Keywords
    LET, IF, ELSE, WHILE,
    PRINT, INPUT,
    FN, RETURN,

    // Special
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string lexeme;
    Value literal;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, Value literal, int line, int column)
        : type(type), lexeme(std::move(lexeme)), literal(literal),
          line(line), column(column) {}
};

// Helper: convert TokenType to string for debugging
inline std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::NUMBER:       return "NUMBER";
        case TokenType::STRING:       return "STRING";
        case TokenType::TRUE:         return "TRUE";
        case TokenType::FALSE:        return "FALSE";
        case TokenType::NIL:          return "NIL";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::PLUS:         return "PLUS";
        case TokenType::MINUS:        return "MINUS";
        case TokenType::STAR:         return "STAR";
        case TokenType::SLASH:        return "SLASH";
        case TokenType::EQUAL_EQUAL:  return "EQUAL_EQUAL";
        case TokenType::BANG_EQUAL:   return "BANG_EQUAL";
        case TokenType::LESS:         return "LESS";
        case TokenType::LESS_EQUAL:   return "LESS_EQUAL";
        case TokenType::GREATER:      return "GREATER";
        case TokenType::GREATER_EQUAL:return "GREATER_EQUAL";
        case TokenType::EQUAL:        return "EQUAL";
        case TokenType::AND:          return "AND";
        case TokenType::OR:           return "OR";
        case TokenType::BANG:         return "BANG";
        case TokenType::LPAREN:       return "LPAREN";
        case TokenType::RPAREN:       return "RPAREN";
        case TokenType::LBRACE:       return "LBRACE";
        case TokenType::RBRACE:       return "RBRACE";
        case TokenType::SEMICOLON:    return "SEMICOLON";
        case TokenType::COMMA:        return "COMMA";
        case TokenType::LET:          return "LET";
        case TokenType::IF:           return "IF";
        case TokenType::ELSE:         return "ELSE";
        case TokenType::WHILE:        return "WHILE";
        case TokenType::PRINT:        return "PRINT";
        case TokenType::INPUT:        return "INPUT";
        case TokenType::FN:           return "FN";
        case TokenType::RETURN:       return "RETURN";
        case TokenType::EOF_TOKEN:    return "EOF";
        default:                      return "UNKNOWN";
    }
}
