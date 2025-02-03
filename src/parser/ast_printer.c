#include "ast_printer.h"
#include <stdio.h>

#define INDENT_STEP 4 // Number of spaces per indentation level

void print_indent(int indent_level) {
  for (int i = 0; i < indent_level * INDENT_STEP; ++i) {
    putchar('-');
  }
}

void print_ast(ast_node_t *node) { print_ast_node(node, 0); }

void print_ast_node(ast_node_t *node, int indent_level) {
  if (!node)
    return;

  // Shows the indentation
  print_indent(indent_level);

  // Shows the name of the node_type
  printf("%s", get_node_type_name(node->type));

  // Open the parenthesis
  printf(" (");

  // Shows node children
  switch (node->type) {
  case AST_PROGRAM:
    printf("\n");
    print_ast_node(node->data.program.decl_list, indent_level + 1);
    break;

  case AST_DECL_LIST:
    printf("\n");
    if (node->data.decl_list.declaration) {
      print_ast_node(node->data.decl_list.declaration, indent_level + 1);
    }
    if (node->data.decl_list.decl_list) {
      printf(",\n");
      print_ast_node(node->data.decl_list.decl_list, indent_level + 1);
    }
    break;

  case AST_DECLARATION:
    printf("\n");
    if (node->data.declaration.declaration) {
      print_ast_node(node->data.declaration.declaration, indent_level + 1);
    }
    break;

  case AST_VAR_DECLARATION:
    printf("\n");
    // Shows the type and the identifier
    print_indent(indent_level + 1);
    printf("Type: %s,\n",
           get_token_type_name(node->data.var_declaration.type_specifier->data
                                   .type_specifier.type));
    print_indent(indent_level + 1);
    printf("ID: %s", node->data.var_declaration.id);

    // Looks if it's an array
    if (node->data.var_declaration.dimension) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Dimension: ");
      print_ast_node(node->data.var_declaration.dimension, 0);
    }
    break;

  case AST_FUN_DECLARATION:
    printf("\n");
    // Shows the type, identifier, params and body
    print_indent(indent_level + 1);
    printf("Type: %s,\n",
           get_token_type_name(node->data.fun_declaration.type_specifier->data
                                   .type_specifier.type));
    print_indent(indent_level + 1);
    printf("ID: %s,\n", node->data.fun_declaration.id);
    print_indent(indent_level + 1);
    printf("Parameters:\n");
    print_ast_node(node->data.fun_declaration.params, indent_level + 2);
    printf(",\n");
    print_indent(indent_level + 1);
    printf("Body:\n");
    print_ast_node(node->data.fun_declaration.compound_decl, indent_level + 2);
    break;

  case AST_PARAM_LIST:
    printf("\n");
    if (node->data.param_list.param) {
      print_ast_node(node->data.param_list.param, indent_level + 1);
    }
    if (node->data.param_list.param_list) {
      printf(",\n");
      print_ast_node(node->data.param_list.param_list, indent_level + 1);
    }
    break;

  case AST_PARAM:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Type: %s,\n",
           get_token_type_name(
               node->data.param.type_specifier->data.type_specifier.type));
    print_indent(indent_level + 1);
    printf("ID: %s", node->data.param.id);
    if (node->data.param.dimension) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Is Array");
    }
    break;

  case AST_COMPOUND_DECL:
  case AST_COMPOUND_STATEMENT:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Local Declarations:\n");
    print_ast_node(node->data.compound_decl.local_declarations,
                   indent_level + 2);
    printf(",\n");
    print_indent(indent_level + 1);
    printf("Statement List:\n");
    print_ast_node(node->data.compound_decl.statement_list, indent_level + 2);
    break;

  case AST_LOCAL_DECLARATIONS:
    if (node->data.local_declarations.var_declaration) {
      printf("\n");
      print_ast_node(node->data.local_declarations.var_declaration,
                     indent_level + 1);
    }
    if (node->data.local_declarations.local_declarations) {
      printf(",\n");
      print_ast_node(node->data.local_declarations.local_declarations,
                     indent_level);
    }
    break;

  case AST_STATEMENT_LIST:
    if (node->data.statement_list.statement) {
      printf("\n");
      print_ast_node(node->data.statement_list.statement, indent_level + 1);
    }
    if (node->data.statement_list.statement_list) {
      printf(",\n");
      print_ast_node(node->data.statement_list.statement_list, indent_level);
    }
    break;

  case AST_STATEMENT:
    printf("\n");
    if (node->data.statement.statement) {
      print_ast_node(node->data.statement.statement, indent_level + 1);
    }
    break;

  case AST_EXPRESSION_STATEMENT:
    printf("\n");
    if (node->data.expression_statement.expression) {
      print_ast_node(node->data.expression_statement.expression,
                     indent_level + 1);
    }
    break;

  case AST_SELECTION_STATEMENT:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Condition:\n");
    print_ast_node(node->data.selection_statement.expression, indent_level + 2);
    printf(",\n");
    print_indent(indent_level + 1);
    printf("Then:\n");
    print_ast_node(node->data.selection_statement.then_statement,
                   indent_level + 2);
    if (node->data.selection_statement.else_statement) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Else:\n");
      print_ast_node(node->data.selection_statement.else_statement,
                     indent_level + 2);
    }
    break;

  case AST_ITERATION_STATEMENT:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Condition:\n");
    print_ast_node(node->data.iteration_statement.expression, indent_level + 2);
    printf(",\n");
    print_indent(indent_level + 1);
    printf("Body:\n");
    print_ast_node(node->data.iteration_statement.body, indent_level + 2);
    break;

  case AST_RETURN_STATEMENT:
    if (node->data.return_statement.expression) {
      printf("\n");
      print_indent(indent_level + 1);
      printf("Expression:\n");
      print_ast_node(node->data.return_statement.expression, indent_level + 2);
    } else {
      printf(" return;");
    }
    break;

  case AST_ASSIGNMENT_EXPRESSION:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Variable: %s", node->data.assignment_expression.var_id);
    if (node->data.assignment_expression.var_index) {
      printf("[");
      print_ast_node(node->data.assignment_expression.var_index, 0);
      printf("]");
    }
    printf(",\n");
    print_indent(indent_level + 1);
    printf("Expression:\n");
    print_ast_node(node->data.assignment_expression.expression,
                   indent_level + 2);
    break;

  case AST_SIMPLE_EXPRESSION:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Left:\n");
    print_ast_node(node->data.simple_expression.left, indent_level + 2);
    if (node->data.simple_expression.relational_op) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Operator: %s",
             get_token_type_name(node->data.simple_expression.relational_op
                                     ->data.relational_operator.relop));
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Right:\n");
      print_ast_node(node->data.simple_expression.right, indent_level + 2);
    }
    break;

  case AST_ADDITIVE_EXPRESSION:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Left:\n");
    print_ast_node(node->data.additive_expression.left, indent_level + 2);
    if (node->data.additive_expression.add_op) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Operator: %c", node->data.additive_expression.add_op->data
                                 .additive_operator.add_operator);
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Right:\n");
      print_ast_node(node->data.additive_expression.right, indent_level + 2);
    }
    break;

  case AST_TERM:
    printf("\n");
    print_indent(indent_level + 1);
    printf("Left:\n");
    print_ast_node(node->data.term.left, indent_level + 2);
    if (node->data.term.mult_op) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf(
          "Operator: %c",
          node->data.term.mult_op->data.multiplicative_operator.mult_operator);
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Right:\n");
      print_ast_node(node->data.term.right, indent_level + 2);
    }
    break;

  case AST_FACTOR:
    if (node->data.factor.expression) {
      printf("\n");
      print_indent(indent_level + 1);
      printf("Expression:\n");
      print_ast_node(node->data.factor.expression, indent_level + 2);
    } else if (node->data.factor.variable) {
      printf("\n");
      print_indent(indent_level + 1);
      printf("Variable:\n");
      print_ast_node(node->data.factor.variable, indent_level + 2);
    } else if (node->data.factor.activation) {
      printf("\n");
      print_indent(indent_level + 1);
      printf("Activation:\n");
      print_ast_node(node->data.factor.activation, indent_level + 2);
    } else {
      printf(" Number: %d", node->data.factor.number);
    }
    break;

  case AST_VARIABLE:
    printf(" %s", node->data.variable.id);
    if (node->data.variable.index) {
      printf("[");
      print_ast_node(node->data.variable.index, 0);
      printf("]");
    }
    break;

  case AST_ACTIVATION:
    printf(" Function Call: %s", node->data.activation.id);
    if (node->data.activation.args) {
      printf(",\n");
      print_indent(indent_level + 1);
      printf("Arguments:\n");
      print_ast_node(node->data.activation.args, indent_level + 2);
    }
    break;

  case AST_ARGUMENT_LIST:
    printf("\n");
    print_ast_node(node->data.argument_list.expression, indent_level + 1);
    if (node->data.argument_list.arg_list) {
      printf(",\n");
      print_ast_node(node->data.argument_list.arg_list, indent_level + 1);
    }
    break;
  default:
    printf(" [Unknown node type]");
    break;
  }

  // Close the node paranthesis
  printf(")\n");
}

const char *get_node_type_name(ast_node_type_t type) {
  switch (type) {
  case AST_PROGRAM:
    return "Program";
  case AST_DECL_LIST:
    return "Declaration List";
  case AST_DECLARATION:
    return "Declaration";
  case AST_VAR_DECLARATION:
    return "Variable Declaration";
  case AST_FUN_DECLARATION:
    return "Function Declaration";
  case AST_TYPE_SPECIFIER:
    return "Type Specifier";
  case AST_PARAM_LIST:
    return "Parameter List";
  case AST_PARAM:
    return "Parameter";
  case AST_COMPOUND_DECL:
    return "Compound Declaration";
  case AST_LOCAL_DECLARATIONS:
    return "Local Declarations";
  case AST_STATEMENT_LIST:
    return "Statement List";
  case AST_STATEMENT:
    return "Statement";
  case AST_EXPRESSION_STATEMENT:
    return "Expression Statement";
  case AST_COMPOUND_STATEMENT:
    return "Compound Statement";
  case AST_SELECTION_STATEMENT:
    return "Selection Statement";
  case AST_ITERATION_STATEMENT:
    return "Iteration Statement";
  case AST_RETURN_STATEMENT:
    return "Return Statement";
  case AST_ASSIGNMENT_EXPRESSION:
    return "Assignment Expression";
  case AST_SIMPLE_EXPRESSION:
    return "Simple Expression";
  case AST_VARIABLE:
    return "Variable";
  case AST_RELATIONAL_OPERATOR:
    return "Relational Operator";
  case AST_ADDITIVE_EXPRESSION:
    return "Additive Expression";
  case AST_ADDITIVE_OPERATOR:
    return "Additive Operator";
  case AST_TERM:
    return "Term";
  case AST_MULTIPLICATIVE_OPERATOR:
    return "Multiplicative Operator";
  case AST_FACTOR:
    return "Factor";
  case AST_ACTIVATION:
    return "Activation";
  case AST_ARGUMENT_LIST:
    return "Argument List";
  default:
    return "Unknown Node Type";
  }
}

const char *get_token_type_name(token_types_t type) {
  switch (type) {
  case TOKEN_INT:
    return "int";
  case TOKEN_VOID:
    return "void";
  case TOKEN_IF:
    return "if";
  case TOKEN_ELSE:
    return "else";
  case TOKEN_WHILE:
    return "while";
  case TOKEN_RETURN:
    return "return";
  case TOKEN_ID:
    return "identifier";
  case TOKEN_NUM:
    return "number";
  case TOKEN_PLUS:
    return "+";
  case TOKEN_MINUS:
    return "-";
  case TOKEN_MULT:
    return "*";
  case TOKEN_DIV:
    return "/";
  case TOKEN_LT:
    return "<";
  case TOKEN_LE:
    return "<=";
  case TOKEN_GT:
    return ">";
  case TOKEN_GE:
    return ">=";
  case TOKEN_EQ:
    return "==";
  case TOKEN_DIFF:
    return "!=";
  case TOKEN_ATTR:
    return "=";
  case TOKEN_DELIM:
    return ";";
  case TOKEN_COMMA:
    return ",";
  case TOKEN_LPARENT:
    return "(";
  case TOKEN_RPARENT:
    return ")";
  case TOKEN_LKEY:
    return "{";
  case TOKEN_RKEY:
    return "}";
  case TOKEN_LBRACKET:
    return "[";
  case TOKEN_RBRACKET:
    return "]";
  default:
    return "Unknown Token";
  }
}
