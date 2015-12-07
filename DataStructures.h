#ifndef _DATA_STRUCTURES_H_                   // include file only once
#define _DATA_STRUCTURES_H_
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iterator>
#include <vector>

#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "common.h"
using namespace std;
typedef Qexpression* (condition) ;
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
		cerr<<"Relation : "<<relation_name<<" has "<<T<<" tuples, and V is :"<<endl;
		//cerr<<schema;
		for(int i = 0; i<schema.getNumOfFields();i++)
		{
			cerr<<schema.getFieldName(i)+"  ";
		}
		cerr<<endl;
		for(int i = 0; i<schema.getNumOfFields();i++)
		{
			cerr<<V[i]<<"  ";
		}
		cerr<<endl;

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
		cerr<<m.relation_name<<" : "<<m.T<<endl;
		if(left!=NULL)left->print();
		if(right!=NULL)right->print();
	}




};

#endif
