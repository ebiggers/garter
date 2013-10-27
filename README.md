# Introduction

This is a compiler and interpreter for a toy programming language called
"garter".  It is primarily intended to serve of an example of implementing
a simple compiler.  For its front-end it a uses hand-coded C++ lexer and a
hand-coded C++ recursive descent parser, and for its backend it uses LLVM.
More specifically, the backend uses the LLVM libraries to translate the AST
(abstract syntax tree) to LLVM IR (intermediate language), optimize the IR,
and generate native code.

# Files and directories

  - COPYING:       License for all source code
  - garter.pdf:    Description of language syntax, grammar, and features
  - frontend/:     Compiler frontend (lexer and parser)
  - backend/:      Compiler backend (bridge to LLVM)
  - runtime/:      Implementations for functions that can be called by
                   garter code.  The `**` operator generates calls to
		   `__garter_exponentiate()` while the `print` statement
		   generates calls to `__garter_print()`.
  - garterc.cpp:   `main()` for compiler program
  - garteri.cpp:   `main()` for interpreter program
  - test/:         Automated tests

# Portability notes

  - Code relies on a compiler supporting C++11
    (tested with g++ 4.8.2 and clang++ 3.3)
  - Code was tested using LLVM 3.3.  Makefile will attempt to dynamically link
    to -lLLVM-3.3; this can be changed to static linking instead.
  - runtime/ is expected to be in the same directory as the `garterc` program.
