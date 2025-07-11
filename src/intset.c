#include "intset.h"

void initSet(Set* set, int maxsize) {
    set->maxN = maxsize;
    set->n = 0;
    set->members = malloc(sizeof(int)*maxsize);
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

int setContains(Set* set, int value) {
    for (int i = 0; i < set->n; i++) {
        if (set->members[i] == value)
            return i;
    }
    return -1;
}

void setAdd(Set* set, int value) {
    int i = setContains(set, value);
    if (i == -1) {
        set->members[set->n++] = value;
    }
}

Set* setUnion(Set* a, Set* b) {
    for (int i = 0; i < b->n; i++) {
        setAdd(a, b->members[i]);
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