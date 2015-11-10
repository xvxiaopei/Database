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
  field_types.push_back(INT);
  field_types.push_back(INT);
  field_types.push_back(STR20);
  string relation_name="ExampleTable1";
  //Relation* relation_ptr=p->
  p->DropTable(relation_name);
  p->CreateTable(relation_name,field_names,field_types);
  vector<string> iFileName;
  vector<int> a;
  vector<string> s;
  iFileName.push_back("f2");
  iFileName.push_back("f4");
  iFileName.push_back("f1");
  iFileName.push_back("f3");
  a.push_back(20);
  a.push_back(10);
  s.push_back("v14");
  s.push_back("v13");
  p->insert(relation_name,iFileName,s,a);

  s.pop_back();
  s.push_back("VVV");
  p->insert(relation_name,iFileName,s,a);
  p->displayRelation(relation_name);

  condition con;
  vector<Tuple> x = p->singleTableSelect("ExampleTable1",con);
  for(vector<Tuple>::iterator it  = x.begin(); it != x.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = x.erase(it);  
   }   
	
  //p->Delete("ExampleTable1",con);
 // p->displayRelation("ExampleTable1");
  p->insert(relation_name,iFileName,s,a);
  p->displayRelation("ExampleTable1");
  p->Delete("ExampleTable1",con);
  p->displayRelation("ExampleTable1");
  cout<<"?";
}
