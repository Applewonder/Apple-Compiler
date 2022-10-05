#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include <assert.h>

struct ast* new_node(char* name, int num, ...) {
    va_list valist;
    struct ast* fa = (struct ast*)malloc(sizeof(struct ast));
    fa -> left = NULL;
    fa -> right = NULL;
    fa -> token_name = name;
    va_start(valist, num);
    if ( num == 0 ) {
        int line_n = va_arg(valist, int);
        fa -> line_num = line_n;
        if(!strcmp(fa -> token_name, "ID") || !strcmp(fa -> token_name, "TYPE")) {
            char* tmp = (char*)malloc(sizeof(char*) * 40);
            strcpy(tmp, yytext);
            fa -> name = tmp;
        } else if (!strcmp(fa -> token_name, "HEX")) {
            fa -> token_name = "INT";
            fa -> ival = strtol(yytext, NULL, 16); 
        } else if (!strcmp(fa -> token_name, "OCT")) {
            fa -> token_name = "INT";
            fa -> ival = strtol(yytext, NULL, 8);
        } else if (!strcmp(fa -> token_name, "INT")) {
            fa -> ival = atoi(yytext);
        } else if (!strcmp(fa -> token_name, "FLOAT")) {
            fa -> fval = atof(yytext);
        } else if (line_n == -1) {

        } else {
            char* tmp = (char*)malloc(sizeof(char*) * 10);
            strcpy(tmp, yytext);
            fa -> name = tmp;
        } 
    } else if (num == 1) {
        struct ast* child = (struct ast*)malloc(sizeof(struct ast));
        child = va_arg(valist, struct ast* );
        fa -> line_num = child -> line_num;
        fa -> left = child;
    } else if ( num >= 2) {
        struct ast* child = (struct ast*)malloc(sizeof(struct ast));
        
        child = va_arg(valist, struct ast* );
        
        fa -> line_num = child -> line_num;
        fa -> left = child;
        
        
        for (int i = 1; i < num; i ++) {
            child -> right = (struct ast*)malloc(sizeof(struct ast));
            child -> right = va_arg(valist, struct ast* );
            child = child -> right;
        }
    }
    return fa;
}

void print_ast(struct ast* root, int depth) {
    if (root != NULL) {
        //printf("%d %8x %8x %8x\n", depth, root, root->left, root->right);
        
        
        if (root -> line_num != -1) {
            for (int i = 0; i < depth; i++)
            {
                printf("  ");
            }
            printf("%s", root -> token_name);
            if(!strcmp(root -> token_name,"TYPE") || !strcmp(root -> token_name,"ID")) {
                printf(": %s", root -> name);
            } else if (!strcmp(root -> token_name, "OCT") || !strcmp(root -> token_name, "HEX") || !strcmp(root -> token_name, "INT")) {
                printf(": %d", root -> ival);
            } else if (!strcmp(root -> token_name, "FLOAT")) {
                printf(": %f", root -> fval);
            } else if (root -> left != NULL) {
                printf(" (%d)", root -> line_num);
            }
            printf("\n");
        }
       
        
        //printf("left    %8x\n", root -> left);
        struct ast* itr = root -> left;
        // if (!strcmp(root -> token_name, "ExtDef")) {
        //     printf("here\n");
        //     printf (" %s\n", root -> left -> token_name);
        //     printf (" %s\n", root -> left -> right -> token_name);
        //     printf (" %s\n", root -> left -> right -> right -> token_name);
        // }
        while (itr != NULL) {
            print_ast(itr, depth + 1);
            itr = itr -> right;
        }
    }
}