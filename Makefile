re_2_dfa:
	gcc -c ./src/ast.c
	gcc -c ./src/parser.c
	gcc -c ./src/tokens.c
	gcc -c ./src/intset.c
	gcc -c ./src/followpos.c
	gcc -c ./src/dfastate.c
	gcc -c ./src/dfa.c
	gcc -c ./src/re_to_dfa.c
	gcc *.o -o re2dfa

debug:
	gcc -c -DDEBUG ./src/ast.c
	gcc -c -DDEBUG ./src/parser.c
	gcc -c -DDEBUG ./src/tokens.c
	gcc -c -DDEBUG ./src/intset.c
	gcc -c -DDEBUG ./src/followpos.c
	gcc -c -DDEBUG ./src/dfastate.c
	gcc -c -DDEBUG ./src/dfa.c
	gcc -c -DDEBUG ./src/re_to_dfa.c
	gcc -DDEBUG *.o -o re2dfa

clean:
	rm *.o
	rm re2dfa