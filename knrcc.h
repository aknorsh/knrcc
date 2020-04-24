#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Type definition
 */

typedef enum {
  TK_RESERVED, // Symbol
  TK_NUM,      // Number
  TK_EOF,      // rep. end of input
} TokenKind;

typedef struct Token Token;

// Type: Token
struct Token {
  TokenKind kind; // kind of token
  Token *next;    // next token
  int val;        // stored value (int)
  char *str;      // stored value (string)
  int len;        // length of token (string)
};

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

/**
 *  Function
 */

// tokenize.c
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();

// parse.c
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();       // = equality
Node *equality();   // = relational ("==" relational | "!=" relational)*
Node *relational(); // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();        // = mul ("+" mul | "-" mul)*
Node *mul();        // = unary ("*" unary | "/" unary)*
Node *unary();      // = ("+" | "-")? primary
Node *primary();    // = num | "(" expr ")"

// codegen.c
void gen(Node *node);

// main.c
extern char *user_input;
extern Token *token;
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);