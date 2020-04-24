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

Node *new_node_ident(char *ident) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;

  // search locals for the same name.
  LVar *lvar = find_lvar(ident);
  if (!lvar) {
    lvar = add_lvar(ident);
  }
  node->offset = lvar->offset;
  return node;
}

void program();    // = stmt*
Node *stmt();       // = expr ";" | "return" expr ";"
Node *expr();       // = assign
Node *assign();     // = equality ("=" assign)?
Node *equality();   // = relational ("==" relational | "!=" relational)*
Node *relational(); // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();        // = mul ("+" mul | "-" mul)*
Node *mul();        // = unary ("*" unary | "/" unary)*
Node *unary();      // = ("+" | "-")? primary
Node *primary();    // = num | ident | "(" expr ")"

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume_keyword(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *expr() {
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
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  // if ( follows, it must be '( expr )'
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // = num | ident | "(" expr ")"
  char *ident = consume_ident();
  if (ident != NULL) {
    return new_node_ident(ident);
  }

  return new_node_num(expect_number());
}