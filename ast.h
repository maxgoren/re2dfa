#ifndef ast_h
#define ast_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
typedef struct re_ast_ {
    int type;
    int number;
    Token token;
    struct re_ast_* left;
    struct re_ast_* right;
} re_ast;

re_ast* makeNode(int type, Token tk) {
    re_ast* node = malloc(sizeof(re_ast));
    node->type = type;
    node->token = tk;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void print(re_ast* node, int d) {
    if (node != NULL) {
        for (int i = 0; i < d; i++) printf("  ");
        printf("<%s, %c> {%d}\n", RESymbolStr[node->token.symbol], node->token.ch, node->number);
        print(node->left, d+1);
        print(node->right, d+1);
    }
}

#endif