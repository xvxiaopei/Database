%{
#include "common.h"
#include "physicalOP.h"
#include "StorageManager/Field.h"

physicalOP *p;
Qtree *current_qt ;
Qtree *head ;
stack<Qexpression*> output_s;
stack<Qexpression*> operators_s;
int precedence(string);
int noperands(string) ;

vector<string> field_names ;
vector<enum FIELD_TYPE > field_types ;
string table_name ;
vector<int> int_values;
vector<string> str_values ;

std::ostream error_output(NULL);

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
OB                    ORDER{WS}+BY
digit                 [0-9]
letter                [a-z]
integer               {digit}+
comp_op               [<>=]
pm_op                 [+-]
STAR                  [*]
md_op                 [*/]
name                  {letter}({digit}|{letter})* 
dotted_name           {name}{DOT}{name}
literal               \"[^"]\"
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
	if(YY_START == WHERE_SUB_STATEMENT){
		while(operators_s.empty() != true){
			if (noperands(operators_s.top()->str) == 2) {
				operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			}else if (noperands(operators_s.top()->str) == 1) {
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			}
			output_s.push(operators_s.top()); operators_s.pop() ;
		}
		output_s.top()->print(0) ; output_s.top()->free() ;output_s.pop();
	}
	if(head != NULL){
		head->exec();
		head->free();
		head = (current_qt = NULL);
	}
	yyterminate();
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>[\n\r] {
	if(YY_START == WHERE_SUB_STATEMENT){
		while(operators_s.empty() != true){
			if (noperands(operators_s.top()->str) == 2) {
				operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			}else if (noperands(operators_s.top()->str) == 1) {
				operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
			}
			output_s.push(operators_s.top()); operators_s.pop() ;
		}
		output_s.top()->print(0) ; output_s.top()->free() ;output_s.pop();
	}
	if(head != NULL){
		head->exec();
		head->free();
		head = (current_qt = NULL);
	}
	BEGIN(INITIAL);
}

<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{SELECT}{WS}+ {
	err_out_START("SELECT");
	yy_push_state(SELECT_STATEMENT);
}
<INITIAL>{D_F}{WS}+ {
	err_out_START("DELETE FROM");
	BEGIN(DELETE_STATEMENT);
}
<INITIAL>{C_T}{WS}+ {
	err_out_START("CREATE TABLE" );
	BEGIN(CREATE_TABLE_STATEMENT);
}
<INITIAL>{D_T}{WS}+ {
	err_out_START("DROP TABLE" );
	BEGIN(DROP_TABLE_STATEMENT);
}
<INITIAL>{I_I}{WS}+ {
	err_out_START("INSERT INTO");
	BEGIN(INSERT_STATEMENT);
}

<SELECT_STATEMENT>{WS}*{DISTINCT}{WS}* {
	head = (current_qt = new Qtree(DELTA) );
	error_output << "DISTINCT" << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		//TODO
	}
	yy_push_state(S_S_DISTINCT_STATE) ;
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>{STAR}  {
	if(current_qt == NULL){	head = (current_qt = new Qtree(PI) );}
	else{ current_qt = (current_qt->left = new Qtree(PI) );	}
	current_qt->info.push_back(yytext);
	error_output << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		//TODO
	}
	BEGIN(S_S_EXPECT_FROM);
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>({name}|{dotted_name}) { 
	if(current_qt == NULL){head = ( current_qt = new Qtree(PI) );}
	else{ current_qt = (current_qt->left = new Qtree(PI) );	 }
	current_qt->info.push_back(yytext);
	error_output << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_EXPECT_VALUES_OR_SELECT){
		//TODO
	}
	BEGIN(S_S_EXPECT_FROM_OR_COMMA);
}
<S_S_EXPECT_FROM>{WS}*{F}{WS}* {
	BEGIN(S_S_EXPECT_TABLES);
}
<S_S_EXPECT_FROM_OR_COMMA>{WS}*{F}{WS}* {
	BEGIN(S_S_EXPECT_TABLES);
}
<S_S_EXPECT_FROM_OR_COMMA>{WS}*{COMMA}{WS}* {
	BEGIN(S_S_EXPECT_COLUMNS);
}
<S_S_EXPECT_COLUMNS>({name}|{dotted_name}) {
	current_qt->info.push_back(yytext);
	error_output << "column_name:\t" << yytext << endl ;
	BEGIN(S_S_EXPECT_FROM_OR_COMMA);
}
<S_S_EXPECT_TABLES>{name} {
	current_qt = (current_qt->left = new Qtree(JOIN) );
	current_qt->info.push_back(yytext) ;
	error_output << "table name:\t" << yytext << endl;
	BEGIN(S_S_EXPECT_WHERE_OR_COMMA);
}
<S_S_EXPECT_WHERE_OR_COMMA>{WS}*{W}{WS}* {
	yy_push_state(WHERE_SUB_STATEMENT) ;
}
<S_S_EXPECT_WHERE_OR_COMMA>{WS}*{COMMA}{WS}* {
	BEGIN(S_S_EXPECT_MORE_TABLES);
}
<S_S_EXPECT_MORE_TABLES>{name} {
	current_qt->info.push_back(yytext); 
	error_output << "table name: \t" << yytext << endl;
	BEGIN(S_S_EXPECT_WHERE_OR_COMMA);
}

<DELETE_STATEMENT>{name} {/* DELETE from table */
	error_output << "delete table:\t" << yytext << endl ;
	BEGIN(D_S_EXPECT_WHERE);
}
<D_S_EXPECT_WHERE>{WS}*{W}{WS}* {
	yy_push_state(WHERE_SUB_STATEMENT) ;
}



<DROP_TABLE_STATEMENT>{name} {
	error_output << "table to Drop:\t" << yytext << endl ;
	scan_over(DROP_TABLE_STATEMENT);
	BEGIN(INITIAL);
}


<INSERT_STATEMENT>{name} {
	error_output << "table to ins:\t" << yytext << endl;
	table_name = string(yytext) ;
	BEGIN(I_S_EXPECT_LEFT_PARENTHESIS);
}
<I_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(I_S_EXPECT_ATTR);
}
<I_S_EXPECT_ATTR>{name} {
	error_output << "attribute :\t" << yytext <<endl ;
	field_names.push_back(string(yytext)) ;
	BEGIN(I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	BEGIN (I_S_EXPECT_ATTR) ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	BEGIN(I_S_EXPECT_VALUES_OR_SELECT) ;
}
<I_S_EXPECT_VALUES_OR_SELECT>{V} {
	BEGIN(I_S_VALUES_EXPECT_LEFT_PARENTHESIS);
}
<I_S_VALUES_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(I_S_VALUES_EXPECT_value);
}
<I_S_EXPECT_VALUES_OR_SELECT>{WS}*{SELECT}{WS}* {
	yy_push_state(SELECT_STATEMENT);
}
<I_S_VALUES_EXPECT_value>{value} {
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
	BEGIN(I_S_VALUES_EXPECT_value) ;
}
<I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	scan_over(INSERT_STATEMENT);
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
	/* Enter CREATE TABLE statement */
	error_output << "create table:\t" << yytext << endl;
	table_name = string(yytext) ;
	BEGIN(C_T_S_EXPECT_LEFT_PARENTHESIS);
}
<C_T_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_ATTR>{name} {
	error_output << "attribute :\t" << yytext <<endl ;
	field_names.push_back( string(yytext) ); 
	BEGIN(C_T_S_EXPECT_WS) ;
}
<C_T_S_EXPECT_WS>{WS}+ {
	BEGIN(C_T_S_EXPECT_TYPE) ;
}
<C_T_S_EXPECT_TYPE>({INT}|{STR}) {
	error_output << "the type is:\t" << yytext << endl ;
	if(yytext[0] == 'I' && yytext[1] == 'N' && yytext[2] == 'T'){
		field_types.push_back( INT);
	}else if(yytext[0 ] == 'S' && yytext[2] == 'R' && yytext[3] == '2' ){
		field_types.push_back( STR20);
	}
	BEGIN(C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	scan_over(CREATE_TABLE_STATEMENT);
	if(field_types.size() != field_names.size() ) {
		perror("Size mismatch");
	}else{
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
<WHERE_SUB_STATEMENT>{OB}{WS}+({name}|{dotted_name}) {
	/* 
	ORDER BY 
	*/
	if(head != NULL){
		Qtree *templeft = head->left ;
		Qtree *tempright = head->right ;
		head->left = new Qtree(TAU); 
		head->left->left = templeft;
		head->left->right = tempright;
	}else{
		perror("fatal error: no query tree");
		exit(EXIT_FAILURE);
	}
	while(operators_s.empty() != true){
		if (noperands(operators_s.top()->str) == 2) {
			operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
		}else if (noperands(operators_s.top()->str) == 1) {
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
		}
		output_s.push(operators_s.top()); operators_s.pop() ;
	}
	output_s.top()->print(0) ; output_s.top()->free() ; output_s.pop();
	yy_pop_state() ;
}
<WHERE_SUB_STATEMENT>{name}|{dotted_name} { 
	output_s.push(new Qexpression(COLUMN, yytext) ) ;
}
<WHERE_SUB_STATEMENT>{literal} { 
	output_s.push(new Qexpression(LITERAL, yytext) ) ;
}
<WHERE_SUB_STATEMENT>{integer} {
	output_s.push(new Qexpression(INTEGER, atoi(yytext) ) ) ;
}
<WHERE_SUB_STATEMENT>{pm_op}|{md_op}|{AND}|{OR}|{NOT}|{comp_op} {
	int current_precedence = precedence(yytext);
	while( (! operators_s.empty() ) && 
		operators_s.top()->type != LEFT && 
		operators_s.top()->number >= current_precedence) {
		if (noperands(operators_s.top()->str) == 2) {
			operators_s.top()->right = (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
		}else if (noperands(operators_s.top()->str) == 1) {
			operators_s.top()->left =  (output_s.top() )  ; output_s.pop() ;
		}
		output_s.push( operators_s.top() ) ;operators_s.pop();
	}
	operators_s.push( new Qexpression(OPERATER, current_precedence, yytext) );
}
<WHERE_SUB_STATEMENT>{LEFT_PARENTHESIS}|[\[] {
	operators_s.push(new Qexpression(LEFT, yytext)) ;
}
<WHERE_SUB_STATEMENT>{RIGHT_PARENTHESIS}|[\]] {
	while(operators_s.top()->type != LEFT){
		if (noperands(operators_s.top()->str) == 2) {
			operators_s.top()->right =  (output_s.top() )  ; output_s.pop() ;
			operators_s.top()->left =   (output_s.top() )  ; output_s.pop() ;
		}else if (noperands(operators_s.top()->str) == 1) {
			operators_s.top()->left =   (output_s.top() )  ; output_s.pop() ;
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
