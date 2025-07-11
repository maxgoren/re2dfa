#ifndef parser_h
#define parser_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
#include "ast.h"

char* addConCat(char* str);

bool leftAssoc(char c);

int precedence(char c);

bool isOp(Token c);

Token* in2post(Token* input);

re_ast* cloneTree(re_ast* node);

re_ast* makeTree(Token* tokens);

re_ast* re2ast(char* regex);

#endif