#include "common.h"
#include "lex.yy.cpp"
#include "physicalOP.h"
#include <climits>


Qtree::Qtree( int t){
	type = t;
	left = NULL;right = NULL;
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
bool Qexpression::judge(Tuple *t ) {
	Qexpression *r = this->right ;
	Qexpression *l = this->left ;
	int rt = r->type ;
	int lt = l->type ;
	string s ; 
	switch(precedence(this->str)){
	case PLUS_MINUS:
	case TIMES_DIVIDES:
	if(rt == COLUMN && lt == COLUMN){

	}else if(rt==COLUMN || lt == COLUMN){

	}else{
		if(lt == rt ){
			if(lt == INTEGER){
				switch(this->str[0]){
				case '+':{return l->number + r->number;} break;
				case '-':{return l->number - r->number;} break;
				case '*':{return l->number * r->number;} break;
				case '/':{return l->number / r->number;} break;
				}
			}else if(lt == LITERAL){
				/* TODO literal plus/minus literal */
				return false; 
			}else{
				return false;
			}
		}else{
			return false;
		}
	}
	break;
	case AND_PCD:
		return l->judge(t) && r->judge(t) ;
	break ;
	case OR_PCD:
		return l->judge(t) || r->judge(t) ;
	break ;
	case NOT_PCD:
		return (! l->judge(t) ) ;
	break ;
	case COMPARE:{
		if( rt == COLUMN && lt == COLUMN) {
			Qexpression *col0 = l, *col1 = r;
			union Field fld0 , fld1;
			int found;
			found = col0->str.find('.');
			if(found == std::string::npos){
				string s(col0->str) ;
				fld0 = t->getField(s) ;//Same name TODO
			}else{
				string s( col0->str.substr(found + 1) ) ;
				fld0 = t->getField(s) ;//Same name TODO
				if(fld0.integer == INT_MIN) {
					fld0 = t->getField(col0->str) ;
				}
				if(fld0.integer == INT_MIN) {
					perror("No such field");
					exit(EXIT_FAILURE);
				}
			}
			found = col1->str.find('.');
			if(found == std::string::npos){
				string s(col1->str) ;
				fld1 = t->getField(s) ;//Same name TODO
			}else{
				string s( col1->str.substr(found + 1) ) ;
				fld1 = t->getField(s) ;//Same name TODO
				if(fld1.integer == INT_MIN) {
					fld1 = t->getField(col1->str) ;
				}
				if(fld1.integer == INT_MIN) {
					perror("No such field");
					exit(EXIT_FAILURE);
				}
			}
			if(this->str[0] == '='){
				if( fld0.integer == fld1.integer){
					return true;
				}else if(fld0.str->compare(*fld1.str) == 0){
					return true;
				}else{ /* STR no > < */
					return false;
				}
			}else if(this->str[0] == '>'){
				return fld0.integer > fld1.integer;
			}else if(this->str[0] == '<'){
				return fld0.integer < fld1.integer;
			}
			
			
		}else if (rt == COLUMN || lt == COLUMN){
			int constantp ; Qexpression *col , *constant ;
			if(rt == COLUMN){
				constantp = lt;
				constant = l;
				col = r ;
			}else{
				constantp = rt; 
				constant = r;
				col = l ;
			}
			
			int found = col->str.find('.');
			union Field fld ;
			if(found == std::string::npos){
				string s(col->str) ;
				fld = t->getField(s) ;//Same name TODO
			}else{
				string s( col->str.substr(found + 1) ) ;
				fld = t->getField(s) ;//Same name TODO
				if(fld.integer == INT_MIN) {
					fld = t->getField(col->str) ;
				}
				if(fld.integer == INT_MIN) {
					perror("No such field");
					exit(EXIT_FAILURE);
				}
	
			}
			

			if(constantp == LITERAL){
				if (this->str[0] == '='){
					return (constant->str.compare(*(fld.str) ) == 0) ;
				}
			}else if(constantp == INTEGER) {
				if (this->str[0] == '='){
					return fld.integer == constant->number ;
				}else if(this->str[0] == '>'){
					if(lt == COLUMN){
						return fld.integer > constant->number ;
					}else{
						return fld.integer < constant->number ;
					}
				}else if(this->str[0] == '<'){
					if(lt == COLUMN){
						return fld.integer < constant->number ;
					}else{
						return fld.integer > constant->number ;
					}
				}
			}else {
				perror("Type error: should be INTEGER or LITERAL") ;
				exit(EXIT_FAILURE);
			}
			
		}else{
			if(rt == lt){
				if(rt == INTEGER){
					if (this->str[0] == '='){
						return l->number == r->number ;
					}else if(this->str[0] == '>'){
						return l->number > r->number ;
					}else if(this->str[0] == '<'){
						return l->number < r->number ;
					}
				}else if(rt == LITERAL) {
					if( this->str[0] == '='){
						return (l->str.compare( r->str) == 0) ;
					}else{
						return false ;
					}
				}
			}else{
				return false;
			}
		}	
	}
	break;
	}
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
void Qtree::exec(){
	print(0);
}

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
