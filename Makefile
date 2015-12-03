CC=clang++
cflag=-lfl -g
all:isql test
SM=StorageManager-c++-2_1_beta_1_fix-linux
isql:  main.cpp lex.yy.cpp common.h StorageManager.o physicalOP.o  common.o
	$(CC)  -o $@ StorageManager.o  physicalOP.o  common.o $< $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
common.o: common.cpp 
	$(CC) -c -o $@ $<
test: test.cpp physicalOP.cpp DataStructures.h physicalOP.h StorageManager.o 
	$(CC) -o $@ StorageManager.o test.cpp physicalOP.cpp common.o
physicalOP.o: physicalOP.cpp physicalOP.h  common.h
	$(CC) -c -o $@  $<
StorageManager.o: $(SM)/StorageManager.cpp
	$(CC) -c -o $@ $<

clean:
	rm *.o isql test
