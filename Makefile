CC=g++

SM=StorageManager-c++-2_1_beta_1_fix-linux

cflag=-lfl 

inc=-I./$(SM)

all:tsql 

tsql:  main.o lex.yy.cpp common.h StorageManager.o physicalOP.o  common.o
	$(CC) $(inc) -g  -o $@ main.o StorageManager.o  physicalOP.o  common.o  $(cflag) 
lex.yy.cpp: scan.l common.h
	flex -o $@ $< 
main.o: main.cpp common.h lex.yy.cpp  physicalOP.h
	$(CC) $(inc) -g -c -o $@ $<
common.o: common.cpp common.h
	$(CC) $(inc) -g -c -o $@ $<
physicalOP.o: physicalOP.cpp physicalOP.h  common.h
	$(CC) $(inc) -g -c -o $@  $<
StorageManager.o: $(SM)/StorageManager.cpp
	$(CC) -g -c -o $@ $<

clean:
	rm *.o tsql
