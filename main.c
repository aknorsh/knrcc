#include "knrcc.h"

// Report error
// args are the same as printf
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Input source code
char *user_input;

// Current Token
Token *token;

// AST list
Node *code[256];

// Local Variable list
LVar *locals;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("Invalid args");
    return 1;
  }

  user_input = argv[1];
  tokenize();
  program();
  codegen();

  return 0;
}