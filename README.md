# C- Compiler

Name: João Pedro Martins Olveira

This is my implementation of a simple C- compiler for the compiler classes at [UNIFESP](https://portal.unifesp.br/).

## How to run the project

If you want to run this project, please make sure you have installed gcc and cmake.

``` {bash}
$ mkdir build && cd build
$ cmake .. && make
$ ./cmc _the executable name_
```

If you don't have cmake, please use the command in the root directory:

``` {bash}
$ gcc -Wall -Wextra src/lexer/lexer.c src/lexer/lexer_hash.c src/parser/ast_printer.c src/parser/parser.c src/main.c -o cmc
```

### Notes

- The parser isn't performing correctly;
- I need to add semantic analysis and mid-level code generation/optimization;
- If you don't know how to use the program, just run cmc
