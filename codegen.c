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

static int _hex_align = 0;

void pop (const char *reg) {
  printf("  pop %s\n", reg);
  _hex_align = !_hex_align;
}

void push (char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  printf("  push ");
  vprintf(fmt, ap);
  printf("\n");
  _hex_align = !_hex_align;
}

void gen(Node *node);

// load address which has val
void gen_lval(Node *node) {
  if (node->kind == ND_DEREF) {
    // lhs must calculate some address. Let it do.
    gen(node->lhs);
  }
  else if (node->kind == ND_LVAR) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->lvar->offset);
    push("rax");
  }
  else if (node->kind == ND_GVAR) {
    printf("  push offset %s\n", node->gvar->name);
  }
  else error("Error: Left value must be Variable.");
}

const char *reg[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

void gen(Node *node) {
  char *key;
  switch (node->kind) {
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);
      pop("rax");
      printf("  mov rax, [rax]\n");
      push("rax");
      return;
    case ND_LVAR:
    case ND_GVAR:
      gen_lval(node);
      pop("rax");
      printf("  mov rax, [rax]\n");
      push("rax");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      pop("rdi");
      pop("rax");
      printf("  mov [rax], rdi\n");
      push("rdi");
      return;
    case ND_DEFV:
      push("rax");
      return;
    case ND_DEFN:
      printf("%s:\n", node->fname);
      // prologue: alloc memory for many variables.
      push("rbp");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, 2048\n");
      // load parameters
      if (node->args) {
        for(int i=0;i<node->args->size;i++) {
          printf("  mov rax, rbp\n");
          printf("  sub rax, %d\n", node->args->node_arr[i]->lvar->offset);
          printf("  mov [rax], %s\n", reg[i]);
        }
      }
      for (int i=0; i<node->vn->size; i++) {
        gen(node->vn->node_arr[i]);
      }
      return;
    case ND_FNCALL:
      if (node->args) {
        for(int i=0;i<node->args->size;i++) {
          gen(node->args->node_arr[i]);
          pop(reg[i]);
        }
      }
      if (!_hex_align) {
        printf("  call %s\n", node->fname);
      } else {
        fprintf(stderr, "Alert: aligning occured!\n");
        push("1");
        printf("  call %s\n", node->fname);
        pop("rdi");
      }
      push("rax");
      return;
    case ND_RETURN:
      gen(node->lhs);
      pop("rax");
      printf("  mov rsp, rbp\n");
      pop("rbp");
      printf("  ret\n");
      return;
    /**
     * CONTROL
     */
    case ND_IF:
      key = yield_key();
      gen(node->cond);
      pop("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%s\n", key);
      gen(node->body);
      printf(".Lend%s:\n", key);
      return;
    case ND_IFELSE:
      key = yield_key();
      gen(node->cond);
      pop("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%s\n", key);
      gen(node->body);
      printf("  je .Lend%s\n", key);
      printf(".Lelse%s:\n", key);
      gen(node->elbody);
      printf(".Lend%s:\n", key);
      return;
    case ND_WHILE:
      key = yield_key();
      printf(".Lbegin%s:\n", key);
      gen(node->cond);
      pop("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%s\n", key);
      gen(node->body);
      printf("  jmp .Lbegin%s\n", key);
      printf(".Lend%s:\n", key);
      return;
    case ND_FOR:
      key = yield_key();
      if (node->for_init)
        gen(node->for_init);
      printf(".Lbegin%s:\n", key);
      if (node->cond)
        gen(node->cond);
      pop("rax");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%s\n", key);
      gen(node->body);
      if (node->for_updt)
        gen(node->for_updt);
      printf("  jmp .Lbegin%s\n", key);
      printf(".Lend%s:\n", key);
      return;
    /**
     * LITERAL
     */
    case ND_NUM:
      push("%d", node->val);
      return;
    case ND_BLOCK:
      for (int i=0; i<node->vn->size; i++) {
        gen(node->vn->node_arr[i]);
        pop("rax");
      }
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  pop("rdi");
  pop("rax");

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

  push("rax");
}

void codegen() {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");

  printf("\n.data\n");
  for (GVar *var = globals; var; var=var->next) {
    printf("%s:\n", var->name);
    printf("  .zero %d\n", var->sz);
  }

  printf("\n.text\n");
  for (int i=0; code[i] != NULL; i++) {
    if (code[i]->kind != ND_DEFV) {
      locals = local_variables[i];
      gen(code[i]);
      pop("rax");
      printf("  mov rsp, rbp\n");
      pop("rbp");
      printf("  ret\n");
    }
  }
}