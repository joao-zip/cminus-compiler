#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int VERBOSE_LEXER = 0;
FILE *filename = NULL;

hash_table_t lexer_hash_table;

int current_line = 1;
int current_column = 0;

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
  int state = 0; // Estado inicial do DFA
  int c;
  char lexeme[LEXEME_MAX_SIZE] = {0};
  int lexeme_index = 0;

  while (1) {
    c = get_next_char();

    if (c == EOF) {
      return NULL; // Fim do arquivo
    }
    if (IS_WHITESPACE(c) && state == 0) {
      continue; // Ignora espaços em branco
    }
    // Implementação do DFA
    switch (state) {
    case 0:
      if (isalpha(c)) {
        lexeme[lexeme_index++] = c;
        state = 1; // Estado para identificadores ou palavras reservadas
      } else if (isdigit(c)) {
        lexeme[lexeme_index++] = c;
        state = 2; // Estado para números
      } else {
        // Verifica operadores e símbolos especiais
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
          break; // Pode ser < ou <=
        case '>':
          state = 4;
          break; // Pode ser > ou >=
        case '=':
          state = 5;
          break; // Pode ser = ou ==
        case '!':
          state = 6;
          break; // Pode ser !=
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
    case 1: // Identificadores ou palavras reservadas
      if (isalnum(c)) {
        lexeme[lexeme_index++] = c;
      } else {
        unget_char(c);
        lexeme[lexeme_index] = '\0';
        token_types_t type = lexer_lookup_reserved_word(lexeme);
        return create_token(type, lexeme);
      }
      break;
    case 2: // Números
      if (isdigit(c)) {
        lexeme[lexeme_index++] = c;
      } else {
        unget_char(c);
        lexeme[lexeme_index] = '\0';
        return create_token(TOKEN_NUM, lexeme);
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

  if (token->type == TOKEN_UNKNOWN)
    print_error(token);

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
  printf("ERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n", token->lexeme,
         token->line, token->column);
}

// HASH SECTION

void lexer_hash_init() {

  // Initializes every table(bucket) as NULL
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    lexer_hash_table.table[i] = NULL;
  }

  const char *reserved_words[] = {"else",   "if",   "int",
                                  "return", "void", "while"};
  token_types_t token_types[] = {TOKEN_ELSE,   TOKEN_IF,   TOKEN_INT,
                                 TOKEN_RETURN, TOKEN_VOID, TOKEN_WHILE};

  // Inserts each word in the table
  for (int i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++) {
    unsigned int index = hash_function(reserved_words[i]);
    hash_node_t *new_node = (hash_node_t *)malloc(sizeof(hash_node_t));

    new_node->key = strdup(reserved_words[i]);
    new_node->type = token_types[i];
    new_node->next = lexer_hash_table.table[index];

    lexer_hash_table.table[index] = new_node;
  }
}

void lexer_hash_delete() {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    hash_node_t *node = lexer_hash_table.table[i];
    while (node != NULL) {
      hash_node_t *temp = node;
      node = node->next;
      free(temp->key);
      free(temp);
    }
  }
}

token_types_t lexer_lookup_reserved_word(const char *key) {
  unsigned int index = hash_function(key);
  hash_node_t *node = lexer_hash_table.table[index];

  while (node != NULL) {
    if (strcmp(node->key, key) == 0)
      return node->type;

    node = node->next;
  }

  // Returns id if it isn't on the reserved words set
  return TOKEN_ID;
}

unsigned int hash_function(const char *key) {
  unsigned int hash = 0;
  while (*key) {
    hash = (hash * 31) + *key;
    key++;
  }

  return hash % HASH_TABLE_SIZE;
}

// HELPER Functions

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
