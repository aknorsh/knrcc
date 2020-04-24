#include "knrcc.h"

// Lvar *locals : global array to store local variables.

LVar *find_lvar(char *name) {
  int len = strlen(name);
  if (locals == NULL) return NULL;
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == len && !memcmp(name, var->name, len)) {
      return var;
    }
  }
  return NULL;
}

LVar *add_lvar(char *name) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->len  = strlen(name);
  if (locals) {
    lvar->offset = locals->offset + 8;
  }
  else {
    lvar->offset = 8;
  }
  locals = lvar;
  return lvar;
}