#include "ast.h"

re_ast* makeNode(int type, Token tk) {
    re_ast* node = malloc(sizeof(re_ast));
    node->type = type;
    node->token = tk;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void printAST(re_ast* node, int d) {
    if (node != NULL) {
        for (int i = 0; i < d; i++) printf("  ");
        printf("<%s, %c> {%d}\n", RESymbolStr[node->token.symbol], node->token.ch, node->number);
        printAST(node->left, d+1);
        printAST(node->right, d+1);
    }
}

void freeTree(re_ast* node) {
    if (node != NULL) {
        freeTree(node->left);
        freeTree(node->right);
        free(node);
    }
}