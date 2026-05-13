#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "ast_printer.hpp"
#include "compiler.hpp"
#include "vm.hpp"
#include "disassembler.hpp"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file: " << path << "\n";
        exit(1);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void run(const std::string& source, bool showTokens,
         bool showAst, bool showDisasm, bool trace) {

    // ── Lex ──────────────────────────────────────────────────
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    if (showTokens) {
        for (const Token& t : tokens) {
            std::cout << "[line " << t.line
                      << ", col " << t.column
                      << "] " << tokenTypeName(t.type)
                      << " '" << t.lexeme << "'"
                      << "\n";
        }
        return;
    }

    if (lexer.hadError()) return;

    // ── Parse ─────────────────────────────────────────────────
    Parser parser(tokens);
    std::vector<StmtPtr> stmts = parser.parse();

    if (showAst) {
        AstPrinter printer;
        printer.print(stmts);
        return;
    }

    if (parser.hadError()) return;

    // ── Compile ───────────────────────────────────────────────
    Compiler compiler;
    Chunk chunk = compiler.compile(stmts);

    if (compiler.hadError()) return;

    if (showDisasm) {
        Disassembler::disassemble(chunk, "main");
        return;
    }

    // ── Execute ───────────────────────────────────────────────
    VM vm;
    vm.run(chunk, trace);
}

void runFile(const std::string& path, bool showTokens,
             bool showAst, bool showDisasm, bool trace) {
    std::string source = readFile(path);
    run(source, showTokens, showAst, showDisasm, trace);
}

void runRepl(bool showTokens, bool showAst, bool showDisasm, bool trace) {
    std::string line;
    std::cout << "CVM++ REPL v0.1  (type :exit to quit)\n";
    while (true) {
        std::cout << "cvm> ";
        if (!std::getline(std::cin, line)) break;
        if (line == ":exit") break;
        if (line.empty()) continue;
        run(line, showTokens, showAst, showDisasm, trace);
    }
}

int main(int argc, char* argv[]) {
    bool showTokens = false;
    bool showAst    = false;
    bool showDisasm = false;
    bool trace      = false;
    std::string filePath;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if      (arg == "--tokens") showTokens = true;
        else if (arg == "--ast")    showAst    = true;
        else if (arg == "--disasm") showDisasm = true;
        else if (arg == "--trace")  trace      = true;
        else filePath = arg;
    }

    if (filePath.empty()) {
        runRepl(showTokens, showAst, showDisasm, trace);
    } else {
        runFile(filePath, showTokens, showAst, showDisasm, trace);
    }

    return 0;
}
