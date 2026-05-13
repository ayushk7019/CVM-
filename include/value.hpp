#pragma once
#include <variant>
#include <string>
#include <iostream>
#include <cstdint>

// Core value type — holds all CVM++ runtime values
using Value = std::variant<int64_t, bool, std::string, std::monostate>;

inline void printValue(const Value& val) {
    std::visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            std::cout << v;
        } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << v;
        } else {
            std::cout << "nil";
        }
    }, val);
}

inline bool isTruthy(const Value& val) {
    if (std::holds_alternative<std::monostate>(val)) return false;
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
    return true;
}

inline bool isEqual(const Value& a, const Value& b) {
    return a == b;
}

inline std::string valueTypeName(const Value& val) {
    if (std::holds_alternative<int64_t>(val))     return "int";
    if (std::holds_alternative<bool>(val))         return "bool";
    if (std::holds_alternative<std::string>(val))  return "string";
    return "nil";
}
