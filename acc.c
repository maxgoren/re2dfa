#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

//step one:
//Not escaped? append p
//is escaped? append p+1;
//step two, havng appended what was appeneded, and looking at the _next_ token, should we also append an '@' ?

char* addConCat(char* str) {
    int n = 2*strlen(str);
    char *ns = malloc(sizeof(char)*n);
    int i = 0, j = 0;
    bool inccl = false;
    bool escaped = false;
    for (i = 0; str[i] != '\0'; i++) {
        printf("Current char: %c\n", str[i]);
        char c = str[i];
        if (c == '\\') {
            printf("Is escaped.\n");
            escaped = true;
        } else if (str[i] == '[') {
            inccl = true;
        } else if (str[i] == ']') {
            inccl = false;
        }
        char p = c; 
        if (!escaped) {
            printf("append %c \n", c);
            ns[j++] = c;
        } else {
            i++;
            ns[j++] = str[i];
            p = str[i];
        } 
        printf("Now: %c\n", p);
        if (p == '\0')
            break;
        bool next_escaped = false;
        if (p == '\\') {
            printf("Next is escaped too!.\n");
            next_escaped = true;
            p = str[i + 2]; // the real next token
            if (p == '\0') break;
        }
        if (p == '(' || p == '|' || p == '*' || p == '+' || p == '?' || p == ')' || p == '#') {  
            if (escaped && next_escaped) {
                ns[j++] = '@';
            } else if (inccl) {
                
            } else {
                ns[j++] = '@';
            }
        }
        escaped = false;
    }
    ns[j++] = '\0';
    printf("In: %s\n,", str);
    printf("Out: %s\n", ns);
    return ns;
}

int main(int argc, char* argv[]) {
    printf("%s\n", addConCat(argv[1]));
}