%{
#include "common.h"
Qtree *current_qt ;
Qtree *head ;
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
%x                    I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT
%x                    I_S_EXPECT_VALUES
%x                    I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS
%x                    I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS
%x                    I_S_VALUES_EXPECT_value


DOT                   [.]
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
WS                    [ \t\n\r]
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
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	yyterminate();
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{SELECT}{WS}+ {
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	err_out_START("SELECT");
	if(YY_START == WHERE_SUB_STATEMENT) { yy_pop_state() ;}
	yy_push_state(SELECT_STATEMENT);
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{D_F}{WS}+ {
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	err_out_START("DELETE FROM");
	if(YY_START == WHERE_SUB_STATEMENT) { yy_pop_state() ;}
	BEGIN(DELETE_STATEMENT);
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{C_T}{WS}+ {
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	err_out_START("CREATE TABLE" );
	if(YY_START == WHERE_SUB_STATEMENT) { yy_pop_state() ;}
	BEGIN(CREATE_TABLE_STATEMENT);
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{D_T}{WS}+ {
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	err_out_START("DROP TABLE" );
	if(YY_START == WHERE_SUB_STATEMENT) { yy_pop_state() ;}
	BEGIN(DROP_TABLE_STATEMENT);
}
<INITIAL,D_S_EXPECT_WHERE,S_S_EXPECT_WHERE_OR_COMMA,WHERE_SUB_STATEMENT>{I_I}{WS}+ {
	if(head != NULL){head->exec();head->free();head = (current_qt = NULL);}
	err_out_START("INSERT INTO");
	if(YY_START == WHERE_SUB_STATEMENT) { yy_pop_state() ;}
	BEGIN(INSERT_STATEMENT);
}

<SELECT_STATEMENT>{DISTINCT} {
	head = (current_qt = new Qtree(DELTA) );
	cerr << "DISTINCT" << endl ;
	yy_pop_state();
	if(YY_START == I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT){
		//TODO
	}
	yy_push_state(S_S_DISTINCT_STATE) ;
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>{STAR}  {
	if(current_qt == NULL){	current_qt = new Qtree(PI) ;}
	else{ current_qt = (current_qt->left = new Qtree(PI) );	}
	current_qt->info.push_back(yytext);
	cerr << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT){
		//TODO
	}
	BEGIN(S_S_EXPECT_FROM);
}
<SELECT_STATEMENT,S_S_DISTINCT_STATE>{dotted_name} { 
	if(current_qt == NULL){	current_qt = new Qtree(PI) ;}
	else{ current_qt = (current_qt->left = new Qtree(PI) );	}
	current_qt->info.push_back(yytext);
	cerr << "column_name:\t" << yytext << endl ;
	yy_pop_state();
	if(YY_START == I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT){
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
<S_S_EXPECT_COLUMNS>{dotted_name} {
	current_qt = (current_qt->left = new Qtree(PI) );
	current_qt->info.push_back(yytext);
	cerr << "column_name:\t" << yytext << endl ;
	BEGIN(S_S_EXPECT_FROM_OR_COMMA);
}
<S_S_EXPECT_TABLES>{name} {
	current_qt = (current_qt->left = new Qtree(JOIN) );
	current_qt->info.push_back(yytext) ;
	cerr << "table name:\t" << yytext << endl;
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
	cerr << "table name: \t" << yytext << endl;
	BEGIN(S_S_EXPECT_WHERE_OR_COMMA);
}

<DELETE_STATEMENT>{name} {/* DELETE from table */
	cerr << "delete table:\t" << yytext << endl ;
	BEGIN(D_S_EXPECT_WHERE);
}
<D_S_EXPECT_WHERE>{WS}*{W}{WS}* {
	yy_push_state(WHERE_SUB_STATEMENT) ;
}


<DROP_TABLE_STATEMENT>{name} {
	cerr << "table to Drop:\t" << yytext << endl ;
	scan_over(DROP_TABLE_STATEMENT);
	BEGIN(INITIAL);
}




<INSERT_STATEMENT>{name} {
	cerr << "table to ins:\t" << yytext << endl;
	BEGIN(I_S_EXPECT_LEFT_PARENTHESIS);
}
<I_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(I_S_EXPECT_ATTR);
}
<I_S_EXPECT_ATTR>{name} {
	BEGIN(I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
	cerr << "attribute :\t" << yytext <<endl ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	BEGIN (I_S_EXPECT_ATTR) ;
}
<I_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	BEGIN(I_S_EXPECT_VALUES) ;
}
<I_S_EXPECT_VALUES>{V} {
	BEGIN(I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT);
}
<I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(I_S_VALUES_EXPECT_value);
}
<I_S_VALUES_EXPECT_LEFT_PARENTHESIS_OR_SELECT>{WS}*{SELECT}{WS}* {
	yy_push_state(SELECT_STATEMENT);
}
<I_S_VALUES_EXPECT_value>{value} {
	cerr << "value:      \t" << yytext << endl;
	BEGIN(I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS);
}
<I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	BEGIN(I_S_VALUES_EXPECT_value) ;
}
<I_S_VALUES_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	scan_over(INSERT_STATEMENT);
	BEGIN(INITIAL);
}


<CREATE_TABLE_STATEMENT>{name} {
	/* Enter CREATE TABLE statement */
	cerr << "create table:\t" << yytext << endl;
	BEGIN(C_T_S_EXPECT_LEFT_PARENTHESIS);
}
<C_T_S_EXPECT_LEFT_PARENTHESIS>{WS}*{LEFT_PARENTHESIS}{WS}* {
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_ATTR>{name} {
	BEGIN(C_T_S_EXPECT_WS) ;
	cerr << "attribute :\t" << yytext <<endl ;
}
<C_T_S_EXPECT_WS>{WS}+ {
	BEGIN(C_T_S_EXPECT_TYPE) ;
}
<C_T_S_EXPECT_TYPE>({INT}|{STR}) {
	cerr << "the type is:\t" << yytext << endl ;
	BEGIN(C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS) ;
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{COMMA}{WS}* {
	BEGIN(C_T_S_EXPECT_ATTR);
}
<C_T_S_EXPECT_COMMA_OR_RIGHT_PARENTHESIS>{WS}*{RIGHT_PARENTHESIS}{WS}* {
	scan_over(CREATE_TABLE_STATEMENT);
	BEGIN(INITIAL);
}


<INITIAL>{WS}* {/* IGNORE white spaces */}       
. {/* all other character */
	fprintf(stderr, "Error: unaccepted character: %s\n", yytext) ;
	exit(EXIT_FAILURE); 
}

%%

