#include "statequeue.h"

qnode* makeQueueNode(DFAState* state) {
    qnode* qn = malloc(sizeof(qnode));
    qn->state = state;
    qn->next = NULL;
    return qn;
}

void initQueue(StateQueue* q) {
    q->head = NULL;
    q->tail = NULL;
}

bool emptyQueue(StateQueue* q) {
    return q->head == NULL;
}

void enQueue(StateQueue* q, DFAState* state) {
    qnode* qn = makeQueueNode(state);
    if (emptyQueue(q)) {
        q->head = qn;
    } else {
        q->tail->next = qn;
    }
    q->tail = qn;
}

DFAState* peekQueue(StateQueue* q) {
    return q->head->state;
}

DFAState* deQueue(StateQueue* q) {
    DFAState* ret = peekQueue(q);
    qnode* tmp = q->head;
    q->head = q->head->next;
    tmp->next = NULL;
    free(tmp);
    return ret;
}