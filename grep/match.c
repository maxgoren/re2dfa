#include <stdio.h>
#include <stdlib.h>
#include "match.h"


DFAState* move(DFA* dfa, DFAState* state, char ch, re_ast** ast_node_table) {
    DFAState* next = NULL;
    Transition* it = findTransition(dfa->dtrans[state->label], ch);
    if (it != NULL) {
        if (ch == it->ch || ast_node_table[state->label]->token.symbol == RE_PERIOD) {
            next = dfa->states[it->to];
        }
    } else if (ast_node_table[state->label]->token.symbol == RE_PERIOD) {
        it = findTransition(dfa->dtrans[state->label], '.');
        if (it != NULL) next = dfa->states[it->to];
    }    
    return next;
}

bool simulateDFA(DFA dfa, char* text, re_ast** ast_node_table) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", state->label, *sp);
#endif
        
    state = move(&dfa, state, *sp, ast_node_table);
        
    if (!state) {
#ifdef DEBUG
            printf("Out of transitions, No match.\n");
#endif
            return false;
        }
        if (state->is_accepting) {
            return true;
        }
    }
#ifdef DEBUG
    printf("Final State: %d\n", state->label);
#endif
    return state->is_accepting;
}


bool all_literals(char* text) {
    for (char *sp = text; *sp; sp++) {
        if (*sp == '(' || *sp == '[' || *sp == '|' || *sp == '*' || *sp == '+' || *sp == '?' || *sp == ']' || *sp == ')')
            if (*(sp-1) != '\\')
                return false;
    }
    return true;
}

char* build_skip(char* pattern, int len) {
    char* skip = (char*)malloc(sizeof(char)*len);
    skip[0] = -1;
    for (int i = 0, j = -1; i < len; i++, j++, skip[i] = j)
        while ((j >= 0) && (pattern[i] != pattern[j]))
            j = skip[j];
    return skip;
}

bool kmp_search(char* pattern, char* text) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    char* skip = build_skip(pattern, pattern_len);
    int i, j;
    for (i = 0, j = 0; i < text_len && j < pattern_len; i++, j++) {
        while ((j >= 0) && (text[i] != pattern[j]))
            j = skip[j];
    }
    return j == pattern_len;
}