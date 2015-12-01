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
	tupAddr(int _block_index,int _offset)
	{
		block_index=_block_index;
		offset=_offset;
	}


};





class relation_data{
public:
	vector<int> V;                            //in the order of field names' order
	Schema schema;
	int T;
	string relation_name;
	Relation* relation_ptr;
	relation_data(vector<int> v,int t,string name,Schema _schema,Relation * ptr=NULL)
	{
		V=v;T=t;relation_name=name; relation_ptr=ptr;schema=_schema;
	}
	relation_data(){
		T=0;relation_name=""; relation_ptr=NULL;
	}
	void print()
	{
		cout<<"Relation : "<<relation_name<<" has "<<T<<" tuples, and V is :"<<endl;
		//cout<<schema;
		for(int i = 0; i<schema.getNumOfFields();i++)
		{
			cout<<schema.getFieldName(i)+"  ";
		}
		cout<<endl;
		for(int i = 0; i<schema.getNumOfFields();i++)
		{
			cout<<V[i]<<"  ";
		}
		cout<<endl;

	}


};
class JoinNode
{
public:
	JoinNode *left;
	JoinNode *right;
	relation_data m;

	JoinNode(relation_data _m,JoinNode *_l=NULL,JoinNode *_r=NULL)
	{
		m=_m;left=_l;right=_r;
	}
	JoinNode()
	{
		left=NULL;right=NULL;
	}
	void print()
	{
		cout<<m.relation_name<<" : "<<m.T<<endl;
		if(left!=NULL)left->print();
		if(right!=NULL)right->print();
	}




};

#endif