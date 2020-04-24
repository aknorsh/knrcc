#include "knrcc.h"

static int _seq_int = 1;

char *yield_key() {
  char *key = calloc(11, sizeof(char));
  int cur = _seq_int++;
  for(int i=0;i<10;i++) {
    key[i] = '0' + cur % 10;
    cur/=10;
  }
  key[10] = '\0';
  return key;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("Left value must be Variable.");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  char *key;
  switch (node->kind) {
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      gen(node->cond);
      key = yield_key();
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%s\n", key);
      gen(node->body);
      printf(".Lend%s:\n", key);
      return;
    case ND_IFELSE:
      gen(node->cond);
      key = yield_key();
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%s\n", key);
      gen(node->body);
      printf("  je .Lend%s\n", key);
      printf(".Lelse%s:\n", key);
      gen(node->elbody);
      printf(".Lend%s:\n", key);
      return;
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQ:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

void codegen() {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue: alloc memory for 26 variables.
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 2048\n");

  for (int i=0; code[i] != NULL; i++) {
    gen(code[i]);
    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}