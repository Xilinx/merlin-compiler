%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tldm_parser.h"

/* prototypes */
//nodeType *opr(int oper, int nops, ...);
//nodeType *id(int i);
//nodeType *con(int value);
//void freeNode(nodeType *p);
//int ex(nodeType *p);
int yylex(void);


//void yyerror(char *s);
//int sym[26];                    /* symbol table */
extern int yydebug;
extern int yylineno;
//extern int yy_flex_debug;

void  ensure_lowercase(CXMLNode * node)
{
    ///////////////////////////////
    // ZP: 2015-04-16
    // Do not need to be lowcase any more
    return ;
    ///////////////////////////////

	string str = node->GetParam("name");
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] >= 'A' and str[i] <= 'Z')
		{
			printf("\n[tldm_parser] ERROR: capital letter in task/data name \"%s\" is not supported, due to renaming in hls.\n", str.c_str());
			assert(0);
		}
	}
}


%}

%union {
    int iValue;                 /* integer value */
    class CXMLNode * pNode;
};

%token <pNode> INTEGER
//%token <iValue> INTEGER
//%token <sValue> VARIABLE
//%token WHILE IF PRINT 
%token CC PT PPT
%token <pNode> comment
%token <pNode> identifier
%token <pNode> RANGE 
%token <pNode> BR_LEFT 
%token <pNode> BR_RIGHT


%token LEFT_QUOTE
%token RIGHT_QUOTE
%token <pNode> QUOTE_INFO

//%nonassoc IFX
//%nonassoc ELSE

%left LSUB LADD
%left GE LE EQ GT LT
%left RANGE
%left '+' '-'
%left '*' '/'
//%nonassoc UMINUS

%type <pNode>  statement_list statement task_header 
        composite_body task_def domain_def domain_ref
        data_def io_def dep_def task_ref data_ref
        var_pos var_list data_ref_list var_list_exp
        var attribute  var_exp

%%

system: 
	  statement_list            { g_xml_tree_for_tldm_parse->setRoot($1); $1->SetName("system");}
	;

statement_list:
	  statement_list statement 	{ $$ = g_xml_tree_for_tldm_parse->AppendList($1, $2); show_node($2); }
	| statement			        { $$ = g_xml_tree_for_tldm_parse->CreateList("statement_list", $1); show_node($1); }
	;

composite_body:
	'{' statement_list '}'		{ $$ = $2; }
	| composite_body comment    { $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;

task_header:
	  domain_ref CC task_ref    { $$ = g_xml_tree_for_tldm_parse->CreateTaskDef($1, $3); ensure_lowercase($3); }
	| task_header comment		{ $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;

task_def:
	  task_header ';'	                { $$ = g_xml_tree_for_tldm_parse->AppendBody($1, 0); }
	| task_header composite_body ';'	{ $$ = g_xml_tree_for_tldm_parse->AppendBody($1, $2); }
	;

domain_def:
	  domain_ref ';'  					{ $$ = g_xml_tree_for_tldm_parse->CreateDomainDef($1,0); }
	| domain_ref composite_body ';'  	{ $$ = g_xml_tree_for_tldm_parse->CreateDomainDef($1,$2); }
	;

data_def:
	  domain_ref CC '[' identifier var_pos ']' ';' { $$ = g_xml_tree_for_tldm_parse->CreateDataDef($1, $4, $5); }
	;
	

io_def:
	  data_ref_list PT task_ref PT data_ref_list ';'  { $$ = g_xml_tree_for_tldm_parse->CreateConnectList($1, $3, $5); }
	| data_ref_list PT task_ref ';'                   { $$ = g_xml_tree_for_tldm_parse->CreateConnectList($1, $3,  0); }
	| task_ref PT data_ref_list ';'                   { $$ = g_xml_tree_for_tldm_parse->CreateConnectList( 0, $1, $3); }
	;

dep_def:
	  task_ref PT task_ref ';'      { $$ = g_xml_tree_for_tldm_parse->CreateDependence("Dataflow", $1, $3); }
	| task_ref PPT task_ref ';'     { $$ = g_xml_tree_for_tldm_parse->CreateDependence("Initial", $1, $3); }
	;

statement:
	  comment                   { $$ = $1; } 
	| task_def                  { $$ = $1; }
	| domain_def                { $$ = $1; }   
	| data_def                  { $$ = $1; }
	| io_def                    { $$ = $1; }
	| dep_def                   { $$ = $1; }
	| attribute                 { $$ = $1; }
    | var_list_exp ';'          { assert($1->GetChildNum() == 1); $$ = $1->GetChildByIndex(0); }
	;

domain_ref:
	  '<' var_pos ':' var_list_exp '>'	{ $$ = g_xml_tree_for_tldm_parse->CreateRef("domain_ref", $2, $4); }
	| '<' var_pos ':' '>'				{ $$ = g_xml_tree_for_tldm_parse->CreateRef("domain_ref", $2, 0); }  
	| '<' var_pos '>'	     			{ $$ = g_xml_tree_for_tldm_parse->CreateRef("domain_ref", $2, 0); }  
	| '<' '>'				            { $$ = 0; }
	| domain_ref comment		        { $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;

task_ref:
	  '(' var_pos ':' var_list_exp ')'	{ $$ = g_xml_tree_for_tldm_parse->CreateRef("task_ref", $2, $4); }
	| '(' var_pos ':' ')'	            { $$ = g_xml_tree_for_tldm_parse->CreateRef("task_ref", $2, 0); }
	| '(' var_pos ')'	                { $$ = g_xml_tree_for_tldm_parse->CreateRef("task_ref", $2, 0); }
	| task_ref comment		            { $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;


data_ref_list:
	  data_ref_list ',' data_ref    { $$ = g_xml_tree_for_tldm_parse->AppendList($1, $3); }
	| data_ref                  { $$ = g_xml_tree_for_tldm_parse->CreateList("data_ref_list", $1); }
	;

data_ref:
	  '[' var_pos ':' var_list_exp ']'	{ $$ = g_xml_tree_for_tldm_parse->CreateRef("data_ref", $2, $4); ensure_lowercase($2); }
	| '[' var_pos ':' ']'	              { $$ = g_xml_tree_for_tldm_parse->CreateRef("data_ref", $2, 0);  ensure_lowercase($2); }
	| '[' var_pos ']'	                  { $$ = g_xml_tree_for_tldm_parse->CreateRef("data_ref", $2, 0);  ensure_lowercase($2); }
  | data_ref composite_body           { $$ = g_xml_tree_for_tldm_parse->AppendBody($1, $2); }
	| data_ref comment		              { $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;

var_list_exp:
      var_list_exp LSUB var_list_exp  { $$ = g_xml_tree_for_tldm_parse->CreateExp("LSUB", $1, $3); }
    | var_list_exp LADD var_list_exp  { $$ = g_xml_tree_for_tldm_parse->CreateExp("LADD", $1, $3); }
    | var_list                        { $$ = $1; }
    ;

var_list:
	  var_list ',' var_pos          { $$ = g_xml_tree_for_tldm_parse->AppendList($1, $3); }
	| var_pos                       { $$ = g_xml_tree_for_tldm_parse->CreateList("vector", $1); }
	;

var_pos:
	  identifier '@' var_exp        { $$ = g_xml_tree_for_tldm_parse->CreateVarPos($1, $3); }
	| var_exp                       { $$ = $1; }
	| var_pos comment		        { $$ = g_xml_tree_for_tldm_parse->AppendComment($1, $2); }
	;

var_exp:
      var                           { $$ = $1; }
    | var_exp GT  var_exp           { $$ = g_xml_tree_for_tldm_parse->CreateExp(">", $1, $3); }
    | var_exp LT  var_exp           { $$ = g_xml_tree_for_tldm_parse->CreateExp("<", $1, $3); }
    | var_exp EQ  var_exp           { $$ = g_xml_tree_for_tldm_parse->CreateExp("==", $1, $3); }
    | var_exp GE  var_exp           { $$ = g_xml_tree_for_tldm_parse->CreateExp(">=", $1, $3); }
    | var_exp LE  var_exp           { $$ = g_xml_tree_for_tldm_parse->CreateExp("<=", $1, $3); }
    | '-' var_exp                   { $$ = g_xml_tree_for_tldm_parse->CreateExp("-", g_xml_tree_for_tldm_parse->CreateLeave("int", "0"), $2); }
    | var_exp '+'  var_exp          { $$ = g_xml_tree_for_tldm_parse->CreateExp("+", $1, $3); }
    | var_exp '-'  var_exp          { $$ = g_xml_tree_for_tldm_parse->CreateExp("-", $1, $3); }
    | var_exp '*'  var_exp          { $$ = g_xml_tree_for_tldm_parse->CreateExp("*", $1, $3); }
    | var_exp '/'  var_exp          { $$ = g_xml_tree_for_tldm_parse->CreateExp("/", $1, $3); }
    | BR_LEFT var_exp BR_RIGHT      { $$ = $2; }
    | RANGE                         { $$ = g_xml_tree_for_tldm_parse->CreateRangeExp(0, 0); }
    | var_exp RANGE var_exp         { $$ = g_xml_tree_for_tldm_parse->CreateRangeExp($1, $3); } 
    | RANGE var_exp                 { $$ = g_xml_tree_for_tldm_parse->CreateRangeExp(0, $2); }
    | var_exp RANGE                 { $$ = g_xml_tree_for_tldm_parse->CreateRangeExp($1, 0); }
    | LEFT_QUOTE QUOTE_INFO RIGHT_QUOTE { $$ = $2; }
    ;

var:
      identifier
    | INTEGER
    ;

attribute:
      var '(' var_list ')' ';'        { $$ = g_xml_tree_for_tldm_parse->CreateAttrib($1, $3); show_node($$);}
    | var '(' var_list ')' '}'       { printf("Missing ';' before '}' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' '{'       { printf("Missing ';' before '{' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' '<'       { printf("Missing ';' before '<' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' '>'       { printf("Missing ';' before '>' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' '['       { printf("Missing ';' before '[' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' ']'       { printf("Missing ';' before ']' in line %d.\n", yylineno); exit(-1); }
    | var '(' var_list ')' var       { printf("Missing ';' before \"%s\" in line %d.\n", $5->GetValue().c_str(), yylineno); exit(-1); }
    ;

//expr:
//          INTEGER               { $$ = con($1); }
//        | VARIABLE              { $$ = id($1); }
//        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
//        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
//        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
//        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
//        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
//        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
//        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
//        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
//        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
//        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
//        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
//        | '(' expr ')'          { $$ = $2; }
//        ;

%%

CTLDMTree * g_xml_tree_for_tldm_parse;



void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
    exit(1);
}

extern FILE * yyin;
extern int g_debug_print;

int yy_main(string sFileIn) {
    if (g_debug_print) yydebug = 1;
//    yy_flex_debug = 1;

    yyin = fopen(sFileIn.c_str(), "rb");

    yyparse();

    fclose(yyin);
    return 0;
}
