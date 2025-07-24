re_2_dfa_grep:
	gcc -c ./re2ast/src/ast.c
	gcc -c ./re2ast/src/parser.c
	gcc -c ./re2ast/src/tokens.c
	gcc -c ./src/intset.c
	gcc -c ./src/followpos.c
	gcc -c ./src/dfastate.c
	gcc -c ./src/dfa.c
	gcc -c ./src/statequeue.c
	gcc -c ./src/re_to_dfa.c
	gcc -c ./src/transition.c
	gcc -c ./grep/match.c
	gcc -c ./grep/grep.c
	gcc -c ./grep/util.c
	gcc *.o -o dfagrep

debug:
	gcc -g -c -DDEBUG ./re2ast/src/ast.c
	gcc -g -c -DDEBUG ./re2ast/src/parser.c
	gcc -g -c -DDEBUG ./re2ast/src/tokens.c
	gcc -g -c -DDEBUG ./src/intset.c
	gcc -g -c -DDEBUG ./src/followpos.c
	gcc -g -c -DDEBUG ./src/dfastate.c
	gcc -g -c -DDEBUG ./src/dfa.c
	gcc -g -c -DDEBUG ./src/statequeue.c
	gcc -g -c -DDEBUG ./src/transition.c
	gcc -g -c -DDEBUG ./src/re_to_dfa.c
	gcc -g -c -DDEBUG ./grep/match.c
	gcc -g -c -DDEBUG ./grep/grep.c
	gcc -g -c -DDEBUG ./grep/util.c
	gcc -g -DDEBUG *.o -o dfagrep-debug

lex_example:
	gcc -c -DDEBUG ./re2ast/src/ast.c 
	gcc -c -DDEBUG ./re2ast/src/tokens.c 
	gcc -c -DDEBUG ./re2ast/src/parser.c 
	gcc -c -DDEBUG ./src/statequeue.c
	gcc -c -DDEBUG ./src/followpos.c 
	gcc -c -DDEBUG ./src/dfa.c 
	gcc -c -DDEBUG ./src/dfastate.c 
	gcc -c -DDEBUG ./src/intset.c 
	gcc -c -DDEBUG ./src/re_to_dfa.c 
	gcc -c -DDEBUG ./lex/lex.c 
	gcc -c ./grep/util.c
	gcc *.o -o lex_ex

clean:
	rm *.o
