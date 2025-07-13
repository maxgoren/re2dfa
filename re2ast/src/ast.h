#ifndef ast_h
#define ast_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
#include "../../src/intset.h"
typedef struct re_ast_ {
    int type;
    int number;
    int tk_token_id;
    Token token;
    Set* firstpos;
    Set* lastpos;
    Set* followpos;
    struct re_ast_* left;
    struct re_ast_* right;
} re_ast;

re_ast* makeNode(int type, Token tk);

void printAST(re_ast* node, int d);

void freeTree(re_ast* node);

re_ast* cloneTree(re_ast* node);

#endif