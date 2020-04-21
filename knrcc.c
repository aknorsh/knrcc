#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token enum
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
    char *str;      // stored value (char)
};

// Current Token
Token *token;

// Input source code
char *user_input;

// -------------------

// Report error
// args are the same as printf
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// -------------------

// if next token is expected, eat 1 token and ret. true; else, ret. false.
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
    token = token->next;
    return true;
}

// if next token is expected, eat 1 token; else, assert error.
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "It is not '%c'", op);
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "It is not Number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// -------------------

// Generate new token with kind and joint it to cur.
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// Tokenize input string p, and ret. it.
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

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Cannot Tokenize.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

// -------------------

int main(int argc, char **argv) {
    if (argc != 2) {
        error("Invalid args");
        return 1;
    }
    user_input = argv[1];

    token = tokenize(user_input);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        // if token is not '+', it must be '-'
        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }

    printf("    ret\n");
    return 0;
}