#include "dfastate.h"

void initDState(DFAState* state, int statenum) {
    state->label = statenum;
    state->positions = NULL;
    state->is_accepting = false;
    state->token_id = -1;
}

DFAState* createState(int statenum, Set* positions) {
    DFAState* ns = malloc(sizeof(DFAState));
    initDState(ns, statenum);
    ns->positions = positions;
    return ns;
}