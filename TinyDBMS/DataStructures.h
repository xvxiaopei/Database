#ifndef _DATA_STRUCTURES_H_                   // include file only once
#define _DATA_STRUCTURES_H_

#include "StorageManager/Block.h"
#include "StorageManager/Config.h"
#include "StorageManager/Disk.h"
#include "StorageManager/Field.h"
#include "StorageManager/MainMemory.h"
#include "StorageManager/Relation.h"
#include "StorageManager/Schema.h"
#include "StorageManager/SchemaManager.h"
#include "StorageManager/Tuple.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iterator>
#include <vector>
using namespace std;

class condition{         //give a tuple and return true or false
	vector<string> field_names;
	
public:
	bool judge(Tuple t);        //judge if this tuple fit the condition
};

class tupAddr{                   //location if a tuple in mem
public:
	int block_index;
	int offset;
	tupAddr()
	{
		block_index=0;
		offset=0;
	}


};

#endif