CC=g++
cflag=-lfl

isql:  main.cpp lex.yy.cpp
	$(CC)  -o $@ $< $(cflag)
lex.yy.cpp: scan.l
	flex  -o $@ $< 
