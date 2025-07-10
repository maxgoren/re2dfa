#include "parser.h"
#include "followpos.h"
#include "dfa.h"

void simulate(DFA dfa, char* text) {
    DFAState* state = dfa.states[1];
    for (char *sp = text; *sp != '\0'; sp++) {
        printf("State: %d, Input: %c\n", state->label, *sp);
        DFAState* next = NULL;
        //Dont ask me why I implemented Dtrans as a linked list...
        for (Transition* it = dfa.dtrans[state->label]; it != NULL; it = it->next) {
            if (*sp == it->ch) {
                next = dfa.states[it->to];
                break;
            }
        }
        if (next == NULL) {
            printf("Out of transitions, No match.\n");
            return;
        } else state = next;
    }
    printf("Final State: %d\n", state->label);
    if (state->is_accepting) {
        printf("Match Found!\n");
    }
}

char* augment(char* orig) {
    char* fixed = malloc(sizeof(char)*strlen(orig)+3);
    sprintf(fixed, "(%s)#", orig);
    return fixed;
}

void straightToDFA(char* re, char *text) {
    re = augment(re);
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
    simulate(dfa, text);
}

int main(int argc, char* argv[]) {

    straightToDFA(argv[1], argv[2]);
    return 0;
}