CC=g++
LEX=flex
target=tsql
SM=StorageManager-c++-2_1_beta_1_fix-linux
lib=-lfl 
inc=-I./$(SM)
intermediate=lex.yy.cpp *.o 


all:tsql 
tsql:  main.o lex.yy.cpp StorageManager.o physicalOP.o  common.o *.h $(SM)/*.h
	$(CC) $(inc) -g -o $@ main.o StorageManager.o  physicalOP.o  common.o $(lib) 
lex.yy.cpp: scan.l 
	$(LEX) -o $@ $< 
main.o: main.cpp common.h lex.yy.cpp  *.h $(SM)/*.h 
	$(CC) $(inc) -g -c -o $@ $<
common.o: common.cpp *.h $(SM)/*.h
	$(CC) $(inc) -g -c -o $@ $<
physicalOP.o: physicalOP.cpp *.h $(SM)/*.h 
	$(CC) $(inc) -g -c -o $@ $<
StorageManager.o: $(SM)/StorageManager.cpp $(SM)/*.h
	$(CC) -g -c -o $@ $<
clean:
	rm $(target) $(intermediate)
