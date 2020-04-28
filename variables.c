#include "knrcc.h"

// Lvar *locals : ptr to local variables belonging current scope.

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

LVar *add_lvar(char *name, Type *ty) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->len  = strlen(name);

  int offset;
  switch (ty->ty) {
    case ARRAY:
      offset = ty->array_size * 8;
      break;
    case INT:
      offset = 8; // this must be 4, actually
      break;
    case PTR:
      offset = 8;
      break;
  }

  if (locals) {
    lvar->offset = locals->offset + offset;
  }
  else {
    lvar->offset = offset;
  }
  locals = lvar;
  return lvar;
}

// Gvar *globals : ptr to global variables.

GVar *find_gvar(char *name) {
  int len = strlen(name);
  if (globals == NULL) return NULL;
  for (GVar *var = globals; var; var = var->next) {
    if (var->len == len && !memcmp(name, var->name, len)) {
      return var;
    }
  }
  return NULL;
}

GVar *add_gvar(char *name, Type *ty) {
  GVar *var = calloc(1, sizeof(GVar));
  var->next = globals;
  var->name = name;
  var->len  = strlen(name);
  var->ty = ty;

  int sz;
  switch (ty->ty) {
    case ARRAY:
      sz = ty->array_size * 8;
      break;
    case INT:
      sz = 8; // this must be 4, actually
      break;
    case PTR:
      sz = 8;
      break;
  }

  var->sz = sz;

  globals = var;
  return var;
}