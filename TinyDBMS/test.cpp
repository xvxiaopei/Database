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
  a.push_back(14);
  a.push_back(10);
  s.push_back("v14");
  s.push_back("v13");
  p->insert(relation_name,iFileName,s,a);

  s.pop_back();
  s.push_back("VVV");
  p->insert(relation_name,iFileName,s,a);
  s.pop_back();
  s.pop_back();
  a.pop_back();
  a.pop_back();
  a.push_back(21);
  a.push_back(10);
  s.push_back("v111");
  s.push_back("mm");
   p->insert(relation_name,iFileName,s,a);
  p->displayRelation(relation_name);

  condition con;
  vector<Tuple> x = p->singleTableSelect("ExampleTable1",con);
  for(vector<Tuple>::iterator it  = x.begin(); it != x.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = x.erase(it);  
   }   
	s.pop_back();
   a.pop_back();
   a.pop_back();
   a.push_back(17);
   a.push_back(10);
   s.push_back("v111");
  
  
	s.pop_back();
    s.push_back("XML");
  //p->Delete("ExampleTable1",con);
 // p->displayRelation("ExampleTable1");
  p->insert(relation_name,iFileName,s,a);
  p->displayRelation("ExampleTable1");
 // p->Delete("ExampleTable1",con);
  //p->displayRelation("ExampleTable1");



  vector<string> field_names2;
  vector<enum FIELD_TYPE> field_types2;
  field_names2.push_back("f3");
  field_names2.push_back("f4");
  field_names2.push_back("f5");
  field_names2.push_back("f6");
  field_types2.push_back(INT);
  field_types2.push_back(STR20);
  field_types2.push_back(INT);
  field_types2.push_back(STR20);
  string relation_name2="ExampleTable2";
  //Relation* relation_ptr=p->
  p->DropTable(relation_name2);
  p->CreateTable(relation_name2,field_names2,field_types2);
  vector<string> iFileName2;
  vector<int> a2;
  vector<string> s2;
  iFileName2.push_back("f3");
  iFileName2.push_back("f4");
  iFileName2.push_back("f5");
  iFileName2.push_back("f6");
  a2.push_back(10);
  a2.push_back(20);
  s2.push_back("v14");
  s2.push_back("v2");
  p->insert(relation_name2,iFileName2,s2,a2);

  s2.pop_back();
  s2.push_back("VVV");
  p->insert(relation_name2,iFileName2,s2,a2);
  p->displayRelation(relation_name2);
  cout << "Calculated Disk I/Os = " << p->IO() << endl;

  
  
   vector<Tuple> y = p->Product("ExampleTable2","ExampleTable1");
  cout<<"cross join: "<<endl;
  for(vector<Tuple>::iterator it  = y.begin(); it != y.end(); )  
   {  
	  (*(it)).printTuple();
                 it = y.erase(it);  
   }   

  cout<<endl;
  cout << "Calculated Disk I/Os = " << p->IO() << endl;


  vector<Tuple> z =  p->JoinOnePass("ExampleTable2","ExampleTable1");
  cout<<"natural join: "<<endl;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = z.erase(it);  
   }   


  cout << "Calculated Disk I/Os = " << p->IO() << endl;

 
   z =   p->SortOnePass("ExampleTable1","f2");
  cout<<"Sort: "<<endl;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = z.erase(it);  
   }   



  p->displayMem();
  for(int i=0;i<16;i++)
  {
	   a.pop_back();
	 a.pop_back();
	a.push_back(i%13);
	a.push_back((i%13)*2);
	s.pop_back();
    s.pop_back();
	char n='a'+(i%13);
	string in="v";
	s.push_back(in+n);
	s.push_back(n+in);
	

	p->insert(relation_name,iFileName,s,a);
  }


  vector<string> xx =   p->sortedSub("ExampleTable1","f4");
  for(vector<string>::iterator it  = xx.begin(); it != xx.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = xx.erase(it);  
   }   

  p->displayMem();

  tupAddr ax = p->getMin("f4",0,10);
  cout<<ax.block_index<<" "<<ax.offset<<endl;

  z =   p->SortTwoPass("ExampleTable1","f1");
  cout<<"Two pass Sort: "<<endl;
  int i=0;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<i<<": "<<*(it)<<endl;  
                 it = z.erase(it);  
				 i++;
   }   
  cout<<"Two pass Sort done!"<<endl;



  z =   p->dupOnePass("ExampleTable1");
  cout<<"Duplicate elimination: "<<endl;
	i=0;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<i<<": "<<*(it)<<endl;  
                 it = z.erase(it);  
				 i++;
   }   




  cout<<"?";
  

}
