#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include "StorageManager/Tuple.h"

using namespace std;

void err_out_START(const char str[]);
void scan_over(int statement);
	

#define GREEN_TEXT  "\e[1;32m"
#define RED_TEXT "\e[1;31m"
#define NORMAL_TEXT "\e[0m"
#define HLINE  "---------------"

#define DELTA 0
#define PI    1
#define SIGMA 2
#define JOIN  3
#define TAU   4

#define COLUMN  0
#define LITERAL 1
#define INTEGER 2
#define OPERATER 3
#define LEFT 4

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
	Qtree *left, *right;
	Qtree(int type); 
	void print( int );
	void free() ;
	void exec();
}; 
class Qexpression {
	public:
	int type ; 
	int number;
	string str;
	Qexpression *left, *right;
	Qexpression(int , int);
	Qexpression(int , string);
	Qexpression(int, int, string);
	void print(int level) ;
	bool judge(Tuple *t) ;
	void free() ;
};
#endif
