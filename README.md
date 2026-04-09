# Recursive-Descent Parser for C-

**Course:** CSCE 4101 – Compiler Design  
**Assignment:** 2 – Recursive-Descent Parser  
**Professor:** Ahmed Rafea  
**Student:** Mai Mahmoud Sayed | 900213311  
**Semester:** Spring 2026

---

## Overview

A recursive-descent parser for the C- (C Minus) programming language, built in C. The parser reads a C- source file, validates it against the grammar, and either reports the first syntax error with its line number and cause, or prints a success message.

The scanner from Assignment 1 is reused as the token source.

---

## Grammar

The parser implements all 18 rules of the C- grammar. Three rules with direct left recursion (Rules 2, 14, 16) were rewritten as iterative loops.

Rule 5 was corrected per the professor's announcement. The original rule:

```
statement-list -> statement-list ; statement | empty
```

was a typo. The correct rule is:

```
statement-list -> statement-list ; statement | statement
```

After left-recursion elimination this becomes `statement { ; statement }`, meaning every statement-list must contain at least one statement.

---

## Project Structure

```
parser_project/
├── tokens.h              Token type definitions (TokenType enum, Token struct)
├── scanner.h             Scanner public interface
├── scanner.c             Lexical analyser implementation
├── parser.h              Parser public interface
├── parser.c              Recursive-descent parser (all 18 grammar rules)
├── main.c                Entry point
├── Makefile              Build system
└── tests/
    ├── test1.cm          Valid test: arithmetic, if-else, while loop
    ├── test2.cm          Valid test: arrays, nested if, relational operators
    ├── test3_err.cm      Error test: missing opening brace
    ├── test4_err.cm      Error test: incomplete expression
    ├── test5_err.cm      Error test: missing assignment operator
    └── test6_err.cm      Error test: empty statement-list (demonstrates corrected Rule 5)
```

---

## Requirements

- GCC (any version supporting C99)
- Make

---

## Build

```bash
git clone https://github.com/your-username/parser_project.git
cd parser_project
make
```

To clean compiled files:

```bash
make clean
```

---

## Usage

```bash
./cparser <source_file.cm>
```

On success:

```
Parsing completed successfully.
```

On error:

```
Syntax error at line <N>: unexpected token "<lexeme>", expected <description>
```

Compilation stops at the first syntax error. Error recovery is not implemented.

---

## Running the Test Cases

```bash
./cparser tests/test1.cm
./cparser tests/test2.cm
./cparser tests/test3_err.cm
./cparser tests/test4_err.cm
./cparser tests/test5_err.cm
./cparser tests/test6_err.cm
```

Expected output:

| File | Expected Output |
|---|---|
| tests/test1.cm | Parsing completed successfully. |
| tests/test2.cm | Parsing completed successfully. |
| tests/test3_err.cm | Syntax error at line 3: unexpected token "int", expected '{' |
| tests/test4_err.cm | Syntax error at line 8: unexpected token "}", expected a factor... |
| tests/test5_err.cm | Syntax error at line 7: unexpected token "10", expected '=' |
| tests/test6_err.cm | Syntax error at line 5: unexpected token "}", expected a statement... |

---

## Implementation Notes

- One C function per non-terminal in the grammar
- Left recursion in Rules 2, 14, and 16 replaced with while loops
- Dangling-else resolved greedily: else always binds to the nearest unmatched if
- Semicolon is a separator between statements, not a terminator
- Syntax errors report line number, offending token, and expected token(s), then exit immediately

---

## Windows

If you are on Windows, compile using WSL:

```bash
wsl
cd /mnt/c/Users/YourName/parser_project
make
./cparser tests/test1.cm
```

Or compile manually without Make:

```bash
gcc -Wall -std=c99 -g main.c scanner.c parser.c -o cparser
```
