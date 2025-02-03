#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "parser.h"

//! Prints the spaces
void print_indent(int indent_level);

//! Prints the ASTree 
void print_ast(ast_node_t *node);

//! Prints an AST sub-tree
void print_ast_node(ast_node_t *node, int indent_level);

//! Prints the node informations
const char *get_node_type_name(ast_node_type_t type);

//! Prints the token informations
const char *get_token_type_name(token_types_t type);

#endif // AST_PRINTER_H
