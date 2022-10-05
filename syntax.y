%{
    #define YYDEBUG 1
    #include "lex.yy.c"
    #include <stdio.h>
    #include "ast.h"
    extern int yylex();
    extern int yyparse();
    extern int yylineno;   
    extern int error_flag; 
    void yyerror(const char *);
%}

%union{
    struct ast* node;
}

%right ASSIGNOP
%left OR
%left AND
%left GREATER LESS GEQ LEQ EQL NEQ
%left MINUS PLUS
%left STAR DIV
%right SOLO NOT
%left DOT LB RB LP RP 
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%token <node> INT OCT HEX
%token <node> FLOAT
%token <node> ID
%token <node> LESS GREATER LEQ GEQ EQL NEQ NOT
%token <node> SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT 
%token <node> TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%type <node> Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec
              StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Exp Def
              DecList Dec Args



%%
Program: ExtDefList {$$ = new_node("Program", 1, $1); if (!error_flag) print_ast($$, 0);}
;

ExtDefList: 
    ExtDef ExtDefList {$$ = new_node("ExtDefList", 2, $1, $2);}
| {$$ = new_node("ExtDefList", 0, -1);}
;

ExtDef:
    Specifier ExtDecList SEMI {$$ = new_node("ExtDef", 3, $1, $2, $3);}
|   Specifier SEMI  {$$ = new_node("ExtDef", 2, $1, $2);}
|   Specifier FunDec CompSt  {$$ = new_node("ExtDef", 3, $1, $2, $3);}
;

ExtDecList:
    VarDec  {$$ = new_node("ExtDecList", 1, $1);}
|   VarDec COMMA ExtDecList  {$$ = new_node("ExtDecList", 1, $1);}
;

Specifier:
    TYPE  {$$ = new_node("Specifier", 1, $1);}
|   StructSpecifier {$$ = new_node("Specifier", 1, $1);}
;

StructSpecifier: 
    STRUCT OptTag LC DefList RC {$$ = new_node("StructSpecifier", 5, $1, $2, $3, $4, $5);}
|   STRUCT Tag {$$ = new_node("StructSpecifier", 2, $1, $2);}
;

OptTag:
    ID {$$ = new_node("OptTag", 1, $1);}
|   {$$ = new_node("OptTag", 0, -1);}
;

Tag:
    ID {$$ = new_node("Tag", 1, $1);}
;

VarDec:
    ID {$$ = new_node("VarDec", 1, $1);}
|   VarDec LB INT RB {$$ = new_node("VarDec", 4, $1, $2, $3, $4);}
;

FunDec:
    ID LP VarList RP {$$ = new_node("FunDec", 4, $1, $2, $3, $4);}
|   ID LP RP {$$ = new_node("FunDec", 3, $1, $2, $3);}
;

VarList:
    ParamDec COMMA VarList {$$ = new_node("VarList", 3, $1, $2, $3);}
|   ParamDec {$$ = new_node("VarList", 1, $1);}
;

ParamDec: 
    Specifier VarDec {$$ = new_node("ParamDec", 2, $1, $2);}
;

CompSt:
    LC DefList StmtList RC {$$ = new_node("CompSt", 4, $1, $2, $3, $4);}
|   LC DefList StmtList error RC
;

StmtList:
    Stmt StmtList {$$ = new_node("StmtList", 2, $1, $2);}
| {$$ = new_node("StmtList", 0, -1);}
;

Stmt:
    Exp SEMI {$$ = new_node("Stmt", 2, $1, $2);}
|   CompSt {$$ = new_node("Stmt", 1, $1);}
|   RETURN Exp SEMI {$$ = new_node("Stmt", 3, $1, $2, $3);}
|   IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = new_node("Stmt", 5, $1, $2, $3, $4, $5);}
|   IF LP Exp RP Stmt ELSE Stmt  {$$ = new_node("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
|   WHILE LP Exp RP Stmt {$$ = new_node("Stmt", 5, $1, $2, $3, $4, $5);}
|   RETURN Exp error SEMI
|   Exp error SEMI
;

DefList:
    Def DefList {$$ = new_node("DefList", 2, $1, $2);}
| {$$ = new_node("DefList", 0, -1);}
;

Def:
    Specifier DecList SEMI {$$ = new_node("Def", 3, $1, $2, $3);}
|   Specifier DecList error SEMI
;

DecList:
    Dec {$$ = new_node("DecList", 1, $1);}
|   Dec COMMA DecList {$$ = new_node("DecList", 3, $1, $2, $3);}
;

Dec:
    VarDec {$$ = new_node("Dec", 1, $1);}
|   VarDec ASSIGNOP Exp {$$ = new_node("Dec", 3, $1, $2, $3);}
;

Exp:
    Exp ASSIGNOP Exp /*{ $$ = $1; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp AND Exp /*{ $$ = $1 && $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp OR Exp /*{ $$ = $1 || $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp GREATER Exp /*{ $$ = }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp LESS Exp {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp GEQ Exp {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp LEQ Exp {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp EQL Exp {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp NEQ Exp {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp PLUS Exp /*{ $$ = $1 + $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp MINUS Exp /*{ $$ = $1 - $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp STAR Exp /*{ $$ = $1 * $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp DIV Exp /*{ $$ = $1 / $3; }*/ {$$ = new_node("Exp", 3, $1, $2, $3);}
|   LP Exp RP {$$ = new_node("Exp", 3, $1, $2, $3);}
|   MINUS Exp %prec SOLO {$$ = new_node("Exp", 2, $1, $2);}
|   NOT Exp {$$ = new_node("Exp", 2, $1, $2);}
|   ID LP Args RP {$$ = new_node("Exp", 4, $1, $2, $3, $4);}
|   ID LP RP {$$ = new_node("Exp", 3, $1, $2, $3);}
|   Exp LB Exp RB {$$ = new_node("Exp", 4, $1, $2, $3, $4);}
|   Exp DOT ID {$$ = new_node("Exp", 3, $1, $2, $3);}
|   ID {$$ = new_node("Exp", 1, $1);}
|   INT {$$ = new_node("Exp", 1, $1);}
|   OCT {$$ = new_node("Exp", 1, $1);}
|   HEX {$$ = new_node("Exp", 1, $1);}
|   FLOAT {$$ = new_node("Exp", 1, $1);}
|   Exp LB Exp error RB
;

Args: 
    Exp COMMA Args {$$ = new_node("Args", 3, $1, $2, $3);}
|   Exp {$$ = new_node("Args", 1, $1);}
;
%%



