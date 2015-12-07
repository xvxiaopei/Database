#include "common.h"
#include "StorageManager/Field.h"
#include "physicalOP.h"
#include <climits>
#include <vector>
#include "StorageManager/Tuple.h"
#include "StorageManager/Relation.h"



Qtree::Qtree( enum Qtree_TYPE t, Qtree *p){
	type = t;
	left = NULL;right = NULL;parent = p ;
}
Qexpression::Qexpression(){
	type = Qexp_ILLEGAL ;
	str = "";
	left = right = NULL ;
}
Qexpression::Qexpression( enum Qexp_TYPE t, string s){
	type = t; 
	str = string(s) ;
	left = right = NULL;
}
Qexpression::Qexpression( enum Qexp_TYPE t, int n){
	type = t;
	number = n ;
	left = right = NULL;
}
Qexpression::Qexpression(enum Qexp_TYPE t, int p, string s){
	type = t;
	number = p;
	str = string(s);
	left = right = NULL;
}

bool Qexpression::judge(Tuple t){
	return (this->judge_(t).integer != 0 ) ;
}
enum FIELD_TYPE Qexpression::field_type(Tuple t){
	enum FIELD_TYPE ret ;
	Schema scm = t.getSchema() ;
	if(this->type == COLUMN){
		int found = this->str.find('.');
		string s ;
		if(found == std::string::npos){
			s = (*(this->tables.begin()))  + "." + this->str;
		}else{
			s = ( this->str.substr(found + 1) ) ;
		}
		if( scm.fieldNameExists(s ) == true ){
			ret = scm.getFieldType(s );
		}else if(scm.fieldNameExists(this->str) ){
			ret = scm.getFieldType(this->str) ;
		}else{
			perror("field_type: No such field");
			return INT ;
		}
		return ret ;
	}else if(this->type == INTEGER){
		return INT ;
	}else if(this->type == LITERAL){
		return STR20 ;
	}else {
		return INT ;
	}
}
union Field Qexpression::judge_(Tuple t ) {
	Schema scm = t.getSchema() ;
	union Field ret ;
	ret.integer = INT_MIN ;
	switch(this->type){
	case COLUMN:{
		int found = this->str.find('.');
		string s ;
		if(found == std::string::npos){
			s = (*(this->tables.begin()))  + "." + this->str;
		}else{
			s = ( this->str.substr(found + 1) ) ;
		}
		if(scm.fieldNameExists( s ) ){
			ret = t.getField(s );
		}else if(scm.fieldNameExists(this->str )) {
			ret = t.getField(this->str) ;
		}else{
			cerr << this->str << " " << s <<  " : " ;
			perror("judge_: No such field") ;
		}
		#ifdef DEBUG
		cerr << "column: " << ret.integer << endl;
		#endif
		return ret ;
	}
	break ;
	case INTEGER:{
		union Field f ;
		f.integer = this->number;
		#ifdef DEBUG
		cerr << "integer: " << f.integer << endl ;
		#endif
		return f;
	} break;
	case LITERAL:{
		union Field f;
		f.str = &(this->str );
		ret = f;
		return f;
	}break;
	case OPERATER:{
		union Field lf;
		lf = this->left->judge_(t) ;
		union Field rf;
		if(this->str[0] !='N') {rf = this->right->judge_(t) ;}

		switch(this->str[0]){
		case '=':{
			union Field ret ;
			if(lf.integer == rf.integer){
				ret.integer = 1;
			}else if(this->left->field_type(t) == STR20 &&
				this->right->field_type(t) == STR20 &&
				0 == lf.str->compare( *(rf.str)  ) ){
				ret.integer = 1;
			}else {
				ret.integer = 0 ;
			}
			return ret ;
		}break;

		case '>':
		case '<':{
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
		cout << str << ": "  ;
		for(set<string>::iterator it = tables.begin(); it != tables.end(); it ++){
			cout << *it << " " ;
		}cout << endl ;
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
		case PRODUCT: cout << "X " << "\t["; break;
		case TAU: cout << "τ" << "\t["; break; 
		case TABLE: cout << "TB" << "\t["; break ;
		case INS: cout << "IS" << "\t["; break;
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

