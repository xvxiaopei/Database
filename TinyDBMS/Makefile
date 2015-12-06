test: test.cpp physicalOP.cpp DataStructures.cpp DataStructures.h physicalOP.h StorageManager.o
	clang++ -o $@ StorageManager.o test.cpp physicalOP.cpp DataStructures.cpp
StorageManager.o: StorageManager/StorageManager.cpp
	clang++ -c -o $@ $<

