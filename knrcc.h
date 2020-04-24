#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize.c

typedef enum {
  TK_RESERVED, // Symbol
  TK_IDENT,    // Identifier
  TK_NUM,      // Number
  TK_EOF,      // End of file
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while
  TK_FOR,      // for
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // kind of token
  Token *next;    // next token
  int val;        // stored value (int)
  char *str;      // stored value (string)
  int len;        // length of token (string)
};

void tokenize();

bool consume(char *op);
char *consume_ident();
bool consume_keyword(TokenKind tk);
void expect(char *op);
int expect_number();
bool at_eof();
bool at_semicolon();

// parse.c

typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_LT,     // <
  ND_LE,     // <=
  ND_EQ,     // ==
  ND_NEQ,    // !=
  ND_ASSIGN, // =
  ND_LVAR,   // Identifier
  ND_NUM,    // Decimal Number
  ND_RETURN, // return
  ND_IF,     // if
  ND_IFELSE, // if_else
  ND_WHILE,  // while
  ND_FOR,    // for
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // Type of Node
  Node *lhs;     // Left hand side
  Node *rhs;     // Right hand side
  int val;       // used when kind is ND_NUM
  int offset;    // used when kind is ND_LVAR
  Node *cond;    // condition (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *body;    // body (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *elbody;  // else body (ND_IFELSE)
  Node *for_init;// init expr (ND_FOR)
  Node *for_updt;// update expr (ND_FOR)
};

void program();

// codegen.c

void codegen();

// leftvalue.c

typedef struct LVar LVar;

struct LVar {
  LVar *next; // next lvar or NULL
  char *name; // name of lvar
  int len;    // length of name
  int offset; // offset from rbp
};

LVar *find_lvar(char *name);
LVar *add_lvar(char *name);

// main.c

extern char *user_input;
extern Token *token;
extern Node *code[];
extern LVar *locals;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);