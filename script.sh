#!/bin/bash
flex lex.l
gcc lex.yy.c -o lex.out
g++ parser.cpp -o parser.out
./lex.out
./parser.out
