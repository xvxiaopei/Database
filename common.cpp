#include "common.h"
#include "StorageManager/Field.h"
#include "physicalOP.h"
#include <climits>
#include <vector>
#include "StorageManager/Tuple.h"
#include "StorageManager/Relation.h"

extern physicalOP *p ;

Qtree::Qtree( int t, Qtree *p){
	type = t;
	left = NULL;right = NULL;parent = p ;
}
Qexpression::Qexpression(){
	type = INT_MIN ;
	str = "";
	left = right = NULL ;
}
Qexpression::Qexpression( int t, string s){
	type = t; 
	str = string(s) ;
	left = right = NULL;
}
Qexpression::Qexpression( int t, int n){
	type = t;
	number = n ;
	left = right = NULL;
}
Qexpression::Qexpression( int t, int p, string s){
	type = t;
	number = p;
	str = string(s);
	left = right = NULL;
}


bool Qexpression::judge(Tuple t){
	return (judge_(t).integer != 0 ) ;
}
union Field Qexpression::judge_(Tuple t ) {
	union Field lf = this->left->judge_(t) ;
	union Field rf = this->right->judge_(t) ;
	union Field ret ;
	ret.integer = INT_MIN ;
	switch(this->type){
	case COLUMN:{
		int found = this->str.find('.');
		if(found == std::string::npos){
			ret = t.getField(this->str) ;
		}else{
			string s( this->str.substr(found + 1) ) ;
			ret = t.getField(s );
			if( ret.integer == INT_MIN){
				ret = t.getField(this->str) ;
			}
			if(ret.integer == INT_MIN){
				perror("No such field") ;
			}
		}
		return ret ;

	}
	break ;
	case INTEGER:{
		union Field f ;
		f.integer = this->number;
		return f;
	} break;
	case LITERAL:{
		union Field f;
		f.str = &(this->str );
		return f;
	}break;
	case OPERATER:{
		switch(this->str[0]){
		case '=':{
			union Field lf = this->left->judge_(t) ;
			union Field rf = this->right->judge_(t) ;
			union Field ret ;
			if(lf.integer == rf.integer){
				ret.integer = 1;
			}else if( ! lf.str->compare( *(rf.str)  ) ){
				ret.integer = 1;
			}else {
				ret.integer = 0 ;
			}
			return ret ;
		}break;

		case '>':
		case '<':{
			union Field lf = this->left->judge_(t) ;
			union Field rf = this->right->judge_(t) ;
			union Field ret ;
			if(this->str[0] == '>'){
				ret.integer = (lf.integer > rf.integer ? 1 : 0 );
			}else{
				ret.integer = (lf.integer < rf.integer ? 1 : 0 );
			}
			return ret;
		}break;

		case '+':{ ret.integer = (lf.integer + rf.integer ) ;return ret;	}break ;
		case '-':{ ret.integer = (lf.integer - rf.integer ) ;return ret;	}break ;
		case '*':{ ret.integer = (lf.integer * rf.integer ) ;return ret;	}break ;
		case '/':{ ret.integer = (lf.integer / rf.integer ) ;return ret;	}break ;

		case 'A':{ ret.integer = (lf.integer && rf.integer) ;return ret;    }break ;
		case 'O':{ ret.integer = (lf.integer || rf.integer) ;return ret;    }break ;
		case 'N':{ ret.integer = (! lf.integer);             return ret;    }break ;
		}
	}
	default:{perror("Illegal type");} break;
	}
	return ret ;
}
void Qexpression::free(){
	if(left != NULL){left->free();}
	if(right!=NULL){right->free();}
	delete this;
}
void Qexpression::print(int level ){
	int i ;
	for ( i = level ; i > 0 ; i --){ cout<< "\t" ;}
	if (type == OPERATER){
		cout << str << endl ;
		if(str[0] == 'N'){
			this->left->print(level + 1); 
		}else{
			this->left->print(level + 1);
			this->right->print(level + 1) ;
		}
	}else if (type == LITERAL || type == COLUMN){
		cout << str << endl;
	}else if ( type == INTEGER ){
		cout << number << endl;
	}
}
void Qtree::print(int level ){
	int i ;

	for ( i = level ; i > 0 ; i --){ cout<< "\t" ;}
	switch(type) {
		case PI: cout << "π " << "\t["; break;
		case SIGMA: cout << "σ " << "\t[";break;
		case DELTA: cout << "δ " << "\t[";break;
		case JOIN: cout << "X " << "\t["; break;
		case TAU: cout << "τ" << "\t["; break; 
		case TABLE: cout << "TB" << "\t["; break ;
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
	delete this;
}
Relation* Qtree::exec_(){
}
vector<Tuple> Qtree::exec(){
	vector<Tuple> ret ;
	#ifdef DEBUG
	this->print(0);
	#endif
	if(this->type == PI){

	}else if(this->type == JOIN){
	}else if(this->type == TABLE){
		ret = p->singleTableSelect(this->info[0], NULL);
		vector<string> field_names = 
			p->schema_manager.getSchema(this->info[0] ).getFieldNames() ; 
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			cout<< *it << ' ' ;
		} cout << endl << "-----------------" << endl;
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
int noperands(string s){
	switch(s[0]){
	case '*':
	case '/':
	case '+': 
	case '-':
	case '=':
	case '<':
	case '>':
	case 'A':
	case 'O':
	return 2;
	break;

	case 'N':
	return 1;
	break; 
	}
	return INT_MIN ;
}
int precedence(string s){
	switch(s[0]){
	case '*':
	case '/':
	return TIMES_DIVIDES;
	break;
	
	case '+': 
	case '-':
	return PLUS_MINUS;
	break;

	case '=':
	case '<':
	case '>':
	return COMPARE;
	break;
	
	case 'N':
	return NOT_PCD;
	break; 

	case 'A':
	return AND_PCD;
	break;

	case 'O':
	return OR_PCD ;
	break;
	}
	return INT_MIN;
}

