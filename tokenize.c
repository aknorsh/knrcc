#include "knrcc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // skip space
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "==", 2) == 0 ||
        strncmp(p, ">=", 2) == 0 ||
        strncmp(p, "<=", 2) == 0 ||
        strncmp(p, "!=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '(' || *p == ')' || *p == '<' || *p == '>') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Cannot Tokenize.");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

// Look the next. Eat it and say True, or just say False.
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// Look the next. Eat it or Abort.
void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "It is not '%c'", op);
  token = token->next;
}

// Look the next, to be Num. Eat it or Abort.
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "It is not Number.");
  int val = token->val;
  token = token->next;
  return val;
}