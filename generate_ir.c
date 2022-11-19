#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
# include <stdlib.h>
# include "generate_ir.h"
# include "data_hash.h"
# include "string.h"
# include "ir_code.h"

//TODO: Args Exp Stmt Cond  
//TODO: VarDec TYPE STRUCT OptTag Tag StructSpecifier DefList Specifier Dec DecList Def ExtDecList FunDec VarList ParamDec StmtList CompSt

extern struct ast* root;

extern InterCodes start;

static int count_tmp = 0;
static int count_label = 0;

char* new_temp(int count) {
    char* tmp = malloc(sizeof(char)*10);
    sprintf(tmp, "%d", count);
    char* t = "t";
    return strcat(t, tmp);
}

char* new_label(int count) {
    char* tmp = malloc(sizeof(char)*10);
    sprintf(tmp, "%d", count);
    char* L = "L";
    return strcat(L, tmp);
}

ArgList translat_Args(struct ast* args, ArgList arg_list) {
    struct ast* exp = args->left;
    if (exp->right == NULL) {
        char* t_1 = new_temp(count_tmp ++);
        translate_Exp(exp, t_1);
        ArgList arg = malloc(sizeof(struct Arg_));
        arg->name = t_1;
        arg->tail = arg_list;
        return arg;
    } else {
        char* t_1 = new_temp(count_tmp ++);
        translate_Exp(exp, t_1);
        ArgList arg = malloc(sizeof(struct Arg_));
        arg->name = t_1;
        arg->tail = arg_list;
        ArgList re_arg = malloc(sizeof(struct Arg_));
        re_arg = translat_Args(exp->right->right, arg);
        return re_arg;
    }
}

void translate_Exp(struct ast* exp, char* place) {
    struct ast* fir = exp->left;
    if (!strcmp(fir->token_name, "INT")) {
        int imm = fir->ival;
        Operand imm_int = construct_const(imm);
        Operand place_var = construct_var_name(place);
        InterCodes ir_int = construct_inter_code_assign(place_var, imm_int);
        insert_inter_code(ir_int);
    } else if (!strcmp(fir->token_name, "ID")) {
        struct ast* sec = fir->right;
        if (sec == NULL) {
            Operand place_var = construct_var_name(place);
            Operand assign_var = construct_var_name(fir->name);
            InterCodes ir_var = construct_inter_code_assign(place_var, assign_var);
            insert_inter_code(ir_var);
        } else if (!strcmp(sec->token_name, "LP")) {
            struct ast* trd = sec->right;
            if (!strcmp(trd->token_name, "RP")) {
                if (!strcmp(fir->name, "read")) {
                    Operand place_var = construct_var_name(place);
                    InterCodes ir_read = construct_inter_code_read_function(place_var);
                    insert_inter_code(ir_read);
                } else {
                    Operand place_var = construct_var_name(place);
                    Operand function = construct_function(fir->name);
                    InterCodes ir_call = construct_inter_code_call_function(place_var, function);
                    insert_inter_code(ir_call);
                }
            } else {
                Sym_node func = find(fir->name, true);
                ArgList arg_list = NULL;
                arg_list = translat_Args(trd, NULL);
                if (!strcmp(fir->name, "write")) {
                    Operand arg_one = construct_var_name(arg_list->name);
                    InterCodes ir_write = construct_inter_code_write_function(arg_one);
                    insert_inter_code(ir_write);
                    Operand zero_int = construct_const(0);
                    Operand place_var = construct_var_name(place);
                    InterCodes ir_place = construct_inter_code_assign(place_var, zero_int);
                    insert_inter_code(ir_place);
                } else {
                    while (arg_list != NULL) {
                        Operand arg = construct_arg(arg_list->name);
                        InterCodes ir_arg = construct_inter_code_arg(arg);
                        insert_inter_code(ir_arg);
                        arg_list = arg_list->tail;
                    }
                    Operand place_var = construct_var_name(place);
                    Operand func_var = construct_function(fir->name);
                    InterCodes ir_func = construct_inter_code_call_function(place_var, func_var);
                    insert_inter_code(ir_func);
                }
            }
        }
    } else if (!strcmp(fir->token_name, "Exp")) {
        struct ast* op = fir->right;
        if (!strcmp(op->token_name, "ASSIGNOP")) {
            char* var_name = fir->name;
            char* t_1 = new_temp(count_tmp ++);
            translate_Exp(op->right, t_1);
            Operand place_var = construct_var_name(place);
            Operand assign_var = construct_var_name(fir->name);
            Operand tmp_var = construct_var_name(t_1);
            InterCodes ir_var = construct_inter_code_assign(assign_var, tmp_var);
            insert_inter_code(ir_var);
            InterCodes ir_place = construct_inter_code_assign(place_var, assign_var);
            insert_inter_code(ir_place);
        } else if (!strcmp(op->token_name, "PLUS")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(fir->name);
            Operand tmp_var_2 = construct_var_name(sec->name);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(ADD, place_var, tmp_var_1, tmp_var_2);
            
        } else if (!strcmp(op->token_name, "MINUS")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(fir->name);
            Operand tmp_var_2 = construct_var_name(sec->name);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(DMINUS, place_var, tmp_var_1, tmp_var_2);
        } else if (!strcmp(op->token_name, "STAR")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(fir->name);
            Operand tmp_var_2 = construct_var_name(sec->name);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(MUL, place_var, tmp_var_1, tmp_var_2);
        } else if (!strcmp(op->token_name, "DIV")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(fir->name);
            Operand tmp_var_2 = construct_var_name(sec->name);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(DIV, place_var, tmp_var_1, tmp_var_2);
        } else if (!strcmp(op->token_name, "AND") || !strcmp(op->token_name, "OR") || !strcmp(op->token_name, "RELOP")) {
            char* label_1 = new_label(count_label ++);
            char* label_2 = new_label(count_label ++);
            Operand place_var = construct_var_name(place);
            Operand zero_int = construct_const(0);
            InterCodes zero_assign = construct_inter_code_assign(place_var, zero_int);
            insert_inter_code(zero_assign);
            translate_Cond(exp, label_1, label_2);
            Operand op_label_1 = construct_label(label_1);
            InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
            insert_inter_code(ir_label_1);
            Operand one_int = construct_const(1);
            InterCodes one_assign = construct_inter_code_assign(place_var, one_int);
            insert_inter_code(one_assign);
            Operand op_label_2 = construct_label(label_2);
            InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
            insert_inter_code(ir_label_2);
        } else if (!strcmp(op->token_name, "LB")) {
            //TODO
        } else if (!strcmp(op->token_name, "DOT")) {
            //TODO
        }
    } else if (!strcmp(fir->token_name, "MINUS")) {
        char* t_1 = new_temp(count_tmp ++);
        translate_Exp(fir->right, t_1);
        Operand tmp_var = construct_var_name(t_1);
        Operand zero_int = construct_const(0);
        Operand place_var = construct_var_name(place);
        InterCodes ir_single_minus = construct_inter_code_binop(SUB, place_var, zero_int, tmp_var);
        insert_inter_code(ir_single_minus);
    } else if (!strcmp(fir->token_name, "NOT")) {
        char* label_1 = new_label(count_label ++);
        char* label_2 = new_label(count_label ++);
        Operand place_var = construct_var_name(place);
        Operand zero_int = construct_const(0);
        InterCodes zero_assign = construct_inter_code_assign(place_var, zero_int);
        insert_inter_code(zero_assign);
        translate_Cond(exp, label_1, label_2);
        Operand op_label_1 = construct_label(label_1);
        InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
        insert_inter_code(ir_label_1);
        Operand one_int = construct_const(1);
        InterCodes one_assign = construct_inter_code_assign(place_var, one_int);
        insert_inter_code(one_assign);
        Operand op_label_2 = construct_label(label_2);
        InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
        insert_inter_code(ir_label_2);
    }
}

void translate_Stmt(struct ast* stmt) {
    struct ast* fir = stmt->left;
    if (!strcmp(fir->token_name, "Exp")) {
        translate_Exp(fir, NULL);
    } else if (!strcmp(fir->token_name, "CompSt")) {
        translate_CompSt(fir);
    } else if (!strcmp(fir->token_name, "WHILE")) {
        char* label_1 = new_label(count_label ++);
        char* label_2 = new_label(count_label ++);
        char* label_3 = new_label(count_label ++);
        Operand op_label_1 = construct_label(label_1);
        Operand op_label_2 = construct_label(label_2);
        Operand op_label_3 = construct_label(label_3);
        InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
        insert_inter_code(ir_label_1);
        translate_Cond(fir->right->right, op_label_2, op_label_3);
        InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
        insert_inter_code(ir_label_2);
        translate_Stmt(fir->right->right->right->right);
        InterCodes goto_label_1 = construct_inter_code_label(op_label_1);
        insert_inter_code(goto_label_1);
        InterCodes ir_label_3 = construct_inter_code_label(op_label_3);
        insert_inter_code(ir_label_3);
    } else if (!strcmp(fir->token_name, "IF")) {
        struct ast* exp = fir->right->right;
        struct ast* stmt_1 = exp->right->right;
        struct ast* el = stmt_1->right;
        char* label_1 = new_label(count_label ++);
        char* label_2 = new_label(count_label ++);
        Operand op_label_1 = construct_label(label_1);
        Operand op_label_2 = construct_label(label_2);
        if (el == NULL) {
            translate_Cond(exp, op_label_1, op_label_2);
            InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
            insert_inter_code(ir_label_1);
            translate_Stmt(stmt_1);
            InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
            insert_inter_code(ir_label_2);
        } else {
            char* label_3 = new_label(count_label ++);
            Operand op_label_3 = construct_label(label_3);
            translate_Cond(exp, op_label_1, op_label_2);
            InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
            insert_inter_code(ir_label_1);
            translate_Stmt(stmt_1);
            InterCodes ir_goto =construct_inter_code_goto(op_label_3);
            insert_inter_code(ir_goto);
            InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
            insert_inter_code(ir_label_2);
            struct ast* stmt_2 = el->right;
            translate_Stmt(stmt_2);
            InterCodes ir_label_3 = construct_inter_code_label(op_label_3);
            insert_inter_code(ir_label_3);
        }
    }
}

void translate_Cond(struct ast* exp, char* label_true, char* label_false) {
    struct ast* fir = exp->left;
    if (!strcmp(fir->token_name, "NOT")) {
        translate_Cond(fir->right, label_false, label_true);
    } else if (!strcmp(fir->token_name, "Exp")) {
        struct ast* op = fir->right;
        if (!strcmp(op->token_name, "RELOP")) {
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            Operand t1 = construct_var_name(t_1);
            Operand t2 = construct_var_name(t_2);
            translate_Exp(fir,t_1);
            translate_Exp(op->right,t_2);
            Operand relop = construct_op(op->name);
            Operand label_t = construct_label(label_true);
            InterCodes ir_relop = construct_inter_code_if(t1, relop, t2, label_t);
            insert_inter_code(ir_relop);
            Operand label_f = construct_label(label_false);
            InterCodes ir_goto = construct_inter_code_goto(label_f);
            insert_inter_code(ir_goto);
        } else if (!strcmp(op->token_name, "AND")) {
            char* label = new_label(count_label ++);
            translate_Cond(fir, label, label_false);
            Operand label_t = construct_label(label);
            InterCodes ir_label = construct_inter_code_label(label_t);
            insert_inter_code(ir_label);
            translate_Cond(op->right, label_true, label_false);
        } else if (!strcmp(op->token_name, "OR")) {
            char* label = new_label(count_label ++);
            translate_Cond(fir, label_true, label);
            Operand label_t = construct_label(label);
            InterCodes ir_label = construct_inter_code_label(label_t);
            insert_inter_code(ir_label);
            translate_Cond(op->right, label_true, label_false);
        }
    } else {
        char* t_1 = new_temp(count_tmp ++);
        translate_Exp(exp, t_1);
        Operand t1 = construct_var_name(t_1);
        Operand op = construct_op("!=");
        Operand zero_int = construct_const(0);
        Operand label_t = construct_label(label_true);
        InterCodes ir_if = construct_inter_code_if(t1, op, zero_int, label_t);
        insert_inter_code(ir_if);
        Operand label_f = construct_label(label_false);
        InterCodes ir_goto = construct_inter_code_goto(label_f);
        insert_inter_code(ir_goto);
    }
}





void generate_ir(struct ast* root_, int depth) {
    if (root_ != NULL) {
        //printf("%d %8x %8x %8x\n", depth, root, root->left, root->right);
        if (root_ -> line_num != -1) {
            if(!strcmp(root_ -> token_name,"ExtDef")) {
                
                struct ast* spec = root_->left;
                
                Type t_spec =  Specifier(spec);
                
                struct ast* judge = spec->right;
            
                if (strcmp(judge->token_name, "SEMI")) {
                    if (!strcmp(judge->token_name, "ExtDecList")) {
                        ExtDecList(judge, t_spec);
                    } else {
                        struct ast* cpmst = judge->right;
                        if (!strcmp(cpmst->token_name, "SEMI")) {
                            Type f_ty = FunDec(judge, t_spec, false);
                        } else {
                            Type f_ty = FunDec(judge, t_spec, true);
                            CompSt(cpmst); 
                        }
                    }
                } 
            } 
        }
       
        
        //printf("left    %8x\n", root -> left);
        struct ast* itr = root_ -> left;
        
        while (itr != NULL) {
            construct_sym_table(itr, depth + 1);
            itr = itr -> right;
        }
    }
}