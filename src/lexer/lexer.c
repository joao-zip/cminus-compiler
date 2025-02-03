#include "lexer.h"
#include "lexer_hash.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int VERBOSE_LEXER = 0;
FILE *filename = NULL;

int current_line = 1;
int current_column = 0;

// ----------------------- Functions ----------------------

void set_verbose_lexer(int is_verbose) { VERBOSE_LEXER = is_verbose; }

void init_lexer(const char *file) {
  if (filename) {
    fclose(filename);
  }
  filename = fopen(file, "r");

  lexer_hash_init();
}

void close_lexer() {
  fclose(filename);

  lexer_hash_delete();
}

token_t *get_next_token() {
  int state = 0; // DFA initial state
  int c;
  char lexeme[LEXEME_MAX_SIZE] = {0};
  int lexeme_index = 0;

  while (1) {
    c = get_next_char();

    if (c == EOF) {
      return NULL; // End of file
    }
    if (IS_WHITESPACE(c) && state == 0) {
      continue; // Ignore whitespaces
    }
    // DFA implementation with switch cases
    switch (state) {
    case 0:
      if (isalpha(c)) {
        lexeme[lexeme_index++] = c;
        state = 1; // State for identifiers and reserved words
      } else if (isdigit(c)) {
        lexeme[lexeme_index++] = c;
        state = 2; // State for numbers
      } else {
        // Verifies operators and special symbols
        switch (c) {
        case '+':
          return create_token(TOKEN_PLUS, "+");
        case '-':
          return create_token(TOKEN_MINUS, "-");
        case '*':
          return create_token(TOKEN_MULT, "*");
        case '/':
          return create_token(TOKEN_DIV, "/");
        case '<':
          state = 3;
          break; // < or <=
        case '>':
          state = 4;
          break; // > or >=
        case '=':
          state = 5;
          break; // = or ==
        case '!':
          state = 6;
          break; // !=
        case ';':
          return create_token(TOKEN_DELIM, ";");
        case ',':
          return create_token(TOKEN_COMMA, ",");
        case '(':
          return create_token(TOKEN_LPARENT, "(");
        case ')':
          return create_token(TOKEN_RPARENT, ")");
        case '{':
          return create_token(TOKEN_LKEY, "{");
        case '}':
          return create_token(TOKEN_RKEY, "}");
        case '[':
          return create_token(TOKEN_LBRACKET, "[");
        case ']':
          return create_token(TOKEN_RBRACKET, "]");
        default:
          return create_token(TOKEN_UNKNOWN, (char[]){c, '\0'});
        }
      }
      break;
    case 1: // Identifiers or reserved words
      if (isalpha(c)) {
        lexeme[lexeme_index++] = c;
      } else if (IS_ID_SEPARATOR(c)) {
        unget_char(c);
        lexeme[lexeme_index] = '\0';
        token_types_t type = lexer_lookup_reserved_word(lexeme);
        return create_token(type, lexeme);
      } else {
        return create_token(TOKEN_UNKNOWN, lexeme);
      }
      break;
    case 2: // Numbers
      if (isdigit(c)) {
        lexeme[lexeme_index++] = c;
      } else if (IS_NUM_SEPARATOR(c)) {
        unget_char(c);
        lexeme[lexeme_index] = '\0';
        return create_token(TOKEN_NUM, lexeme);
      } else {
        return create_token(TOKEN_UNKNOWN, lexeme);
      }
      break;
    case 3: // < or <=
      if (c == '=') {
        return create_token(TOKEN_LE, "<=");
      } else {
        unget_char(c);
        return create_token(TOKEN_LT, "<");
      }
      break;
    case 4: // > or >=
      if (c == '=') {
        return create_token(TOKEN_GE, ">=");
      } else {
        unget_char(c);
        return create_token(TOKEN_GT, ">");
      }
      break;
    case 5: // = or ==
      if (c == '=') {
        return create_token(TOKEN_EQ, "==");
      } else {
        unget_char(c);
        return create_token(TOKEN_ATTR, "=");
      }
      break;
    case 6: // !=
      if (c == '=') {
        return create_token(TOKEN_DIFF, "!=");
      } else {
        unget_char(c);
        return create_token(TOKEN_UNKNOWN, "!");
      }
      break;
    default:
      return create_token(TOKEN_UNKNOWN, (char[]){c, '\0'});
    }
  }
}

token_t *create_token(token_types_t type, const char *lexeme) {
  token_t *token = (token_t *)malloc(sizeof(token_t));
  token->type = type;
  strncpy(token->lexeme, lexeme, LEXEME_MAX_SIZE - 1);
  token->lexeme[LEXEME_MAX_SIZE - 1] = '\0';
  token->line = current_line;
  token->column = current_column - strlen(lexeme);

  if (VERBOSE_LEXER)
    print_token(token);

  if (token->type == TOKEN_UNKNOWN) {
    print_error(token);
    exit(EXIT_FAILURE);
  }

  return token;
}

token_t *delete_token(token_t *token) {
  free(token);

  return NULL;
}

char *print_token_classes(token_types_t type) {
  switch (type) {
  case TOKEN_ID:
    return "IDENTIFIER";
  case TOKEN_NUM:
    return "NUMBER";
  case TOKEN_ELSE:
    return "ELSE";
  case TOKEN_IF:
    return "IF";
  case TOKEN_INT:
    return "INT";
  case TOKEN_RETURN:
    return "RETURN";
  case TOKEN_VOID:
    return "VOID";
  case TOKEN_WHILE:
    return "WHILE";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_MULT:
    return "MULTIPLY";
  case TOKEN_DIV:
    return "DIVIDE";
  case TOKEN_LT:
    return "LESS THAN";
  case TOKEN_LE:
    return "LESS OR EQUAL";
  case TOKEN_GT:
    return "GREATER THAN";
  case TOKEN_GE:
    return "GREATER OR EQUAL";
  case TOKEN_EQ:
    return "EQUAL";
  case TOKEN_DIFF:
    return "NOT EQUAL";
  case TOKEN_ATTR:
    return "ASSIGN";
  case TOKEN_DELIM:
    return "SEMICOLON";
  case TOKEN_COMMA:
    return "COMMA";
  case TOKEN_LCOMM:
    return "LEFT COMMENT";
  case TOKEN_RCOMM:
    return "RIGHT COMMENT";
  case TOKEN_LPARENT:
    return "LEFT PARENTHESIS";
  case TOKEN_RPARENT:
    return "RIGHT PARENTHESIS";
  case TOKEN_LKEY:
    return "LEFT BRACE";
  case TOKEN_RKEY:
    return "RIGHT BRACE";
  case TOKEN_LBRACKET:
    return "LEFT BRACKET";
  case TOKEN_RBRACKET:
    return "RIGHT BRACKET";
  case TOKEN_UNKNOWN:
    return "UNKNOWN";
  default:
    return "INVALID TOKEN TYPE";
  }
}

void print_token(token_t *token) {
  printf("%s \"%s\" [linha: %d]\n", print_token_classes(token->type),
         token->lexeme, token->line);
}

void print_error(token_t *token) {
  printf("\033[31mERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n\033[0m", token->lexeme,
         token->line, token->column);
}

// ----------------------- General Helpers ----------------------

int get_next_char() {
  int c = fgetc(filename);
  if (c == '\n') {
    current_line++;
    current_column = 0;
  } else {
    current_column++;
  }

  return c;
}

void unget_char(int c) {
  if (c != EOF) {
    ungetc(c, filename);
    if (c == '\n') {
      current_line--;
    } else {
      current_column--;
    }
  }
}
