#ifndef intset_h
#define intset_h
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int* members;
    int n;
    int maxN;
} Set;

void initSet(Set* set, int maxsize);
Set* createSet(int size);
bool isSetEmpty(Set* s);
void setAdd(Set* set, int value);
int  setContains(Set* set, int value);
Set* setUnion(Set* a, Set* b);
bool setsEqual(Set* a, Set* b);
void printSet(Set* set);
Set* resizeSet(Set* set, int newsize);
Set* copySet(Set* a);
void freeSet(Set* set);
void pm_insert(Set* set, int value, int j);

#ifdef __cplusplus
}
#endif
#endif