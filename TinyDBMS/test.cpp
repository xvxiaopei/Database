#include <iostream>
#include "physicalOP.h"
using namespace std;


void test();
void testForCount();
void testForJoin();
void testCombine();
int main()
{
	//testForJoin();
	//testForCount();
	testCombine();
	cout<<"??";
}





void testCombine()
{
	/*
	physicalOP* p=physicalOP::getInstance();
	vector<int> a;
	int num=5;
	for(int i=0;i<num;i++){a.push_back(i);}
	vector<vector<int>> combines=p->getCombine(num, 3,a);
	for(int i=0;i<combines.size();i++)
	{
		for(int j=0;j<combines[i].size();j++)
		{
			cout<<combines[i][j];
		}

		cout<<endl;
	}
	*/
	physicalOP* p=physicalOP::getInstance();
	vector<string> field_names;
	vector<enum FIELD_TYPE> field_types;
	field_names.push_back("a");
	field_names.push_back("b");
	field_types.push_back(STR20);
	field_types.push_back(STR20);
	string relation_nameA="A";
	p->DropTable(relation_nameA);
	p->CreateTable(relation_nameA,field_names,field_types);

	string relation_nameB="B";
	field_names.pop_back();field_names.pop_back();
	field_names.push_back("b");
	field_names.push_back("c");
	p->DropTable(relation_nameB);
	p->CreateTable(relation_nameB,field_names,field_types);

	string relation_nameC="C";
	field_names.pop_back();field_names.pop_back();
	field_names.push_back("c");
	field_names.push_back("d");
	p->DropTable(relation_nameC);
	p->CreateTable(relation_nameC,field_names,field_types);

	string relation_nameD="D";
	field_names.pop_back();field_names.pop_back();
	field_names.push_back("d");
	field_names.push_back("e");
	p->DropTable(relation_nameD);
	p->CreateTable(relation_nameD,field_names,field_types);

	string relation_nameE="E";
	field_names.pop_back();field_names.pop_back();
	field_names.push_back("e");
	field_names.push_back("f");
	p->DropTable(relation_nameE);
	p->CreateTable(relation_nameE,field_names,field_types);

	string relation_nameF="F";
	field_names.pop_back();field_names.pop_back();
	field_names.push_back("f");
	field_names.push_back("a");
	p->DropTable(relation_nameF);
	p->CreateTable(relation_nameF,field_names,field_types);



 

	vector<string> relation_names;
	relation_names.push_back(relation_nameA);
	relation_names.push_back(relation_nameB);
	relation_names.push_back(relation_nameC);
	relation_names.push_back(relation_nameD);
	//relation_names.push_back(relation_nameE);
	//relation_names.push_back(relation_nameF);
	

	vector<string> iFileNameA;
	iFileNameA.push_back("a");
	iFileNameA.push_back("b");
	int numOfTuplesInA=12;
	for(int i=0;i<numOfTuplesInA;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameA,iFileNameA,s,a);
	}
	p->displayRelation(relation_nameA);

	vector<string> iFileNameB;
	iFileNameB.push_back("b");
	iFileNameB.push_back("c");
	int numOfTuplesInB=15;
	for(int i=0;i<numOfTuplesInB;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameB,iFileNameB,s,a);
	}
	p->displayRelation(relation_nameB);



	
	vector<string> iFileNameC;
	iFileNameC.push_back("c");
	iFileNameC.push_back("d");
	int numOfTuplesInC=10;
	for(int i=0;i<numOfTuplesInC;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameC,iFileNameC,s,a);
	}


	vector<string> iFileNameD;
	iFileNameD.push_back("d");
	iFileNameD.push_back("e");
	int numOfTuplesInD=50;
	for(int i=0;i<numOfTuplesInD;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameD,iFileNameD,s,a);
	}

	vector<string> iFileNameE;
	iFileNameE.push_back("e");
	iFileNameE.push_back("f");
	int numOfTuplesInE=15;
	for(int i=0;i<numOfTuplesInE;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameE,iFileNameE,s,a);
	}

	vector<string> iFileNameF;
	iFileNameF.push_back("f");
	iFileNameF.push_back("a");
	int numOfTuplesInF=20;
	for(int i=0;i<numOfTuplesInF;i++)
	{
		vector<int> a;
		vector<string> s;
		char n='a'+(i%10);
		char m='a'+(i%10);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_nameF,iFileNameF,s,a);
	}


	cout<<"A join B has tuples "<<p->JoinOnePass(relation_nameA,relation_nameB).size()<<endl;
	cout<<"Join: "<<endl;
	vector<Tuple> z = p->JoinTables(relation_names);
	cout<<"natural join: "<<endl;
	cout<<"Tuples: "<<z.size()<<endl;
	for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
	 {  
                 cout<<*(it)<<endl;  
                 it = z.erase(it);  
	 }   

	//p->computeJoin(relation_nameA,relation_nameB).print();
	
	
}



void testForJoin()
{
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
	p->DropTable(relation_name);
	p->CreateTable(relation_name,field_names,field_types);
	vector<string> iFileName;
  
	iFileName.push_back("f1");
	iFileName.push_back("f2");
	iFileName.push_back("f3");
	iFileName.push_back("f4");
	int numOfTuplesIn1=22;
	for(int i=0;i<numOfTuplesIn1;i++)
	{
		vector<int> a;
		vector<string> s;
		a.push_back(i);
		a.push_back(numOfTuplesIn1-i);

		char n='a'+(i%26);
		char m='a'+((numOfTuplesIn1-i)%26);
		string in="a";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_name,iFileName,s,a);
	}
	p->displayRelation(relation_name);


	vector<string> field_names2;
	vector<enum FIELD_TYPE> field_types2;
	field_names2.push_back("f3");
	field_names2.push_back("f4");
	field_names2.push_back("f5");
	field_types2.push_back(INT);
	field_types2.push_back(STR20);
	field_types2.push_back(INT);

	string relation_name2="ExampleTable2";
	p->DropTable(relation_name2);
	p->CreateTable(relation_name2,field_names2,field_types2);
	vector<string> iFileName2;
  
	iFileName2.push_back("f3");
	iFileName2.push_back("f4");
	iFileName2.push_back("f5");
	int numOfTuplesIn2=22;
	for(int i=0;i<numOfTuplesIn2;i++)
	{
		vector<int> a;
		vector<string> s;
		a.push_back(i);
		a.push_back(numOfTuplesIn2-i);

		char n='a'+(i%26);
		string in="";
		s.push_back(in+n);
		p->insert(relation_name2,iFileName2,s,a);
	}
	vector<int> a;
	vector<string> s;
	a.push_back(22);
	a.push_back(7);
	s.push_back("wa");
	p->insert(relation_name2,iFileName2,s,a);
	a.pop_back();a.push_back(39);
	p->insert(relation_name2,iFileName2,s,a);

	a.clear();s.clear();
	a.push_back(21);a.push_back(33);
	s.push_back("va");
	p->insert(relation_name2,iFileName2,s,a);


	p->displayRelation(relation_name2);

	
	 cout << "Calculated Disk I/Os = " << p->IO() << endl;
	 p->IOreset();
	vector<Tuple> z =  p->JoinTwoPass(relation_name,relation_name2);
  cout<<"natural join(two pass): "<<endl;
	Relation * test=p->CreateTable("test",z);
 /* for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = z.erase(it);  
   }   */
  p->displayRelation("test");

	
  cout << "Calculated Disk I/Os = " << p->IO() << endl;


  p->RelationCount("test").print();


}


void testForCount()
{
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
	p->DropTable(relation_name);
	p->CreateTable(relation_name,field_names,field_types);
	vector<string> iFileName;
  
	iFileName.push_back("f1");
	iFileName.push_back("f2");
	iFileName.push_back("f3");
	iFileName.push_back("f4");
	int numOfTuplesIn1=40;
	for(int i=0;i<numOfTuplesIn1;i++)
	{
		vector<int> a;
		vector<string> s;
		a.push_back(i%20);
		a.push_back(numOfTuplesIn1-i);

		char n='a'+(i%26);
		char m='a'+((numOfTuplesIn1-i)%17);
		string in="";
		s.push_back(in+n);
		s.push_back(m+in);
		p->insert(relation_name,iFileName,s,a);
	}
	p->displayRelation(relation_name);

	p->RelationCount(relation_name).print();
}


void test()
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

  cout<<"One Pass Sort: "<<endl;
   z =   p->SortOnePass("ExampleTable1","f2");
 
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<*(it)<<endl;  
                 it = z.erase(it);  
   }   



  p->displayMem();
  for(int i=0;i<40;i++)
  {
	   a.pop_back();
	 a.pop_back();
	a.push_back(i%15);
	a.push_back((i%15)*2);
	s.pop_back();
    s.pop_back();
	char n='a'+(i%15);
	string in="v";
	s.push_back(in+n);
	s.push_back(n+in);
	

	p->insert(relation_name,iFileName,s,a);
  }

   cout<<"Two Pass Sort: "<<endl;
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


  /*
  z =   p->dupOnePass("ExampleTable1");
  cout<<"Duplicate elimination: "<<endl;
	i=0;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<i<<": "<<*(it)<<endl;  
                 it = z.erase(it);  
				 i++;
   }   
   */

  z =   p->dupTwoPass("ExampleTable1");
  cout<<"Duplicate elimination(two pass): "<<endl;
	i=0;
  for(vector<Tuple>::iterator it  = z.begin(); it != z.end(); )  
   {  
                 cout<<i<<": "<<*(it)<<endl;  
                 it = z.erase(it);  
				 i++;
   }   




  cout<<"?";
  return;

}
