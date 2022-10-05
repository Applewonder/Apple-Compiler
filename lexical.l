%option yylineno

%{
    #include <stdio.h>
    #include "syntax.tab.h"
    #include "ast.h"
    extern int error_flag;
%}

SPACE [ \t\n]+
BLOCKCOMMENT_START \/\*
BLOCKCOMMENT_END "*"+"/"
COMMENT \/\/.*$
HEX 0[xX][1-9A-Fa-f][0-9A-Fa-f]*|0x0
OCT 0[1-7][0-7]*|00
INT [1-9][0-9]*|0
FLOAT [0-9]+\.[0-9]+|(([0-9]+\.[0-9]+)|([0-9]+\.)|(\.[0-9]+))[Ee][+-]?[0-9]+
ID [A-Za-z_][A-Za-z0-9_]*
SEMI ;
COMMA ,
ASSIGNOP =
GREATER >
LESS <
GEQ >=
LEQ <=
EQL ==
NEQ !=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR ||
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

%x comment

%%
{SPACE} 
{COMMENT}                       
{BLOCKCOMMENT_START}            {BEGIN(comment);}
<comment>[^*\n]*                
<comment>\*+[^*/\n]*           
<comment>\n            
<comment>{BLOCKCOMMENT_END}     {BEGIN(INITIAL);}

"+" {yylval.node = new_node("PLUS", 0, yylineno); return PLUS; }
"-" {yylval.node = new_node("MINUS", 0, yylineno); return MINUS; }
"&&" {yylval.node = new_node("AND", 0, yylineno); return AND; }
"||" {yylval.node = new_node("OR", 0, yylineno); return OR; }

{HEX}                            {yylval.node = new_node("HEX", 0, yylineno); return HEX;}
{OCT}                            {yylval.node = new_node("OCT", 0, yylineno); return OCT;}
{INT}                            {yylval.node = new_node("INT", 0, yylineno); return INT;}
{FLOAT}                          {yylval.node = new_node("FLOAT", 0, yylineno); return FLOAT;}

{SEMI} {yylval.node = new_node("SEMI", 0, yylineno); return SEMI; }
{COMMA} {yylval.node = new_node("COMMA", 0, yylineno); return COMMA; }
{ASSIGNOP} {yylval.node = new_node("ASSIGNOP", 0, yylineno); return ASSIGNOP; }
{GREATER} {yylval.node = new_node("RELOP", 0, yylineno); return GREATER; }
{LESS} {yylval.node = new_node("RELOP", 0, yylineno); return LESS; }
{GEQ} {yylval.node = new_node("RELOP", 0, yylineno); return GEQ; }
{LEQ} {yylval.node = new_node("RELOP", 0, yylineno); return LEQ; }
{EQL} {yylval.node = new_node("RELOP", 0, yylineno); return EQL; }
{NEQ} {yylval.node = new_node("RELOP", 0, yylineno); return NEQ; }

{STAR} {yylval.node = new_node("STAR", 0, yylineno); return STAR; }
{DIV} {yylval.node = new_node("DIV", 0, yylineno); return DIV; }
{NOT} {yylval.node = new_node("NOT", 0, yylineno); return NOT; }
{TYPE} {yylval.node = new_node("TYPE", 0, yylineno); return TYPE; }

{DOT} {yylval.node = new_node("DOT", 0, yylineno); return DOT; }
{LP} {yylval.node = new_node("LP", 0, yylineno); return LP; }
{RP} {yylval.node = new_node("RP", 0, yylineno); return RP; }
{LB} {yylval.node = new_node("LB", 0, yylineno); return LB; }
{RB} {yylval.node = new_node("RB", 0, yylineno); return RB; }
{LC} {yylval.node = new_node("LC", 0, yylineno); return LC; }
{RC} {yylval.node = new_node("RC", 0, yylineno); return RC; }
{STRUCT} {yylval.node = new_node("STRUCT", 0, yylineno); return STRUCT; }
{RETURN} {yylval.node = new_node("RETURN", 0, yylineno); return RETURN; }
{IF} {yylval.node = new_node("IF", 0, yylineno); return IF; }
{ELSE} {yylval.node = new_node("ELSE", 0, yylineno); return ELSE; }
{WHILE} {yylval.node = new_node("WHILE", 0, yylineno); return WHILE; }
{ID} {yylval.node = new_node("ID", 0, yylineno); return ID; }
. {
    printf("Error type A at Line %d: Mysterious characters \'%s'\n", yylineno, yytext);
    error_flag = 1;
    }
%%

