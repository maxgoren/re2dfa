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

clean:
	rm *.o
	rm re2dfa