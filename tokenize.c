#include "knrcc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token *new_token_ident(Token *cur, char *str)
{
  char *runner = str;
  int len = 0;
  while (*runner == '_' ||
      ('A' <= *runner && *runner <= 'Z') ||
      ('a' <= *runner && *runner <= 'z') ||
      ('0' <= *runner && *runner <= '9')) {
        runner ++;
        len ++;
      }

  char *ident = calloc(len, sizeof(char) + 1);
  memcpy(ident, str, sizeof(char) * len);
  ident[len] = '\0';

  Token *tok = calloc(1, sizeof(Token));
  tok->kind = TK_IDENT;
  tok->str = ident;
  tok->len = len;
  cur->next = tok;
  return tok;
}

void tokenize() {
  char *p = user_input;
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
        *p == '(' || *p == ')' || *p == '<' || *p == '>' ||
        *p == '=' || *p == ';') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (('a' <= *p && *p <= 'z') || *p == '_') {
      cur = new_token_ident(cur, p);
      p += cur->len;
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
  token = head.next;
}

// consume: Look at the next, and return Boolean

bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

char *consume_ident() { // I want it to return bool....
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  char *ident = token->str;
  token = token->next;
  return ident;
}

// expect: Look at the next, and Eat Or Abort

void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "It is not '%s'", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "It is not Number.");
  int val = token->val;
  token = token->next;
  return val;
}

// at: Check current token

bool at_eof() {
  return token->kind == TK_EOF;
}