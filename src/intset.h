#ifndef intset_h
#define intset_h
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* members;
    int n;
    int maxN;
} Set;

void initSet(Set* set, int maxsize) {
    set->maxN = maxsize;
    set->n = 0;
    set->members = malloc(sizeof(int)*set->maxN);
}

Set* createSet(int size) {
    Set* s = malloc(sizeof(Set));
    initSet(s, size);
    return s;
}

void freeSet(Set* set) {
    if (set == NULL)
        return;
    free(set->members);
    free(set);
}

Set* resizeSet(Set* set, int newsize) {
    int* nm = malloc(sizeof(int)*newsize);
    for (int i = 0; i < set->n; i++) {
        nm[i] = set->members[i];
    }
    free(set->members);
    set->members = nm;
    set->maxN = newsize;
    return set;
}

Set* copySet(Set* a) {
    Set* copy = createSet(a->maxN);
    for (int i = 0; i < a->n; i++) {
        copy->members[copy->n++] = a->members[i];
    }
    return copy;
}

bool isSetEmpty(Set* s) {
    return s->n == 0;
}

int findSet(Set* set, int value) {
    for (int i = 0; i < set->n; i++) {
        if (set->members[i] == value)
            return i;
    }
    return -1;
}

void addSet(Set* set, int value) {
    int i = findSet(set, value);
    if (i == -1) {
        set->members[set->n++] = value;
    }
}

Set* setUnion(Set* a, Set* b) {
    for (int i = 0; i < b->n; i++) {
        addSet(a, b->members[i]);
    }
    return a;
}

bool setsEqual(Set* a, Set* b) {
    int i = 0, j = 0;
    while (i < a->n && j < b->n) {
        if (a->members[i] == b->members[j]) {
            i++; j++;
        } else return false;
    }
    return i == a->n && j == b->n;
}

void printSet(Set* set) {
    for (int i = 0; i < set->n; i++) {
        printf("%d ", set->members[i]);
    }
    printf("\n");
}

#endif