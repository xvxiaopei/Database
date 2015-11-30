#include "physicalOP.h"


physicalOP * physicalOP::physicalop=NULL;


void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) {
  Block* block_ptr;
  if (relation_ptr->getNumOfBlocks()==0) {
    //cout << "The relation is empty" << endl;
   // cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
    block_ptr=mem.getBlock(memory_block_index);
    block_ptr->clear(); //clear the block
    block_ptr->appendTuple(tuple); // append the tuple
    //cout << "Write to the first block of the relation" << endl;
    relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
  } else {
   // cout << "Read the last block of the relation into memory block :" << endl;
    relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
    block_ptr=mem.getBlock(memory_block_index);

    if (block_ptr->isFull()) {
      //cout << "(The block is full: Clear the memory block and append the tuple)" << endl;
      block_ptr->clear(); //clear the block
      block_ptr->appendTuple(tuple); // append the tuple
     // cout << "Write to a new block at the end of the relation" << endl;
      relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index); //write back to the relation
    } else {
     // cout << "(The block is not full: Append it directly)" << endl;
      block_ptr->appendTuple(tuple); // append the tuple
     // cout << "Write to the last block of the relation" << endl;
      relation_ptr->setBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index); //write back to the relation
    }
  }  
}





void physicalOP::displayMem()
{
	cout << "The memory contains " << physicalop->mem.getMemorySize() << " blocks" << endl;
	cout << physicalop->mem << endl << endl;
}

void physicalOP::displaySchema(Schema &schema)
{
	cout << schema << endl;
  cout << "The schema has " << schema.getNumOfFields() << " fields" << endl;
  cout << "The schema allows " << schema.getTuplesPerBlock() << " tuples per block" << endl;
  cout << "The schema has field names: " << endl;
  vector<string> field_names=schema.getFieldNames();
  copy(field_names.begin(),field_names.end(),ostream_iterator<string,char>(cout," "));
  cout << endl;
  cout << "The schema has field types: " << endl;
  vector <enum FIELD_TYPE> field_types=schema.getFieldTypes();
  for (int i=0;i<schema.getNumOfFields();i++) {
    cout << (field_types[i]==0?"INT":"STR20") << "\t";
  }
  cout << endl;  
}

void physicalOP::displayRelation(string relation_name)
{
	Relation* relation_ptr=schema_manager.getRelation(relation_name);
	cout << "The table has name " << relation_ptr->getRelationName() << endl;
  cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
  cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl ;
  cout << "Now the relation contains: " << endl;
  cout << *relation_ptr << endl<<endl;
}




Relation * physicalOP::CreateTable(string relation_name,
							 vector<string> & field_names, 
							 vector <enum FIELD_TYPE> & field_types)
{
	//cout << "****************Creating table " << relation_name<<" ***********" << endl;
	//cout << "Creating a schema" << endl;
	Schema schema(field_names,field_types);
	//displaySchema(schema);
	
	Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
	//displayRelation(relation_name);
	//cout << "***********************Creation Done*****************************"<<endl;
	return relation_ptr;

}





void physicalOP::DropTable(string relation_name)
{
	cout << "Drop table " << relation_name << endl;
	schema_manager.deleteRelation(relation_name);
}

void physicalOP::insert(string relation_name,
				vector<string> field_names,
				vector<string> STRv,
				vector<int> INTv)
{
	//cout<<"Insert a tuple to the relation "<<relation_name<<endl;
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	if (relation_ptr==NULL){
                cout << "No Such Relation"<<endl;
                return;
	}
	Tuple tuple = relation_ptr->createTuple();
	int counti=0;
	int counts=0;
	for(int i =0;i<tuple.getNumOfFields();i++)
	{
		if(tuple.getSchema().getFieldType(field_names[i])==INT)
		{
			tuple.setField(field_names[i],INTv[counti]);
			counti++;
		}
		else
		{
			tuple.setField(field_names[i],STRv[counts]);
			counts++;
		}
	}
	appendTupleToRelation(relation_ptr,mem,0,tuple);
	//cout<<"Insert Done."<<endl;
	//cout<<*relation_ptr << endl << endl;
}


Relation * physicalOP::CreateTable(string relation_name,vector<Tuple> tuples)
{
	Schema schema=tuples.back().getSchema();
	int tuplePerBlock=schema.getTuplesPerBlock();
	Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
	Block * block_ptr=mem.getBlock(0);
	block_ptr->clear();
	int offset=0;
	int numOfBlocks=0;
	while(!tuples.empty())
	{
		vector<string> field_names=schema.getFieldNames();
		vector<string> STRv;
		vector<int> INTv;

		Tuple t=tuples.back();
		for(int i=0;i<schema.getNumOfFields();i++)
		{
			if(schema.getFieldType(i)==INT) INTv.push_back(t.getField(i).integer);
			else STRv.push_back(*t.getField(i).str);
		}

		Tuple tuple = relation_ptr->createTuple();
		int counti=0;
		int counts=0;
		for(int i =0;i<tuple.getNumOfFields();i++)
		{
			if(tuple.getSchema().getFieldType(field_names[i])==INT)
			{
				tuple.setField(field_names[i],INTv[counti]);
				counti++;
			}
			else
			{
				tuple.setField(field_names[i],STRv[counts]);
				counts++;
			}
		}
		cout<<tuple<<endl;
		block_ptr->setTuple(offset++,tuple);
		if(offset==tuplePerBlock)
		{
			relation_ptr->setBlock(numOfBlocks++,0);
			block_ptr->clear();
			offset=0;
		}
		tuples.pop_back();
	}
	return relation_ptr;
}



vector<Tuple> physicalOP::singleTableSelect(string relation_name,
						   condition con)
{
	cout<<"select"<<endl;
	vector<Tuple> result;
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	Block* block_ptr;
	if (relation_ptr==NULL){
                cout << "No Such Relation"<<endl;
                return result;
	}
	Tuple tuple = relation_ptr->createTuple();

	for(int i=0;i<relation_ptr->getNumOfBlocks();i++)
	{
		relation_ptr->getBlock(i,0);
		block_ptr=mem.getBlock(0);
		int NumOfTuples = block_ptr->getNumTuples();
		vector<Tuple> Tuples = block_ptr->getTuples();
		for(vector<Tuple>::iterator it  = Tuples.begin(); it != Tuples.end(); )  
        {  
			if(con.judge(*it))           //if this tuple fit the condition
			{
				result.push_back(*it);
				it = Tuples.erase(it);
			}
        }  
	}
	return result;
}


void physicalOP::Delete(string relation_name,              //using mem 0 to get the block, 1 to store the tuples to write back
						condition con)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	Block* block_ptr,*block_back;
	int tuplesBack = 0;
	int blockback=0;
	//int blockin;
	cout<<"Delete"<<endl;
	block_ptr=mem.getBlock(0); block_ptr->clear();
	block_back=mem.getBlock(1); block_back->clear();


	if (relation_ptr==NULL){
                cout << "No Such Relation"<<endl;
				return;
	}
	Tuple tuple = relation_ptr->createTuple();

	for(int i=0;i<relation_ptr->getNumOfBlocks();i++)
	{
		relation_ptr->getBlock(i,0);
		
		int NumOfTuples = block_ptr->getNumTuples();
		
		for(int j=0;j<NumOfTuples;j++)
		{
			if(!con.judge(block_ptr->getTuple(j)))
			{
				block_back->setTuple(tuplesBack,block_ptr->getTuple(j));
				tuplesBack++;
				if(tuplesBack==relation_ptr->getSchema().getTuplesPerBlock())
				{
					relation_ptr->setBlock(blockback,1);
					tuplesBack=0;
					blockback++;
				}
			}
		}
	}
	relation_ptr->deleteBlocks(blockback);


	return;
}

tupAddr  physicalOP::getMin(string field_name,int start_block,int num_blocks)
{
	tupAddr result;
	Block *block_ptr;
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		if(!mem.getBlock(i)->isEmpty()) {block_ptr=mem.getBlock(i);break;}
	}
	vector<Tuple> a=block_ptr->getTuples();
	//cout<<"block 0 has "<<mem.getBlock(0)->getNumTuples()<<endl;
	int tupPerBlock =a[0].getSchema().getTuplesPerBlock() ;

	if(a[0].getSchema().getFieldType(field_name)==INT)
	{
		int minInt=INT_MAX;
		result.block_index=start_block;
		result.offset=0;
		for(int i=start_block;i<start_block+num_blocks;i++)
		{
			block_ptr=mem.getBlock(i);
			for(int j=0;j<tupPerBlock;j++)
			{
				if(block_ptr->getTuple(j).isNull())continue;
				if(minInt>block_ptr->getTuple(j).getField(field_name).integer)
				{
					minInt=block_ptr->getTuple(j).getField(field_name).integer;
					result.block_index=i;
					result.offset=j;
				}
			}
		}
		return result;
	}
	else
	{
		char x=(char)INT_MAX;
		char c[1];
		c[0]=x;
		string minString = string(c,1);
		result.block_index=start_block;
		result.offset=0;
		for(int i=start_block;i<start_block+num_blocks;i++)
		{
			block_ptr=mem.getBlock(i);
			for(int j=0;j<tupPerBlock;j++)
			{
				if(block_ptr->getTuple(j).isNull())continue;
				if(minString>*block_ptr->getTuple(j).getField(field_name).str)
				{
					minString=*block_ptr->getTuple(j).getField(field_name).str;
					result.block_index=i;
					result.offset=j;
				}
			}
		}
		return result;
	}
}


vector<Tuple> physicalOP::sortOnMemory(
							  string field_name,
								int start_block,int num_blocks)
{
	Block *block_ptr;
	block_ptr=mem.getBlock(start_block);
	Schema schema =block_ptr->getTuple(0).getSchema();
	vector<Tuple> tuples,results;
	
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		block_ptr=mem.getBlock(i);
		for(int j=0;j<block_ptr->getNumTuples();j++)
		{
			Tuple t=block_ptr->getTuple(j);
			if(t.getSchema()!=schema) {cout<<"Different schema!"<<endl;return results;}
			tuples.push_back(t);
		}
	}
	int i,j;             //sort
	for(j=0;j<tuples.size()-1;j++)
	{
		for(i = 0;i<tuples.size()-1-j;i++)
		{
			if(schema.getFieldType(field_name)==INT)
			{
				if(tuples[i].getField(field_name).integer<tuples[i+1].getField(field_name).integer)
				{
					Tuple tmp=tuples[i];
					tuples[i]=tuples[i+1];
					tuples[i+1]=tmp;
				}
			}
			else if(schema.getFieldType(field_name)==STR20)
			{
				if(*tuples[i].getField(field_name).str<*tuples[i+1].getField(field_name).str)
				{
					Tuple tmp=tuples[i];
					tuples[i]=tuples[i+1];
					tuples[i+1]=tmp;
				}
			}
		}
	}
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		block_ptr=mem.getBlock(i);
		int j=0;
		for(;j<schema.getTuplesPerBlock();j++)
		{
			if(tuples.empty()) break;
			Tuple t=tuples.back();
			results.push_back(t);
			tuples.pop_back();
			cout<<t<<endl;
			block_ptr->setTuple(j,t);
		}
		//if(j<schema.getTuplesPerBlock()) {block_ptr->nullTuple(j-1);break;}
	}
	return results;
}

vector<Tuple> physicalOP::SortOnePass(string relation_name,string field_name)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	vector<Tuple> result;
	if(relation_ptr->getNumOfBlocks()>10) {cout<<"This sort can't be in one pass! "<<endl;return result;}
	relation_ptr->getBlocks(0,0,relation_ptr->getNumOfBlocks());
	return sortOnMemory(field_name,0,relation_ptr->getNumOfBlocks());
}

vector<string> physicalOP::sortedSub(string relation_name,string field_name)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	vector<string> result;
	Schema schema=relation_ptr->getSchema();
	int leftBlocks = relation_ptr->getNumOfBlocks();
	int count=0;
	int numOfSub=0;
	while(leftBlocks>0)
	{
		int getNumOfBlocks=(leftBlocks>10)?10:leftBlocks;
		relation_ptr->getBlocks(count,0,getNumOfBlocks);
		leftBlocks-=getNumOfBlocks;
		//displayMem();
		char nameplus='0'+ numOfSub;
		sortOnMemory(field_name,0,getNumOfBlocks);
		//cout<<relation_name+'-'+nameplus<<"  "<<leftBlocks<<endl;
		//displayMem();
		string name=relation_name+'-'+nameplus;
		if(schema_manager.relationExists(name)) DropTable(name);
		Relation* sublist=schema_manager.createRelation(name,schema);
		sublist->setBlocks(0,0,getNumOfBlocks);
		result.push_back(name);
		numOfSub++;
		count+=getNumOfBlocks;
		cout<<"sublist: "<<name<<" done!"<<endl;
		displayRelation(name);
	}
	return result;
}



vector<Tuple> physicalOP::SortTwoPass(string relation_name,string field_name)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	vector<Tuple> result;
	tupAddr tmp;
	Block *block_ptr;
	Relation* sublist_ptr;
	if(relation_ptr->getNumOfBlocks()<=10) {cout<<"This sort can be in one pass! "<<endl;return SortOnePass(relation_name,field_name);}
	vector<string> sublist=sortedSub(relation_name,field_name);  //get sublists
	int *numOfBlocks = new int[sublist.size()];            //count how many  blocks of each sublist has been written to mem
	int sum=relation_ptr->getNumOfTuples();


	for(int i=0;i<sublist.size();i++)                        //put first block of each sublist to mem
	{
		sublist_ptr = schema_manager.getRelation(sublist[i]);
		sublist_ptr->getBlock(0,i);
		numOfBlocks[i]=0;
	}

	while(sum>0)
	{
		//displayMem();
		tmp=getMin(field_name,0,sublist.size());
		cout<<"Min  Index is "<<tmp.block_index<<" , off set is "<<tmp.block_index<<endl;
		block_ptr=mem.getBlock(tmp.block_index);
		result.push_back(block_ptr->getTuple(tmp.offset));
		sum--;
		block_ptr->nullTuple(tmp.offset);
		if(block_ptr->getNumTuples()==0) 
		{
			sublist_ptr = schema_manager.getRelation(sublist[tmp.block_index]);
			if(numOfBlocks[tmp.block_index]+1<sublist_ptr->getNumOfBlocks())
			{
				numOfBlocks[tmp.block_index]=numOfBlocks[tmp.block_index]+1;
				sublist_ptr->getBlock(numOfBlocks[tmp.block_index],tmp.block_index);
			}
		}
	}


	return result;
}


bool physicalOP::tupleEqual(Tuple a,Tuple b)
{
	if(a.isNull()||b.isNull())return false;
	if(a.getSchema()!=b.getSchema()) return false;
	Schema schema=a.getSchema();
	for(int i=0;i<schema.getNumOfFields();i++)
	{
		if(schema.getFieldType(i)==INT)
		{
			if(a.getField(i).integer!=b.getField(i).integer) return false;	
		}
		if(schema.getFieldType(i)==STR20)
		{
			if(*a.getField(i).str!=*b.getField(i).str) return false;	
		}
	}
	return true;
}

bool physicalOP::fieldLarger(Tuple a,Tuple b,string field_name)
{
	if(a.isNull()||b.isNull())return false;

	Schema schema1 =  a.getSchema();
	Schema schema2 =  b.getSchema();
	if(!schema1.fieldNameExists(field_name)||!schema2.fieldNameExists(field_name))
	{
		cout<<"field name not exists. "<<endl;
		return false;
	}

	if(schema1.getFieldType(field_name)==INT && schema2.getFieldType(field_name)==INT)
	{
		if(a.getField(field_name).integer>b.getField(field_name).integer) 
		{
			//cout<<a.getField(field_name).integer<<" > "<<b.getField(field_name).integer<<endl;
			return true; 
		}	
		else 
		{
			//cout<<a.getField(field_name).integer<<" <= "<<b.getField(field_name).integer<<endl;
			return false; 
		}

	}

	else if(schema1.getFieldType(field_name)==STR20&& schema2.getFieldType(field_name)==STR20)
	{
		if(*a.getField(field_name).str>*b.getField(field_name).str) 
		{
			//cout<<*a.getField(field_name).str<<" > "<<*b.getField(field_name).str<<endl;
			return true; 
		}
		else 
		{
			//cout<<*a.getField(field_name).str<<" <= "<<*b.getField(field_name).str<<endl;
			return false; 
		}
		
	}

	else return false;	
}
bool physicalOP::fieldEqual(Tuple a,Tuple b,string field_name)
{
	if(a.isNull()||b.isNull())return false;

	Schema schema1 =  a.getSchema();
	Schema schema2 =  b.getSchema();
	if(!schema1.fieldNameExists(field_name)||!schema2.fieldNameExists(field_name))
	{
		cout<<"field name not exists. "<<endl;
		return false;
	}

	if(schema1.getFieldType(field_name)==INT && schema2.getFieldType(field_name)==INT)
	{
		if(a.getField(field_name).integer==b.getField(field_name).integer) return true; 
	}

	else if(schema1.getFieldType(field_name)==STR20&& schema2.getFieldType(field_name)==STR20)
	{
		if(*a.getField(field_name).str==*b.getField(field_name).str) return true; 
	}

	else return false;	
}


vector<tupAddr*> physicalOP::findDupOnMemory(Tuple t,int start_block,int num_blocks)
{
	vector<tupAddr*> results;
	Block *block_ptr;
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		if(!mem.getBlock(i)->isEmpty()) {block_ptr=mem.getBlock(i);break;}
	}
	vector<Tuple> a=block_ptr->getTuples();
	int tupPerBlock =a[0].getSchema().getTuplesPerBlock();
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		if(!mem.getBlock(i)->isEmpty()) {block_ptr=mem.getBlock(i);}
		else continue;
		//vector<Tuple> m=block_ptr->getTuples();
		for(int j=0;j<tupPerBlock;j++)
		{
			if(tupleEqual(block_ptr->getTuple(j),t)) results.push_back(new tupAddr(i,j));
		}
	}
	return results;

}

vector<Tuple> physicalOP::dupOnePass(string relation_name)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	vector<Tuple> result;
	Block *block_ptr;
	int numOfBlocks=relation_ptr->getNumOfBlocks();
	if(numOfBlocks>10) {cout<<"This duplicate elimination can't be in one pass! "<<endl;return result;}
	relation_ptr->getBlocks(0,0,numOfBlocks);
	Schema schema=relation_ptr->getSchema();
	int tupPerBlock=schema.getTuplesPerBlock();
	displayMem();

	for(int i=0;i<numOfBlocks;i++)
	{
		block_ptr=mem.getBlock(i);
		for(int j=0;j<tupPerBlock;j++)
		{
			if(block_ptr->getTuple(j).isNull())continue;
			vector<tupAddr*> dupAddr=findDupOnMemory(block_ptr->getTuple(j),i,numOfBlocks-i);
			cout<<"There are "<<dupAddr.size()<<" duplicates"<<endl;
			while(!dupAddr.empty())
			{
				tupAddr * a=dupAddr.back();
				cout<<"duplicate is on "<<a->block_index<<"  "<<a->offset<<endl;
				if(a->block_index!=i||a->offset!=j) mem.getBlock(a->block_index)->nullTuple(a->offset); //not itself,eliminate
				dupAddr.pop_back();
			}
		}
	}
	return mem.getTuples(0,relation_ptr->getNumOfBlocks());
}

vector<Tuple> physicalOP::dupTwoPass(string relation_name)
{
	Relation* relation_ptr = schema_manager.getRelation(relation_name);
	vector<Tuple> result;
	Block *block_ptr;
	string field_name = relation_ptr->getSchema().getFieldName(0);
	if(relation_ptr->getNumOfBlocks()<=10) {cout<<"This duplicate elimination can be in one pass! "<<endl;return dupOnePass(relation_name);}
	vector<string> sublist=sortedSub(relation_name,field_name);
	Relation* sublist_ptr;
	tupAddr tmp;
	int *numOfBlocks = new int[sublist.size()];            //count how many  blocks of each sublist has been written to mem
	int sum=relation_ptr->getNumOfTuples();


	for(int i=0;i<sublist.size();i++)                        //put first block of each sublist to mem
	{
		sublist_ptr = schema_manager.getRelation(sublist[i]);
		sublist_ptr->getBlock(0,i);
		numOfBlocks[i]=0;
	}

	while(sum>0)
	{
		tmp=getMin(field_name,0,sublist.size());
		cout<<"Min  Index is "<<tmp.block_index<<" , off set is "<<tmp.block_index<<endl;
		block_ptr=mem.getBlock(tmp.block_index);
		result.push_back(block_ptr->getTuple(tmp.offset));

		Tuple minTuple=block_ptr->getTuple(tmp.offset);
		vector<tupAddr*> dupAddr=findDupOnMemory(minTuple,0,sublist.size());
		cout<<"There are "<<dupAddr.size()<<" duplicates"<<endl;
		while(!dupAddr.empty())
		{
			tupAddr * a=dupAddr.back();
			cout<<"duplicate is on "<<a->block_index<<"  "<<a->offset<<endl;
			block_ptr=mem.getBlock(a->block_index);
			block_ptr->nullTuple(a->offset); //eliminate
			sum--;
			dupAddr.pop_back();
			if(block_ptr->getNumTuples()==0)      //if this block of sublist is empty, put next block of this sublist
			{
				sublist_ptr = schema_manager.getRelation(sublist[a->block_index]);
				if(numOfBlocks[a->block_index]+1<sublist_ptr->getNumOfBlocks())
				{
					numOfBlocks[a->block_index]=numOfBlocks[a->block_index]+1;
					sublist_ptr->getBlock(numOfBlocks[a->block_index],a->block_index);
					dupAddr=findDupOnMemory(minTuple,0,sublist.size());
				}
			}
		}
	}
	return result;

}




vector<Tuple> physicalOP::Product(string relation_name1,
							   string relation_name2)
{
	Relation* relation_ptr1 = schema_manager.getRelation(relation_name1);
	Relation* relation_ptr2 = schema_manager.getRelation(relation_name2);
	vector<Tuple> result;
	if (relation_ptr1==NULL||relation_ptr2==NULL){
                cout << "No Such Relation"<<endl;
				return result;
	}
	Schema schema1 =  relation_ptr1->getSchema();
	Schema schema2 =  relation_ptr2->getSchema();
	vector<string> field_names;        // for new schema
	vector<enum FIELD_TYPE> field_types;
	Block* block_ptr0,*block_ptr;
	block_ptr0=mem.getBlock(0); block_ptr0->clear();


	for(int i=0;i<schema1.getFieldNames().size();i++)
	{
		if(schema2.fieldNameExists(schema1.getFieldName(i)))
		{
			field_names.push_back(relation_name1+"."+schema1.getFieldName(i));
		}
		else
		{
			field_names.push_back(schema1.getFieldName(i));
		}
		field_types.push_back(schema1.getFieldType(i));

	}

	for(int i=0;i<schema2.getFieldNames().size();i++)
	{
		if(schema1.fieldNameExists(schema2.getFieldName(i)))
		{
			field_names.push_back(relation_name2+"."+schema2.getFieldName(i));
		}
		else
		{
			field_names.push_back(schema2.getFieldName(i));
		}
		field_types.push_back(schema2.getFieldType(i));
	}
	string relation_name=relation_name1+"¡Á"+relation_name2;
	if(schema_manager.relationExists(relation_name))DropTable(relation_name);
	Relation* product_ptr=CreateTable(relation_name,field_names,field_types);
	//displayRelation(relation_name);
	if(product_ptr==NULL){
		cout<<"Create Relaton Failed,Return.";
		return result;
	}
	Relation* Rsmall=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr2:relation_ptr1;
	Relation* Rlarge=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr1:relation_ptr2;

	if(!Rsmall->getBlocks(0,1,Rsmall->getNumOfBlocks()))
	{
		cout << "Can't be done in ONE PASS! "<<endl;
		return result;
	}


	Tuple tuple = product_ptr->createTuple();
	//int blockNumInPorduct;
	for(int i=0;i<Rlarge->getNumOfBlocks();i++)
	{
		Rlarge->getBlock(i,0);
		//while(!block_back->isFull())
		for(int j=0;j<block_ptr0->getNumTuples();j++)
		{
			Tuple Tlarge=block_ptr0->getTuple(j);
			//cout<<Tlarge<<endl;
			for(int k=1;k<1+Rsmall->getNumOfBlocks();k++)
			{
				block_ptr=mem.getBlock(k);
				for(int l=0;l<block_ptr->getNumTuples();l++)
				{
					Tuple Tsmall=block_ptr->getTuple(l);
					//cout<<Tsmall<<endl;
					int m,n;
					if(Rsmall==relation_ptr1){
						for(m=0;m<Tsmall.getNumOfFields();m++)
						{
							if(schema1.getFieldType(m)==INT)
								tuple.setField(m,Tsmall.getField(m).integer);
							else tuple.setField(m,*Tsmall.getField(m).str);
						}
						for(n=0;n<Tlarge.getNumOfFields();n++,m++)
						{
							if(schema2.getFieldType(n)==INT)
								tuple.setField(m,Tlarge.getField(n).integer);
							else tuple.setField(m,*Tlarge.getField(n).str);
						}
					}
					else{
						for(m=0;m<Tlarge.getNumOfFields();m++)
						{
							if(schema1.getFieldType(m)==INT)
								tuple.setField(m,Tlarge.getField(m).integer);
							else tuple.setField(m,*Tlarge.getField(m).str);
						}
						for(n=0;n<Tsmall.getNumOfFields();n++,m++)
						{
							if(schema2.getFieldType(n)==INT)
								tuple.setField(m,Tsmall.getField(n).integer);
							else tuple.setField(m,*Tsmall.getField(n).str);
						}
					}   //complete tuple
					//now insert
					//cout<<tuple;
					result.push_back(tuple);	
				}
			}
		}
	}
	return result;

}


Tuple physicalOP::JoinOneTuple(Tuple t1,Tuple t2)
{
	

	Schema schema1 =  t1.getSchema();
	Schema schema2 =  t2.getSchema();
	vector<string> field_names,common_fields;        
	vector<enum FIELD_TYPE> field_types;
	for(int i=0;i<schema1.getFieldNames().size();i++)
	{
		if(schema2.fieldNameExists(schema1.getFieldName(i)))
		{
			common_fields.push_back(schema1.getFieldName(i));
			continue;
		}
		
		else
		{
			field_names.push_back(schema1.getFieldName(i));
			field_types.push_back(schema1.getFieldType(i));
		}
	}
	
	for(int i=0;i<schema2.getFieldNames().size();i++)
	{
		field_names.push_back(schema2.getFieldName(i));
		field_types.push_back(schema2.getFieldType(i));
	}
	string relation_name="JoinedRelation:";
	for(int i=0;i<common_fields.size();i++) relation_name+=common_fields[i];
	Relation* Join_ptr;
	if(schema_manager.relationExists(relation_name))Join_ptr=schema_manager.getRelation(relation_name);
	else Join_ptr=CreateTable(relation_name,field_names,field_types);
	//displayRelation(relation_name);
	Tuple tuple = Join_ptr->createTuple();
	if(t1.isNull()||t2.isNull())
	{
		tuple.null();
		return tuple;
	}
	for(int i=0;i<schema1.getFieldNames().size();i++)
	{
		string Filed_Name=schema1.getFieldName(i);
		if(schema2.fieldNameExists(Filed_Name))
		{
			if(schema1.getFieldType(Filed_Name)==INT&&schema2.getFieldType(Filed_Name)==INT)
			{
				if(t1.getField(Filed_Name).integer==t2.getField(Filed_Name).integer)
				{
					tuple.setField(Filed_Name,t1.getField(Filed_Name).integer);
					continue;
				}
				else{tuple.null();return tuple;}
			}
			else if(schema1.getFieldType(Filed_Name)==STR20&&schema2.getFieldType(Filed_Name)==STR20)
			{
				if(*t1.getField(Filed_Name).str==*t2.getField(Filed_Name).str)
				{
					tuple.setField(Filed_Name,*t1.getField(Filed_Name).str);
					continue;
				}
				else{tuple.null();return tuple;}
			}
			else 
			{
				tuple.null();
				return tuple;
			}
		}

		if(schema1.getFieldType(Filed_Name)==INT) tuple.setField(Filed_Name,t1.getField(Filed_Name).integer);
		else  tuple.setField(Filed_Name,*t1.getField(Filed_Name).str);
	}

	for(int i=0;i<schema2.getFieldNames().size();i++)
	{
		string Filed_Name=schema2.getFieldName(i);
		if(schema1.fieldNameExists(Filed_Name))
		{
			continue;
		}
		if(schema2.getFieldType(Filed_Name)==INT) tuple.setField(Filed_Name,t2.getField(Filed_Name).integer);
		else  tuple.setField(Filed_Name,*t2.getField(Filed_Name).str);
	}

	return tuple;


}



vector<Tuple>  physicalOP::JoinOnePass(string relation_name1,
									   string relation_name2)
{
	Relation* relation_ptr1 = schema_manager.getRelation(relation_name1);
	Relation* relation_ptr2 = schema_manager.getRelation(relation_name2);
	vector<Tuple> result;
	if (relation_ptr1==NULL||relation_ptr2==NULL){
                cout << "No Such Relation"<<endl;
				return result;
	}
	Block* block_ptr0,*block_ptr;
	block_ptr0=mem.getBlock(0); block_ptr0->clear();

	Relation* Rsmall=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr2:relation_ptr1;
	Relation* Rlarge=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr1:relation_ptr2;

	if(!Rsmall->getBlocks(0,1,Rsmall->getNumOfBlocks()))
	{
		cout << "Can't be done in ONE PASS! "<<endl;
		return result;
	}

	for(int i=0;i<Rlarge->getNumOfBlocks();i++)
	{
		Rlarge->getBlock(i,0);
		for(int j=0;j<block_ptr0->getNumTuples();j++)
		{
			Tuple Tlarge=block_ptr0->getTuple(j);
			cout<<"Tlarge: "<<Tlarge<<endl;
			for(int k=1;k<1+Rsmall->getNumOfBlocks();k++)
			{
				block_ptr=mem.getBlock(k);
				for(int l=0;l<block_ptr->getNumTuples();l++)
				{
					Tuple Tsmall=block_ptr->getTuple(l);
					cout<<"Tsmall: "<<Tsmall<<endl;
					Tuple tuple=JoinOneTuple(Tsmall,Tlarge);
					cout<<"tuple: "<<tuple<<endl;
					if(!tuple.isNull())result.push_back(tuple);	
				}
			}
		}
	}
	return result;

}
vector<Tuple>  physicalOP::JoinTwoPass(string relation_name1,
									   string relation_name2)
{
	Relation* relation_ptr1 = schema_manager.getRelation(relation_name1);
	Relation* relation_ptr2 = schema_manager.getRelation(relation_name2);
   	vector<Tuple> result;
	if (relation_ptr1==NULL||relation_ptr2==NULL){
                cout << "No Such Relation"<<endl;
				return result;
	}
	
	Schema schema1 =  relation_ptr1->getSchema();
	Schema schema2 =  relation_ptr2->getSchema();
	vector<string> common_fields;   

	int tupPerBlock1 =schema1.getTuplesPerBlock();
	int tupPerBlock2 =schema2.getTuplesPerBlock();

	for(int i=0;i<schema1.getFieldNames().size();i++)
	{
		if(schema2.fieldNameExists(schema1.getFieldName(i)))
		{
			common_fields.push_back(schema1.getFieldName(i));
			continue;
		}
	}
	if(common_fields.empty()) {cout<<"NO common fields, cross join.";return Product(relation_name1,relation_name2);}
	string field_name = common_fields.back();   //sorted bt this field

	if(relation_ptr1->getNumOfBlocks()<=10||relation_ptr2->getNumOfBlocks()<=10) 
	{cout<<"This Join can be in one pass! "<<endl;return JoinOnePass(relation_name1,relation_name2);}

	vector<string> sublist1=sortedSub(relation_name1,field_name);
	vector<string> sublist2=sortedSub(relation_name2,field_name);
	Relation* sublist_ptr1,* sublist_ptr2;
	//tupAddr tmp;
	
	Block* block_ptr1, *block_ptr2;

	int *numOfBlocks1 = new int[sublist1.size()];            //count how many  blocks of each sublist has been written to mem
	int sum1=relation_ptr1->getNumOfTuples();
	int *numOfBlocks2 = new int[sublist2.size()];            //count how many  blocks of each sublist has been written to mem
	int sum2=relation_ptr2->getNumOfTuples();
	int  i=0;

	for(i=0;i<sublist1.size();i++)                               //put first block of each sublist of relation 1 to mem
	{
		sublist_ptr1 = schema_manager.getRelation(sublist1[i]);
		sublist_ptr1->getBlock(0,i);
		numOfBlocks1[i]=0;
	}

	for(int j=0;i<sublist1.size()+sublist2.size();i++,j++)               //put first block of each sublist of relation 2 to mem
	{
		sublist_ptr2 = schema_manager.getRelation(sublist2[j]);
		//displayRelation(sublist2[j]);
		sublist_ptr2->getBlock(0,i);
		numOfBlocks2[j]=0;
	}
	displayMem();
	
	while(sum1>0&&sum2>0)
	{
		tupAddr min1=getMin(field_name,0,sublist1.size());
		tupAddr min2=getMin(field_name,sublist1.size(),sublist2.size());
		//cout<<"Min1  Index is "<<min1.block_index<<" , off set is "<<min1.offset<<endl;
		//cout<<"Min2  Index is "<<min2.block_index<<" , off set is "<<min2.offset<<endl;
		block_ptr1=mem.getBlock(min1.block_index);
		block_ptr2=mem.getBlock(min2.block_index);

		Tuple min1Tuple=block_ptr1->getTuple(min1.offset);
		Tuple min2Tuple=block_ptr2->getTuple(min2.offset);
		//cout<<min1Tuple<<endl<<min2Tuple<<":  ";
		if(JoinOneTuple(min1Tuple,min2Tuple).isNull())   //can't join
		{   //delete smaller
			//cout<<"can't join: ";
			if(fieldLarger(min1Tuple,min2Tuple,field_name)) 
			{
				//cout<<"delete 2"<<endl;
				block_ptr2->nullTuple(min2.offset);
				sum2--;
				//displayMem();
				if(block_ptr2->getNumTuples()==0)      //if this block of sublist is empty, put next block of this sublist
				{
					int sublist2index=min2.block_index-sublist1.size();
					sublist_ptr2 = schema_manager.getRelation(sublist2[sublist2index]);
					//cout<<sublist2[sublist2index]<<endl;
					if(numOfBlocks2[sublist2index]+1<sublist_ptr2->getNumOfBlocks())
					{
						numOfBlocks2[sublist2index]=numOfBlocks2[sublist2index]+1;
						sublist_ptr2->getBlock(numOfBlocks2[sublist2index],min2.block_index);
						//cout<<"Read "<<numOfBlocks2[sublist2index]<<"th block of sublist "<<sublist2index<<" into mem block "<<min2.block_index<<endl;
						//displayMem();
					}
				}
			}
		
			else 
			{
				//cout<<"delete 1"<<endl;
				block_ptr1->nullTuple(min1.offset);
				sum1--;
				if(block_ptr1->getNumTuples()==0)      //if this block of sublist is empty, put next block of this sublist
				{
					sublist_ptr1 = schema_manager.getRelation(sublist1[min1.block_index]);
					if(numOfBlocks1[min1.block_index]+1<sublist_ptr1->getNumOfBlocks())
					{
						numOfBlocks1[min1.block_index]=numOfBlocks1[min1.block_index]+1;
						sublist_ptr1->getBlock(numOfBlocks1[min1.block_index],min1.block_index);
						
					}
				}
			}
		}
		else    //can join
		{
			//cout<<"can join"<<endl;
			vector<Tuple> MinTuples1,MinTuples2;
			for(i=0;i<sublist1.size();i++)
			{
				block_ptr1=mem.getBlock(i);
				for(int j=0;j<tupPerBlock1;j++)
				{
					if(block_ptr1->getTuple(j).isNull())continue;
					if(!JoinOneTuple(min2Tuple,block_ptr1->getTuple(j)).isNull())
					{
						MinTuples1.push_back(block_ptr1->getTuple(j));
						block_ptr1->nullTuple(j);
						sum1--;
						if(block_ptr1->getNumTuples()==0)      //if this block of sublist is empty, put next block of this sublist
						{
							sublist_ptr1 = schema_manager.getRelation(sublist1[i]);
							if(numOfBlocks1[i]+1<sublist_ptr1->getNumOfBlocks())
							{
								numOfBlocks1[i]=numOfBlocks1[i]+1;
								sublist_ptr1->getBlock(numOfBlocks1[i],i);
							}
							j=-1;  
						}
					}
				}
			}

			for(int k=0;i<sublist1.size()+sublist2.size();i++,k++)
			{
				block_ptr2=mem.getBlock(i);
				for(int j=0;j<tupPerBlock2;j++)
				{
					if(block_ptr2->getTuple(j).isNull())continue;
					if(!JoinOneTuple(min1Tuple,block_ptr2->getTuple(j)).isNull())
					{
						MinTuples2.push_back(block_ptr2->getTuple(j));
						block_ptr2->nullTuple(j);
						sum2--;
						if(block_ptr2->getNumTuples()==0)      //if this block of sublist is empty, put next block of this sublist
						{
							sublist_ptr2 = schema_manager.getRelation(sublist2[k]);
							if(numOfBlocks2[k]+1<sublist_ptr2->getNumOfBlocks())
							{
								numOfBlocks2[k]=numOfBlocks2[k]+1;
								sublist_ptr2->getBlock(numOfBlocks2[k],i);
							}
							j=-1;
						}
					}
				}
			}
			while(!MinTuples1.empty())
			{
				for(int l=0;l<MinTuples2.size();l++)
				{
					//cout<<l;
					result.push_back(JoinOneTuple(MinTuples1.back(),MinTuples2[l]));
					//cout<<"!!"<<endl;
				}
				MinTuples1.pop_back();
			}
		}
	}
	return result;
}





