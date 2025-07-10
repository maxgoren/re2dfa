#ifndef intset_h
#define intset_h
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* members;
    int n;
    int maxN;
} Set;

void initSet(Set* set) {
    set->maxN = 255;
    set->n = 0;
    set->members = malloc(sizeof(int)*set->maxN);
}

Set* createSet() {
    Set* s = malloc(sizeof(Set));
    initSet(s);
    return s;
}

bool isSetEmpty(Set* s) {
    return s->n == 0;
}

int findSet(Set* set, int value) {
    int l = 0, r = set->n-1;
    while (l <= r) {
        int i = (l+r)/2;
        if (value < set->members[i]) r = i - 1;
        else if (value > set->members[i]) l = i + 1;
        else return i;
    }
    return -1;
}

void addSet(Set* set, int value) {
    int i = findSet(set, value);
    if (i == -1) {
        int j = 0;
        for (j = set->n; j > 0 && set->members[j-1] > value; j--) 
            set->members[j] = set->members[j-1];
        set->members[j] = value;
        set->n++;
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