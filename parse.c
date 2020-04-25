#include "knrcc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_defvar(char *name, Type* ty) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_DEFV;

  if (find_lvar(name)) error("Error: It is already defined: %s", name);

  node->lvar = add_lvar(name);
  node->lvar->ty = ty;
  return node;
}

Node *new_node_lvar(char *name) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->lvar = find_lvar(name);

  if (!node->lvar) error("Error: Declaration is needed: %s", name);
  return node;
}

void program();     // = func*
Node *func();       // = type fname "(" (type ident ("," type ident)* )? ")" "{" stmt* "}"
Node *stmt();       // = expr ";"
                    // | "{" stmt* "}"
                    // | "return" expr ";"
                    // | "if" "(" expr ")" stmt ("else" stmt)?
                    // | "while" "(" expr ")" stmt
                    // | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *expr();       // = assign | type ident
Node *assign();     // = equality ("=" assign)?
Node *equality();   // = relational ("==" relational | "!=" relational)*
Node *relational(); // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();        // = mul ("+" mul | "-" mul)*
Node *mul();        // = unary ("*" unary | "/" unary)*
Node *unary();      // = ("+" | "-")? primary
                    // | ("*" | "&") unary
Node *primary();    // = num
                    // | ident ("(" (expr ("," expr)* )* ")")?
                    // | "(" expr ")"
Type *type();       // "int" ("*")*

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = func();
  }
  code[i] = NULL;
}

Node *func() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_DEFN;

  type(); // just discard info about type that fn returns.

  node->fname = consume_ident();
  if (node->fname == NULL) {
    error("Error: Program has to be begin with DEFN.");
  }

  expect("(");
  node->args = init_vn();
  for(;;) {
    if (at_researved("int")) {
      Type *ty = type();
      char *ident = consume_ident();
      if (!ident)
        error("Type is needed.");
      pushback_vn(node->args, new_node_defvar(ident, ty));
    } else break;
    if(!consume(",")) break;
  }
  expect(")");

  expect("{");
  node->vn = init_vn();
  while(!consume("}")) {
    pushback_vn(node->vn, stmt());
  }
  return node;
}

Node *stmt() {
  Node *node;

  if (consume_keyword(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
  }
  else if (consume_keyword(TK_IF)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
    if (consume_keyword(TK_ELSE)) {
      node->kind = ND_IFELSE;
      node->elbody = stmt();
    }
  }
  else if (consume_keyword(TK_WHILE)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
  }
  else if (consume_keyword(TK_FOR)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    if (!at_researved(";")) {
      node->for_init = expr();
    }
    expect(";");
    if (!at_researved(";")) {
      node->cond = expr();
    }
    expect(";");
    if (!at_researved(";")) {
      node->for_updt = expr();
    }
    expect(")");
    node->body = stmt();
  }
  else if (consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->vn = init_vn();
    while (!consume("}")) {
      pushback_vn(node->vn, stmt());
    }
  }
  else {
    node = expr();
    expect(";");
  }

  return node;
}

Node *expr() {
  if (at_researved("int")) {
    Type *ty = type();
    char *ident = consume_ident();
    if (!ident) error("There is no var after 'int'.");
    Node *node = new_node_defvar(ident, ty);
    return node;
  }
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  for(;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("*")) 
    return new_node(ND_DEREF, unary(), NULL);
  if (consume("&"))
    return new_node(ND_ADDR, unary(), NULL);
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  char *ident = consume_ident();
  if (ident != NULL) {
    if (consume("(")) {
      Node *node = calloc(1, sizeof(Node));
      node->kind = ND_FNCALL;
      node->fname = ident;
      if (consume(")")) {
        return node;
      }

      node->args = init_vn();
      for(;;) {
        pushback_vn(node->args, expr());
        if(!consume(",")) break;
      }
      expect(")");
      return node;
    }
    return new_node_lvar(ident);
  }

  return new_node_num(expect_number());
}

Type *gen_type();

Type *type() {
  expect("int");
  return gen_type();
}

Type *gen_type() {
  Type *ty = calloc(1, sizeof(Type));
  if (consume("*")) {
    ty->ty = PTR;
    ty->ptr_to = gen_type();
  } else ty->ty = INT;
  return ty;
}