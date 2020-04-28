#include "knrcc.h"

int memSize(Type *ty) {
  switch (ty->ty) {
  case CHAR:
    return 1;
  case INT:
    return 4;
  case PTR:
    return 8;
  case ARRAY:
    return ty->array_size * memSize(ty->ptr_to);
  }
}

Type *arraynize(Type *ty, int size) {
  Type *ar_ty = calloc(1, sizeof(Type));
  ar_ty->ty = ARRAY;
  ar_ty->array_size = size;
  ar_ty->ptr_to = ty;
  return ar_ty;
}