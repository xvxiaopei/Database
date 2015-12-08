#include "lex.yy.cpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "physicalOP.h"
#include "common.h"

extern physicalOP *p ;
extern stack<Qexpression*> output_s;
extern vector<string> temp_relations ;

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
Qexpression* Qexpression::optimize_join(vector<string> &commons, map<string, bool> &joined_key ){
	if(this->type == OPERATER && this->str[0] == 'A'){
		if(this->tables.size() >= 2){
			Qexpression *lqexp = this->left->optimize_join(commons, joined_key) ;
			Qexpression *rqexp = this->right->optimize_join(commons , joined_key) ;
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
			string table0, table1 ;
			unsigned long pos0 = this->left->str.rfind('.'), pos1 = this->right->str.rfind('.') ;
			set<string>::iterator it = this->tables.begin();
			it++;

			field0 = string(this->left->str.substr(pos0 + 1) ) ;
			field1 = string(this->right->str.substr( pos1 + 1) ) ;
			table0 = string(this->left->str.substr(0, pos0));
			table1 = string(this->right->str.substr(0, pos1));
			if(field0 == field1){
				if(table0 <= table1){
					key = string( table0 + "-x-"+ table1 ) ;
					if(joined_key[key] == false){
						commons.push_back(this->left->str);
						commons.push_back(this->right->str);
						joined_key[key] = true;
					}
				}else{
					key = string( table1 + "-x-"+ table0 ) ;
					if(joined_key[key] == false){
						commons.push_back(this->right->str);
						commons.push_back(this->left->str);
						joined_key[key] = true;
					}
				}
				return NULL;
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
	}else if( ( this->type == OPERATER && this->str[0] == '=' ) ||
	( this->type == OPERATER && this->str[0] == '>')||
	( this->type == OPERATER && this->str[0] == '<')||
	( this->type == OPERATER && this->str[0] == 'N' ) ){
		
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
vector<Tuple> Qtree::exec(bool print, string *table_name){
	vector<Tuple> ret ;
	#ifdef DEBUG
	this->print(0);
	#endif
	if(this->type == INS){
		vector<Tuple> temp = this->left->exec( false, NULL ) ;
		if(temp.size() != 0){
			Schema sins_from  =  temp[0].getSchema() ;
			vector<enum FIELD_TYPE> field_types_from = sins_from.getFieldTypes() ;
			vector<string> field_names_from = sins_from.getFieldNames() ;
			if(field_types_from.size() == this->info.size() - 1){
				Schema sins_to = p->schema_manager.getSchema( this->info[0] ) ;
				vector<enum FIELD_TYPE> field_types_to ;
				vector<union Field> fields ;
				vector<string>::iterator it0 = this->info.begin() ;
				vector<enum FIELD_TYPE>::iterator it1 = field_types_from.begin();
				vector<string>::iterator it2 = field_names_from.begin();
				vector<string> STRv;
				vector<int> INTv ;
				string table_n = (*it0)  ;
				vector<string> field_names_to ;
				it0 ++ ;
				for( ; it0 != this->info.end()  ; it0 ++, it1++){
					unsigned long found = it0->rfind('.')  ;
					string s_table ;
					if(found == std::string::npos){
						s_table = string( table_n + "." + (*it0) ) ;
					}else{
						s_table = string( it0->substr( it0->rfind('.') + 1 )  ) ;
					}
					if( sins_to.fieldNameExists( *it0 ) ){
						field_names_to.push_back(string(  *it0)  ) ;
						if(sins_to.getFieldType( *it0) == *it1 ){
						}else{
							perror(  ": Type mismatch");
							return ret; 
						}
					}else{
						if(sins_to.fieldNameExists(s_table) ) {
							field_names_to.push_back(string(  s_table ) ) ;
							if(sins_to.getFieldType( s_table) == *it1 ){
							}else{
								perror( ": Type mismatch");
								return ret; 
							}
						} else{
								perror( "exec: No such field");
						}
					}
				}	
				for(vector<Tuple>::iterator it_tuple = temp.begin(); it_tuple != temp.end(); it_tuple ++) {
					for(it1 = field_types_from.begin(), it2 = field_names_from.begin()  ; 
					it1 != field_types_from.end()  ; it1++, it2++){
						if(*it1 == INT){
							INTv.push_back( it_tuple->getField( *it2).integer ) ;
						}else{
							STRv.push_back( *(it_tuple->getField( *it2).str) ) ;
						}
					}
					p->insert(table_n, field_names_to, STRv, INTv) ;
					INTv.clear();
					STRv.clear() ;
				}
			}else{
				perror("Size mismatch");
				return ret;
			}
		}else{
			return ret;
		}
	}else if(this->type == TAU){
		string table_n;
		if(this->left->type == TABLE && (output_s.empty() || output_s.top() == NULL) ){
			Schema s = p->schema_manager.getSchema( this->left->info[0] ) ;
			string s_table ;
			unsigned long found = this->info[0].rfind('.')  ;
			table_n = this->left->info[0] ;
			if(found == std::string::npos){
				s_table = string( table_n + "." + this->info[0]  ) ;
			}else{
				s_table = string( this->info[0].substr( this->info[0].rfind('.') + 1 )  ) ;
			}
			if( s.fieldNameExists( this->info[0] ) ){
				ret = p->SortTwoPass(table_n, this->info[0])  ;
			}else if(s.fieldNameExists(s_table) ) {
				ret = p->SortTwoPass(table_n, s_table)  ;
			}else{
				perror("No such field");
				return ret ;
			}
		}else{
			vector<Tuple> temp = this->left->exec( false, &table_n ) ;
			if(table_name != NULL) { (*table_name ) = string( this->info[0] ) ;}
			if(temp.size() != 0){
				Schema s  =  temp[0].getSchema() ;
				string temp_table_name = "temp_table" ;
				while(p->schema_manager.relationExists(temp_table_name) ){
					temp_table_name += "-a" ;
				}
				p->CreateTable(temp_table_name, temp ) ;
				temp_relations.push_back( temp_table_name ) ;
				unsigned long found = this->info[0].rfind('.')  ;
				string s_table ;
				if(found == std::string::npos){
					s_table = string( table_n + "." + this->info[0]  ) ;
				}else{
					s_table = string( this->info[0].substr( this->info[0].rfind('.') + 1 )  ) ;
				}
				if( s.fieldNameExists( this->info[0] ) ){
					ret = p->SortTwoPass(temp_table_name, this->info[0])  ;
				}else if(s.fieldNameExists(s_table) ) {
					ret = p->SortTwoPass(temp_table_name, s_table)  ;
				}else{
					perror("No such field");
					return ret ;
				}
			}else{
				return ret;
			}
		}
	}else if(this->type == DELTA ){
		string table_n;
		if(this->left->type == TABLE){
			table_n = this->left->info[0] ;
			ret = p->dupTwoPass(table_n) ;
		}else{
			vector<Tuple> temp = this->left->exec( false , &table_n) ;
			if(table_name != NULL) { (*table_name ) = string( this->info[0] ) ;}
	
			if(temp.size() != 0){
				Schema s  =  temp[0].getSchema() ;
				string temp_table_name = "temp_table" ;
				while(p->schema_manager.relationExists(temp_table_name) ){
					temp_table_name += "-a" ;
				}
				p->CreateTable(temp_table_name, temp );
				temp_relations.push_back(temp_table_name  ) ;
				ret = p->dupTwoPass(temp_table_name) ;
			}else{
				return ret;
			}
		}
	}else if(this->type == PI ){
		string table_n;
		vector<Tuple> temp = this->left->exec( false, &table_n ) ;
		if(table_name != NULL) { (*table_name ) = string( this->info[0] ) ;}
		if(temp.size() != 0){
			Schema s  =  temp[0].getSchema() ;
			vector<string> field_names ;
			vector<enum FIELD_TYPE> field_types  ;
			for(vector<string>::iterator it= this->info.begin(); it != this->info.end(); it++){
				unsigned long found = it->rfind('.')  ;
				string s_table ;
				if(found == std::string::npos){
					s_table = string( table_n + "." + (*it) ) ;
				}else{
					s_table = string( it->substr( it->rfind('.') + 1 )  ) ;
				}
				if( s.fieldNameExists( *it ) ){
					field_names.push_back(string(*it) ) ;
					field_types.push_back(s.getFieldType( *it) ) ;
				}else{
					if(s.fieldNameExists(s_table) ) {
						field_names.push_back(string( s_table ) ) ;
						field_types.push_back( s.getFieldType( s_table )  );
					} else{
						perror( "exec: No such field");
					}
				}
			}
			string temp_table_name = "temp_table" ;
			Relation *rlt = NULL;
			while(p->schema_manager.relationExists(temp_table_name) ){
				temp_table_name += "-a" ;
			}
			rlt = p->CreateTable(temp_table_name, field_names, field_types) ;
			temp_relations.push_back(temp_table_name  ) ;
			for(vector<Tuple>::iterator tit = temp.begin(); tit != temp.end(); tit++){
				Tuple t = rlt->createTuple() ;
	
				for(vector<string>::iterator it = field_names.begin(); it != field_names.end() ; it++){
					union Field f= tit->getField(*it) ;
					if( s.getFieldType(*it) == INT ){
						t.setField(  *it,  f.integer ) ;
					}else{
						t.setField( *it, *(f.str)) ;
					}
				}
				ret.push_back( t ) ;
			}
		}else{
			return ret;
		}
	}else if(this->type == PRODUCT){
		vector<string> ptables;
		vector<Relation *> relations ;
		map<string, Qexpression *> sigma_operation ;
		vector<string> commons ;
		map<string, bool> joined_keys;

		vector<string>::iterator it = ptables.begin();

		ptables.insert(ptables.end(), this->info.begin(), this->info.end() );
		
		if(output_s.empty() ){
		}else if(output_s.top()->type == INTEGER || output_s.top()->type == LITERAL ){
			Tuple *t = NULL;
			if(output_s.top()->judge(*t) ){
				/* WHERE clasuse always true */
		 		 while(! output_s.empty() ){ output_s.top()->free() ;output_s.pop();}
			}else{
				/* empty results */
				return ret; 
			}
		}else{
			Qexpression *optimized = output_s.top()->optimize_sigma(&sigma_operation) ;
			output_s.pop(); if(optimized != NULL){ output_s.push(optimized) ;}
			
			#ifdef DEBUG
			for(map<string, Qexpression *>::iterator it = sigma_operation.begin(); it != sigma_operation.end(); it ++){
				cout << it->first << "->" << endl;
				it->second->print(0);
			}
			#endif

			if( ! output_s.empty() ){
				optimized = output_s.top()->optimize_join(commons, joined_keys) ;
				output_s.pop(); 
				if(optimized != NULL){ 
					output_s.push(optimized) ;
				}else{
					while(! output_s.empty() ){output_s.top()->free() ; output_s.pop();}
				}

				if(! output_s.empty()){
					#ifdef DEBUG
					output_s.top()->print(0); 
					#endif
				}
			}
			#ifdef DEBUG
			cerr << "commons: ";
			for(vector<string>::iterator it = commons.begin(); it != commons.end(); it++){
				cerr<< *it << " " ;
			}
			cerr << endl ;
			#endif
		}
		vector<string> to_drop ;
		for(vector<string>::iterator it = ptables.begin(); it != ptables.end(); ){
			if(sigma_operation[*it] == NULL){
				it++;
			}else{
				Relation *temp_relation;
				vector<Tuple> tuples  =  p->singleTableSelect( *it  , sigma_operation[*it] ) ;
				if(tuples.size() != 0){
					temp_relation = p->CreateTable( ( *it) + "-SIGMA",  tuples) ;
				}else{
					vector<string> field_names = p->schema_manager.getRelation(*it)->getSchema().getFieldNames(); 
					vector<enum FIELD_TYPE> field_types =  p->schema_manager.getRelation(*it)->getSchema().getFieldTypes() ;
					temp_relation = p->CreateTable( (*it) + "-SIGMA" , field_names, field_types ) ;
				}
				to_drop.push_back( temp_relation->getRelationName() ) ;
				it = ptables.erase(it) ;ptables.insert( it, temp_relation->getRelationName() ) ;
			}
		}
		if(ptables.size() == 2){
			if(ptables[0] <= ptables[1]){
				ret = p->JoinTwoPass(ptables[0], ptables[1], commons ) ;
			}else{
				ret = p->JoinTwoPass(ptables[1], ptables[0], commons ) ;
			}
		}else{
			ret = p->JoinTables(ptables, commons) ;
		}
		for(vector<string>::iterator it = to_drop.begin(); it != to_drop.end(); it++){
			p->DropTable(*it) ;
		}
		if(output_s.empty() ){
		}else{
			string temp_table_name = "temp_table";
			while(p->schema_manager.relationExists(temp_table_name)) {
				temp_table_name += "-a";
			}
			p->CreateTable( temp_table_name, ret ) ;
			temp_relations.push_back(temp_table_name) ;

			ret = p->singleTableSelect(temp_table_name, output_s.top() ) ;
		}
	}else if(this->type == TABLE){
		if(table_name != NULL) { (*table_name ) = string( this->info[0] ) ;}
		ret = p->singleTableSelect(this->info[0], output_s.empty() ? NULL : output_s.top() );
	}else{
		return ret;
	}
	if(ret.size() != 0 && print){
		vector<string> field_names = 
			ret[0].getSchema( ).getFieldNames() ;
		cout <<  "-----------------" << endl ;
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			cout<< *it << ' ' ;
		} cout << endl << "-----------------" << endl ;
		for(vector<Tuple>::iterator it = ret.begin(); it != ret.end(); it ++ ){
			cout << (*it) << endl;
		}cout <<  "-----------------" << endl ;
	}
	return  ret;
}
int main( int argc, char **argv ){
	p = physicalOP::getInstance();

	/* For debug */
	#ifdef DEBUG
		error_output.rdbuf(std::cerr.rdbuf() );
	#else
		cerr.rdbuf(error_output.rdbuf() );
	#endif
	
	++argv, --argc;  /* skip over program name */
	if ( argc > 0 ){
		yyin = fopen( argv[0], "r" );
	}else{
		yyin = stdin;
	}
	yylex();
}
