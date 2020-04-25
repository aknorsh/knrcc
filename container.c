#include "knrcc.h"

VecNode *init_vn() {
  VecNode *vec = calloc(1, sizeof(VecNode));
  vec->node_arr = calloc(4, sizeof(Node));
  vec->size = 0;
  vec->max_size = 4;
  return vec;
}

void pushback_vn(VecNode* vec, Node *input_node) {
  if (vec->size == vec->max_size) {
    Node **tmp = vec->node_arr;
    vec->max_size *= 2;
    vec->node_arr = calloc(vec->max_size, sizeof(Node));
    memcpy(vec->node_arr, tmp, sizeof(Node) * vec->size);
  }
  vec->node_arr[vec->size] = calloc(1, sizeof(Node));
  memcpy(vec->node_arr[vec->size], input_node, sizeof(Node));
  vec->size++;
}