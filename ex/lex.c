#include <stdio.h>
#include <stdlib.h>
#include "../re2ast/src/ast.h"
#include "../re2ast/src/parser.h"
#include "../src/re_to_dfa.h"

enum TKSymbol {
    TK_NUM, TK_REAL, TK_ID,
    TK_ADD, TK_SUB, TK_MUL, TK_DIV, 
    TK_MOD, TK_ASSIGN, TK_LAMBDA,
    TK_PRINT, TK_IF, TK_ELSE, TK_WHILE, 
    TK_FOREACH, TK_IN, TK_FUNC,
    TK_MAP, TK_FILTER, TK_REDUCE, TK_SORT, 
    TK_PUSH, TK_APPEND, TK_SIZE, 
    TK_EMPTY, TK_REVERSE, TK_FIRST, TK_REST, 
    TK_BLESS, TK_STRUCT, TK_TYPEOF, TK_BREAK, 
    TK_FOPEN, TK_RETURN, TK_LET, 
    TK_TRUE, TK_FALSE, TK_NIL,
    
    TK_COMA, TK_SEMI,TK_PERIOD, TK_LP, TK_RP, TK_LC, TK_RC, TK_LB, TK_RB,
    TK_EOF
};


typedef struct {
    char* pattern;
    enum TKSymbol token;
} TokenRule;


int nr = 49;

TokenRule rules[] = {
    {"(\\d+)\\.(\\d+)",TK_REAL},
    {"[0-9]+", TK_NUM},
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
    {"lambda", TK_LAMBDA},
    {"filter", TK_FILTER},
    {"reduce", TK_REDUCE},
    {"return", TK_RETURN},
    {"struct", TK_STRUCT},
    {"typeOf", TK_TYPEOF},
    {"append", TK_APPEND},
    {"foreach", TK_FOREACH},
    {"reverse", TK_REVERSE},
    {"[A-Za-z]+", TK_ID},
    {"\\(", TK_LP},
    {"\\)", TK_RP},
    {"\\[", TK_LB},
    {"\\]", TK_RB},
    {"{", TK_LC},
    {"}", TK_RC},
    {",", TK_COMA},
    {";", TK_SEMI},
    {"\\+", TK_ADD},
    {"-", TK_SUB},
    {"\\*", TK_MUL},
    {"/", TK_DIV},
    {"%", TK_MOD},
    {":=", TK_ASSIGN},
    {"&\\(", TK_LAMBDA},
    {"\\.", TK_PERIOD}
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
    cre->pattern = re; //augmentRE(re);
    cre->patlen = strlen(cre->pattern);
    cre->ast = root;
    printf("Combined into: %s\n", cre->pattern);
    return cre;
}



typedef struct {
    int rule_id;
    int length;
    char* text;
} TKToken;

TKToken* makeTKToken(int rid, int len) {
    TKToken* tkt = malloc(sizeof(TKToken));
    tkt->rule_id = rid;
    tkt->length = len;
    if (rid != -1 && len > 0) {
        tkt->text = malloc(sizeof(char)*(len+1));
    }
    return tkt;
}

//Classic Maximal-much: Prefer longest match, and equal length matches are chosen by priority
//with priority being order in the rule list.
TKToken* nextToken(DFA* dfa, const char* input) {
    int curr = 1;
    int last_accept = -1;
    int last_token_id = -1;
    int i = 0;
    for (; input[i] != '\0'; i++) {
        int next = -1;
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", dfa->states[curr]->label, input[i]);
#endif
        for (Transition* it = dfa->dtrans[curr]; it != NULL; it = it->next) {
            if (input[i] == it->ch || ast_node_table[curr]->token.symbol == RE_PERIOD) {
                next = it->to;
                break;
            }
        }
        if (next == -1) {
            break;
        }
        curr = next;
        if (dfa->states[curr]->is_accepting) { 
            if (last_accept < i+1) {
                last_accept = i+1;
                last_token_id = dfa->states[curr]->token_id;
            } else {
                if (dfa->states[curr]->token_id < dfa->states[last_accept]->token_id) {
                    last_accept = i+1;
                    last_token_id = dfa->states[curr]->token_id;
                }
            }
        }
    }
    TKToken* t;
    if (last_accept == -1) {
        t = makeTKToken(-1, 0);
    } else {
        t = makeTKToken(last_token_id, last_accept);
    }
    return t;
}

typedef struct TKTokenListNode_ {
    TKToken* token;
    struct TKTokenListNode_* next;
} TKTokenListNode;

TKTokenListNode* makeTokenListNode(TKToken* token) {
    TKTokenListNode* tn = malloc(sizeof(TKTokenListNode));
    tn->token = token;
    tn->next = NULL;
    return tn;
}

TKTokenListNode* lex_input(char* input) {
    CombinedRE* cre = combine(nr);
    DFA dfa = re2dfa(cre->pattern, cre->ast);
    char* p = input;
    TKTokenListNode dummy;
    TKTokenListNode* thead = &dummy; 
    while (*p != '\0') {
    // skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n') p++;

        TKToken* t = nextToken(&dfa, p);
        if (t->rule_id == -1) {
            printf("Lex error near: %s\n", p);
            break;
        }
        thead->next = makeTokenListNode(t);
        thead = thead->next;
        printf("Got token ID %d, text: '", t->rule_id);
        int i;
        for (i = 0; i < t->length; i++) {
            t->text[i] = p[i];
            putchar(p[i]);
        }
        t->text[i++] = '\0';
        printf("'\n");
        p += t->length;
    }
    for (TKTokenListNode* it = dummy.next; it != NULL; it = it->next) {
        printf("<%d, %s, %s> \n", rules[it->token->rule_id].token, rules[it->token->rule_id].pattern, it->token->text);
    }
}

int main(int argc, char* argv[]) {
    char* input = "print 1234 if (goblin42) { funccall() { return 13.66; } else { print arr[12]; } }";
    lex_input(input);
    return 0;
}