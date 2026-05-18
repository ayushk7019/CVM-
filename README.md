# CVM++ (Custom Virtual Machine)

> A complete stack-based programming language and bytecode virtual machine written in modern C++.

CVM++ is an educational yet fully functional compiler toolchain that takes source code written in a custom language (`.cvm` files), converts it into bytecode, and executes it on a custom-built virtual machine.

---

## 🔗 Quick Links & Demos

- 🎬 **Demo Video:** [Watch the Full Compiler & VM Trace Pipeline in Action](https://drive.google.com/file/d/19MzQ9OTiXQt9k7xAWxA2MIF4-BUnmIQW/view?usp=drivesdk)
- 🌐 **Live Web Demo:** [Interactive Online Compiler & Playpen](https://6a0b04c4ddba698d4ad170e0--tranquil-sawine-7418fe.netlify.app/)

---

This project demonstrates the core concepts behind language design and implementation, including:

- Lexical Analysis (Tokenizer / Scanner)
- Parsing and Abstract Syntax Tree (AST) generation
- Bytecode Compilation
- Stack-Based Virtual Machine Execution
- Disassembly and Debug Tracing
- REPL (Read-Eval-Print Loop)
- Variable Scoping and Control Flow

---

## ✨ Project Highlights

- Built from scratch in **C++17**
- Uses **CMake** for cross-platform builds
- Supports both **interactive REPL** and **script execution**
- Includes debugging tools:
  - Token viewer
  - AST printer
  - Bytecode disassembler
  - Execution trace
- Implements global and local variables
- Supports arithmetic, logical operations, and comparisons
- Includes conditional statements and loops
- Designed with clean modular architecture

---

## 📚 Table of Contents

1. [Project Overview](#project-overview)
2. [Language Features](#language-features)
3. [Example Programs](#example-programs)
4. [Architecture](#architecture)
5. [Compilation Pipeline](#compilation-pipeline)
6. [Project Structure](#project-structure)
7. [Build Instructions](#build-instructions)
8. [Running the Compiler](#running-the-compiler)
9. [Running the VM](#running-the-vm)
10. [Usage Guide](#usage-guide)
11. [Debugging Modes](#debugging-modes)
12. [Bytecode Instruction Set](#bytecode-instruction-set)
13. [Internal Components](#internal-components)
14. [Design Decisions](#design-decisions)
15. [Future Improvements](#future-improvements)
16. [Educational Value](#educational-value)
17. [License](#license)

---

## Project Overview

CVM++ is a mini programming language implementation inspired by language runtimes such as:

- Python
- Lua
- Java Virtual Machine (JVM)
- CPython Bytecode Interpreter
- The clox VM from the book *Crafting Interpreters* by Robert Nystrom

The project allows users to write source code like:

```swift
let x = 10;

if x > 5 {
    let y = x * 2;
    print y;
} else {
    print false;
}
```

And execute it through the following stages:

```
Source Code
    ↓
  Lexer
    ↓
 Tokens
    ↓
 Parser
    ↓
   AST
    ↓
Compiler
    ↓
Bytecode Chunk
    ↓
Virtual Machine
    ↓
  Output
```

---

## Language Features

### Data Types

CVM++ currently supports:

| Type | Description |
|------|-------------|
| `Number` | Floating-point values |
| `Boolean` | `true`, `false` |
| `Nil` | `nil` |

### Variables

**Declaration**
```swift
let x = 42;
```

**Assignment**
```swift
x = x + 1;
```

**Scope** — Variables respect block scope:
```swift
let x = 10;
{
    let x = 20;
    print x;  // 20
}
print x;  // 10
```

### Arithmetic Operators

| Operator | Meaning |
|----------|---------|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| Unary `-` | Negation |

### Comparison Operators

| Operator | Meaning |
|----------|---------|
| `==` | Equal |
| `!=` | Not Equal |
| `<` | Less Than |
| `<=` | Less Than or Equal |
| `>` | Greater Than |
| `>=` | Greater Than or Equal |

### Logical Operators

| Operator | Meaning |
|----------|---------|
| `!` | Logical NOT |

### Input and Output

**Print**
```swift
print x;
```

**Input**
```swift
let age = input;
print age;
```

### Conditional Statements

```swift
if x > 10 {
    print true;
} else {
    print false;
}
```

### Loops

The VM includes jump and loop opcodes, enabling iterative constructs:

```swift
while x < 10 {
    x = x + 1;
}
```

> **Note:** Exact syntax depends on parser support, but the opcode infrastructure (`OP_LOOP`, `OP_JUMP`) is fully implemented.

---

## Example Programs

### 1. Basic Arithmetic

```swift
print 5 + 3 * 2;
```

**Output:**
```
11
```

### 2. Variables

```swift
let a = 10;
let b = 20;
print a + b;
```

**Output:**
```
30
```

### 3. Conditional Execution

```swift
let x = 4;
if x > 5 {
    print 100;
} else {
    print 0;
}
```

**Output:**
```
0
```

### 4. User Input

```swift
print input;
```

---

## Architecture

```
┌─────────────┐
│ Source Code │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│    Lexer    │
└──────┬──────┘
       │ Tokens
       ▼
┌─────────────┐
│   Parser    │
└──────┬──────┘
       │ AST
       ▼
┌─────────────┐
│  Compiler   │
└──────┬──────┘
       │ Bytecode
       ▼
┌─────────────┐
│ Virtual VM  │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Output    │
└─────────────┘
```

---

## Compilation Pipeline

### 1. Lexer (`lexer.cpp`)

Converts raw source code into tokens.

**Example** — `let x = 10;` produces:

```
LET  IDENTIFIER  EQUAL  NUMBER  SEMICOLON  EOF
```

**Responsibilities:**
- Character scanning
- Number parsing
- Keyword recognition
- Error reporting with line and column numbers

### 2. Parser (`parser.cpp`)

Uses recursive descent parsing to transform tokens into an Abstract Syntax Tree.

**Handles:**
- Expressions
- Variable declarations
- Assignments
- Print statements
- If/else blocks
- Block scopes

### 3. AST (`ast.hpp`)

Represents the syntactic structure of programs.

**Expression nodes:**
- `LiteralExpr`
- `UnaryExpr`
- `BinaryExpr`
- `VariableExpr`
- `AssignExpr`

**Statement nodes:**
- `ExpressionStmt`
- `PrintStmt`
- `VarStmt`
- `BlockStmt`
- `IfStmt`

### 4. Compiler (`compiler.cpp`)

Traverses the AST and emits bytecode instructions.

**Responsibilities:**
- Constant pool management
- Scope tracking
- Symbol resolution
- Jump patching
- Emitting opcodes and operands

### 5. Bytecode Chunk (`chunk.hpp`)

Stores:
- Instruction bytes
- Constant values
- Source line information

### 6. Virtual Machine (`vm.cpp`)

Executes bytecode using:
- Operand stack
- Global variable table
- Instruction pointer

### 7. Disassembler (`disassembler.cpp`)

Converts bytecode into human-readable instructions. Useful for debugging and understanding generated machine code.

---

## Project Structure

```
CVM/
├── CMakeLists.txt
├── include/
│   ├── ast.hpp
│   ├── ast_printer.hpp
│   ├── chunk.hpp
│   ├── compiler.hpp
│   ├── disassembler.hpp
│   ├── lexer.hpp
│   ├── opcode.hpp
│   ├── parser.hpp
│   ├── token.hpp
│   ├── value.hpp
│   └── vm.hpp
│
├── src/
│   ├── ast_printer.cpp
│   ├── compiler.cpp
│   ├── disassembler.cpp
│   ├── lexer.cpp
│   ├── main.cpp
│   ├── parser.cpp
│   └── vm.cpp
│
├── test.cvm
├── test2.cvm
├── test3.cvm
├── test4.cvm
└── README.md
```

---

## Build Instructions

### Prerequisites

| Tool | Requirement |
|------|-------------|
| C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+) | C++17 support required |
| CMake | 3.10+ |

### Clone Repository

```bash
git clone <repository-url>
cd CVM
```

### Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

This produces an executable:
- **Windows:** `cvm.exe`
- **Linux/macOS:** `cvm`

---

## Running the Compiler

The compiler is invoked automatically when you pass a `.cvm` source file to the executable. Internally it runs the full front-end pipeline — **Lexer → Parser → Compiler** — producing a `Chunk` of bytecode that is handed to the VM for execution.

### Compile and run a source file

**Linux / macOS**
```bash
./cvm test.cvm
```

**Windows**
```powershell
.\cvm.exe test.cvm
```

### Inspect each compiler stage

Use the flags below to halt the pipeline at a specific stage and print what the compiler produced, without running the VM.

**Step 1 — View the token stream (after lexing)**
```bash
./cvm --tokens test.cvm
```

Example — for `let x = 10;`:
```
[1] LET        'let'
[1] IDENTIFIER 'x'
[1] EQUAL      '='
[1] NUMBER     '10'
[1] SEMICOLON  ';'
[1] EOF        ''
```

**Step 2 — View the Abstract Syntax Tree (after parsing)**
```bash
./cvm --ast test.cvm
```

Example — for `let x = 10;`:
```
VarDecl(x)
  LiteralExpr(10)
```

**Step 3 — View the disassembled bytecode (after compilation)**
```bash
./cvm --disasm test.cvm
```

Example — for `let x = 10;`:
```
== <script> ==
0000  1 OP_CONSTANT       0 '10'
0002  | OP_DEFINE_GLOBAL  1 'x'
0004  | OP_RETURN
```

Each line shows: `byte offset · source line · opcode name · operand index · resolved value or name`.

> This is the compiled output — the sequence of bytecode instructions that will be handed to the VM for execution.

---

## Running the VM

The VM executes the bytecode `Chunk` produced by the compiler. Under normal operation it runs silently — you only see output from `print` statements in your source. Use `--trace` to make the VM's internal state visible at every instruction cycle.

### Execute a source file (normal run)

**Linux / macOS**
```bash
./cvm test.cvm
```

**Windows**
```powershell
.\cvm.exe test.cvm
```

### Run with VM execution tracing

`--trace` prints the current instruction and a full dump of the value stack before each dispatch step, letting you follow the exact state of the VM cycle by cycle.

```bash
./cvm --trace test.cvm
```

Example — for `print 1 + 2;`:
```
          [ ]
0000 OP_CONSTANT       0 '1'
          [ 1 ]
0002 OP_CONSTANT       1 '2'
          [ 1 ][ 2 ]
0004 OP_ADD
          [ 3 ]
0005 OP_PRINT
3
          [ ]
0006 OP_RETURN
```

Each stack slot is shown in `[ ]` brackets. The result of `OP_PRINT` appears inline when it fires.

### Recommended demo sequence

Run these four commands in order to show the full pipeline — from source text to final output:

```bash
# 1. Show the token stream produced by the lexer
./cvm --tokens test.cvm

# 2. Show the AST produced by the parser
./cvm --ast test.cvm

# 3. Show the bytecode produced by the compiler  ← the "compiled" output
./cvm --disasm test.cvm

# 4. Execute on the VM with full instruction tracing
./cvm --trace test.cvm

# 5. Clean final run — just the program output
./cvm test.cvm
```

---

## Usage Guide

### Run a Script

```bash
./cvm test.cvm
```

### Start REPL

```bash
./cvm
```

Example session:
```
CVM++ REPL v0.1 (type :exit to quit)
cvm> print 2 + 3;
5
cvm> let x = 10;
cvm> print x * 2;
20
cvm> :exit
```

> **Note:** Variables declared in the REPL persist for the duration of the session because they are stored in the VM's `globals` map.

---

## Debugging Modes

### View Tokens

```bash
./cvm --tokens test.cvm
```

Displays all tokens with their type, lexeme, and line number.

### View AST

```bash
./cvm --ast test.cvm
```

Prints the full parsed Abstract Syntax Tree.

### View Bytecode Disassembly

```bash
./cvm --disasm test.cvm
```

Shows all generated bytecode instructions with byte offsets and operand values.

### Trace Execution

```bash
./cvm --trace test.cvm
```

Prints the value stack and the currently executing instruction before every dispatch step.

---

## Bytecode Instruction Set

### Literals and Constants

| Opcode | Description |
|--------|-------------|
| `OP_CONSTANT` | Push a constant value onto the stack |
| `OP_TRUE` | Push boolean `true` |
| `OP_FALSE` | Push boolean `false` |
| `OP_NIL` | Push `nil` |

### Stack Operations

| Opcode | Description |
|--------|-------------|
| `OP_POP` | Discard the top of the stack |

### Arithmetic

| Opcode | Description |
|--------|-------------|
| `OP_ADD` | Addition |
| `OP_SUB` | Subtraction |
| `OP_MUL` | Multiplication |
| `OP_DIV` | Division |
| `OP_NEGATE` | Unary negation |

### Comparison

| Opcode | Description |
|--------|-------------|
| `OP_EQUAL` | Equality check |
| `OP_NOT_EQUAL` | Inequality check |
| `OP_LESS` | Less than |
| `OP_LESS_EQUAL` | Less than or equal |
| `OP_GREATER` | Greater than |
| `OP_GREATER_EQUAL` | Greater than or equal |

### Logical

| Opcode | Description |
|--------|-------------|
| `OP_NOT` | Logical NOT |

### Variables

| Opcode | Description |
|--------|-------------|
| `OP_DEFINE_GLOBAL` | Define a global variable |
| `OP_GET_GLOBAL` | Read a global variable |
| `OP_SET_GLOBAL` | Write a global variable |
| `OP_GET_LOCAL` | Read a local variable by stack slot |
| `OP_SET_LOCAL` | Write a local variable by stack slot |

### Control Flow

| Opcode | Description |
|--------|-------------|
| `OP_JUMP` | Unconditional forward jump |
| `OP_JUMP_IF_FALSE` | Conditional forward jump |
| `OP_LOOP` | Unconditional backward jump |

### Input / Output

| Opcode | Description |
|--------|-------------|
| `OP_PRINT` | Pop and print top of stack |
| `OP_INPUT` | Read a line from stdin, push as value |

### Program Termination

| Opcode | Description |
|--------|-------------|
| `OP_RETURN` | End execution |

---

## Internal Components

### Value

Represents dynamically typed runtime values. Supports:
- Numbers
- Booleans
- Nil

### Token

Contains:
- Token type
- Lexeme
- Literal value
- Line and column information

### Chunk

Stores:
- Bytecode array
- Constant pool
- Line metadata

### VM

Maintains:
- Operand stack
- Globals map
- Instruction pointer

---

## Design Decisions

### Stack-Based VM

A stack machine simplifies code generation. For example:

```
2 + 3 * 4
```

Compiles conceptually to:

```
PUSH 2
PUSH 3
PUSH 4
MUL
ADD
```

### Recursive Descent Parsing

Chosen because it is:
- Easy to implement
- Highly readable
- Simple to extend
- Ideal for expression grammars

### Bytecode Interpretation

Separates front-end and runtime concerns.

**Benefits:**
- Faster than directly walking the AST
- Easier debugging and optimization
- Closer to production language runtimes

---

## Future Improvements

Potential enhancements include:

- Functions and return values
- Closures
- Strings
- Arrays and dictionaries
- Classes and objects
- Garbage collection
- Modules and imports
- Native standard library functions
- JIT compilation
- Optimizations such as constant folding

---

## Educational Value

This project is highly valuable for understanding:

- Compiler design
- Programming language implementation
- Virtual machines
- Parsing techniques
- Memory and stack management
- Symbol tables and scoping
- Debugging infrastructure

It is suitable for:
- Systems programming portfolios
- Compiler and interpreter coursework
- Interview discussions
- Teaching language internals

---

## 📄 Sample Resume Description

> **Custom Programming Language and Virtual Machine (C++17)**
> Designed and implemented a complete stack-based programming language from scratch in C++17, featuring lexical analysis, recursive descent parsing, AST generation, bytecode compilation, and a custom virtual machine with REPL, disassembler, and execution tracing.

---

## Performance Characteristics

| Stage | Time Complexity |
|-------|----------------|
| Lexing | O(n) |
| Parsing | O(n) |
| Compilation | O(n) |
| Execution | O(number of bytecode instructions) |

*Where `n` is the length of the source code.*

---

## Why This Project Stands Out

Unlike small toy interpreters, CVM++ includes the **complete language toolchain**:

| Layer | Component |
|-------|-----------|
| Frontend | Lexer + Parser |
| Intermediate Representation | AST |
| Backend | Compiler |
| Runtime | VM |
| Developer Tooling | REPL, AST Printer, Disassembler, Trace Mode |

This demonstrates strong understanding of:
- Low-level programming
- Software architecture
- Compiler theory
- Runtime systems

---

## License

See `LICENSE` for details.
