CC=g++
cflag=-lfl -g
SM=StorageManager-c++-2_1_beta_1_fix-linux

all:isql 

isql:  main.o lex.yy.cpp common.h StorageManager.o physicalOP.o  common.o
	$(CC)  -o $@ main.o StorageManager.o  physicalOP.o  common.o  $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
main.o: main.cpp common.h lex.yy.cpp
	$(CC) -c -o $@ $<
common.o: common.cpp common.h
	$(CC) -c -o $@ $<
test: test.cpp physicalOP.cpp DataStructures.h physicalOP.h StorageManager.o common.o
	$(CC) -o $@ StorageManager.o test.cpp physicalOP.cpp common.o
physicalOP.o: physicalOP.cpp physicalOP.h  common.h
	$(CC) -c -o $@  $<
StorageManager.o: $(SM)/StorageManager.cpp
	$(CC) -c -o $@ $<

clean:
	rm *.o isql test
