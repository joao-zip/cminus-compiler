#include "lexer_hash.h"

#include <stdlib.h>
#include <string.h>

hash_table_t lexer_hash_table;

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
