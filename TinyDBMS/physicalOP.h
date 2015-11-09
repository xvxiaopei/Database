#ifndef _PHYSICAL_OP_H_                   // include file only once
#define _PHYSICAL_OP_H_

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

class physicalOP{
private:
	MainMemory mem;
	Disk disk;
	SchemaManager   schema_manager;
	clock_t start_time;
	static physicalOP *physicalop;
    physicalOP():schema_manager(&mem, &disk) {  
		disk.resetDiskIOs();
		disk.resetDiskTimer(); }    //singleton

public:
	//add some for debug
	void displayMem();
	void displaySchema(Schema &schema);
	void displayRelation(Relation * relation_ptr);

	static physicalOP *getInstance() {
        if (physicalop == NULL) {
            physicalop = new physicalOP();
			cout<<"Physical operator initialized!"<<endl;
			physicalop->displayMem();
        }
        return physicalop;
    }

	Relation * CreateTable(string relation_name,
                     vector<string> & field_names, 
                     vector <enum FIELD_TYPE> & field_types);
};





#endif