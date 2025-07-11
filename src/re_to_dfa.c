#include "parser.h"
#include "followpos.h"
#include "dfa.h"

bool simulateDFA(DFA dfa, char* text) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
#ifdef DEBUG
        printf("Current State: %d, Input Symbol: %c\n", state->label, *sp);
#endif
        DFAState* next = NULL;
        for (Transition* it = dfa.dtrans[state->label]; it != NULL; it = it->next) {
            if (*sp == it->ch || it->ch == '.') {
                next = dfa.states[it->to];
                break;
            }
        }
        if (!next) {
#ifdef DEBUG
            printf("Out of transitions, No match.\n");
#endif
            return false;
        }
        state = next;
    }
#ifdef DEBUG
    printf("Final State: %d\n", state->label);
#endif
    return state->is_accepting;
}

char* augmentRE(char* orig) {
    char* fixed = malloc(sizeof(char)*strlen(orig)+3);
    sprintf(fixed, "(%s)#", orig);
    return fixed;
}

bool matchDFA(char* re, char *text) {
    re = augmentRE(re);
    re_ast* ast = re2ast(re);
    computeFollowPos(ast);
    DFA dfa = buildDFA(ast, toString(in2post(tokenize(re))));
#ifdef DEBUG
    printf("AST: \n");
    printAST(ast, 1); 
    printf("Followpos Table: \n");   
    for (int i = 1; i < numleaves+1; i++) {
        printf("%d: ", i);
        printSet(followpos[i]);
    }
    printf("DFA: \n");
    printDFA(dfa);
#endif
    return simulateDFA(dfa, text);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <re> <text>\n", argv[0]);
        return 0;
    }
    if (matchDFA(argv[1], argv[2])) {
        printf("Match found.\n");
    }
    return 0;
}