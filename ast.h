#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <string.h>

extern int yylineno;
extern char* yytext;
void yyerror();

struct ast {
    int line_num;
    char* token_name;
    int ival;
    float fval;
    char* name;
    struct ast* left;
    struct ast* right;
    
};

struct ast* new_node(char* name, int num, ...);
void print_ast(struct ast* root, int depth);

#endif