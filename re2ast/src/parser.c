#include "parser.h"




int appendCurrent(char* ns, char* str, int pos, int sp, char ch, bool prev_escaped, bool curr_escaped) {
    printf("Position: %d, Char: %c\n", pos, ch);
    if (!curr_escaped && !prev_escaped) {
        printf("1\n");
        ns[pos] = ch;
        for (int k = 0; k <= pos; k++) putchar(ns[k]);
        printf("\n");
        return 1;
    } else if (prev_escaped && !curr_escaped) {
        printf("2\n");
        if (str[sp-1] == '\\') {
            ns[pos] = '\\';
        }
        ns[pos+1] = ch;
        return 2;
    } else if (prev_escaped && curr_escaped) {
        printf("3\n");
        ns[pos] = ch;
        return 1;
    } else {
        printf("Not prev escaped, but curr escaped\n");
        printf("4\n");
        return 0;
    }
    return 0;
}

char* addConCat(char* str) {
    int n = 2*strlen(str);
    char *ns = malloc(sizeof(char)*n);
    int i = 0, j = 0;
    bool inccl = false;
    bool curr_escaped = false;
    bool prev_escaped = false;
    for (i = 0; str[i] != '\0'; i++) {
        char current = str[i];
        if (current == '\\') {
            curr_escaped = true;
        } else if (current == '[') {
            inccl = true;
        } else if (current == ']') {
            inccl = false;
        }
        
        int added = appendCurrent(ns, str, j, i, current, prev_escaped, curr_escaped);
        j += added;
        if ((!prev_escaped && curr_escaped)) {
            prev_escaped = curr_escaped;
            curr_escaped = false;
            continue;
        }
        if (!curr_escaped && (str[i] == '(' || str[i] == '|')) {
            prev_escaped = curr_escaped;
            curr_escaped = false;
            continue;
        }
        if (i+1 < strlen(str) && str[i+1] != '#') {
            char lookahead = str[i+1];
            printf("curr_escaped: %c, prev_escaped: %c, Curr: %c, lookahead: %c\n",  curr_escaped ? 'T':'F', prev_escaped ? 'T':'F', current, lookahead);
            if (!prev_escaped && (lookahead == '|' || lookahead == '*' || lookahead == '+' || lookahead == '?' || lookahead == ')')) {
                printf("Unescaped metachar, skipping concat.\n");
                continue;
            }
            if (!inccl && !curr_escaped && !(lookahead == '|' || lookahead == '*' || lookahead == '+' || lookahead == '?' || lookahead == ')') && ns[j-1] != '@')
                ns[j++] = '@';
            else if (!inccl && !prev_escaped && !curr_escaped) {
                 ns[j++] = '@';
            }
            //it was about this time I started _seriously_ reconsidering my use of the shunting yard algorithm
            //allllll of this uglyness would go away with a simple recursive descent parser. 
            //*sigh* sunk cost falacy strikes again.
        } else if (i+1 < strlen(str) && str[i+1] == '#') {
            printf("Next state is accept, adding concat.\n");
            ns[j++] = '@';
        }
        prev_escaped = curr_escaped;
        curr_escaped = false;
    }
    ns[j++] = '\0';
#ifdef DEBUG
    printf("In: %s\nOut: %s\n", str, ns);
#endif
    return ns;
}



bool leftAssoc(char c) {
    return c == '*' || c == '+' || c == '?' || c == '@' || c == '|';
}

int precedence(char c) {
    switch (c) {
        case '+': return 50;
        case '?': return 50;
        case '*': return 50;
        case '@': return 30;
        case '|': return 20;

        default:
            break;
    }
    return 10;
}

bool isOp(Token c) {
    switch (c.symbol) {
        case RE_STAR:
        case RE_PLUS:
        case RE_QUESTION: 
        case RE_CONCAT:
        case RE_OR: return true;
        default:
            break;
    }
    return false;
}

Token* in2post(Token* input) {
    int len = tokensLength(input);
    Token dummy;
    Token* opstack[2048];
    int sp = 0;
    Token* fixed = &dummy;
    int j = 0;
    for (Token* it = input; it != NULL; it = it->next) {
        if (it->symbol == RE_LPAREN)
            opstack[sp++] = it;
        else if (isOp(*it)) {
            if (sp > 0 && (precedence(it->ch) < precedence(opstack[sp-1]->ch) || (precedence(it->ch) == precedence(opstack[sp-1]->ch) && leftAssoc(it->ch)))) {
                fixed->next = opstack[--sp];
                fixed = fixed->next;
                opstack[sp++] = it;
            } else {
                opstack[sp++] = it;
            }
        } else if (it->symbol == RE_RPAREN) {
            while (sp > 0) {
                if (opstack[sp-1]->symbol == RE_LPAREN) {
                    sp--;
                    break;
                } else {
                    fixed->next = opstack[sp-1];
                    fixed = fixed->next;
                    sp--;
                }
            }
        } else {
            fixed->next = it;
            fixed = fixed->next;
        }
    }
    while (sp > 0) {
        if (opstack[sp-1]->symbol == RE_LPAREN) {
            sp--;
        } else {
            fixed->next = opstack[sp-1];
            fixed = fixed->next;
            sp--;
        }
    }
    fixed->next = NULL;
    return dummy.next;;
}

re_ast* makeTree(Token* tokens) {
    int len = tokensLength(tokens);
    re_ast* st[len];
    int sp = 0;
    for (Token* it = tokens; it != NULL; it = it->next) {
            if (isOp(*it)) {
                re_ast* n = makeNode(1, *it);
                switch (it->symbol) {
                    case RE_OR:
                    case RE_CONCAT:
                        if (sp < 2) {
                            fprintf(stderr, "ERROR: stack underflow for binary operator %c\n", it->symbol);
                            exit(EXIT_FAILURE);
                        }
                        n->right = (sp > 0) ? st[--sp]:NULL;
                        n->left = (sp > 0) ? st[--sp]:NULL;
                        break;
                    case RE_STAR: 
                        n->left = (sp > 0) ? st[--sp]:NULL;
                        break;
                    case RE_QUESTION: {
                        free(n);
                        n = makeNode(1, *makeToken(RE_OR, '|'));
                        n->left = (sp > 0) ? st[--sp]:NULL;
                        n->right = makeNode(2, *makeToken(RE_EPSILON, '&'));
                    } break;
                    case RE_PLUS: {
                        free(n);
                        n = makeNode(1, *makeToken(RE_CONCAT, '@'));
                        re_ast* operand = (sp > 0) ? st[--sp]:NULL;
                        n->left = operand;
                        n->right = makeNode(1, *makeToken(RE_STAR, '*'));
                        n->right->left = cloneTree(operand);
                    } break;
                    default: break;
                }
                st[sp++] = n;
            } else {
                re_ast* t = makeNode(2, *it);
                st[sp++] = t;
            }
    }
    return st[--sp];
}

re_ast* re2ast(char* regex) {
    /*
    char* explicitConcat = addConCat(regex);  <- insert explicit concat operators to regex string
    Token* tokenized = tokenize(explicitConcat); <- convert string to list of tokens
    Token* postfix = in2post(tokenized);        <- convert token list from infix to postfix form via shunting yard
    re_ast* ast = makeTree(postfix);         <- create AST from postfix expression
    */
    Token* tokens = in2post(tokenize(addConCat(regex)));
    re_ast* ast = makeTree(tokens);
    freeTokenStream(tokens);
    return ast;
}