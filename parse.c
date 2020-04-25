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

void program();     // = stmt*
Node *stmt();       // = expr ";"
                    // | "{" stmt* "}"
                    // | "return" expr ";"
                    // | "if" "(" expr ")" stmt ("else" stmt)?
                    // | "while" "(" expr ")" stmt
                    // | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *expr();       // = assign
Node *assign();     // = equality ("=" assign)?
Node *equality();   // = relational ("==" relational | "!=" relational)*
Node *relational(); // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();        // = mul ("+" mul | "-" mul)*
Node *mul();        // = unary ("*" unary | "/" unary)*
Node *unary();      // = ("+" | "-")? primary
Node *primary();    // = num
                    // | ident ("(" (expr ("," expr)* )* ")")?
                    // | "(" expr ")"

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

                    // | ident ("(" (expr ("," expr)* )* ")")?
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
    return new_node_ident(ident);
  }

  return new_node_num(expect_number());
}