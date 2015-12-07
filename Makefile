CC=g++
target=tsql
SM=StorageManager-c++-2_1_beta_1_fix-linux
cflag=-lfl 
inc=-I./$(SM)
all:tsql 
intermediate=lex.yy.cpp *.o 


tsql:  main.o lex.yy.cpp common.h StorageManager.o physicalOP.o  common.o
	$(CC) $(inc) -g  -o $@ main.o StorageManager.o  physicalOP.o  common.o  $(cflag) 
lex.yy.cpp: scan.l 
	flex -o $@ $< 
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
