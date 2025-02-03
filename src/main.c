#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definitions

int LEXER_ONLY = 0;
int PARSER_ONLY = 0;

// Functions

//! Default help function
void show_help();

//! Option to run only the lexer part
void lexer_only(int option);

//! Option to run only the sintatic analysis part
void parser_only(int option);



int main(int argc, char *argv[]) {

  if (argc < 2) {
    show_help();
    return EXIT_FAILURE;
  }

  int file_position = -1;
  for (int i = 1; i < argc; i++) {
    if (!strcmp("-l", argv[i]) || !strcmp("-L", argv[i]) ||
        !strcmp("--lexer", argv[i])) {
      set_verbose_lexer(1);
    } else if (!strcmp("-p", argv[i]) || !strcmp("-P", argv[i]) ||
               !strcmp("--parser", argv[i])) {
      set_verbose_parser(1);
    } else if (!strcmp("-lexer-only", argv[i])) {
      lexer_only(1);
    } else if (!strcmp("-parser-only", argv[i])) {
      parser_only(1);
    } else if (strstr(argv[i], ".c") != NULL) {
      file_position = i;
    }
  }

  if (file_position != -1)
    init_lexer(argv[file_position]);
  else {
    fprintf(stderr, "Error while opening file: %s\n", argv[file_position]);
    return EXIT_FAILURE;
  }

  if (LEXER_ONLY && !PARSER_ONLY) {
    token_t *token;
    while((token = get_next_token())) {
      delete_token(token);
    }

    close_lexer();

    return EXIT_SUCCESS;
  } else if (PARSER_ONLY) {
    ast_node_t *ast = parse_program();

    destroy_ast_root(ast);
    close_lexer();

    return EXIT_SUCCESS;
  }

  ast_node_t *ast = parse_program();

  destroy_ast_root(ast);
  close_lexer();

  return EXIT_SUCCESS;
}

// Function implementations

void show_help() {
  puts("--------------------------------------- C- Compiler "
       "---------------------------------------");
  puts("Use: ./cmc <args..> <file>");
  puts("Options:");
  puts("  -l  -L --lexer                     -- prints the tokens of the lexic "
       "analysis");
  puts("  -p  -P --parser                    -- prints the ASTree after "
       "completing the sintatic analysis");
  puts("  --lexer-only                       -- stops the execution of the "
       "program after finishing the lexic analysis");
  puts("  --parser-only                      -- stops the execution of the "
       "program after finishing the sintatic analysis");
  puts("\nCreator: João Pedro Martins Oliveira, for the compiler classes at "
       "UNIFESP");
}

void lexer_only(int option) {
  LEXER_ONLY = option;
}

void parser_only(int option) {
  PARSER_ONLY = option;
}
