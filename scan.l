%{
#include "common.h"
#include "physicalOP.h"
#include "Field.h"
physicalOP *p = NULL;
Qtree *current_qt =NULL;
Qtree *head = NULL;
stack<Qexpression*> output_s;
stack<Qexpression*> operators_s;
int precedence(string);
int noperands(string) ;

vector<string> field_names ;
vector<enum FIELD_TYPE > field_types ;
string table_name ;
vector<int> int_values;
vector<string> str_values ;
vector<string> temp_relations ;

std::ostream error_output(NULL);

int start_time, stop_time;


%}
%option               stack

%x                    DISTINCT_STATE

%x                    WHERE_SUB_STATEMENT

%x                    DROP_TABLE_STATEMENT

%x                    SELECT_STATEMENT
%x                    S_S_EXPECT_FROM_OR_COMMA
%x                    S_S_EXPECT_TABLES
%x                    S_S_DISTINCT_STATE
%x                    S_S_EXPECT_COLUMNS
%x                    S_S_EXPECT_WHERE_OR_COMMA
%x                    S_S_EXPECT_MORE_TABLES
%x                    S_S_EXPECT_FROM

%x                    DELETE_STATEMENT
%x                    D_S_EXPECT_WHERE


%x                    CREATE_TABLE_STATEMENT
%x                    C_T_S_EXPECT_LEFT_PARENTHESIS
%x                    C_T_S_EXPECT_ATTR
%x                    C_T_S_EXPECT_TYPE
%x                    C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS
%x                    C_T_S_EXPECT_WS

%x                    INSERT_STATEMENT
%x                    I_S_EXPECT_LEFT_PARENTHESIS
%x                    I_S_EXPECT_ATTR
%x                    I_S_VALUES_EXPECT_LEFT_PARENTHESIS
%x                    I_S_EXPECT_VALUES_OR_SELECT
%x                    I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS
%x                    I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS
%x                    I_S_VALUES_EXPECT_value


DOT                   [.]
OB                    {WS}*ORDER{WS}+BY
digit                 [0-9]
letter                [a-z]
integer               {digit}+
comp_op               [<>=]
pm_op                 [+-]
STAR                  [*]
md_op                 [*/]
name                  {letter}({digit}|{letter})* 
dotted_name           {name}{DOT}{name}
literal               \"[^"]*\"
value                 {literal}|{integer}|{NULL}
WS                    [ \t]
V                     VALUES
S                     SELECT
C                     CREATE
T                     TABLE
W                     WHERE
F                     FROM
IT                    INTO
IS                    INSERT
BY                    BY
DP                    DROP
DT                    DELETE
OR                    OR
INT                   INT
STR                   STR20
AND                   AND
NOT                   NOT
NULL                  NULL
DISTINCT              DISTINCT
C_T                   {C}{WS}+{T}            
D_T                   {DP}{WS}+{T}
I_I                   {IS}{WS}+{IT}
D_F                   {DT}{WS}+{F}
SELECT                {S}
LEFT_PARENTHESIS      [(]
RIGHT_PARENTHESIS     [)]
COMMA                 [,]

%%

<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT><<EOF>> {

	cout << endl << "";
	if(YY_START == WHERE_SUB_STATEMENT){
		while(operators_s.empty() != true){
			if (noperands(operators_s.top()->str) == 2) {
				operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->tables.insert(
					operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
				operators_s.top()->tables.insert(
					operators_s.top()->right->tables.begin() , operators_s.top()->right->tables.end() ) ;
			}else if (noperands(operators_s.top()->str) == 1) {
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->tables.insert(
					operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
			}
			output_s.push(operators_s.top()); operators_s.pop() ;
		}
		#ifdef DEBUG
		output_s.top()->print(0) ; 
		#endif
	}
	if(head != NULL){
		head->exec(true, NULL);
		head->free();
		head = (current_qt = NULL);
	}
	if(YY_START == WHERE_SUB_STATEMENT){
		yy_pop_state();
		if(YY_START == D_S_EXPECT_WHERE){
			p->Delete(table_name, output_s.empty()? NULL: output_s.top() ) ;
		}
		while(! output_s.empty() ){ output_s.top()->free() ;output_s.pop();}
	}else if(YY_START == D_S_EXPECT_WHERE){
		p->Delete(table_name, NULL) ;
	}

	cout << "Disk IOs: " << p->IO() << endl ; 
	stop_time = clock() ;
	cout << "Execution Time: " << ( stop_time - start_time ) / double(CLOCKS_PER_SEC) << " s" << endl ;
	cout << endl << "=================================" << endl << endl;
	yyterminate();
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>[\n\r] {
	cout << endl << "";
	if(YY_START == WHERE_SUB_STATEMENT){
		while(operators_s.empty() != true){
			if (noperands(operators_s.top()->str) == 2) {
				operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->tables.insert(
					operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
				operators_s.top()->tables.insert(
					operators_s.top()->right->tables.begin() , operators_s.top()->right->tables.end() ) ;
			}else if (noperands(operators_s.top()->str) == 1) {
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->tables.insert(
					operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
			}
			output_s.push(operators_s.top()); operators_s.pop() ;
		}
		#ifdef DEBUG
		output_s.top()->print(0) ;
		#endif
	}
	if(head != NULL){
		head->exec(true, NULL);
		head->free();
		head = (current_qt = NULL);
	}
	if(YY_START == WHERE_SUB_STATEMENT){
		yy_pop_state();
		if(YY_START == D_S_EXPECT_WHERE){
			p->Delete(table_name, output_s.empty()? NULL: output_s.top() ) ;
		}
		while(! output_s.empty() ){ output_s.top()->free() ;output_s.pop();}
	}else if(YY_START == D_S_EXPECT_WHERE){
		p->Delete(table_name, NULL) ;
	}
	for(vector<string>::iterator it = temp_relations.begin(); it != temp_relations.end(); ){
		p->DropTable(*it) ;it = temp_relations.erase(it) ;
	}
	BEGIN(INITIAL);
	
	cout << "Disk IOs: " << p->IO() << endl ; 
	stop_time = clock() ;
	cout << "Execution Time: " << ( stop_time - start_time ) / double(CLOCKS_PER_SEC) << " s" <<endl ;
	cout << endl << "=================================" << endl << endl;
	p->IOreset() ;
	start_time = clock();
}

<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{SELECT}{WS}+ {
	cout <<"" << "SELECT " ;
	err_out_START("SELECT");
	yy_push_state(SELECT_STATEMENT);
}
<INITIAL>{D_F}{WS}+ {
	cout <<""<< "DELETE FROM " ;
	err_out_START("DELETE FROM");
	BEGIN(DELETE_STATEMENT);
}
<INITIAL>{C_T}{WS}+ {
	cout <<""<< "CREATE TABLE " ;
	err_out_START("CREATE TABLE" );
	BEGIN(CREATE_TABLE_STATEMENT);
}
<INITIAL>{D_T}{WS}+ {
	cout <<""<< "DROP TABLE " ;
	err_out_START("DROP TABLE" );
	BEGIN(DROP_TABLE_STATEMENT);
}
<INITIAL>{I_I}{WS}+ {
	cout <<""<< "INSERT INTO ";
	err_out_START("INSERT INTO");
	BEGIN(INSERT_STATEMENT);
}

<SELECT_STATEMENT>{WS}*{DISTINCT}{WS}* {
	cout << "DISTINCT " ;
	head = (current_qt = new Qtree(DELTA, NULL) );
	error_output << "DISTINCT" << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		Qtree *ins_node = new Qtree(INS, NULL);
		ins_node->left = head ;
		head->parent = ins_node ;
		head = ins_node ;
		ins_node->info.push_back(string(table_name) ) ;
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			ins_node->info.push_back(string(*it)) ;
		}
	}
	yy_push_state(S_S_DISTINCT_STATE) ;
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>{STAR}  {
	cout << "* " ;
	error_output << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		Qtree *ins_node = new Qtree(INS, NULL);
		head = current_qt = ins_node ;
		ins_node->info.push_back(string(table_name) ) ;
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			ins_node->info.push_back(string(*it)) ;
		}
	}
	BEGIN(S_S_EXPECT_FROM);
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>({name}|{dotted_name}) { 
	cout << yytext << " " ;
	if(current_qt == NULL){head = ( current_qt = new Qtree(PI, NULL) );}
	else{ 
		current_qt->left = new Qtree(PI, current_qt) ;	 
		current_qt = current_qt->left;
	}
	current_qt->info.push_back(string( yytext) ) ;
	error_output << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		Qtree *ins_node = new Qtree(INS, NULL);
		ins_node->left = head ;
		head->parent = ins_node ;
		head = ins_node ;
		ins_node->info.push_back(string(table_name) ) ;
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++){
			ins_node->info.push_back(string(*it)) ;
		}
	}
	BEGIN(S_S_EXPECT_FROM_OR_COMMA);
}
<S_S_EXPECT_FROM>{WS}*{F}{WS}* {
	cout << "FROM " ;
	BEGIN(S_S_EXPECT_TABLES);
}
<S_S_EXPECT_FROM_OR_COMMA>{WS}*{F}{WS}* {
	cout << "FROM " ;
	BEGIN(S_S_EXPECT_TABLES);
}
<S_S_EXPECT_FROM_OR_COMMA>{WS}*{COMMA}{WS}* {
	cout << ", " ;
	BEGIN(S_S_EXPECT_COLUMNS);
}
<S_S_EXPECT_COLUMNS>({name}|{dotted_name}) {
	cout << yytext << " " ;
	current_qt->info.push_back(string( yytext) );
	error_output << "column_name:\t" << yytext << endl ;
	BEGIN(S_S_EXPECT_FROM_OR_COMMA);
}
<S_S_EXPECT_TABLES>{name} {
	cout << yytext << " " ;
	Qtree *table_node = new Qtree(TABLE, current_qt);
	if(current_qt != NULL){ 
		current_qt->left = table_node; 
		current_qt = current_qt->left ;
	}else{ 
		current_qt = table_node; 
		head = table_node; 
	}
	current_qt->info.push_back(string( yytext) ) ;
	error_output << "table name:\t" << yytext << endl;
	BEGIN(S_S_EXPECT_WHERE_OR_COMMA);
}
<S_S_EXPECT_WHERE_OR_COMMA>{WS}*{W}{WS}* {
	cout << "WHERE" << " " ;
	yy_push_state(WHERE_SUB_STATEMENT) ;
}
<S_S_EXPECT_WHERE_OR_COMMA>{WS}*{COMMA}{WS}* {
	cout << "," << " " ;
	BEGIN(S_S_EXPECT_MORE_TABLES);
}
<S_S_EXPECT_MORE_TABLES>{name} {
	cout << yytext << " " ;
	Qtree *join_node = new Qtree(PRODUCT, current_qt->parent);
	Qtree *table_node = new Qtree(TABLE, join_node);
	table_node->info.push_back(string(yytext) ) ;
	join_node->info.insert(join_node->info.end(), current_qt->info.begin(), current_qt->info.end()) ;
	join_node->info.push_back(string(yytext) ) ;
	if(current_qt->parent != NULL){current_qt->parent->left = join_node ;}
	join_node->left = current_qt ;
	current_qt->parent = join_node;
	join_node->right = table_node ;
	if(head == current_qt){ 
		head = join_node;
		current_qt = join_node ; 
	}else{
		current_qt = join_node ; 
	}

	error_output << "table name: \t" << yytext << endl;
	BEGIN(S_S_EXPECT_WHERE_OR_COMMA);
}

<DELETE_STATEMENT>{name} {/* DELETE from table */
	cout << yytext << " " ;
	error_output << "delete table:\t" << yytext << endl ;
	table_name = string(yytext) ;
	BEGIN(D_S_EXPECT_WHERE);
}
<D_S_EXPECT_WHERE>{WS}*{W}{WS}* {
	cout << "WHERE" << " " ;
	yy_push_state(WHERE_SUB_STATEMENT) ;
}



<DROP_TABLE_STATEMENT>{name} {
	cout << yytext << " " ;
	error_output << "table to Drop:\t" << yytext << endl ;
	scan_over(DROP_TABLE_STATEMENT);
	p->DropTable(yytext) ;
	BEGIN(INITIAL);
}


<INSERT_STATEMENT>{name} {
	cout << yytext << " " ;
	error_output << "table to ins:\t" << yytext << endl;
	table_name = string(yytext) ;
	BEGIN(I_S_EXPECT_LEFT_PARENTHESIS);
}
<I_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	cout << "(" << " " ;
	BEGIN(I_S_EXPECT_ATTR);
}
<I_S_EXPECT_ATTR>{name} {
	cout << yytext << " " ;
	error_output << "attribute :\t" << yytext <<endl ;
	field_names.push_back(string(yytext)) ;
	BEGIN(I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	cout << "," << " " ;
	BEGIN (I_S_EXPECT_ATTR) ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	cout << ")" << " " ;
	BEGIN(I_S_EXPECT_VALUES_OR_SELECT) ;
}
<I_S_EXPECT_VALUES_OR_SELECT>{V} {
	cout << "VALUES" << " " ;
	BEGIN(I_S_VALUES_EXPECT_LEFT_PARENTHESIS);
}
<I_S_VALUES_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	cout << "(" << " " ;
	BEGIN(I_S_VALUES_EXPECT_value);
}
<I_S_EXPECT_VALUES_OR_SELECT>{WS}*{SELECT}{WS}* {
	cout << "SELECT" << " " ;
	yy_push_state(SELECT_STATEMENT);
}
<I_S_VALUES_EXPECT_value>{value} {
	cout << yytext << " " ;
	error_output << "value:      \t" << yytext << endl;
	
	if(yytext[0] == 'N' && yytext[1] == 'U' && yytext[2] == 'L' ){
		perror("NULL is undefined");
		int_values.push_back(0) ;
	}else if(yytext[0] == '"' ) {
		string value_str(yytext + 1) ;
		str_values.push_back( value_str.substr(0, value_str.size() - 1) ) ;
	}else if(yytext[0] >= '0' && yytext[0] <= '9' ) { //No negative numbers 
		int_values.push_back(atoi(yytext) )  ;
	}else{
		perror("unknown value");
	}
	BEGIN(I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS);
}
<I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	cout << "," << " " ;
	BEGIN(I_S_VALUES_EXPECT_value) ;
}
<I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	cout << ")" << " " ;
	scan_over(INSERT_STATEMENT);
	for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++ ){
		(*it) = string( table_name + "."+ (*it) )  ;
	}
	p->insert(table_name, field_names, str_values, int_values) ;
	field_names.clear();
	str_values.clear();
	int_values.clear() ;
	#ifdef DEBUG
	p->displayRelation(table_name);
	#endif
	BEGIN(INITIAL);
}


<CREATE_TABLE_STATEMENT>{name} {
	cout << yytext << " " ;
	/* Enter CREATE TABLE statement */
	error_output << "create table:\t" << yytext << endl;
	table_name = string(yytext) ;
	BEGIN(C_T_S_EXPECT_LEFT_PARENTHESIS);
}
<C_T_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	cout << "(" << " " ;
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_ATTR>{name} {
	cout << yytext << " " ;
	error_output << "attribute :\t" << yytext <<endl ;
	field_names.push_back( string(yytext) ); 
	BEGIN(C_T_S_EXPECT_WS) ;
}
<C_T_S_EXPECT_WS>{WS}+ {
	BEGIN(C_T_S_EXPECT_TYPE) ;
}
<C_T_S_EXPECT_TYPE>({INT}|{STR}) {
	cout << yytext << " " ;
	error_output << "the type is:\t" << yytext << endl ;
	if(yytext[0] == 'I' && yytext[1] == 'N' && yytext[2] == 'T'){
		field_types.push_back( INT);
	}else if(yytext[0 ] == 'S' && yytext[2] == 'R' && yytext[3] == '2' ){
		field_types.push_back( STR20);
	}
	BEGIN(C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	cout << "," << " " ;
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	cout << ")" << " " ;
	scan_over(CREATE_TABLE_STATEMENT);
	if(field_types.size() != field_names.size() ) {
		perror("Size mismatch");
	}else{
		for(vector<string>::iterator it = field_names.begin(); it != field_names.end(); it++ ){
			(*it) = string( table_name + "."+ (*it) )  ;
		}
		p->CreateTable(table_name, field_names, field_types) ; 
		field_names.clear();
		field_types.clear();
		#ifdef DEBUG
		p->displayRelation(table_name);
		#endif
	}
	BEGIN(INITIAL);
}


<WHERE_SUB_STATEMENT>{WS}* { /* Eat white spaces */ }
<S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{OB}{WS}+({name}|{dotted_name}) {
	cout << "ORDER BY" << " " ;
	/* 
	ORDER BY 
	*/
	string match (yytext) , name_, name ;
	int pos  ;
	Qtree *order_node = new Qtree(TAU, current_qt->parent );
	order_node->left = current_qt;
	order_node->right = NULL;
	if(current_qt->parent != NULL){
		current_qt->parent->left = order_node ;
	}else{
		head = order_node ;
	}
	current_qt->parent = order_node ;

	pos = match.find(" BY") ;
	name_ = match.substr(pos + 3) ;
	pos = name_.rfind(' ') ;
	name = name_.substr(pos + 1) ;
	order_node->info.push_back( name ) ;
	cout << name << " " ;
}
<WHERE_SUB_STATEMENT>{name} { 
	cout << yytext << " " ;
	output_s.push(new Qexpression(COLUMN, yytext) ) ;
	if(current_qt != NULL && current_qt->info.size() == 1){
		output_s.top()->tables.insert( current_qt->info[0]  ) ;
	}else{
		output_s.top()->tables.insert( table_name );
	}
}
<WHERE_SUB_STATEMENT>{dotted_name} {
	string s(yytext) ;
	int found ;
	cout << yytext << " " ;
	output_s.push(new Qexpression(COLUMN, yytext) ) ;
	found = s.find('.') ;
	s = string( s.substr(0, found) );
	output_s.top()->tables.insert( s ) ;
}
<WHERE_SUB_STATEMENT>{literal} { 
	cout << yytext << " " ;
	string l(yytext + 1) ;
	l = l.substr(0, l.size() - 1) ;
	output_s.push(new Qexpression(LITERAL, l) ) ;
}
<WHERE_SUB_STATEMENT>{integer} {
	cout << yytext << " " ;
	output_s.push(new Qexpression(INTEGER, atoi(yytext) ) ) ;
}
<WHERE_SUB_STATEMENT>{pm_op}|{md_op}|{AND}|{OR}|{NOT}|{comp_op} {
	cout << yytext << " " ;
	int current_precedence = precedence(yytext);
	while( (! operators_s.empty() ) && 
		operators_s.top()->type != LEFT && 
		operators_s.top()->number >= current_precedence) {
		if (noperands(operators_s.top()->str) == 2) {
			operators_s.top()->right = (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->tables.insert(
				operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
			operators_s.top()->tables.insert(
				operators_s.top()->right->tables.begin() , operators_s.top()->right->tables.end() ) ;
		}else if (noperands(operators_s.top()->str) == 1) {
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->tables.insert(
				operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
		}
		output_s.push( operators_s.top() ) ;operators_s.pop();
	}
	operators_s.push( new Qexpression(OPERATER, current_precedence, yytext) );
}
<WHERE_SUB_STATEMENT>{LEFT_PARENTHESIS}|[\[] {
	cout << yytext << " " ;
	operators_s.push(new Qexpression(LEFT, yytext)) ;
}
<WHERE_SUB_STATEMENT>{RIGHT_PARENTHESIS}|[\]] {
	cout << yytext << " " ;
	while(operators_s.top()->type != LEFT){
		if (noperands(operators_s.top()->str) == 2) {
			operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->left =   (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->tables.insert(
				operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
			operators_s.top()->tables.insert(
				operators_s.top()->right->tables.begin() , operators_s.top()->right->tables.end() ) ;
		}else if (noperands(operators_s.top()->str) == 1) {
			operators_s.top()->left =   (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->tables.insert(
				operators_s.top()->left->tables.begin() , operators_s.top()->left->tables.end() ) ;
		}
		output_s.push( operators_s.top() ) ;operators_s.pop();
	}
	operators_s.pop() ;
}


<INITIAL>{WS}* {/* IGNORE white spaces */}       
. {/* all other character */
	fprintf(stderr, "Error: unaccepted character: %s\n", yytext) ;
	exit(EXIT_FAILURE); 
}

%%
