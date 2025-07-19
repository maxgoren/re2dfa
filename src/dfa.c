#include "dfa.h"
#include "statequeue.h"
Transition* makeTransition(int from, int to, char ch) {
    Transition* nt = malloc(sizeof(Transition));
    nt->from = from;
    nt->to = to;
    nt->ch = ch;
    nt->left = NULL;
    nt->right = NULL;
    return nt;
}

void initDFA(DFA* dfa, int numstates) {
    dfa->numstates = 0;
    dfa->states = malloc(sizeof(DFAState*)*numstates+1);
    dfa->dtrans = malloc(sizeof(Transition*)*numstates+1);
    for (int i = 0; i < numstates;i++) {
        dfa->dtrans[i] = NULL;
    }
}

void pm_addState(DFA* dfa, DFAState* state) {
    dfa->states[state->label] = state;
}

bool findInCharClass(char* ccl, char input_symbol) {
    for (char *sp = ccl; *sp; sp++) {
        if (*sp == input_symbol)
            return true;
    }
    return false;
}

Set* pm_calculateNextStatesPositions(DFAState* curr_state, char input_symbol) {
    Set* next_states = createSet(nonleaves+1);
    for (int i = 0; i < curr_state->positions->n; i++) {
        int t = curr_state->positions->members[i];
#ifdef DEBUG
        if (ast_node_table[t]->token.symbol == RE_CCL) {
            printf("%c in %s?\n", input_symbol, ast_node_table[t]->token.ccl);
        } else {
            printf("%c == %c?\n",  ast_node_table[t]->token.ch, input_symbol);
        }
#endif
        if ((ast_node_table[t]->token.symbol == RE_CCL && findInCharClass(ast_node_table[t]->token.ccl, input_symbol)) || 
            (ast_node_table[t]->token.symbol != RE_CCL && (ast_node_table[t]->token.ch == input_symbol || ast_node_table[t]->token.symbol == RE_PERIOD))) {
            next_states = setUnion(next_states, ast_node_table[t]->followpos);
        }
    }
    return next_states;
}

int pm_findStateByPositions(DFA* dfa, Set* next_states) {
    for (int i = 1; i <= dfa->numstates; i++) {
        if (setsEqual(next_states, dfa->states[i]->positions)) {
            return i;
        }
    }
    return -1;
}

int pm_nextStateNum(DFA* dfa) {
    return ++dfa->numstates;
}

DFA buildDFA(re_ast* ast, char* re) {
    int found;
    int max_table_size = (strlen(re)+5);
    char* alphabet = malloc(sizeof(char)*max_table_size); 
    if (alphabet == NULL) {
        printf("Malloc failed :(\n");
        exit(EXIT_FAILURE);
    }
    DFA dfa;
    StateQueue fq;
    pm_initAlphabet(ast, alphabet, re);
    initDFA(&dfa,numleaves+1);
    pm_addState(&dfa, createState(pm_nextStateNum(&dfa), copySet(ast->firstpos)));

    initQueue(&fq);
    enQueue(&fq,  dfa.states[1]);
    while (!emptyQueue(&fq)) {
        DFAState* curr_state = deQueue(&fq);
        for (char *input_symbol = alphabet; *input_symbol; input_symbol++) {
#ifdef DEBUG
            printf("Input Symbol: %c\n", *input_symbol);
#endif
            Set* next_states = pm_calculateNextStatesPositions(curr_state, *input_symbol);
            if (!isSetEmpty(next_states)) {
                if ((found = pm_findStateByPositions(&dfa, next_states)) > -1) {
                    dfa.dtrans[curr_state->label] = addTransition(dfa.dtrans[curr_state->label], curr_state->label, dfa.states[found]->label, *input_symbol);
                    freeSet(next_states);
#ifdef DEBUG                    
                    printf("State Already Exists, Adding Transition:  %d - (%c) - %d\n", curr_state->label, *input_symbol, dfa.states[found]->label);
#endif
                } else {
                    DFAState* new_state = createState(pm_nextStateNum(&dfa), copySet(next_states)); 
                    pm_addState(&dfa, new_state);
                    dfa.dtrans[curr_state->label] = addTransition(dfa.dtrans[curr_state->label], curr_state->label, new_state->label, *input_symbol);
                    enQueue(&fq, new_state);
#ifdef DEBUG
                    printf("Adding new state: %d, Transition: %d - (%c) - %d\n", new_state->label, curr_state->label, *input_symbol, new_state->label);
#endif
                }
            } else {
                free(next_states);
            }
        }
    }
    for (int i = 1; i <= dfa.numstates; i++) {
        dfa.states[i] = pm_markAcceptState(dfa.states[i]);
    }
    return dfa;
}

DFAState* pm_markAcceptState(DFAState* next_state) {
    for (int j = 0; j < next_state->positions->n; j++) {
        int pos = next_state->positions->members[j];
        if (ast_node_table[pos]->token.ch == '#')  {      
            next_state->is_accepting = true; 
            int leaf_id = ast_node_table[pos]->tk_token_id;
            if (next_state->token_id == -1 || leaf_id < next_state->token_id) {
                next_state->token_id = leaf_id;
            }
        }
    }
    return next_state;
}

void printDFA(DFA dfa) {
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: ", i);
        printSet(dfa.states[i]->positions);
    }
    int mintc = 0, maxtc = 0, ttc = 0;
    for (int i = 1; i <= dfa.numstates; i++) {
        printf("%d: \n", i);
        int tc = 0;
        Transition* it = dfa.dtrans[i];
        Transition* st[255];
        int stsp = 0;
        st[++stsp] = it; 
        while (stsp > 0) {
            it = st[stsp--];
            if (it != NULL) {
                printf("{%d ->(%c)-> %d} ", it->from, it->ch, it->to);
                tc++;
                st[++stsp] = it->right;
                st[++stsp] = it->left; 
            }
        }
        printf("\n");
        if (mintc == 0 || tc < mintc) mintc = tc;
        if (maxtc == 0 || tc > maxtc) maxtc = tc;
        ttc += tc;
        printf("\nTransition Count: %d\n", tc);
    }
    printf("Lowest transition count: %d\n", mintc);
    printf("Highest Trnaisiton count: %d\n", maxtc);
    printf("Num states: %d\n",dfa.numstates);
    printf("Total transitions: %d\n", ttc);
    printf("Possible Transitions: %d\n",(dfa.numstates+1)*128);
}

int pm_symbolIsInAlphabet(char* str, int n, char c) {
    for (int i = 0; i < n; i++) {
        if (str[i] == c)
            return i;
    }
    return -1;
}


void pm_initAlphabet(re_ast* ast, char* alphabet,  char* re) {
    int k = 0;
#ifdef DEBUG
    printf("Getting alphabet from %s\n", re);
#endif
    for (int i = 0; re[i]; i++) {
        if (re[i] != '#' && (is_char(re[i]) || is_digit(re[i]) || is_special(re[i])) && pm_symbolIsInAlphabet(alphabet, k, re[i]) == -1)
            alphabet[k++] = re[i];
    }
#ifdef DEBUG
    printf("Alphabet: %s \n", alphabet);
#endif
}
