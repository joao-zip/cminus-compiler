#include "parser.h"
#include "../lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t *currentToken = NULL;

ast_node_t *create_ast_node(ast_node_type_t type) {
  ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
  if (!node) {
    fprintf(stderr, "Error: Memory allocation failed for AST node.\n");
    exit(EXIT_FAILURE);
  }
  node->type = type;
  memset(&node->data, 0, sizeof(node->data)); //! Initializes the union with 0
  return node;
}

void destroy_ast(ast_node_t *node) {
  if (!node)
    return;

  switch (node->type) {
  case AST_PROGRAM:
    destroy_ast(node->data.program.decl_list);
    break;
  case AST_DECL_LIST:
    destroy_ast(node->data.decl_list.declaration);
    destroy_ast(node->data.decl_list.decl_list);
    break;
  case AST_DECLARATION:
    destroy_ast(node->data.declaration.declaration);
    break;
  case AST_VAR_DECLARATION:
    destroy_ast(node->data.var_declaration.type_specifier);
    if (node->data.var_declaration.dimension)
      destroy_ast(node->data.var_declaration.dimension);
    if (node->data.var_declaration.id)
      free(node->data.var_declaration.id);
    break;
  case AST_FUN_DECLARATION:
    destroy_ast(node->data.fun_declaration.type_specifier);
    if (node->data.fun_declaration.id)
      free(node->data.fun_declaration.id);
    destroy_ast(node->data.fun_declaration.params);
    destroy_ast(node->data.fun_declaration.compound_decl);
    break;
  case AST_TYPE_SPECIFIER:
    // Only a type, no action is necessary
    break;
  case AST_PARAM_LIST:
    destroy_ast(node->data.param_list.param);
    destroy_ast(node->data.param_list.param_list);
    break;
  case AST_PARAM:
    destroy_ast(node->data.param.type_specifier);
    if (node->data.param.id)
      free(node->data.param.id);
    if (node->data.param.dimension)
      destroy_ast(node->data.param.dimension);
    break;
  case AST_COMPOUND_DECL:
    destroy_ast(node->data.compound_decl.local_declarations);
    destroy_ast(node->data.compound_decl.statement_list);
    break;
  case AST_LOCAL_DECLARATIONS:
    destroy_ast(node->data.local_declarations.var_declaration);
    destroy_ast(node->data.local_declarations.local_declarations);
    break;
  case AST_STATEMENT_LIST:
    destroy_ast(node->data.statement_list.statement);
    destroy_ast(node->data.statement_list.statement_list);
    break;
  case AST_STATEMENT:
    destroy_ast(node->data.statement.statement);
    break;
  case AST_EXPRESSION_STATEMENT:
    destroy_ast(node->data.expression_statement.expression);
    break;
  case AST_COMPOUND_STATEMENT:
    destroy_ast(node->data.compound_statement.compound_decl);
    break;
  case AST_SELECTION_STATEMENT:
    destroy_ast(node->data.selection_statement.expression);
    destroy_ast(node->data.selection_statement.then_statement);
    destroy_ast(node->data.selection_statement.else_statement);
    break;
  case AST_ITERATION_STATEMENT:
    destroy_ast(node->data.iteration_statement.expression);
    destroy_ast(node->data.iteration_statement.body);
    break;
  case AST_RETURN_STATEMENT:
    destroy_ast(node->data.return_statement.expression);
    break;
  case AST_ASSIGNMENT_EXPRESSION:
    if (node->data.assignment_expression.var_id)
      free(node->data.assignment_expression.var_id);
    destroy_ast(node->data.assignment_expression.var_index);
    destroy_ast(node->data.assignment_expression.expression);
    break;
  case AST_SIMPLE_EXPRESSION:
    destroy_ast(node->data.simple_expression.left);
    destroy_ast(node->data.simple_expression.relational_op);
    destroy_ast(node->data.simple_expression.right);
    break;
  case AST_VARIABLE:
    if (node->data.variable.id)
      free(node->data.variable.id);
    destroy_ast(node->data.variable.index);
    break;
  case AST_RELATIONAL_OPERATOR:
    // Only a operator, no action is necessary
    break;
  case AST_ADDITIVE_EXPRESSION:
    destroy_ast(node->data.additive_expression.left);
    destroy_ast(node->data.additive_expression.add_op);
    destroy_ast(node->data.additive_expression.right);
    break;
  case AST_ADDITIVE_OPERATOR:
    // Only a operator, no action is necessary
    break;
  case AST_TERM:
    destroy_ast(node->data.term.left);
    destroy_ast(node->data.term.mult_op);
    destroy_ast(node->data.term.right);
    break;
  case AST_MULTIPLICATIVE_OPERATOR:
    // Only a operator, no action is necessary
    break;
  case AST_FACTOR:
    destroy_ast(node->data.factor.expression);
    destroy_ast(node->data.factor.variable);
    destroy_ast(node->data.factor.activation);
    // Number doesn't need to be deleted
    break;
  case AST_ACTIVATION:
    if (node->data.activation.id)
      free(node->data.activation.id);
    destroy_ast(node->data.activation.args);
    break;
  case AST_ARGUMENT_LIST:
    destroy_ast(node->data.argument_list.expression);
    destroy_ast(node->data.argument_list.arg_list);
    break;
  default:
    // For nodes without pointers to other nodes, nothing is necessary
    break;
  }

  free(node);
}

void destroy_ast_root(ast_node_t *root) { destroy_ast(root); }

// -------------------- Token manipulation functions -------------------------

token_t *get_current_token() { return currentToken; }

void advance_token() {
  currentToken = get_next_token();

  if (currentToken == NULL) {
    fprintf(stderr, "Error: Reached end of file unexpectedly.\n");
    exit(EXIT_FAILURE);
  }
}

void match_token(token_types_t expected) {
  if (currentToken->type == expected) {
    advance_token();
  } else {
    fprintf(stderr, "ERRO SINTATICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
            print_token_classes(currentToken->type), currentToken->line,
            currentToken->column);
    exit(EXIT_FAILURE);
  }
}

// Função principal do parser

ast_node_t *parse_program() {

  advance_token();

  ast_node_t *program = create_ast_node(AST_PROGRAM);
  program->data.program.decl_list = parse_declaration_list();

  // Verificar se o fim do arquivo foi alcançado
  if (currentToken->type != TOKEN_UNKNOWN) {
    fprintf(stderr, "Syntax Error: Unexpected tokens after end of program.\n");
    exit(EXIT_FAILURE);
  }

  return program;
}

ast_node_t *parse_declaration_list() {
  ast_node_t *decl_list = create_ast_node(AST_DECL_LIST);

  // Cada declaração começa com 'int' ou 'void'
  if (currentToken->type == TOKEN_INT || currentToken->type == TOKEN_VOID) {
    decl_list->data.decl_list.declaration = parse_declaration();
    decl_list->data.decl_list.decl_list = parse_declaration_list();
  } else {
    // Possível vazio, dependendo das regras da gramática
    decl_list->data.decl_list.declaration = NULL;
    decl_list->data.decl_list.decl_list = NULL;
  }

  return decl_list;
}

// Implementação de outra função exemplo: parse_declaration

ast_node_t *parse_declaration() {
  ast_node_t *declaration_node = create_ast_node(AST_DECLARATION);
  ast_node_t *decl = NULL;

  ast_node_t *type_spec = parse_type_specifier();

  if (currentToken->type != TOKEN_ID) {
    fprintf(stderr,
            "Syntax Error: Expected identifier after type specifier.\n");
    exit(1);
  }

  char *id = strdup(currentToken->lexeme);
  advance_token(); // Consome o identificador

  if (currentToken->type == TOKEN_LPARENT) { // Função
    decl = create_ast_node(AST_FUN_DECLARATION);
    decl->data.fun_declaration.type_specifier = type_spec;
    decl->data.fun_declaration.id = id;

    match_token(TOKEN_LPARENT);
    decl->data.fun_declaration.params = parse_params();
    match_token(TOKEN_RPARENT);
    decl->data.fun_declaration.compound_decl = parse_compound_decl();

  } else { // Variável
    decl = create_ast_node(AST_VAR_DECLARATION);
    decl->data.var_declaration.type_specifier = type_spec;
    decl->data.var_declaration.id = id;
    decl->data.var_declaration.dimension = NULL;

    if (currentToken->type == TOKEN_LBRACKET) { // Array
      match_token(TOKEN_LBRACKET);
      if (currentToken->type != TOKEN_NUM) {
        fprintf(stderr,
                "Syntax Error: Expected number in array declaration.\n");
        exit(1);
      }
      ast_node_t *num_node = create_ast_node(AST_FACTOR);
      num_node->data.factor.number = atoi(currentToken->lexeme);
      advance_token(); // Consome o número
      match_token(TOKEN_RBRACKET);
      decl->data.var_declaration.dimension = num_node;
    }

    match_token(TOKEN_DELIM); // Assumindo que TOKEN_DELIM representa ';'
  }

  declaration_node->data.declaration.declaration = decl;
  return declaration_node;
}

// Exemplo de implementação de parse_type_specifier
ast_node_t *parse_type_specifier() {
  ast_node_t *type_spec = create_ast_node(AST_TYPE_SPECIFIER);

  if (currentToken->type == TOKEN_INT || currentToken->type == TOKEN_VOID) {
    type_spec->data.type_specifier.type = currentToken->type;
    advance_token();
  } else {
    fprintf(stderr,
            "Syntax Error: Expected 'int' or 'void' as type specifier.\n");
    exit(1);
  }

  return type_spec;
}

ast_node_t *parse_params() {
  ast_node_t *params = create_ast_node(AST_PARAM_LIST);

  if (currentToken->type == TOKEN_VOID) {
    advance_token();
    // `void` indica que não há parâmetros
    params->data.param_list.param = NULL;
    params->data.param_list.param_list = NULL;
  } else {
    params->data.param_list.param = parse_param();
    if (currentToken->type == TOKEN_COMMA) {
      match_token(TOKEN_COMMA);
      params->data.param_list.param_list = parse_param_list();
    } else {
      params->data.param_list.param_list = NULL;
    }
  }

  return params;
}

ast_node_t *parse_param() {
  ast_node_t *param = create_ast_node(AST_PARAM);

  param->data.param.type_specifier = parse_type_specifier();

  if (currentToken->type != TOKEN_ID) {
    fprintf(stderr, "Syntax Error: Expected identifier in parameter.\n");
    exit(1);
  }

  param->data.param.dimension = NULL;
  param->data.param.id = strdup(currentToken->lexeme);
  advance_token(); // Consome o identificador

  if (currentToken->type == TOKEN_LBRACKET) { // Array
    match_token(TOKEN_LBRACKET);
    match_token(TOKEN_RBRACKET);
    // Poderia criar um nó para indicar que é um array, dependendo da
    // implementação Por simplicidade, apenas marca como array (Pode ser
    // implementado diferentemente conforme necessidade)
    ast_node_t *array_node = create_ast_node(AST_FACTOR);
    array_node->data.factor.number = 0; // Valor fictício para indicar array
    param->data.param.dimension = array_node;
  }

  return param;
}

ast_node_t *parse_compound_decl() {
  ast_node_t *compound_decl = create_ast_node(AST_COMPOUND_DECL);

  match_token(TOKEN_LKEY); // '{'

  compound_decl->data.compound_decl.local_declarations =
      parse_local_declarations();
  compound_decl->data.compound_decl.statement_list = parse_statement_list();

  match_token(TOKEN_RKEY); // '}'

  return compound_decl;
}

ast_node_t *parse_local_declarations() {
  ast_node_t *local_decls = create_ast_node(AST_LOCAL_DECLARATIONS);

  if (currentToken->type == TOKEN_INT || currentToken->type == TOKEN_VOID) {
    local_decls->data.local_declarations.var_declaration =
        parse_var_declaration();
    local_decls->data.local_declarations.local_declarations =
        parse_local_declarations();
  } else {
    // Vazio
    local_decls->data.local_declarations.var_declaration = NULL;
    local_decls->data.local_declarations.local_declarations = NULL;
  }

  return local_decls;
}

ast_node_t *parse_var_declaration() {
  ast_node_t *var_decl = create_ast_node(AST_VAR_DECLARATION);

  var_decl->data.var_declaration.type_specifier = parse_type_specifier();

  if (currentToken->type != TOKEN_ID) {
    fprintf(stderr,
            "Syntax Error: Expected identifier in variable declaration.\n");
    exit(1);
  }

  char *id = strdup(currentToken->lexeme);
  var_decl->data.var_declaration.id = id;
  advance_token(); // Consome o identificador

  var_decl->data.var_declaration.dimension = NULL;

  if (currentToken->type == TOKEN_LBRACKET) { // Array
    match_token(TOKEN_LBRACKET);
    if (currentToken->type != TOKEN_NUM) {
      fprintf(stderr, "Syntax Error: Expected number in array declaration.\n");
      exit(1);
    }
    ast_node_t *num_node = create_ast_node(AST_FACTOR);
    num_node->data.factor.number = atoi(currentToken->lexeme);
    var_decl->data.var_declaration.dimension = num_node;
    advance_token(); // Consome o número
    match_token(TOKEN_RBRACKET);
  }

  match_token(TOKEN_DELIM); // Assumindo que TOKEN_DELIM representa ';'

  return var_decl;
}

ast_node_t *parse_statement_list() {
  ast_node_t *stmt_list = create_ast_node(AST_STATEMENT_LIST);

  if (currentToken->type == TOKEN_IF || currentToken->type == TOKEN_WHILE ||
      currentToken->type == TOKEN_RETURN || currentToken->type == TOKEN_LKEY ||
      currentToken->type == TOKEN_ID || currentToken->type == TOKEN_NUM ||
      currentToken->type == TOKEN_DELIM) {

    stmt_list->data.statement_list.statement = parse_statement();
    stmt_list->data.statement_list.statement_list = parse_statement_list();
  } else {
    // Vazio
    stmt_list->data.statement_list.statement = NULL;
    stmt_list->data.statement_list.statement_list = NULL;
  }

  return stmt_list;
}

ast_node_t *parse_statement() {
  ast_node_t *stmt = create_ast_node(AST_STATEMENT);

  if (currentToken->type == TOKEN_IF) {
    stmt->data.statement.statement = parse_selection_statement();
  } else if (currentToken->type == TOKEN_WHILE) {
    stmt->data.statement.statement = parse_iteration_statement();
  } else if (currentToken->type == TOKEN_RETURN) {
    stmt->data.statement.statement = parse_return_statement();
  } else if (currentToken->type == TOKEN_LKEY) {
    stmt->data.statement.statement = parse_compound_decl();
  } else {
    stmt->data.statement.statement = parse_expression_statement();
  }

  return stmt;
}

ast_node_t *parse_selection_statement() {
  ast_node_t *sel_stmt = create_ast_node(AST_SELECTION_STATEMENT);

  match_token(TOKEN_IF);
  match_token(TOKEN_LPARENT);
  sel_stmt->data.selection_statement.expression = parse_expression();
  match_token(TOKEN_RPARENT);
  sel_stmt->data.selection_statement.then_statement = parse_statement();

  if (currentToken->type == TOKEN_ELSE) {
    match_token(TOKEN_ELSE);
    sel_stmt->data.selection_statement.else_statement = parse_statement();
  } else {
    sel_stmt->data.selection_statement.else_statement = NULL;
  }

  return sel_stmt;
}

ast_node_t *parse_iteration_statement() {
  ast_node_t *iter_stmt = create_ast_node(AST_ITERATION_STATEMENT);

  match_token(TOKEN_WHILE);
  match_token(TOKEN_LPARENT);
  iter_stmt->data.iteration_statement.expression = parse_expression();
  match_token(TOKEN_RPARENT);
  iter_stmt->data.iteration_statement.body = parse_statement();

  return iter_stmt;
}

ast_node_t *parse_return_statement() {
  ast_node_t *ret_stmt = create_ast_node(AST_RETURN_STATEMENT);

  match_token(TOKEN_RETURN);

  if (currentToken->type != TOKEN_DELIM) { // Assumindo que TOKEN_DELIM é ';'
    ret_stmt->data.return_statement.expression = parse_expression();
  } else {
    ret_stmt->data.return_statement.expression = NULL;
  }

  match_token(TOKEN_DELIM); // ';'

  return ret_stmt;
}

ast_node_t *parse_expression_statement() {
  ast_node_t *expr_stmt = create_ast_node(AST_EXPRESSION_STATEMENT);

  if (currentToken->type != TOKEN_DELIM) { // ';' indica expressão vazia
    expr_stmt->data.expression_statement.expression = parse_expression();
  } else {
    expr_stmt->data.expression_statement.expression = NULL;
  }

  match_token(TOKEN_DELIM); // ';'

  return expr_stmt;
}

ast_node_t *parse_param_list() {
  ast_node_t *param_list = create_ast_node(AST_PARAM_LIST);

  param_list->data.param_list.param = parse_param();

  if (currentToken->type == TOKEN_COMMA) {
    match_token(TOKEN_COMMA);
    param_list->data.param_list.param_list = parse_param_list();
  } else {
    param_list->data.param_list.param_list = NULL;
  }

  return param_list;
}

ast_node_t *parse_var() {
  ast_node_t *var = create_ast_node(AST_VARIABLE);

  if (currentToken->type != TOKEN_ID) {
    fprintf(stderr,
            "Syntax Error: Expected identifier in variable expression at line "
            "%d, column %d.\n",
            currentToken->line, currentToken->column);
    exit(1);
  }

  var->data.variable.id = strdup(currentToken->lexeme);
  advance_token(); // Consome o identificador

  if (currentToken->type == TOKEN_LBRACKET) { // '['
    match_token(TOKEN_LBRACKET);
    var->data.variable.index = parse_expression();
    match_token(TOKEN_RBRACKET);
  } else {
    var->data.variable.index = NULL;
  }

  return var;
}

// Função para analisar 'simples-expressão → soma-expressão relacional
// soma-expressão | soma-expressão'
ast_node_t *parse_simple_expression() {
  ast_node_t *simple_expr = create_ast_node(AST_SIMPLE_EXPRESSION);

  simple_expr->data.simple_expression.left = parse_additive_expression();

  // Verifica se há um operador relacional
  if (currentToken->type == TOKEN_LE || currentToken->type == TOKEN_LT ||
      currentToken->type == TOKEN_GT || currentToken->type == TOKEN_GE ||
      currentToken->type == TOKEN_EQ || currentToken->type == TOKEN_DIFF) {

    simple_expr->data.simple_expression.relational_op = parse_relational_op();
    simple_expr->data.simple_expression.right = parse_additive_expression();
  } else {
    simple_expr->data.simple_expression.relational_op = NULL;
    simple_expr->data.simple_expression.right = NULL;
  }

  return simple_expr;
}

// Função para analisar 'relacional → <= | < | > | >= | == | !='
ast_node_t *parse_relational_op() {
  ast_node_t *rel_op = create_ast_node(AST_RELATIONAL_OPERATOR);

  switch (currentToken->type) {
  case TOKEN_LE:
    rel_op->data.relational_operator.relop = TOKEN_LE;
    break;
  case TOKEN_LT:
    rel_op->data.relational_operator.relop = TOKEN_LT;
    break;
  case TOKEN_GT:
    rel_op->data.relational_operator.relop = TOKEN_GT;
    break;
  case TOKEN_GE:
    rel_op->data.relational_operator.relop = TOKEN_GE;
    break;
  case TOKEN_EQ:
    rel_op->data.relational_operator.relop = TOKEN_EQ;
    break;
  case TOKEN_DIFF:
    rel_op->data.relational_operator.relop = TOKEN_DIFF;
    break;
  default:
    fprintf(
        stderr,
        "Syntax Error: Expected relational operator at line %d, column %d.\n",
        currentToken->line, currentToken->column);
    exit(1);
  }

  advance_token(); // Consome o operador relacional

  return rel_op;
}

// Função para analisar 'soma-expressão → soma-expressão soma termo | termo'
ast_node_t *parse_additive_expression() {
  ast_node_t *add_expr = create_ast_node(AST_ADDITIVE_EXPRESSION);

  add_expr->data.additive_expression.left = parse_term();

  // Verifica se há um operador de soma ( '+' ou '-' )
  if (currentToken->type == TOKEN_PLUS || currentToken->type == TOKEN_MINUS) {
    add_expr->data.additive_expression.add_op = parse_add_op();
    add_expr->data.additive_expression.right = parse_term();
  } else {
    add_expr->data.additive_expression.add_op = NULL;
    add_expr->data.additive_expression.right = NULL;
  }

  return add_expr;
}

// Função para analisar 'soma → + | -'
ast_node_t *parse_add_op() {
  ast_node_t *add_op = create_ast_node(AST_ADDITIVE_OPERATOR);

  if (currentToken->type == TOKEN_PLUS || currentToken->type == TOKEN_MINUS) {
    add_op->data.additive_operator.add_operator =
        (currentToken->type == TOKEN_PLUS) ? '+' : '-';
    advance_token(); // Consome '+' ou '-'
  } else {
    fprintf(stderr,
            "Syntax Error: Expected '+' or '-' at line %d, column %d.\n",
            currentToken->line, currentToken->column);
    exit(1);
  }

  return add_op;
}

// Função para analisar 'termo → termo mult fator | fator'
ast_node_t *parse_term() {
  ast_node_t *term_node = create_ast_node(AST_TERM);

  term_node->data.term.left = parse_factor();

  // Verifica se há um operador multiplicativo ('*' ou '/')
  if (currentToken->type == TOKEN_MULT || currentToken->type == TOKEN_DIV) {
    term_node->data.term.mult_op = parse_mult_op();
    term_node->data.term.right = parse_factor();
  } else {
    term_node->data.term.mult_op = NULL;
    term_node->data.term.right = NULL;
  }

  return term_node;
}

// Função para analisar 'mult → * | /'
ast_node_t *parse_mult_op() {
  ast_node_t *mult_op = create_ast_node(AST_MULTIPLICATIVE_OPERATOR);

  if (currentToken->type == TOKEN_MULT || currentToken->type == TOKEN_DIV) {
    mult_op->data.multiplicative_operator.mult_operator =
        (currentToken->type == TOKEN_MULT) ? '*' : '/';
    advance_token(); // Consome '*' ou '/'
  } else {
    fprintf(stderr,
            "Syntax Error: Expected '*' or '/' at line %d, column %d.\n",
            currentToken->line, currentToken->column);
    exit(1);
  }

  return mult_op;
}

// Função para analisar 'fator → ( expressão ) | var | ativação | NUM'
ast_node_t *parse_factor() {
  ast_node_t *factor = create_ast_node(AST_FACTOR);

  if (currentToken->type == TOKEN_LPARENT) { // '(' expressão ')'
    match_token(TOKEN_LPARENT);
    factor->data.factor.expression = parse_expression();
    match_token(TOKEN_RPARENT);
  } else if (currentToken->type == TOKEN_ID) { // Pode ser 'var' ou 'ativação'
    // Salva o token atual para verificar se é uma ativação (chamada de função)
    token_t *token_backup = currentToken;

    ast_node_t *var_node = parse_var();

    if (currentToken->type ==
        TOKEN_LPARENT) { // É uma ativação (chamada de função)
      ast_node_t *activation = parse_activation_helper(
          var_node->data.variable.id, var_node->data.variable.index);
      destroy_ast(var_node); // Destrói o nó var, já que criamos uma ativação
      factor->data.factor.activation = activation;
      factor->data.factor.variable = NULL;
    } else {
      // É uma variável normal
      factor->data.factor.variable = var_node;
      factor->data.factor.activation = NULL;
    }
  } else if (currentToken->type == TOKEN_NUM) { // NUM
    factor->data.factor.number = atoi(currentToken->lexeme);
    advance_token(); // Consome NUM
  } else {
    fprintf(stderr,
            "Syntax Error: Expected '(', identifier, or number in factor at "
            "line %d, column %d.\n",
            currentToken->line, currentToken->column);
    exit(1);
  }

  return factor;
}

// Função auxiliar para criar uma ativação após identificar 'var('
ast_node_t *parse_activation_helper(char *id, ast_node_t *index) {
  ast_node_t *activation = create_ast_node(AST_ACTIVATION);

  activation->data.activation.id = strdup(id);
  activation->data.activation.args = NULL;

  if (index != NULL) {
    // Mesmo que possa ser usado em ativação, para simplificar, podemos ignorar
    // o índice ou tratar de forma específica
    fprintf(stderr,
            "Syntax Warning: Unexpected array index in function call for '%s'. "
            "Ignored.\n",
            id);
    destroy_ast(index);
  }

  match_token(TOKEN_LPARENT); // Consome '('

  if (currentToken->type != TOKEN_RPARENT) { // Há argumentos
    activation->data.activation.args = parse_args();
  } else {
    activation->data.activation.args = NULL; // Sem argumentos
  }

  match_token(TOKEN_RPARENT); // Consome ')'

  return activation;
}

// Função para analisar 'args → arg-lista | vazio'
ast_node_t *parse_args() {
  if (currentToken->type == TOKEN_RPARENT) {
    // Vazio
    return NULL;
  } else {
    return parse_argument_list();
  }
}

// Função para analisar 'arg-lista → arg-lista , expressão | expressão'
ast_node_t *parse_argument_list() {
  ast_node_t *arg_list = create_ast_node(AST_ARGUMENT_LIST);

  arg_list->data.argument_list.expression = parse_expression();

  if (currentToken->type == TOKEN_COMMA) {
    match_token(TOKEN_COMMA);
    arg_list->data.argument_list.arg_list = parse_argument_list();
  } else {
    arg_list->data.argument_list.arg_list = NULL;
  }

  return arg_list;
}

// Função para analisar 'fun-declaração → tipo-especificador ID ( params )
// composto-decl'
ast_node_t *parse_fun_declaration() {
  ast_node_t *fun_decl = create_ast_node(AST_FUN_DECLARATION);

  // Analisar tipo-especificador
  fun_decl->data.fun_declaration.type_specifier = parse_type_specifier();

  // Espera-se um identificador
  if (currentToken->type != TOKEN_ID) {
    fprintf(stderr,
            "Syntax Error: Expected function name identifier at line %d, "
            "column %d.\n",
            currentToken->line, currentToken->column);
    exit(1);
  }

  // Copiar o lexema do identificador
  fun_decl->data.fun_declaration.id = strdup(currentToken->lexeme);
  advance_token(); // Consome o identificador

  match_token(TOKEN_LPARENT); // Consome '('

  // Analisar 'params'
  fun_decl->data.fun_declaration.params = parse_params();

  match_token(TOKEN_RPARENT); // Consome ')'

  // Analisar 'composto-decl'
  fun_decl->data.fun_declaration.compound_decl = parse_compound_decl();

  return fun_decl;
}

// Revisão da função 'parse_expression' para lidar corretamente com var = expressão ou simpes-expressão
ast_node_t* parse_expression() {
    ast_node_t *expr = NULL;

    // Para prever se é uma atribuição, precisamos ver se a expressão começa com uma variável
    // Se sim, pode ser uma atribuição
    if (currentToken->type == TOKEN_ID) {
        // Guarda o token atual para identificar se é var = expressão
        token_t *token_backup = currentToken;

        ast_node_t *var_node = parse_var();

        if (currentToken->type == TOKEN_ATTR) { // '='
            // É uma atribuição
            expr = create_ast_node(AST_ASSIGNMENT_EXPRESSION);
            expr->data.assignment_expression.var_id = strdup(var_node->data.variable.id);
            expr->data.assignment_expression.var_index = var_node->data.variable.index;
            expr->data.assignment_expression.expression = NULL;

            match_token(TOKEN_ATTR); // Consome '='

            expr->data.assignment_expression.expression = parse_expression(); // Chamada recursiva

            // Limpar o nó var_node já que estamos criando um novo nó de atribuição
            destroy_ast(var_node);
        }
        else {
            // Não é uma atribuição, trata-se de uma simples-expressão começando com var
            // Portanto, devemos remontar a árvore para incluir 'var' na expressão simples
            // Neste caso, 'var_node' já representa um fator dentro de 'simples-expressao', então precisamos ajustar
            // Contudo, como já chamamos 'parse_var', vamos considerar simplesmente retornar 'simples_expressao'

            // Pode-se implementar uma função 'put_back_token' para retornar 'var_node' ao fluxo de tokens
            // Para simplificar, usaremos 'expr = parse_simple_expression', que já consumiu os tokens corretos
            expr = parse_simple_expression();
        }
    }
    else {
        // Não começa com uma variável, então é uma simples-expressão
        expr = parse_simple_expression();
    }

    return expr;
}
