re_2_dfa:
	gcc -c ./re2ast/src/ast.c
	gcc -c ./re2ast/src/parser.c
	gcc -c ./re2ast/src/tokens.c
	gcc -c ./src/intset.c
	gcc -c ./src/followpos.c
	gcc -c ./src/dfastate.c
	gcc -c ./src/dfa.c
	gcc -c ./src/re_to_dfa.c
	gcc -c ./src/match.c
	gcc *.o -o matchdfa

debug:
	gcc -c ./re2ast/src/ast.c
	gcc -c ./re2ast/src/parser.c
	gcc -c ./re2ast/src/tokens.c
	gcc -c -DDEBUG ./src/intset.c
	gcc -c -DDEBUG ./src/followpos.c
	gcc -c -DDEBUG ./src/dfastate.c
	gcc -c -DDEBUG ./src/dfa.c
	gcc -c -DDEBUG ./src/re_to_dfa.c
	gcc -c -DDEBUG ./src/match.c
	gcc -DDEBUG *.o -o matchdfa

clean:
	rm *.o