#include "parser.h"
#include "followpos.h"
#include "dfa.h"

bool simulateDFA(DFA dfa, char* text) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
        printf("Current State: %d, Input Symbol: %c\n", state->label, *sp);
        DFAState* next = NULL;
        for (Transition* it = dfa.dtrans[state->label]; it != NULL; it = it->next) {
            if (*sp == it->ch) {
                next = dfa.states[it->to];
                break;
            }
        }
        if (next == NULL) {
            printf("Out of transitions, No match.\n");
            return false;
        } else state = next;
    }
    printf("Final State: %d\n", state->label);
    if (state->is_accepting) {
        return true;
    }
    return false;
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
    printf("AST: \n");
    print(ast, 1); 
    printf("Followpos Table: \n");   
    for (int i = 1; i < numleaves+1; i++) {
        printf("%d: ", i);
        printSet(followpos[i]);
    }
    printf("DFA: \n");
    printDFA(dfa);
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