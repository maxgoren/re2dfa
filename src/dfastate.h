#ifndef dfastate_h
#define dfastate_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "intset.h"

typedef struct {
    int label;
    int token_id;
    bool is_accepting;
    Set* positions;
} DFAState;

void      initDState(DFAState* state, int statenum);
DFAState* createState(int statenum, Set* positions);


#ifdef __cplusplus
}
#endif
#endif