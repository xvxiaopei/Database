#include "lex.yy.cpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "physicalOP.h"
#include "common.h"

extern physicalOP *p ;
extern stack<Qexpression*> output_s;

void err_out_START(const char str[]) {
	if(YY_START == D_S_EXPECT_WHERE) {
		scan_over(DELETE_STATEMENT) ;
	}
	#ifndef  NO_ERROR_OUTPUT
	error_output << GREEN_TEXT << HLINE << endl << "Scan start:\t" <<
	str << NORMAL_TEXT << endl;
	#endif
}
void scan_over(int statement){
	char buf[64];
	switch(statement){
	case CREATE_TABLE_STATEMENT:
	strcpy(buf, "CREATE TABLE") ;
	break;

	case INSERT_STATEMENT:
	strcpy(buf, "INSERT INTO") ;
	break;

	case DELETE_STATEMENT:
	strcpy(buf, "DELETE FROM") ;
	break;

	case DROP_TABLE_STATEMENT:
	strcpy(buf, "DROP TABLE") ;
	break;

	case SELECT_STATEMENT:
	strcpy(buf, "SELECT [DISTINCT]") ;
	break;

	default:
	perror("unknow statement over");
	exit(EXIT_FAILURE);
	break;
	}
	
	#ifndef  NO_ERROR_OUTPUT
	error_output << RED_TEXT << "Scan over:\t" <<
	buf <<  endl << 
	HLINE << NORMAL_TEXT <<  endl  ;
	#endif
}
Relation* Qtree::exec_(){
	Relation *ret;
	if(this->type == TABLE){
		ret = p->schema_manager.getRelation(this->info[0] ) ;
	}
	return ret;
}
vector<Tuple> Qtree::exec(){
	vector<Tuple> ret ;
	#ifdef DEBUG
	this->print(0);
	#endif
	if(this->type == PI){

	}else if(this->type == JOIN){
		Relation *ltable = this->left->exec_();
		Relation *rtable = this->right->exec_() ;
		//ret = p->singleTableSelect(r, output_s.top() ) ;
		
	}else if(this->type == TABLE){
		ret = p->singleTableSelect(this->info[0], output_s.empty() ? NULL : output_s.top() );
		vector<string> field_names = 
			ret[0].getSchema( ).getFieldNames() ;
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			cout<< *it << ' ' ;
		} cout << endl << "-----------------" << endl ;
		for(vector<Tuple>::iterator it = ret.begin(); it != ret.end(); it ++ ){
			cout << (*it) << endl;
		}cout <<  "-----------------" << endl << endl ;
	}
	/*
	if(this->left == NULL && this->right == NULL){
		int i = 0;
		
	}else if(this->left != NULL && this->right == NULL){

	}else if(this->left != NULL && this->right != NULL){
	}
	*/

	return  ret;
}
int main( int argc, char **argv ){
	p = physicalOP::getInstance();

	/* For debug */
	#ifdef DEBUG
	error_output.rdbuf(std::cerr.rdbuf() );
	#endif
	
	++argv, --argc;  /* skip over program name */
	if ( argc > 0 ){
		yyin = fopen( argv[0], "r" );
	}else{
		yyin = stdin;
	}
	yylex();
}
