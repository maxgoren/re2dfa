#ifndef tokens_h
#define tokens_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum RESymbol {
    RE_CHAR, RE_PERIOD, RE_LPAREN, RE_RPAREN, 
    RE_CCL, RE_STAR, RE_PLUS, RE_QUESTION, 
    RE_CONCAT, RE_OR, RE_EPSILON, RE_NONE
};

extern char* RESymbolStr[];

typedef struct Token_ {
    enum RESymbol symbol;
    char ch;
    char *ccl;
    struct Token_* next;
} Token;

Token* makeToken(enum RESymbol sym, char ch);

bool is_digit(char c);
bool is_char(char c);

Token* tokenize(char* str);

int tokensLength(Token* list);

char* toString(Token* tokens);

void printTokenStream(Token* tokens);

void freeTokenStream(Token* tokens);

#endif