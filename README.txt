Group: CG32
Students: Nimish Agrawal(1801113), Sayan Kar(1801162)

# Contributions

Nimish's contribution:
Implemented removeLeftRecursion(), findFirst(), findFollow(), findParseTable()

Sayan's contribution:
Implemented Lex Analyzer,  readTokenType(), readRules(), getProductions(), parseInput(), displayResults(...)


# Steps to run program

	1. FORMAT for production rules in "rules.txt" file
		A) First line contains whitespace separated terminal tokens. 
		B) Second line contains whitespace separated non-terminal tokens. 
		C) One production rule per line
		D) LHS and RHS separated by colon
		E) First LHS symbol of production rules is the START symbol
		F) Symbols in RHS should be space separated
		
		NOTE1) All the lines should end with $$
		NOTE2) Epsilon is denoted by EPS

	2. FORMAT for "lex.l" file
		A) Tokens generated should be written to "input.txt" file
		B) One line can have atmost 1 matching token in the format LEXEME <space> TOKEN

	3. Run "bash script.sh" and give input string

	4. Program will display resultant Production ruled,FIRST,FOLLOW,ParseTable, Accept/Reject.


Sample implementation for the language EXPRESSIONS is provided