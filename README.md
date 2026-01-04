# C Compiler (x86 Generation) 💻

A fully functional **C Compiler** implemented from scratch. This project handles the entire compilation pipeline, from source code to optimized x86 assembly language.

## 🚀 Features

- **Lexical Analysis**: Tokenization using **Lex**.
- **Parsing**: Grammar validation and parse tree generation using **Yacc**.
- **Symbol Table**: Robust management of identifiers, scopes, and types.
- **Code Generation**: Outputs standard **x86 Assembly** code.
- **Optimization**: Basic code optimization techniques.

## 🛠️ Tech Stack

- **Language**: C/C++
- **Tools**: Lex (Flex), Yacc (Bison)
- **Target**: x86 Assembly

## 📦 Usage

1. **Build the Compiler**
   ```bash
   make
   ```

2. **Compile a C file**
   ```bash
   ./compiler input.c -o output.asm
   ```

3. **Assemble and Link (Linux)**
   ```bash
   nasm -f elf32 output.asm -o output.o
   gcc -m32 output.o -o output
   ./output
   ```

## 📄 License

MIT License
