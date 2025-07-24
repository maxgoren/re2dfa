#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "match.h"
#include "util.h"

int grep(char* pattern, char* filename) {
    char* readbuff = slurp_file(filename);
    re_ast** ast_node_table;
    re_ast* ast = re2ast(augmentRE(pattern));
    DFA dfa = re2dfa(pattern, ast, &ast_node_table);
    printAST(ast, 1);
    int line_num = 0;
    int match_count = 0;
    for (char *sp = readbuff; *sp; sp++) {
        if (*sp == '\n') {
            line_num++;
            sp++;
        }
        if (simulateDFA(dfa, readbuff, ast_node_table)) {
            printf("Match found on line %d\n",line_num);
            match_count++;
        }
        while (*sp && *sp++ != '\n');
    }
    printf("Read %d lines.\n", line_num);
    return match_count;
}

void grep_piped_data(char* pattern) {
    char buff[256];
    re_ast** ast_node_table;
    re_ast* ast = re2ast(augmentRE(pattern));
    DFA dfa = re2dfa(pattern, ast, &ast_node_table);
    //printAST(ast, 1);
    int line_num = 0;
    int match_count = 0;
    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        line_num++;
        if (simulateDFA(dfa, buff, ast_node_table)) {
            printf("%d: %s", line_num, buff);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <pattern> <filename> to search file for match\n", argv[0]);
        printf("       %s <pattern> to read from stdin\n", argv[0]);
        return 0;
    }
    if (argc < 3) {
        grep_piped_data(argv[1]);
    } else {
        int matches = grep(argv[1], argv[2]);
        printf("%d matches found\n", matches);
    } 
    return 0;
}
