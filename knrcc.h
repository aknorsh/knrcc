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
typedef struct GVar GVar;
typedef struct Type Type;

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
  TK_SIZEOF,   // sizeof
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
  ND_LVAR,   // Local Variable
  ND_GVAR,   // Global Variable
  ND_NUM,    // Decimal Number
  ND_RETURN, // return
  ND_IF,     // if
  ND_IFELSE, // if_else
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_BLOCK,  // { }
  ND_FNCALL, // function call
  ND_DEFN,   // function definition
  ND_DEFV,   // def variable
  ND_ADDR,   // address (unary &)
  ND_DEREF,  // dereference (unary *)
} NodeKind;

struct Node {
  NodeKind kind; // Type of Node
  Node *lhs;     // Left hand side
  Node *rhs;     // Right hand side
  int val;       // value         (ND_NUM)
  char *vname;   // var name      (ND_LVAR)
  LVar *lvar;    // local var     (ND_LVAR|ND_DEFV)
  GVar *gvar;    // global var    (ND_DEFV)
  Node *cond;    // condition     (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *body;    // body          (ND_IF/ND_IFELSE/ND_WHILE/ND_FOR)
  Node *elbody;  // else body     (ND_IFELSE)
  Node *for_init;// init expr     (ND_FOR)
  Node *for_updt;// update expr   (ND_FOR)
  VecNode *vn;   // node vector   (ND_BLOCK|ND_DEFN)
  char *fname;   // function name (ND_FNCALL|ND_DEFN)
  VecNode *args; // arguments     (ND_FNCALL|ND_DEFN)
  LVar *scope;   // ptr to local scope
};

void program();

// codegen.c

void codegen();

// variables.c

struct LVar {
  LVar *next; // next lvar or NULL
  char *name; // name of lvar
  int len;    // length of name
  int offset; // offset from rbp
  Type *ty;    // var type
};

LVar *find_lvar(char *name);
LVar *add_lvar(char *name, Type* ty);

struct GVar {
  GVar *next; // next gvar or NULL
  char *name; // name of gvar
  int len;    // length of name
  Type *ty;   // var type
  int sz;     // var size
};

GVar *find_gvar(char *name);
GVar *add_gvar(char *name, Type* ty);

// type
struct Type {
  enum { INT, PTR, ARRAY } ty;
  Type *ptr_to;
  size_t array_size;
};

// main.c

extern char *user_input;
extern Token *token;
extern Node *code[];
extern LVar *local_variables[256];
extern LVar *locals;
extern GVar *globals;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);