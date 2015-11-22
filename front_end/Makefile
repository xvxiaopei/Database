CC=g++
cflag=-lfl

isql:  main.cpp lex.yy.cpp common.h
	$(CC)  -o $@ $< $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
