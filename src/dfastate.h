#ifndef dfastate_h
#define dfastate_h
#include <stdlib.h>
#include <stdio.h>
#include "intset.h"

typedef struct {
    int label;
    Set* positions;
    bool is_accepting;
} DFAState;

void initDState(DFAState* state, int statenum) {
    state->label = statenum;
    state->positions = malloc(sizeof(Set));
    initSet(state->positions);
    state->is_accepting = false;
}

DFAState* createState(int statenum, Set* positions) {
    DFAState* ns = malloc(sizeof(DFAState));
    initDState(ns, statenum);
    ns->positions = positions;
    return ns;
}

typedef struct qnode_ {
    DFAState* state;
    struct qnode_* next;
} qnode;

qnode* makeQueueNode(DFAState* state) {
    qnode* qn = malloc(sizeof(qnode));
    qn->state = state;
    qn->next = NULL;
    return qn;
}

typedef struct {
    qnode* head;
    qnode* tail;
    int count;
} StateQueue;

void initQueue(StateQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

bool emptyQueue(StateQueue* q) {
    return q->count == 0;
}

void enQueue(StateQueue* q, DFAState* state) {
    qnode* qn = makeQueueNode(state);
    if (emptyQueue(q)) {
        q->head = qn;
    } else {
        q->tail->next = qn;
    }
    q->tail = qn;
    q->count++;
}

DFAState* peekQueue(StateQueue* q) {
    return q->head->state;
}

DFAState* deQueue(StateQueue* q) {
    DFAState* ret = peekQueue(q);
    qnode* tmp = q->head;
    q->head = q->head->next;
    tmp->next = NULL; q->count--;
    free(tmp);
    return ret;
}

#endif