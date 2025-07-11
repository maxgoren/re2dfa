#ifndef dfa_h
#define dfa_h
#include <stdlib.h>
#include <stdio.h>
#include "intset.h"
#include "dfastate.h"
#include "followpos.h"

typedef struct Transition_ {
    int from;
    char ch;
    int to;
    struct Transition_* next;
} Transition;

typedef struct {
    DFAState** states;
    Transition** dtrans;
    int numstates;
} DFA;


Transition* makeTransition(int from, int to, char ch, Transition* set);
void initDFA(DFA* dfa, int numstates);
void addState(DFA* dfa, DFAState* state);
Transition* addTransition(Transition* trans, int from, int to, char ch);
Set* calculateNextStatesPositions(DFAState* curr_state, char input_symbol, char* re, char** ccl, enum RESymbol* posn_type);
int findStateByPositions(DFA* dfa, Set* next_states);
void initAlphabetAndPositions(re_ast* ast, char* alphabet, char* posns, char** ccl, enum RESymbol* posn_type, char* re);
int symbolIsInAlphabet(char* str, int n, char c);
int nextStateNum(DFA* dfa);
DFA buildDFA(re_ast* ast, char* re);
void printDFA(DFA dfa);



#endif