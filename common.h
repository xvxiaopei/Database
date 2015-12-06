#ifndef __COMMON_H__
#define __COMMON_H__

//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include "StorageManager/Tuple.h"
#include "StorageManager/Field.h"
#include "StorageManager/Relation.h"

using namespace std;
void err_out_START(const char str[]);
void scan_over(int statement);
	

#define GREEN_TEXT  "\e[1;32m"
#define RED_TEXT "\e[1;31m"
#define NORMAL_TEXT "\e[0m"
#define HLINE  "---------------"


enum Qtree_TYPE {
DELTA ,
PI   , 
SIGMA, 
PRODUCT , 
TAU  , 
TABLE, 
INS  }; 

enum Qexp_TYPE{
Qexp_ILLEGAL ,
COLUMN  ,
LITERAL, 
INTEGER,  
OPERATER ,
LEFT 
};

/* precedence  */

#define TIMES_DIVIDES 5
#define PLUS_MINUS    4
#define COMPARE       3
#define NOT_PCD       2
#define AND_PCD       1
#define OR_PCD        0

class Qtree {
	public:
	int type ; 
	vector<string> info;
	Qtree *left, *right, *parent;
	Qtree(enum Qtree_TYPE type, Qtree *parent); 
	void print( int );
	void free() ;
	vector<Tuple> exec(bool print, string *table_name);
	private:
	Relation* exec_();
}; 
class Qexpression {
	public:
	enum Qexp_TYPE type ; 
	int number;
	string str;
	set<string> tables ;
	Qexpression *left, *right;
	Qexpression() ;
	Qexpression(enum Qexp_TYPE , int);
	Qexpression(enum Qexp_TYPE , string);
	Qexpression(enum Qexp_TYPE , int, string);
	Qexpression* optimize_sigma(map<string, Qexpression *>* sigma_operation) ;
	Qexpression* optimize_join(map<string, vector<string>* >* theta_operation) ;
	bool judge(Tuple t);
	void print(int level) ;
	void free() ;
	enum FIELD_TYPE field_type(Tuple ) ;
	private:
	union Field judge_(Tuple t) ;
};

#endif
