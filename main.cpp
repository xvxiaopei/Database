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
Qexpression* Qexpression::optimize_join(map<string, vector<string>* >* commons){
	if(this->type == OPERATER && this->str[0] == 'A'){
		if(this->tables.size() >= 2){
			Qexpression *lqexp = this->left->optimize_join(commons) ;
			Qexpression *rqexp = this->right->optimize_join(commons) ;
			if(lqexp != NULL && rqexp != NULL){
				this->left = lqexp ;
				this->right = rqexp ;
				return this;
			}else if(lqexp == NULL && rqexp != NULL){
				delete this;
				return rqexp ;
			}else if(lqexp != NULL && rqexp == NULL){
				delete this;
				return lqexp ;
			}else{
				delete this;
				return NULL;
			}
		}else{
			return this;
		}
	}else if(this->type == OPERATER && this->str[0] == '=' &&
	this->left->type == COLUMN && this->right->type == COLUMN ){
		if(this->tables.size() == 2){
			string key, field0, field1;
			set<string>::iterator it = this->tables.begin();
			it++;
			if(this->tables.begin()->compare(* (it  ) ) >= 0){
				key = string( *(this->tables.begin() ) + "-x-"+ *(it ) ) ;
			}else{
				key = string( *(it) + "-x-"+*(this->tables.begin() ) ) ;
			}
			field0 = string(this->left->str.substr(this->left->str.rfind('.') + 1) ) ;
			field1 = string(this->right->str.substr(this->right->str.rfind('.') + 1) ) ;
			
			if(field1.compare(field0) == 0){
				int found ;
				if(  (*commons)[ key ] == NULL){
					vector<string> *v = new vector<string> ;
					(*commons)[ key ] = v;
					v->push_back(field0) ;
					return NULL;
				}else{
					for(vector<string>::iterator it = (*commons)[ key ]->begin(); 
					it != (*commons)[ key ]->end(); it++){
						if( it->compare(field0) == 0){
							return NULL ;
						}
					}
					(*commons)[ key ]->push_back(field0 ) ; 
					return NULL ;
				}
			}else{
				return this;
			}
		}else{
			return this;
		}
	}else{
		return this ;
	}
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
				delete this;
				return rqexp ;
			}else if(lqexp != NULL && rqexp == NULL){
				delete this;
				return lqexp ;
			}else{
				delete this;
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
	if(this->type == DELTA){

		return ret;//TODO remove this
	}else if(this->type == PI){

		return ret;//TODO remove this
	}else if(this->type == PRODUCT){
		vector<string> ptables;
		vector<Relation *> relations ;
		map<string, Qexpression *> sigma_operation ;
		map<string, vector<string>* > commons ;
		vector<string>::iterator it = ptables.begin();

		ptables.insert(ptables.end(), this->tables.begin(), this->tables.end() );
		
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
			output_s.pop(); if(optimized != NULL){ output_s.push(optimized) ;}
			
			for(map<string, Qexpression *>::iterator it = sigma_operation.begin(); it != sigma_operation.end(); it ++){
				cout << it->first << "->" << endl;
				it->second->print(0);
			}

			if(output_s.empty() != true){
				optimized = output_s.top()->optimize_join(&commons) ;
				output_s.pop(); if(optimized != NULL){ output_s.push(optimized) ;}

				for(map<string, vector<string>* >::iterator it = commons.begin(); it != commons.end(); it ++){
					cout << it->first << "->" <<(*(it->second))[0] << endl ;
				}
				if(output_s.empty()){}else{ output_s.top()->print(0); }
			}
			
		}
		vector<string> to_drop ;
		for(vector<string>::iterator it = ptables.begin(); it != ptables.end(); ){
			if(sigma_operation[*it] == NULL){
				it++;
			}else{
				vector<Tuple> tuples  =  p->singleTableSelect( *it  , sigma_operation[*it] ) ;
				Relation *temp_relation = p->CreateTable( ( *it) + "-SIGMA",  tuples) ;
				to_drop.push_back( temp_relation->getRelationName() ) ;
				it = ptables.erase(it) ;ptables.push_back(  temp_relation->getRelationName() ) ;
			}
		}
		if(ptables.size() == 2){
			ret = p->JoinTwoPass(ptables[0], ptables[1] ) ;
		}else{
			ret = p->JoinTables(ptables) ;
		}
		for(vector<string>::iterator it = to_drop.begin(); it != to_drop.end(); it++){
			p->DropTable(*it) ;
		}

		//ret = p->singleTableSelect(r, output_s.top() ) ;
		
	}else if(this->type == TABLE){
		ret = p->singleTableSelect(this->tables[0], output_s.empty() ? NULL : output_s.top() );
	}else{
		return ret;
	}
	vector<string> field_names = 
		ret[0].getSchema( ).getFieldNames() ;
	for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
		cout<< *it << ' ' ;
	} cout << endl << "-----------------" << endl ;
	for(vector<Tuple>::iterator it = ret.begin(); it != ret.end(); it ++ ){
		cout << (*it) << endl;
	}cout <<  "-----------------" << endl << endl ;
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
