#include "followpos.h"

int isLeaf(re_ast* node) {
    return (node->left == NULL && node->right == NULL);
}

void number_nodes(re_ast* node, int pass) {
    if (node != NULL) {
        number_nodes(node->left, pass);
        number_nodes(node->right, pass);
        if (isLeaf(node)) {
            if (pass == 1)
                node->number = ++numleaves;
        } else {
            if (pass == 2)
                node->number = ++nonleaves;
        }
    }
}

bool nullable(re_ast* node) {
    if (node == NULL)
        return false;
    if (isLeaf(node) && node->token.symbol == RE_EPSILON)
        return true;
    if (isLeaf(node) && node->token.symbol == RE_CHAR)
        return false;
    if (node->token.symbol == RE_OR)
        return nullable(node->left) || nullable(node->right);
    if (node->token.symbol == RE_CONCAT) {
        return nullable(node->left) && nullable(node->right);
    }
    if (node->token.symbol == RE_STAR)
        return true;
    return false;
}
int numleaves;
int nonleaves;
Set** firstpos;
Set** lastpos;
Set** followpos;

void initAttributeSets() {
    firstpos = malloc(sizeof(Set*)*nonleaves+1);
    lastpos = malloc(sizeof(Set*)*nonleaves+1);
    followpos = malloc(sizeof(Set*)*nonleaves+1);
    for (int i = 0; i < nonleaves+1; i++) {
        firstpos[i] = malloc(sizeof(Set));
        initSet(firstpos[i], nonleaves+1);
        lastpos[i] = malloc(sizeof(Set));
        initSet(lastpos[i], nonleaves+1);
        followpos[i] = malloc(sizeof(Set));
        initSet(followpos[i], nonleaves+1);
    }
}

void mergeFirstPos(re_ast* node) {
    if (node->left) {
        firstpos[node->number] = setUnion(firstpos[node->number], firstpos[node->left->number]);
    }
    if (node->right) {
        firstpos[node->number] = setUnion(firstpos[node->number], firstpos[node->right->number]);
    }
}

void mergeLastPos(re_ast* node) {
    if (node->left) {
        lastpos[node->number] = setUnion(lastpos[node->number], lastpos[node->left->number]);
    }
    if (node->right) {
        lastpos[node->number] = setUnion(lastpos[node->number], lastpos[node->right->number]);
    }
}

/*

Firstpos:
Identifies the set of positions (leaf nodes) that can be the first symbol in a string derived from that node.
Lastpos:
Identifies the set of positions that can be the last symbol in a string derived from that node.

*/

void calcFirstandLastPos(re_ast* node) {
    if (node != NULL) {
        calcFirstandLastPos(node->left);
        calcFirstandLastPos(node->right);
        if (isLeaf(node)) {
            addSet(firstpos[node->number], node->number);
            addSet(lastpos[node->number], node->number);
        } else if (node->token.symbol == RE_OR) {
            mergeFirstPos(node);
            mergeLastPos(node);
        } else if (node->token.symbol == RE_CONCAT) {
            if (nullable(node->left)) {
                mergeFirstPos(node);
            } else if (node->left) {
                firstpos[node->number] = setUnion(firstpos[node->number], firstpos[node->left->number]);
            }
            if (nullable(node->right)) {
                mergeLastPos(node);
            } else if (node->right) {
                lastpos[node->number] = setUnion(lastpos[node->number], lastpos[node->right->number]);
            }
        } else if (node->token.symbol == RE_STAR && node->left) {
            firstpos[node->number] = setUnion(firstpos[node->number], firstpos[node->left->number]);
            lastpos[node->number] = setUnion(lastpos[node->number], lastpos[node->left->number]);
        }
    }
}

/*
Followpos:
Defines how positions relate to each other during DFA state transitions. It determines which positions can follow a given position in a string. 
*/

void calcFollowPos(re_ast* node) {
    if (node == NULL)
        return;
    if (node->token.symbol == RE_CONCAT) {
        if (!node->left || !node->right) {
            printf("ERROR: RE_CONCAT node %d missing children (left=%p, right=%p)\n",
                   node->number, (void*)node->left, (void*)node->right);
            return;
        }
    }
    calcFollowPos(node->left);
    calcFollowPos(node->right);
    if (node->token.symbol == RE_CONCAT) {
        if (node->left == NULL || node->right == NULL)
            return;
        for (int i = 0; i < lastpos[node->left->number]->n; i++) {
            int t = lastpos[node->left->number]->members[i];
            followpos[t] = setUnion(followpos[t], firstpos[node->right->number]);
        }
    }
    if (node->token.symbol == RE_STAR) {
        for (int i = 0; i < lastpos[node->number]->n; i++) {
            int t = lastpos[node->number]->members[i];
            followpos[t] = setUnion(followpos[t], firstpos[node->number]);
        }
    }
}

void computeFollowPos(re_ast* node) {
    numleaves = 0;
    number_nodes(node, 1);
    nonleaves = numleaves;
    number_nodes(node, 2);
    initAttributeSets();
    calcFirstandLastPos(node);
    calcFollowPos(node);
}