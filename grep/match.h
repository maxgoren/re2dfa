#ifndef match_h
#define match_h
#ifdef __cplusplus
extern "C" {
#endif
#include "../src/re_to_dfa.h"

bool simulateDFA(DFA dfa, char* text, re_ast** ast_node_table);
bool all_literals(char* text);
char* build_skip(char* pattern, int len);
bool kmp_search(char* pattern, char* text);



#ifdef __cplusplus
}
#endif
#endif