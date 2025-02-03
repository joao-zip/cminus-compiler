#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

//! Pointer to the current token
extern token_t *currentToken;

//! Global controller to print the ASTree after sintatic analysis
extern int VERBOSE_PARSER;

// ----------------------- Abstract Syntax Tree (AST) Structures ----------------------

//! Enumeration for AST node types
typedef enum {
  AST_PROGRAM,
  AST_DECL_LIST,
  AST_DECLARATION,
  AST_VAR_DECLARATION,
  AST_FUN_DECLARATION,
  AST_TYPE_SPECIFIER,
  AST_PARAM_LIST,
  AST_PARAM,
  AST_COMPOUND_DECL,
  AST_LOCAL_DECLARATIONS,
  AST_STATEMENT_LIST,
  AST_STATEMENT,
  AST_EXPRESSION_STATEMENT,
  AST_COMPOUND_STATEMENT,
  AST_SELECTION_STATEMENT,
  AST_ITERATION_STATEMENT,
  AST_RETURN_STATEMENT,
  AST_ASSIGNMENT_EXPRESSION,
  AST_SIMPLE_EXPRESSION,
  AST_VARIABLE,
  AST_RELATIONAL_OPERATOR,
  AST_ADDITIVE_EXPRESSION,
  AST_ADDITIVE_OPERATOR,
  AST_TERM,
  AST_MULTIPLICATIVE_OPERATOR,
  AST_FACTOR,
  AST_ACTIVATION,
  AST_ARGUMENT_LIST,
} ast_node_type_t;

//! Structure for an AST node
typedef struct ast_node {
  ast_node_type_t type;
  union {
    //! Program Node
    struct {
      struct ast_node *decl_list;
    } program;

    //! Declaration List Node
    struct {
      struct ast_node *declaration;
      struct ast_node *decl_list;
    } decl_list;

    //! Declaration Node
    struct {
      struct ast_node *declaration;
    } declaration;

    //! Variable Declaration Node
    struct {
      struct ast_node *type_specifier;
      char *id;
      struct ast_node *dimension; // NULL if not an array
    } var_declaration;

    //! Function Declaration Node
    struct {
      struct ast_node *type_specifier;
      char *id;
      struct ast_node *params;
      struct ast_node *compound_decl;
    } fun_declaration;

    //! Type Specifier Node
    struct {
      token_types_t type;
    } type_specifier;

    //! Parameter List Node
    struct {
      struct ast_node *param;
      struct ast_node *param_list;
    } param_list;

    //! Parameter Node
    struct {
      struct ast_node *type_specifier;
      char *id;
      struct ast_node *dimension; // NULL if not an array
    } param;

    //! Compound Declaration Node
    struct {
      struct ast_node *local_declarations;
      struct ast_node *statement_list;
    } compound_decl;

    //! Local Declarations Node
    struct {
      struct ast_node *var_declaration;
      struct ast_node *local_declarations;
    } local_declarations;

    //! Statement List Node
    struct {
      struct ast_node *statement;
      struct ast_node *statement_list;
    } statement_list;

    //! Statement Node
    struct {
      struct ast_node *statement;
    } statement;

    //! Expression Statement Node
    struct {
      struct ast_node *expression;
    } expression_statement;

    //! Compound Statement Node
    struct {
      struct ast_node *compound_decl;
    } compound_statement;

    //! Selection Statement Node (if-else)
    struct {
      struct ast_node *expression;
      struct ast_node *then_statement;
      struct ast_node *else_statement; // NULL if no else
    } selection_statement;

    //! Iteration Statement Node (while)
    struct {
      struct ast_node *expression;
      struct ast_node *body;
    } iteration_statement;

    //! Return Statement Node
    struct {
      struct ast_node *expression; // NULL if no expression
    } return_statement;

    //! Assignment Expression Node
    struct {
      char *var_id;
      struct ast_node *var_index; // NULL if not an array
      struct ast_node *expression;
    } assignment_expression;

    //! Simple Expression Node
    struct {
      struct ast_node *left;
      struct ast_node *relational_op; // NULL if no operator
      struct ast_node *right;         // NULL if no operator
    } simple_expression;

    //! Variable Node
    struct {
      char *id;
      struct ast_node *index; // NULL if not an array
    } variable;

    //! Relational Operator Node
    struct {
      token_types_t relop;
    } relational_operator;

    //! Additive Expression Node
    struct {
      struct ast_node *left;
      struct ast_node *add_op;
      struct ast_node *right;
    } additive_expression;

    //! Additive Operator Node
    struct {
      char add_operator;
    } additive_operator;

    //! Term Node
    struct {
      struct ast_node *left;
      struct ast_node *mult_op;
      struct ast_node *right;
    } term;

    //! Multiplicative Operator Node
    struct {
      char mult_operator;
    } multiplicative_operator;

    //! Factor Node
    struct {
      struct ast_node *expression;
      struct ast_node *variable;
      struct ast_node *activation;
      int number; // If the factor is a number
    } factor;

    //! Activation Node (Function Call)
    struct {
      char *id;
      struct ast_node *args;
    } activation;

    //! Argument List Node
    struct {
      struct ast_node *expression;
      struct ast_node *arg_list;
    } argument_list;

  } data;
} ast_node_t;

// ----------------------- AST Management Functions ----------------------

//! Function to create a new AST node
ast_node_t *create_ast_node(ast_node_type_t type);

//! Function to destroy the AST and free memory
void destroy_ast(ast_node_t *node);

//! Function to destroy the AST root node
void destroy_ast_root(ast_node_t *root);

// ----------------------- Parser Functions ----------------------

//! Sets the option to print the tree after the sintatic analysis
void set_verbose_parser(int is_verbose);

//! Parser default error
void parser_print_error();

//! Main parser function
ast_node_t *parse_program();

//! Parsing functions corresponding to grammar rules
ast_node_t *parse_declaration_list();
ast_node_t *parse_declaration();
ast_node_t *parse_var_declaration();
ast_node_t *parse_fun_declaration();
ast_node_t *parse_type_specifier();
ast_node_t *parse_params();
ast_node_t *parse_param_list();
ast_node_t *parse_param();
ast_node_t *parse_compound_decl();
ast_node_t *parse_local_declarations();
ast_node_t *parse_statement_list();
ast_node_t *parse_statement();
ast_node_t *parse_expression_statement();
ast_node_t *parse_selection_statement();
ast_node_t *parse_iteration_statement();
ast_node_t *parse_return_statement();
ast_node_t *parse_expression();
ast_node_t *parse_var();
ast_node_t *parse_simple_expression();
ast_node_t *parse_relational_op();
ast_node_t *parse_additive_expression();
ast_node_t *parse_add_op();
ast_node_t *parse_term();
ast_node_t *parse_mult_op();
ast_node_t *parse_factor();
ast_node_t *parse_activation();
ast_node_t* parse_activation_helper(char *id, ast_node_t *index);
ast_node_t *parse_args();
ast_node_t *parse_argument_list();

// ----------------------- Token Handling Functions ----------------------

//! Function to match and consume a token
void match_token(token_types_t expected);

//! Get the current token being analyzed
token_t *get_current_token();

//! Function to advance to the next token
void advance_token();

#endif // PARSER_H
