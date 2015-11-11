#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>

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
Qtree::Qtree( int t){
	type = t;
	left = NULL;right = NULL;
}
void Qtree::print(int level ){
	int i ;

	for ( i = level ; i > 0 ; i --){ cout<< "\t" ;}
	switch(type) {
	case PI: cout << "π " << "\t["; break;
	case SIGMA: cout << "σ " << "\t[";break;
	case DELTA: cout << "δ " << "\t[";break;	
	case JOIN: cout << "X " << "\t["; break;
	}
	for(i = 0; i < info.size(); i++){
		cout << info[i] << " " ;
	} cout << "]" << endl ;
	if (left != NULL){left->print(level + 1) ;}
	if(right != NULL){ right->print(level + 1) ; }
}

void Qtree::free(){
	info.clear();
	if(left != NULL){left->free();}
	if(right!=NULL){right->free();}
}
void Qtree::exec(){
	print(0);	
}
#endif
