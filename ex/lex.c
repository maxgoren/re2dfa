#include <stdio.h>
#include <stdlib.h>
#include "../re2ast/src/ast.h"
#include "../re2ast/src/parser.h"
#include "../src/re_to_dfa.h"

enum TKSymbol {
    TK_NUM, TK_REAL, TK_ID, TK_LP, TK_RP, TK_LC, TK_RC,
    TK_PRINT, TK_IF, TK_ELSE, TK_WHILE, TK_IN, TK_FUNC,
    TK_MAP, TK_FILTER, TK_REDUCE, TK_NIL, TK_PUSH, TK_SIZE, TK_SORT,
    TK_FIRST, TK_REST, TK_BLESS, TK_BREAK, TK_FOPEN, TK_RETURN,
    TK_EMPTY, TK_REVERSE, TK_STRUCT, TK_TYPEOF, TK_FOREACH,
    TK_LET, TK_TRUE, TK_FALSE, TK_COMA, TK_SEMI,
    TK_PERIOD,
    TK_EOF
};


typedef struct {
    char* pattern;
    enum TKSymbol token;
} TokenRule;


int nr = 36;

TokenRule rules[] = {
    {"[0-9]+", TK_NUM},
    {"\\(", TK_LP},
    {"\\)", TK_RP},
    {"{", TK_LC},
    {"}", TK_RC},
    {"if", TK_IF},
    {"in", TK_IN},
    {"let", TK_LET},
    {"var", TK_LET},
    {"def", TK_FUNC},
    {"map", TK_MAP},
    {"nil", TK_NIL},
    {"else",TK_ELSE},
    {"push", TK_PUSH},
    {"size", TK_SIZE},
    {"sort", TK_SORT},
    {"rest", TK_REST},
    {"true", TK_TRUE},
    {"false", TK_FALSE},
    {"break", TK_BREAK},
    {"fopen", TK_FOPEN},
    {"first", TK_FIRST},
    {"while", TK_WHILE},
    {"print", TK_PRINT},
    {"empty", TK_EMPTY},
    {"bless", TK_BLESS},
    {"filter", TK_FILTER},
    {"reduce", TK_REDUCE},
    {"return", TK_RETURN},
    {"struct", TK_STRUCT},
    {"typeOf", TK_TYPEOF},
    {"foreach", TK_FOREACH},
    {"reverse", TK_REVERSE},
    {"[A-Za-z]+[A-Za-z0-9]*", TK_ID},
    //{"\\.", TK_PERIOD},
    {",", TK_COMA},
    {";", TK_SEMI}
};

void tag_final_pos_with_token_id(re_ast* ast, int rulenum) {
    if (ast != NULL) {
        if (isLeaf(ast) && ast->token.ch == '#')
            ast->tk_token_id = rulenum;
        tag_final_pos_with_token_id(ast->left, rulenum);
        tag_final_pos_with_token_id(ast->right, rulenum);
    }
}

typedef struct {
    char* pattern;
    int patlen;
    re_ast* ast;
} CombinedRE;

CombinedRE* combine(int numrules) {
    re_ast* root = re2ast(augmentRE(rules[0].pattern));
    tag_final_pos_with_token_id(root, 0);
    char* re = malloc(sizeof(char)*1024);
    re[0] = '(';
    int p = 1;
    char* pat = augmentRE(rules[0].pattern);
    for (int k = 0; k < strlen(pat); k++)
        re[p++] = pat[k];
    for (int i = 1; i < numrules; i++) {
        re[p++] = '|';
        re_ast* tmp = re2ast(augmentRE(rules[i].pattern));
        tag_final_pos_with_token_id(tmp, i);
        re_ast* alt = makeNode(1, *makeToken(RE_OR, '|'));
        alt->left = root;
        alt->right = tmp;
        root = alt;
        pat = augmentRE(rules[i].pattern);
        for (int k = 0; k < strlen(pat); k++)
            re[p++] = pat[k];
    }
     re[p++] = ')';
    re[p++] = '\0';
    CombinedRE* cre = malloc(sizeof(CombinedRE));
    cre->pattern = augmentRE(re);
    cre->patlen = strlen(cre->pattern);
    cre->ast = root;
    printf("Combined into: %s\n", cre->pattern);
    return cre;
}



typedef struct {
    int rule_id;
    int length;
} TKToken;

TKToken nextToken(DFA* dfa, const char* input) {
    int curr = 1;
    int last_accept = -1;
    int last_token_id = -1;
    int i = 0;
    for (; input[i] != '\0'; i++) {
        int next = -1;
        printf("Current State: %d, Input Symbol: %c\n", dfa->states[curr]->label, input[i]);
        for (Transition* it = dfa->dtrans[curr]; it != NULL; it = it->next) {
            if (input[i] == it->ch || it->ch == '.') {
                next = it->to;
                break;
            }
        }
        if (next == -1) {
            break;
        }
        curr = next;
        if (dfa->states[curr]->is_accepting) {
            last_accept = i+1;
            last_token_id = dfa->states[curr]->token_id;
        }
    }
    TKToken t;
    if (last_accept == -1) {
        t.length = 0;
        t.rule_id = -1;
    } else {
        t.length = last_accept;
        t.rule_id = last_token_id;
    }
    return t;
}

int main() {
    CombinedRE* cre = combine(nr);
    DFA dfa = re2dfa(cre->pattern, cre->ast);
    char* input = "print 1234 if goblin42 funccall()";
    char* p = input;

    while (*p != '\0') {
    // skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n') p++;

        TKToken t = nextToken(&dfa, p);
        if (t.rule_id == -1) {
            printf("Lex error near: %s\n", p);
            break;
        }

        printf("Got token ID %d, text: '", t.rule_id);
        for (int i = 0; i < t.length; i++) putchar(p[i]);
        printf("'\n");

        p += t.length;
    }
}