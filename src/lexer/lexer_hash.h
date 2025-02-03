#ifndef LEXER_HASH_H
#define LEXER_HASH_H

#include "lexer.h"

#define HASH_TABLE_SIZE 30

typedef struct hash_node {
  char *key;
  token_types_t type;
  struct hash_node *next; // case of collision
} hash_node_t;

typedef struct {
  hash_node_t *table[HASH_TABLE_SIZE];
} hash_table_t;

extern hash_table_t lexer_hash_table;

//! Init the hash table with the reserved words
void lexer_hash_init();

//! Deallocates the hash table used
void lexer_hash_delete();

//! Search for a word in the hash table and returns the reserved word token OR ID if the key isn't a reserved word
token_types_t lexer_lookup_reserved_word(const char *key);

//! Hash Function to get the key index
unsigned int hash_function(const char *key);

#endif // !LEXER_HASH_H
