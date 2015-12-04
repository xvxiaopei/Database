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
		ret = p->schema_manager.getRelation(this->tables[0] ) ;
	}
	return ret;
}
Qexpression* Qexpression::optimize_sigma(map<string, Qexpression *>* sigma_operation) {
	if(this->type == COLUMN){
		(*sigma_operation)[ * (this->tables.begin() ) ] = this ;
		return NULL ;
	}else if(this->type == OPERATER && this->str[0] == 'O'){
		if(this->tables.size() == 1){
			if(sigma_operation == NULL){
				return this ;
			}else{
				(*sigma_operation)[ * (this->tables.begin() ) ] = this ;
				return NULL;
			}
		}else if(this->tables.size() > 1){
			Qexpression *lqexp = this->left->optimize_sigma(NULL);
			Qexpression *rqexp = this->right->optimize_sigma(NULL);
			this->left = lqexp ;
			this->right = rqexp ;
			return this;
		}else{
			Tuple *t = NULL;
			if(this->judge(*t)){
				Qexpression *ret = new Qexpression(INTEGER, 1);
				this->free() ;
				return ret;
			}else{
				Qexpression *ret = new Qexpression(INTEGER, 0);
				this->free() ;
				return ret;
			}
		}
	}else if(this->type == OPERATER && this->str[0] == 'A'){
		if(this->tables.size() == 1){
			if(sigma_operation == NULL){
				return this ;
			}else{
				(*sigma_operation)[* (this->tables.begin() ) ] = this ;
				return NULL;
			}
		}else if(this->tables.size() > 1){
			Qexpression *lqexp = this->left->optimize_sigma(sigma_operation);
			Qexpression *rqexp = this->right->optimize_sigma(sigma_operation);
			if(lqexp != NULL && rqexp != NULL){
				this->left = lqexp ;
				this->right = rqexp ;
				return this;
			}else if(lqexp == NULL && rqexp != NULL){
				return rqexp ;
			}else if(lqexp != NULL && rqexp == NULL){
				return lqexp ;
			}else{
				return NULL;
			}
		}else{
			Tuple *t = NULL;
			if(this->judge(*t)){
				Qexpression *ret = new Qexpression(INTEGER, 1);
				this->free() ;
				return ret;
			}else{
				Qexpression *ret = new Qexpression(INTEGER, 0);
				this->free() ;
				return ret;
			}
		}
	}else if(this->type == OPERATER && this->str[0] == '='||
	this->type == OPERATER && this->str[0] == '>'||
	this->type == OPERATER && this->str[0] == '<'||
	this->type == OPERATER && this->str[0] == 'N'){
		
		if(this->tables.size() == 1){
			if(sigma_operation == NULL){
				return this ;
			}else{
				(*sigma_operation)[* (this->tables.begin() ) ] = this ;
				return NULL;
			}
		}else if(this->tables.size() > 1){
			return this;
		}else{
			Tuple *t = NULL;
			if(this->judge(*t)){
				Qexpression *ret = new Qexpression(INTEGER, 1);
				this->free() ;
				return ret;
			}else{
				Qexpression *ret = new Qexpression(INTEGER, 0);
				this->free() ;
				return ret;
			}
		}
	}else{
		return this ;
	}
}
vector<Tuple> Qtree::exec(){
	vector<Tuple> ret ;
	#ifdef DEBUG
	this->print(0);
	#endif
	if(this->type == PI){

	}else if(this->type == PRODUCT){
		vector<Relation *> relations ;
		map<string, Qexpression *> sigma_operation ;
		
		for(vector<string>::iterator it = this->tables.begin(); it != this->tables.end(); it++){

		}
		if(output_s.empty() == true){
		}else if(output_s.top()->type == INTEGER || output_s.top()->type == LITERAL ){
			Tuple *t = NULL;
			if(output_s.top()->judge(*t) == true){
				/* WHERE clasuse always true */
		 		output_s.top()->free() ; while(! output_s.empty() ){ output_s.pop();}
			}else{
				/* empty results */
				return ret; 
			}
		}else{
			Qexpression *optimized = output_s.top()->optimize_sigma(&sigma_operation) ;
			output_s.pop(); output_s.push(optimized) ;
			output_s.top()->print(0);
			for(map<string, Qexpression *>::iterator it = sigma_operation.begin(); it != sigma_operation.end(); it ++){
				cout << it->first << "->" << endl;
				it->second->print(0);
			}
		}
		//ret = p->singleTableSelect(r, output_s.top() ) ;
		
	}else if(this->type == TABLE){
		ret = p->singleTableSelect(this->tables[0], output_s.empty() ? NULL : output_s.top() );
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
