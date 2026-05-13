# CVM++

CVM++ is a small C++ virtual machine/compiler project. It builds a single executable (`cvm`) from the sources in `src/` and headers in `include/`.

## Features

- Lexing and token scanning
- Parsing into an AST
- AST printing
- Bytecode compilation
- VM execution
- Optional disassembler output
- REPL support

## Building

From the repository root:

```powershell
mkdir -Force build
cd build
cmake ..
cmake --build . --config Release
```

If you are using non-MSVC tooling, the `--config Release` option is optional.

## Running

Run the executable from `build` or provide the path to a `.cvm` source file.

```powershell
.uild\cvm.exe ..\test.cvm
```

Or from inside `build`:

```powershell
.\in\cvm.exe ..\test.cvm
```

If no file path is provided, `cvm` starts an interactive REPL.

## CLI Options

- `--tokens`: print scanned tokens and exit
- `--ast`: print the AST and exit
- `--disasm`: disassemble the compiled chunk and exit
- `--trace`: execute with VM tracing enabled

Example:

```powershell
.uild\cvm.exe --ast ..\test.cvm
```

## Notes

- The current `CMakeLists.txt` defines only the `cvm` executable.
- There is no automated test suite configured in this repository yet.

## Project structure

- `include/`: public headers
- `src/`: implementation files
- `build/`: CMake build artifacts
- `test*.cvm`: sample source files
