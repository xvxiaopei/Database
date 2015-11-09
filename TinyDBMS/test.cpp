#include <iostream>
#include "physicalOP.h"
using namespace std;


int main()
{
	cout<<"test"<<endl;
	physicalOP* p=physicalOP::getInstance();
	
	vector<string> field_names;
  vector<enum FIELD_TYPE> field_types;
  field_names.push_back("f1");
  field_names.push_back("f2");
  field_names.push_back("f3");
  field_names.push_back("f4");
  field_types.push_back(STR20);
  field_types.push_back(STR20);
  field_types.push_back(INT);
  field_types.push_back(STR20);
  string relation_name="ExampleTable1";
  p->CreateTable(relation_name,field_names,field_types);


  cout<<"?";
}
