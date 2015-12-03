CC=clang++
cflag=-lfl -g
all:isql test
SM=StorageManager-c++-2_1_beta_1_fix-linux
isql:  main.cpp lex.yy.cpp common.h StorageManager.o physicalOP.o DataStructures.o
	$(CC)  -o $@ StorageManager.o  physicalOP.o DataStructures.o $< $(cflag)
lex.yy.cpp: scan.l common.h
	flex  -o $@ $< 
test: test.cpp physicalOP.cpp DataStructures.cpp DataStructures.h physicalOP.h StorageManager.o
	$(CC) -o $@ StorageManager.o test.cpp physicalOP.cpp DataStructures.cpp
physicalOP.o: physicalOP.cpp physicalOP.h StorageManager.o
	$(CC) -c -o $@ StorageManager.o $<
DataStructures.o: DataStructures.cpp DataStructures.h  StorageManager.o
	$(CC) -c -o $@ StorageManager.o $<
StorageManager.o: $(SM)/StorageManager.cpp
	$(CC) -c -o $@ $<

clean:
	rm *.o isql test
