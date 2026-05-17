# CVM++

> A bytecode compiler and virtual machine implemented from scratch in modern C++20.

CVM++ is a fully hand-written language runtime — no parser generators, no third-party VM frameworks. It takes source code written in a custom scripting language, compiles it to bytecode, and executes it on a register-stack virtual machine. The project demonstrates a complete compiler pipeline: lexing → parsing → AST construction → bytecode compilation → VM execution.

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Pipeline](#pipeline)
- [Features](#features)
- [Project Structure](#project-structure)
- [Components](#components)
  - [Lexer](#lexer)
  - [Parser](#parser)
  - [AST](#ast)
  - [Compiler](#compiler)
  - [Virtual Machine](#virtual-machine)
  - [Disassembler](#disassembler)
- [Language Reference](#language-reference)
- [Building](#building)
- [Running the Compiler](#running-the-compiler)
- [Running the VM](#running-the-vm)
- [CLI Options](#cli-options)
- [Dependencies](#dependencies)

---

## Overview

CVM++ is a from-scratch implementation of a complete language runtime in C++20. It is designed to be readable, educational, and extensible. The codebase covers every stage of a modern language implementation:

- **Scanning** — tokenising raw source text
- **Parsing** — constructing a typed Abstract Syntax Tree
- **Compilation** — walking the AST and emitting bytecode instructions
- **Execution** — running bytecode on a stack-based virtual machine
- **Debugging** — disassembling bytecode chunks for inspection

---

## Architecture

```
Source Code (.cvm)
       │
       ▼
  ┌─────────┐
  │  Lexer  │  ── Scans characters → produces Token stream
  └─────────┘
       │
       ▼
  ┌─────────┐
  │ Parser  │  ── Consumes tokens → builds typed AST (StmtPtr / ExprPtr)
  └─────────┘
       │
       ▼
  ┌──────────────┐
  │ AST Printer  │  ── (optional) pretty-prints the AST for debugging
  └──────────────┘
       │
       ▼
  ┌──────────────┐
  │   Compiler   │  ── Walks AST via Visitor pattern → emits Chunk (bytecode)
  └──────────────┘
       │
       ▼
  ┌──────────────┐
  │ Disassembler │  ── (optional) decodes bytecode instructions for inspection
  └──────────────┘
       │
       ▼
  ┌─────────────────────┐
  │  Virtual Machine    │  ── Executes the Chunk on a value stack
  └─────────────────────┘
```

---

## Pipeline

1. **Source text** is read from a `.cvm` file (or typed interactively in REPL mode).
2. The **Lexer** scans the text character-by-character, emitting a flat list of typed `Token` objects.
3. The **Parser** consumes the token list using recursive descent and constructs a typed AST made of `Stmt` and `Expr` nodes.
4. The **Compiler** traverses the AST via the Visitor pattern, resolving variable scopes and emitting bytecode into a `Chunk`.
5. The **VM** fetches, decodes, and dispatches each `OpCode` in the chunk, maintaining a value stack and a global/local variable store.
6. Optionally, the **Disassembler** can decode and print each instruction for debugging.

---

## Features

- **Hand-written recursive descent parser** — no tools like Bison or ANTLR
- **Typed AST** with full visitor pattern dispatch
- **Lexical scoping** with compile-time local variable slot resolution
- **Bytecode compilation** into a compact `Chunk` format
- **Stack-based VM** with a clean fetch-decode-execute loop
- **Short-circuit evaluation** for `and` / `or`
- **Control flow** — `if/else`, `while` loops with correct jump patching
- **Two-pass jump resolution** — forward jumps are emitted with placeholders and back-patched
- **First-class value types** — numbers (double), booleans, nil, strings via `std::variant`
- **Global and local variables** resolved at compile time (locals) and runtime (globals)
- **REPL** — interactive read-eval-print loop
- **Disassembler** for bytecode inspection
- **AST printer** for parse tree debugging
- **CLI flags** for toggling each pipeline stage

---

## Project Structure

```
cvm_new/
├── include/                  # Public headers
│   ├── token.hpp             # Token types and Token struct
│   ├── lexer.hpp             # Lexer interface
│   ├── ast.hpp               # AST node definitions (Stmt + Expr)
│   ├── ast_printer.hpp       # AST pretty-printer
│   ├── compiler.hpp          # Compiler interface and Local struct
│   ├── vm.hpp                # Virtual machine interface
│   └── disassembler.hpp      # Bytecode disassembler interface
│
├── src/                      # Implementation files
│   ├── main.cpp              # Entry point, CLI parsing, REPL
│   ├── lexer.cpp             # Lexer implementation
│   ├── parser.cpp            # Recursive descent parser
│   ├── ast_printer.cpp       # AST visitor printer
│   ├── compiler.cpp          # Bytecode compiler (Visitor over AST)
│   ├── vm.cpp                # VM fetch-decode-execute loop
│   └── disassembler.cpp      # Instruction decoder
│
├── build/                    # CMake build output (generated)
├── test.cvm                  # Sample source files
├── CMakeLists.txt            # Build configuration
└── README.md
```

---

## Components

### Lexer

**Files:** `include/lexer.hpp`, `src/lexer.cpp`

The lexer performs a single linear pass over the source string. It recognises:

- **Literals** — integer/float numbers, double-quoted strings, `true`, `false`, `nil`
- **Identifiers** and **reserved keywords** — `if`, `else`, `while`, `var`, `print`, `input`, `and`, `or`, `not`
- **Operators** — `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `=`, `!`
- **Delimiters** — `(`, `)`, `{`, `}`, `;`
- **Whitespace and comments** — skipped silently

Each token carries its `type`, `lexeme` (raw source text), and `line` number for error reporting.

---

### Parser

**Files:** `src/parser.cpp`

A hand-written **recursive descent parser** that consumes the token stream and produces a typed AST. Each grammar rule maps directly to a C++ method. The parser handles:

- Variable declarations (`var x = expr;`)
- Assignment expressions
- Binary and unary operators with correct precedence
- Logical operators with short-circuit semantics
- `if/else` statements
- `while` loops
- Block statements with `{}`
- `print` statements
- `input` statements
- Grouping with parentheses
- All literal types — numbers, strings, booleans, nil

---

### AST

**Files:** `include/ast.hpp`

The AST uses a **Visitor pattern** with two node hierarchies:

- `Stmt` — statement nodes: `VarDecl`, `BlockStmt`, `IfStmt`, `WhileStmt`, `PrintStmt`, `InputStmt`, `ExprStmt`
- `Expr` — expression nodes: `NumberLiteral`, `StringLiteral`, `BoolLiteral`, `NilLiteral`, `Variable`, `Assign`, `BinaryExpr`, `UnaryExpr`, `GroupingExpr`

Nodes are heap-allocated and stored as `unique_ptr` (`StmtPtr`, `ExprPtr`) for safe ownership.

---

### Compiler

**Files:** `include/compiler.hpp`, `src/compiler.cpp`

The compiler walks the AST using the Visitor pattern and emits bytecode into a `Chunk`. Key responsibilities:

- **Scope tracking** — `beginScope()` / `endScope()` manage a `m_locals` stack; local variables are resolved to numeric stack slots at compile time, eliminating name lookups at runtime.
- **Constant folding** — literals are added to the chunk's constant table and loaded with `OP_CONSTANT`.
- **Jump patching** — forward jumps (`OP_JUMP`, `OP_JUMP_IF_FALSE`) are emitted with `0xFFFF` placeholder offsets and back-patched once the target address is known.
- **Loop emission** — `emitLoop()` calculates and emits a backward jump offset for `while` loops.
- **Short-circuit logic** — `and`/`or` use conditional jumps rather than computing both sides unconditionally.

---

### Virtual Machine

**Files:** `include/vm.hpp`, `src/vm.cpp`

A **stack-based bytecode interpreter** with a clean fetch-decode-execute loop. The VM maintains:

- A **value stack** for operands and intermediate results
- A **globals map** (`string → Value`) for global variables
- A **locals array** using stack slots (indexed directly, O(1) access)

Value types are represented with `std::variant<std::monostate, double, bool, std::string>`, avoiding heap allocation for primitives.

**Supported opcodes include:**

| Opcode | Description |
|---|---|
| `OP_CONSTANT` | Push a constant from the table |
| `OP_TRUE / OP_FALSE / OP_NIL` | Push literal values |
| `OP_ADD / OP_SUB / OP_MUL / OP_DIV` | Arithmetic |
| `OP_EQUAL / OP_NOT_EQUAL` | Equality |
| `OP_LESS / OP_LESS_EQUAL / OP_GREATER / OP_GREATER_EQUAL` | Comparison |
| `OP_NEGATE / OP_NOT` | Unary operators |
| `OP_PRINT` | Pop and print top of stack |
| `OP_INPUT` | Read line from stdin, push as string |
| `OP_POP` | Discard top of stack |
| `OP_DEFINE_GLOBAL / OP_GET_GLOBAL / OP_SET_GLOBAL` | Global variable access |
| `OP_GET_LOCAL / OP_SET_LOCAL` | Local variable access by slot |
| `OP_JUMP / OP_JUMP_IF_FALSE` | Conditional and unconditional forward jumps |
| `OP_LOOP` | Unconditional backward jump |
| `OP_RETURN` | End execution |

---

### Disassembler

**Files:** `include/disassembler.hpp`, `src/disassembler.cpp`

Decodes a compiled `Chunk` and prints a human-readable listing of every instruction, its byte offset, line number, and operands. Used with the `--disasm` flag.

---

## Language Reference

```swift
// Variable declaration
var x = 10;
var name = "Alice";
var flag = true;

// Arithmetic
var result = (x + 5) * 2 / 3;

// String concatenation
var greeting = "Hello, " + name;

// Print
print result;
print greeting;

// Input
input name;

// If / else
if (x > 5) {
    print "big";
} else {
    print "small";
}

// While loop
var i = 0;
while (i < 5) {
    print i;
    i = i + 1;
}

// Logical operators (short-circuit)
if (x > 0 and flag) {
    print "both true";
}

// Blocks and lexical scoping
{
    var inner = 42;
    print inner;
}
// inner is out of scope here
```

---

## Building

### Prerequisites

| Tool | Minimum Version |
|---|---|
| g++ / MinGW | 10+ (C++20 support required; g++ 15 recommended) |
| CMake | 3.20+ |
| make / mingw32-make | any recent version |

### Steps

```powershell
# From the repository root
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

The executable `cvm.exe` will be produced in the `build/` directory.

> **Note:** Ensure your g++ is version 10 or higher for C++20 support. On Windows, install a modern g++ via [MSYS2](https://www.msys2.org):
> ```bash
> pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
> ```

---

## Running the Compiler

The compiler is invoked automatically whenever you pass a `.cvm` source file to `cvm.exe`. Internally, it runs the full pipeline: Lexer → Parser → Compiler, producing a `Chunk` of bytecode that is immediately handed off to the VM for execution.

### Compile and run a source file

```powershell
.\cvm.exe ..\test.cvm
```

### Inspect intermediate compiler outputs

Each flag below halts the pipeline at a specific stage, so you can see exactly what the compiler is producing without running the VM.

**1. View the token stream (after lexing)**

```powershell
.\cvm.exe --tokens ..\test.cvm
```

Example output for `var x = 10;`:

```
[1] VAR 'var'
[1] IDENTIFIER 'x'
[1] EQUAL '='
[1] NUMBER '10'
[1] SEMICOLON ';'
[1] EOF ''
```

**2. View the Abstract Syntax Tree (after parsing)**

```powershell
.\cvm.exe --ast ..\test.cvm
```

Example output for `var x = 10;`:

```
VarDecl(x)
  NumberLiteral(10)
```

**3. View the disassembled bytecode (after compilation)**

```powershell
.\cvm.exe --disasm ..\test.cvm
```

Example output for `var x = 10;`:

```
== <script> ==
0000  1 OP_CONSTANT         0 '10'
0002  | OP_DEFINE_GLOBAL    1 'x'
0004  | OP_RETURN
```

Each line shows: byte offset, source line, opcode name, operand index, and the resolved value or name.

---

## Running the VM

The VM executes the bytecode `Chunk` produced by the compiler. Under normal operation it runs silently — you only see output produced by `print` statements in your source. For debugging, the `--trace` flag enables per-instruction tracing.

### Execute a source file

```powershell
.\cvm.exe ..\test.cvm
```

### Run with VM instruction tracing

`--trace` prints the current instruction and a full dump of the value stack before each dispatch step. This lets you follow the exact state of the VM at every cycle.

```powershell
.\cvm.exe --trace ..\test.cvm
```

Example trace output for `print 1 + 2;`:

```
          [ ]
0000 OP_CONSTANT         0 '1'
          [ 1 ]
0002 OP_CONSTANT         1 '2'
          [ 1 ][ 2 ]
0004 OP_ADD
          [ 3 ]
0005 OP_PRINT
3
          [ ]
0006 OP_RETURN
```

Each stack slot is shown in `[ ]` brackets. The printed value appears inline when `OP_PRINT` fires.

### Start the interactive REPL

Running `cvm.exe` with no arguments launches the REPL. Each line you enter is independently lexed, compiled, and executed by the VM:

```powershell
.\cvm.exe
```

```
cvm> var x = 5;
cvm> print x * 2;
10
cvm> print "hello " + "world";
hello world
cvm>
```

> **Note:** The REPL treats each line as a self-contained program. Variables declared on one line persist for the duration of the session since they are stored in the VM's globals map.

---

## CLI Options

| Flag | Description |
|---|---|
| *(no flag)* | Compile and execute the source file |
| `--tokens` | Lex the source and print all tokens, then exit |
| `--ast` | Parse the source and print the AST, then exit |
| `--disasm` | Compile and print the disassembled bytecode, then exit |
| `--trace` | Execute with per-instruction VM tracing (stack dump at each step) |

### Examples

```powershell
# Print the token stream
.\cvm.exe --tokens ..\test.cvm

# Print the AST
.\cvm.exe --ast ..\test.cvm

# Disassemble the compiled bytecode
.\cvm.exe --disasm ..\test.cvm

# Run with VM instruction tracing
.\cvm.exe --trace ..\test.cvm
```

---

## Dependencies

CVM++ has **no external dependencies**. It uses only the C++20 standard library:

- `<variant>` — tagged union value type
- `<vector>`, `<string>`, `<unordered_map>` — standard containers
- `<fstream>`, `<sstream>` — file and string I/O
- `<iostream>` — console I/O
- `<memory>` — `unique_ptr` for AST node ownership

---

## Implementation Notes

- **Local variable resolution is entirely at compile time.** The compiler assigns each local a stack slot index; the VM accesses locals by index with no name lookup at runtime.
- **Forward jump offsets use 16-bit placeholders** (`0xFFFF`) emitted at compile time and back-patched once the target offset is known — the standard technique used in production bytecode compilers.
- **`std::variant` avoids boxing.** Primitive values (numbers, booleans, nil) live directly in the variant without heap allocation.
- **The visitor pattern** cleanly separates the AST structure from the operations performed on it, making it straightforward to add new pipeline stages (e.g. a type checker or optimizer) without modifying the AST nodes.
