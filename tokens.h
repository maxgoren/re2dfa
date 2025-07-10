#ifndef tokens_h
#define tokens_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum RESymbol {
    RE_CHAR, RE_PERIOD, RE_LPAREN, RE_RPAREN,
    RE_STAR, RE_PLUS, RE_QUESTION, 
    RE_CONCAT, RE_OR, RE_NONE
};

char* RESymbolStr[] = {
    "CHAR", "PERIOD", "LPAREN", "RPAREN",
    "STAR", "PLUS", "QUESTION", 
    "CONCAT", "OR", "NONE"
};

typedef struct Token_ {
    enum RESymbol symbol;
    char ch;
    struct Token_* next;
} Token;

Token* makeToken(enum RESymbol sym, char ch) {
    Token* nt = malloc(sizeof(Token));
    nt->ch = ch;
    nt->symbol = sym;
    nt->next = NULL;
    return nt;
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_char(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '#';
}

Token* tokenize(char* str) {
    Token header; Token* t = &header;
    int sp = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (is_digit(str[i]) || is_char(str[i])) {
            t->next = makeToken(RE_CHAR, str[i]);
            t = t->next;
        } else if (str[i] == '\\' && str[i+1] == '.') {
            t->next = makeToken(RE_CHAR, '.');
            t = t->next;
        } else if (str[i] == '.') {
            t->next = makeToken(RE_PERIOD, str[i]);
            t = t->next;
        } else {
            switch (str[i]) {
                case '@': { 
                    t->next = makeToken(RE_CONCAT, str[i]);
                    t = t->next;  
                } break;
                case '|': { 
                    t->next = makeToken(RE_OR, str[i]);
                    t = t->next;
                 } break;
                case '+': { 
                    t->next = makeToken(RE_PLUS, str[i]);
                    t = t->next; 
                } break;
                case '*': { 
                    t->next = makeToken(RE_STAR, str[i]);
                    t = t->next;  
                } break;
                case '?': { 
                    t->next = makeToken(RE_QUESTION, str[i]);
                    t = t->next; 
                } break;
                case '(': { 
                    t->next = makeToken(RE_LPAREN, str[i]);
                    t = t->next; 
                } break;
                case ')': { 
                    t->next = makeToken(RE_RPAREN, str[i]);
                    t = t->next; 
                } break;
                default:
                    break;
            }
        }
    }
    return header.next;
}

int tokensLength(Token* list) {
    int len = 0;
    for (Token* it = list; it != NULL; it = it->next, len++);
    return len;
}

char* toString(Token* tokens) {
    char* asStr = malloc(sizeof(char)*tokensLength(tokens)+1);
    int n = 0;
    for (Token* it = tokens; it != NULL; it = it->next) {
        asStr[n++] = it->ch;
    }
    asStr[n++] = '\0';
    return asStr;
}

void printTokenStream(Token* tokens) {
    for (Token* it = tokens; it != NULL; it = it->next) {
        printf("<%d, %c>\n", it->symbol, it->ch);
    }
}

#endif