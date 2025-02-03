#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define IS_WHITESPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define IS_ID_SEPARATOR(c) (IS_WHITESPACE(c) || (c) == ';' || (c) == '[' || (c) == '(' || (c) == ')' || (c) == '{')
#define IS_NUM_SEPARATOR(c) (IS_WHITESPACE(c) || (c) == ';')

#define LEXEME_MAX_SIZE 101 // 100 + \0

extern int VERBOSE_LEXER;
extern FILE *filename;

extern int current_line;
extern int current_column;

typedef enum {
  // Identifiers and Numbers
  TOKEN_ID, TOKEN_NUM,
  // Reserved words
  TOKEN_ELSE, TOKEN_IF, TOKEN_INT, TOKEN_RETURN, TOKEN_VOID, TOKEN_WHILE,
  // Operators
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULT, TOKEN_DIV,
  TOKEN_LT, TOKEN_LE, TOKEN_GT, TOKEN_GE, TOKEN_EQ,
  TOKEN_DIFF, TOKEN_ATTR, TOKEN_DELIM, TOKEN_COMMA,
  // Comments, ;, ,, (), [] and {}
  TOKEN_LCOMM, TOKEN_RCOMM, TOKEN_LPARENT, TOKEN_RPARENT,
  TOKEN_LKEY, TOKEN_RKEY, TOKEN_LBRACKET, TOKEN_RBRACKET,
  // Error
  TOKEN_UNKNOWN,
} token_types_t;

typedef struct {
  token_types_t type;
  char lexeme[LEXEME_MAX_SIZE];
  int line;
  int column;
} token_t;

// ----------------------- Functions ----------------------

//! Sets the option to print each token after getting it
void set_verbose_lexer(int is_verbose);

//! Setup the lexer internal state and current file being read
void init_lexer(const char *filename);

//! Correctly close the file being read by the lexer
void close_lexer();

//! Return the next token of the file or NULL if an error occurs or found EOF
token_t *get_next_token();

//! Creates a new token with a predetermined type and lexeme
token_t *create_token(token_types_t type, const char *lexeme);

//! Deletes the given token
token_t *delete_token(token_t *token);

//! Helper function to print the correct token type
char *print_token_classes(token_types_t type);

//! Prints the token information: the type, lexeme and line found
void print_token(token_t *token);

//! Print an error message with the current unknown token found
void print_error(token_t *token);

// ----------------------- General Helpers ----------------------

//! Returns the next char of the current file
int get_next_char();

//! Tell the lexer to return one position of the file pointer
void unget_char(int c);

#endif // !LEXER_H
