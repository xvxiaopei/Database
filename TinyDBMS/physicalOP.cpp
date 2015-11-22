#include "physicalOP.h"


physicalOP * physicalOP::physicalop=NULL;


void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) {
  Block* block_ptr;
  if (relation_ptr->getNumOfBlocks()==0) {
    cout << "The relation is empty" << endl;
    cout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
    block_ptr=mem.getBlock(memory_block_index);
    block_ptr->clear(); //clear the block
    block_ptr->appendTuple(tuple); // append the tuple
    cout << "Write to the first block of the relation" << endl;
    relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
  } else {
    cout << "Read the last block of the relation into memory block :" << endl;
    relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
    block_ptr=mem.getBlock(memory_block_index);

    if (block_ptr->isFull()) {
      cout << "(The block is full: Clear the memory block and append the tuple)" << endl;
      block_ptr->clear(); //clear the block
      block_ptr->appendTuple(tuple); // append the tuple
      cout << "Write to a new block at the end of the relation" << endl;
      relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index); //write back to the relation
    } else {
      cout << "(The block is not full: Append it directly)" << endl;
      block_ptr->appendTuple(tuple); // append the tuple
      cout << "Write to the last block of the relation" << endl;
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
	cout<<"Insert a tuple to the relation "<<relation_name<<endl;
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
	cout<<"Insert Done."<<endl;
	//cout<<*relation_ptr << endl << endl;
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



void physicalOP::sortOnMemory(string relation_name,
							  string field_name,
								int start_block,int num_blocks)
{
	Block *block_ptr;
	block_ptr=mem.getBlock(start_block);
	Schema schema =block_ptr->getTuple(0).getSchema();
	vector<Tuple> tuples;
	
	for(int i=start_block;i<start_block+num_blocks;i++)
	{
		block_ptr=mem.getBlock(i);
		for(int j=0;j<block_ptr->getNumTuples();j++)
		{
			Tuple t=block_ptr->getTuple(j);
			if(t.getSchema()!=schema) {cout<<"Different schema!"<<endl;return;}
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
				if(tuples[i].getField(field_name).integer>tuples[i+1].getField(field_name).integer)
				{
					Tuple tmp=tuples[i];
					tuples[i]=tuples[i+1];
					tuples[i+1]=tmp;
				}
			}
			else if(schema.getFieldType(field_name)==STR20)
			{
				if(tuples[i].getField(field_name).str>tuples[i+1].getField(field_name).str)
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
		for(int j=0;j<schema.getTuplesPerBlock();j++)
		{
			if(tuples.empty()) break;
			Tuple t=tuples.back();
			tuples.pop_back();
			cout<<t<<endl;
			block_ptr->setTuple(j,t);
		}
		if(j<schema.getTuplesPerBlock()) {block_ptr->nullTuple(j-1);break;}
	}
	return;
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
	Block* block_ptr0,*block_ptr;
	block_ptr0=mem.getBlock(0); block_ptr0->clear();


	return result;
	//Relation* Rsmall=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr2:relation_ptr1;
	//Relation* Rlarge=(relation_ptr1->getNumOfBlocks()>relation_ptr2->getNumOfBlocks())?relation_ptr1:relation_ptr2;


}
