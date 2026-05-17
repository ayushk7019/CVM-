# CVM++ Project Report

## Overview

CVM++ is a high-performance, educational bytecode compiler and virtual machine built entirely in modern C++20. It implements a complete, self-contained language runtime from the ground up — encompassing lexical analysis, recursive descent parsing, typed AST construction, scope-aware bytecode compilation, and stack-based execution — with zero dependencies on external parser generators or VM frameworks.

The project serves as both a functional scripting runtime and a reference implementation for understanding how interpreted languages work under the hood.

---

## Architecture

The runtime pipeline follows a classic multi-stage design:

```
Source Code
    │
    ▼
  Lexer         →  Token stream
    │
    ▼
  Parser        →  Typed AST (recursive descent)
    │
    ▼
  Compiler      →  Bytecode Chunk (scope resolution + jump backpatching)
    │
    ▼
Virtual Machine →  Stack-based execution
```

**Optional debug tooling** operates as a side channel:
- **AST Printer** — pretty-prints the AST after parsing
- **Disassembler** — decodes emitted bytecode instructions for inspection

---

## Core Components

### Entry Point
- `src/main.cpp`
  - CLI argument handling
  - Dispatches to REPL mode or file execution mode
  - Top-level error reporting

### Lexer
- `include/lexer.hpp` / `src/lexer.cpp`
- Hand-written tokenizer with no regex dependencies
- Converts raw source text into a flat token stream
- Handles string literals, numeric literals, keywords, operators, and identifiers

### Parser
- `include/parser.hpp` / `src/parser.cpp`
- Recursive descent parser with explicit precedence rules
- Produces a fully typed, heap-allocated AST
- Provides clear, structured syntax error reporting

### AST
- `include/ast.hpp`
- Defines all expression and statement node types
- Implements the **Visitor pattern** for type-safe AST traversal
- Cleanly separates syntax representation from semantic processing

### Compiler
- `include/compiler.hpp` / `src/compiler.cpp`
- Walks the AST via the Visitor interface
- Resolves lexical scopes (local and global variables)
- Emits bytecode instructions into a `Chunk`
- Implements **jump backpatching** for control flow (`if/else`, `while`)

### Virtual Machine
- `include/vm.hpp` / `src/vm.cpp`
- Stack-based execution engine
- Dispatches on opcode using a tight interpret loop
- Manages the value stack, call frames, and runtime state
- Handles runtime type errors and built-in operations

### Disassembler
- `include/disassembler.hpp` / `src/disassembler.cpp`
- Decodes raw bytecode `Chunk` into human-readable instruction listings
- Supports offset tracking and operand annotation
- Used for debugging both the compiler output and VM behavior

---

## Features

### Language
- Numbers (floating-point), strings, booleans (`true`/`false`), and `nil`
- Arithmetic, comparison, logical, and string concatenation operators
- Global and local variable declarations and assignment
- Control flow: `if / else`, `while` loops
- Built-in functions: `print`, `input`

### Compiler
- Single-pass bytecode compiler with constant table management
- Lexical scope resolution using compile-time depth tracking
- Forward jump patching for conditional and loop constructs

### Runtime
- Stack-based virtual machine with a tight dispatch loop
- No heap allocation during normal expression evaluation
- Structured runtime error reporting with source context

### Developer Tooling
- Full AST pretty-printer for parse-stage debugging
- Bytecode disassembler with instruction-level annotation
- REPL for interactive development and testing

---

## Project Structure

```
cvm_new/
├── include/
│   ├── ast.hpp
│   ├── compiler.hpp
│   ├── disassembler.hpp
│   ├── lexer.hpp
│   ├── parser.hpp
│   └── vm.hpp
├── src/
│   ├── compiler.cpp
│   ├── disassembler.cpp
│   ├── lexer.cpp
│   ├── main.cpp
│   ├── parser.cpp
│   └── vm.cpp
├── build/
├── test.cvm
├── test2.cvm
├── test3.cvm
├── test4.cvm
├── CMakeLists.txt
├── README.md
└── PROJECT_REPORT.md
```

---

## Build Instructions

### Prerequisites
- CMake 3.16 or newer
- A C++20-capable compiler (GCC 10+, Clang 13+, MSVC 19.29+)

### Steps

1. **Configure** — generate build files:
```bash
   cmake -S . -B build
```

2. **Build** — compile the executable:
```bash
   cmake --build build
```

3. **Run** — execute a source file or launch the REPL:
```bash
   ./build/cvm++ test.cvm     # File mode
   ./build/cvm++              # REPL mode
```

---

## Design Decisions

| Decision | Rationale |
|---|---|
| Hand-written lexer and parser | Full control over error messages and language evolution |
| Visitor pattern for AST | Type-safe traversal without dynamic casts; clean separation of passes |
| Single-pass compiler | Sufficient for the current feature set; simpler than multi-pass |
| Stack-based VM | Easier to implement and debug than a register-based VM |
| No external dependencies | Keeps the project self-contained and educational |

---

## Limitations & Known Constraints

- No first-class functions or closures (planned extension)
- No garbage collector — values are value types; no heap-allocated objects yet
- Error recovery in the parser is minimal (fails fast on first error)
- No module or import system

---

## Roadmap (Suggested Extensions)

- [ ] First-class functions and call frames
- [ ] Closures with upvalue capturing
- [ ] Mark-and-sweep garbage collector
- [ ] Arrays and hash maps as built-in types
- [ ] User-defined classes / structs
- [ ] Improved error recovery in the parser
- [ ] Source-mapped stack traces

---

## Notes

- Designed to be educational, readable, and incrementally extensible
- All components are intentionally decoupled to allow individual replacement or study
- Compiler warnings are enabled for both MSVC (`/W4`) and GCC/Clang (`-Wall -Wextra`)
- Test scripts (`test.cvm` – `test4.cvm`) cover core language features end-to-end