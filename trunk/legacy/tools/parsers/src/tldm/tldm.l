
letter  [a-zA-Z]
digit   [0-9]

%{
#include <assert.h>
#include <stdlib.h>
#include "y.tab.h"
#include "tldm_parser.h"


%}

%x _IN_QUOTE_

%%

\/\/.*\n    {
                    assert(strlen(yytext) < 2048);
                    char yytext_tmp[2048];
                    strcpy(yytext_tmp, yytext);
                    yytext_tmp[strlen(yytext)-1] = 0; // remove the last \n

					yylval.pNode = g_xml_tree_for_tldm_parse->CreateLeave("comment", string(yytext_tmp));
					return comment;
}


({letter}|_)({letter}|{digit}|_)* 	{
					yylval.pNode = g_xml_tree_for_tldm_parse->CreateLeave("identifier", string(yytext));
					return identifier;
				}

0           {
				yylval.pNode = g_xml_tree_for_tldm_parse->CreateLeave("int", string(yytext));
                return INTEGER;
            }

[1-9][0-9]* {
				yylval.pNode = g_xml_tree_for_tldm_parse->CreateLeave("int", string(yytext));
                return INTEGER;
            }

"::"            return CC;
"->"            return PT;
"=>"            return PPT;
".SUB"          return LSUB;
".ADD"          return LADD;
".GT"           return GT;
".LT"           return LT;
".GE"           return GE;
".LE"           return LE;
".EQ"           return EQ;
".."            return RANGE;
"(("            return BR_LEFT;
"))"            return BR_RIGHT;

[()<>:;{},\[\]+-/*@] {
                return *yytext;
             }

[\"]                          { BEGIN _IN_QUOTE_; return LEFT_QUOTE; }

<_IN_QUOTE_>[^\"]*          { yylval.pNode = g_xml_tree_for_tldm_parse->CreateLeave("string", string(yytext)); return QUOTE_INFO; }

<_IN_QUOTE_>[\"]              { BEGIN INITIAL; return RIGHT_QUOTE; }


[ \t\n]+        ;       /* ignore whitespace */

.              {  /*yyerror("Unknown character");*/ printf("Unknown charactor: %c, (0x%x)  \n",
        *yytext, (int)(*yytext) ); exit(-1); }

%%


int yywrap(void) {
    return 1;
}
