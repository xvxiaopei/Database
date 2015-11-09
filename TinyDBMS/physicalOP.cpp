#include "physicalOP.h"


physicalOP * physicalOP::physicalop=NULL;

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

void physicalOP::displayRelation(Relation * relation_ptr)
{
	cout << "The table has name " << relation_ptr->getRelationName() << endl;
  cout << "The table has schema:" << endl;
  cout << relation_ptr->getSchema() << endl;
  cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
  cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl << endl;
}




Relation * physicalOP::CreateTable(string relation_name,
							 vector<string> & field_names, 
							 vector <enum FIELD_TYPE> & field_types)
{
	cout << "Creating a schema" << endl;
	Schema schema(field_names,field_types);
	//displaySchema(schema);
	cout << "Creating table " << relation_name << endl;
	Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);
	displayRelation(relation_ptr);
	return relation_ptr;

}