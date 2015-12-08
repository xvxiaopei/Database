#ifndef _PHYSICAL_OP_H_                   // include file only once
#define _PHYSICAL_OP_H_
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <list>
#include <climits>


#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "DataStructures.h"
using namespace std;

class physicalOP{
private:
	MainMemory mem;
	Disk disk;
	clock_t physicalOP_start_time;
	static physicalOP *physicalop;
    physicalOP():schema_manager(&mem, &disk) {  
		disk.resetDiskIOs();
		disk.resetDiskTimer();
		physicalOP_start_time = clock() ;
    }    //singleton

public:
	SchemaManager   schema_manager;

	//add some for debug
	void displayMem();
	void displaySchema(Schema &schema);
	void displayRelation(string relation_name);
	int IO(){return disk.getDiskIOs();}
	void IOreset(){disk.resetDiskIOs();
		disk.resetDiskTimer();}
	static physicalOP *getInstance() {
        if (physicalop == NULL) {
            physicalop = new physicalOP();
			//cout<<"Physical operator initialized!"<<endl;
			//physicalop->displayMem();
        }
        return physicalop;
    }

	Relation * CreateTable(string relation_name,
                     vector<string> & field_names, 
                     vector <enum FIELD_TYPE> & field_types);

	Relation * CreateTable(string relation_name,vector<Tuple> tuples); //create a relation with relation_name and tuples



	void DropTable(string relation_name);

	void insert(string relation_name,
				vector<string> field_names,
				vector<string> STR,
				vector<int> INT);

	vector<Tuple> singleTableSelect(string relation_name,
									condition con);
	
	void Delete(string relation_name,
				condition con);

	vector<Tuple> sortOnMemory(string field_name,
						int start_block,int num_blocks);  //sort the tuples in Mem,return a vector of Tuples sorted

	vector<Tuple> SortOnePass(string relation_name,string field_name);  //sort ONE PASS algorithm(don't write back)
	vector<string> sortedSub(string relation_name,string field_name);  //return a list of sublists name in disk(each is a relation)
	tupAddr getMin(string field_name,int start_block,int num_blocks);  //return the location of the minimum tuple
	vector<Tuple> SortTwoPass(string relation_name,string field_name);  //sort TWO PASS algorithm(write back once)

	bool tupleEqual(Tuple a,Tuple b);             //judge if these two tuples are same
	bool fieldLarger(Tuple a,Tuple b,string field_name);  //judge if the field of a is larger than which of b
	bool fieldLarger(Tuple a,Tuple b,string field_name1,string field_name2);  //judge if the field of a is larger than another of b
	bool fieldEqual(Tuple a,Tuple b, string field_name);   //judge if the field of a is equal to which of b
	vector<tupAddr*> findDupOnMemory(Tuple t,int start_block,int num_blocks);//Find duplicate on mem
	vector<Tuple> dupOnePass(string relation_name);  //duplicate elimination ONE PASS algorithm(don't write back)
	vector<Tuple> dupTwoPass(string relation_name);  

	


	vector<Tuple> Product(string relation_name1,
				       string relation_name2);   //cross join, one pass

	vector<Tuple> JoinOnePass(string relation_name1,
				       string relation_name2);   //natural join, one pass
	vector<Tuple> JoinOnePass(string relation_name1,
				       string relation_name2,vector<string> common_fields);   //natural join, one pass
	Tuple JoinOneTuple(Tuple t1,Tuple t2);         //return a joined tuple if can join, or an invalid tuple
	Tuple JoinOneTuple(string relation_name1,string relation_name2,Tuple t1,Tuple t2,vector<string> common_fields);

	vector<Tuple> JoinTwoPass(string relation_name1, 
							string relation_name2);  //natural join, two pass

	vector<Tuple> JoinTwoPass(string relation_name1, 
							string relation_name2,vector<string> common_fields);  //natural join, two pass

	relation_data RelationCount(string relation_name);

	 void combine(int n,int m,vector<int>a,vector<int>&b,vector<vector<int> > &combines);   //pick m elements in a;
	 
	 relation_data computeJoin(relation_data relationData1,relation_data relationData2);                    //compute the data after join
	 vector<vector<int> >getCombine(int n,int m,vector<int>a);
	 vector<Tuple> JoinTree(JoinNode & root);
	 vector<Tuple> JoinTree(JoinNode & root,vector<string> common_fields);
	 vector<Tuple> JoinTables(vector<string> relation_names);
	 vector<Tuple> JoinTables(vector<string> relation_names,vector<string> common_fields);
	 

};

	



#endif
