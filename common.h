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
#include "Tuple.h"
#include "Field.h"
#include "Relation.h"

using namespace std;
void err_out_START(const char str[]);
void scan_over(int statement);
	

#define GREEN_TEXT  "\e[1;32m"
#define RED_TEXT "\e[1;31m"
#define NORMAL_TEXT "\e[0m"
#define HLINE  "---------------"

enum Qtree_TYPE {
    DELTA ,//duplicate elimination operation
    PI ,   //projection operation
    SIGMA, //select operation
    PRODUCT ,//cross-join operation, 
             //which can optimized to natural join operation, 
             //for some cross-join operation and select operation
    TAU  ,//sorting operation
    TABLE,//table scan operation: get all tuples from specified table
    INS   //Insert operation: insert selected tuples to a table
};

enum Qexp_TYPE{
    Qexp_ILLEGAL ,
    COLUMN  ,//leaf tree node with a column name
    LITERAL, //leaf tree node with a string
    INTEGER, //leaf tree node with an integer
    OPERATER ,//an operator tree node
    LEFT      //special node for left parenthesis: used with the operator stack 
};

/* precedence  */
#define TIMES_DIVIDES 5//* /
#define PLUS_MINUS    4//+ -
#define COMPARE       3//comparing operators including >,<,and =
#define NOT_PCD       2//NOT
#define AND_PCD       1//AND
#define OR_PCD        0//OR

class Qtree {
	public:
	int type ; 
	vector<string> info;
	Qtree *left, *right, *parent;
	Qtree(enum Qtree_TYPE type, Qtree *parent); 
	void print( int );
	void free() ;
	vector<Tuple> exec(bool print, string *table_name);
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
	Qexpression* optimize_join(vector<string> &commons, map<string, bool> &joined_key) ;
	bool judge(Tuple t);
	void print(int level) ;
	void free() ;
	enum FIELD_TYPE field_type(Tuple ) ;
	private:
	union Field judge_(Tuple t) ;
};

#endif
