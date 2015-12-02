CC=g++
cflag=-lfl -g
all:isql test
isql:  main.cpp lex.yy.cpp common.h StorageManager.o
	$(CC)  -o $@ StorageManager.o $< $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
test: test.cpp physicalOP.cpp DataStructures.cpp DataStructures.h physicalOP.h StorageManager.o
	clang++ -o $@ StorageManager.o test.cpp physicalOP.cpp DataStructures.cpp
StorageManager.o: StorageManager/StorageManager.cpp
	clang++ -c -o $@ $<

