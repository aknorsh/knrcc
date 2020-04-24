#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize.c

typedef enum {
  TK_RESERVED, // Symbol
  TK_NUM,      // Number
  TK_EOF,      // rep. end of input
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // kind of token
  Token *next;    // next token
  int val;        // stored value (int)
  char *str;      // stored value (string)
  int len;        // length of token (string)
};

Token *tokenize(char *p);

bool consume(char *op);
void expect(char *op);
int expect_number();

// parse.c

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_LT,  // <
  ND_LE,  // <=
  ND_EQ,  // ==
  ND_NEQ, // !=
  ND_NUM, // Decimal Number
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // Type of Node
  Node *lhs;     // Left hand side
  Node *rhs;     // Right hand side
  int val;       // used when kind is ND_NUM
};

Node *expr();

// codegen.c

void codegen(Node *node);

// main.c

extern char *user_input;
extern Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);