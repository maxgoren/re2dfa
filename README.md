 # re2dfa
Constructs a DFA from a regular expression using
the direct method outlined in the red dragon book.

The algorithm skips the need to create a separate NFA to perform subset construction on,
instead outputting a DFA directly by constructing firstpos, lastpos, and followpos
sets for each position in the RE string by performing DFS traversals on
the augmented AST.

 Dstates is constructed in the usual fashion, with each DFAState containing a set of positions.
 Dtrans is an array of AVL Trees instead of an 2D array. 
 
 ## Why is Dtrans stored as AVL trees?
 
 This is a design consideration as the intended use is for a lexer which will construct the DFA once and use it many times. 
 For owlscripts grammar, There are 171 states, with 8,369 transitions. If Dtrans was implemented as a 2d array, it would be 62% empty.
 seeing as the state with the most transitions has 87, a worst case lookup will take 5 or 6 comparisons. On average, 2 or 3. To me, that
 is more than an acceptable trade off from O(1) lookup but wasting 62% of allocated memory.

# Aho, Sethi, Ullman Direct DFA Construction

## Steps

1) A Regular Expression is Augmented with a special "END" marker - '#'.
    re   ->  (re)#

2) An AST is then built for the augmented re.

3) The nodes are numbered, and using the rules for nullable(node) we calculate the following sets:

nullable(node): 

      returns true for nodes which can represent the empty set (epsilon)

Firstpos:

      the set of positions that can be the first symbol in a string derived from that node.

Lastpos:

      the set of positions that can be the last symbol in a string derived from that node.

Followpos:

      Defines how positions relate to each other during DFA state transitions. It determines which positions can follow a given position in a string derived from that node. 



4) From the Followpos table, we can build a DFA to recognize the provided regular expression.
