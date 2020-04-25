#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token Token;
typedef struct Node Node;
typedef struct VecNode VecNode;
typedef struct LVar LVar;

// container.c

struct VecNode {
  Node **node_arr;
  int size;
  int max_size;
};

VecNode *init_vn();
void pushback_vn(VecNode* vec, Node *input_node);

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
bool at_researved(char *str);

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
  ND_BLOCK,  // { }
  ND_FNCALL, // function call
  ND_DEFN,   // function definition
  ND_ADDR,   // address (unary &)
  ND_DEREF,  // dereference (unary *)
} NodeKind;

struct Node {
  NodeKind kind; // Type of Node
  Node *lhs;     // Left hand side
  Node *rhs;     // Right hand side
  int val;       // value         (ND_NUM)
  int offset;    // stack offset  (ND_LVAR)
  Node *cond;    // condition     (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *body;    // body          (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *elbody;  // else body     (ND_IFELSE)
  Node *for_init;// init expr     (ND_FOR)
  Node *for_updt;// update expr   (ND_FOR)
  VecNode *vn;   // node vector   (ND_BLOCK|ND_DEFN)
  char *fname;   // function name (ND_FNCALL|ND_DEFN)
  VecNode *args; // arguments     (ND_FNCALL|ND_DEFN)
};

void program();

// codegen.c

void codegen();

// leftvalue.c

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