CC=g++
cflag=-lfl 

SM=StorageManager-c++-2_1_beta_1_fix-linux

all:isql 

isql:  main.o lex.yy.cpp common.h StorageManager.o physicalOP.o  common.o
	$(CC) -g  -o $@ main.o StorageManager.o  physicalOP.o  common.o  $(cflag) 
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
main.o: main.cpp common.h lex.yy.cpp  physicalOP.h
	$(CC) -g -c -o $@ $<
common.o: common.cpp common.h
	$(CC)  -g -c -o $@ $<
test: test.cpp physicalOP.cpp DataStructures.h physicalOP.h StorageManager.o common.o
	$(CC)  -g -o $@ StorageManager.o test.cpp physicalOP.cpp common.o
physicalOP.o: physicalOP.cpp physicalOP.h  common.h
	$(CC)  -g -c -o $@  $<
StorageManager.o: $(SM)/StorageManager.cpp
	$(CC)  -g -c -o $@ $<

clean:
	rm *.o isql test
