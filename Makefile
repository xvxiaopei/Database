CC=g++
cflag=-lfl -g
all:isql test
isql:  main.cpp lex.yy.cpp common.h StorageManager.o physicalOP.o DataStructures.o
	$(CC)  -o $@ StorageManager.o  physicalOP.o DataStructures.o $< $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
test: test.cpp physicalOP.cpp DataStructures.cpp DataStructures.h physicalOP.h StorageManager.o
	clang++ -o $@ StorageManager.o test.cpp physicalOP.cpp DataStructures.cpp
physicalOP.o: physicalOP.cpp physicalOP.h StorageManager.o
	clang++ -c -o $@ StorageManager.o $<
DataStructures.o: DataStructures.cpp DataStructures.h  StorageManager.o
	clang++ -c -o $@ StorageManager.o $<
StorageManager.o: StorageManager/StorageManager.cpp
	clang++ -c -o $@ $<

