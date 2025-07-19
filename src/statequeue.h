#ifndef statequeue_h
#define statequeue_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "dfastate.h"

typedef struct qnode_ {
    DFAState* state;
    struct qnode_* next;
} qnode;

typedef struct {
    qnode* head;
    qnode* tail;
    int count;
} StateQueue;

//FIFO Queue Operations
void      initQueue(StateQueue* q);
bool      emptyQueue(StateQueue* q);
void      enQueue(StateQueue* q, DFAState* state);
DFAState* deQueue(StateQueue* q);
DFAState* peekQueue(StateQueue* q);
qnode*    makeQueueNode(DFAState* state);

#ifdef __cplusplus
}
#endif
#endif