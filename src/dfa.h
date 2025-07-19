#ifndef dfa_h
#define dfa_h
#include <stdlib.h>
#include <stdio.h>
#include "intset.h"
#include "dfastate.h"
#include "followpos.h"
#include "transition.h"
#include "statequeue.h"
#include "../re2ast/src/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    DFAState** states;
    Transition** dtrans;
    int numstates;
} DFA;

//public API
void initDFA(DFA* dfa, int numstates);
DFA buildDFA(re_ast* ast, char* re);
void printDFA(DFA dfa);


//methods for internal use
void pm_initAlphabet(re_ast* ast, char* alphabet, char* re);
void pm_addState(DFA* dfa, DFAState* state);
int  pm_nextStateNum(DFA* dfa);
Set* pm_calculateNextStatesPositions(DFAState* curr_state, char input_symbol);
int  pm_findStateByPositions(DFA* dfa, Set* next_states);
int  pm_symbolIsInAlphabet(char* str, int n, char c);
DFAState* pm_markAcceptState(DFAState* state);

#ifdef __cplusplus
}
#endif

#endif