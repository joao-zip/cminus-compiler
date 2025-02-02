#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "parser.h"

void print_indent(int indent_level);

void print_ast(ast_node_t *node);

void print_ast_node(ast_node_t *node, int indent_level);

const char *get_node_type_name(ast_node_type_t type);

const char *get_token_type_name(token_types_t type);

#endif // AST_PRINTER_H
