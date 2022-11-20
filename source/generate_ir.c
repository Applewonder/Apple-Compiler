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
//TODO: VarDec TYPE STRUCT OptTag Tag StructSpecifier DefList Specifier Dec DecList Def FunDec VarList ParamDec StmtList CompSt

extern struct ast* root;
extern InterCodes start;
extern FILE* out_file;

static int count_tmp = 0;
static int count_label = 0;

char* print_operand(Operand ope, bool is_dec) {
    int kind = ope->kind;
    switch (kind)
    {
    case VARIABLE:
        return ope->u.var_name;
        break;
    case CONSTANT:{
        char* num = malloc(sizeof(char) * 10);
        if (is_dec) {
            sprintf(num, "%d", ope->u.value);
        } else {
            sprintf(num, "#%d", ope->u.value);
        }
        return num;
    }
    case ADDRESS:
        break;
    case LABEl:
        return ope->u.var_name;
        break;
    case OP:
        return ope->u.op;
        break;
    case FUNC:
        return ope->u.f_name;
        break;
    case ARGUE:
        return ope->u.arg_name;
        break;
    default:
        printf("fun\n");
        
        break;
    }
    return NULL;
}

void printf_binop(int kind, char* result, char* op1, char* op2) {
    switch (kind)
    {
    case ADD:{
        fprintf(out_file, "%s := %s + %s\n", result, op1, op2);
        break;
    }  
    case SUB:{
        fprintf(out_file, "%s := %s - %s\n", result, op1, op2);
        break;
    }
    case MUL:{
        fprintf(out_file, "%s := %s * %s\n", result, op1, op2);
        break; 
    }
    case DIV:{
        fprintf(out_file, "%s := %s / %s\n", result, op1, op2);
        break;
    }   
    default:
    printf("so fun\n");
        break;
    }
}

void print_ir(InterCodes cur) {
    while (cur != NULL) {
        InterCode code = cur->code;
        int kind = code->kind;
        switch (kind)
        {
        case ASSIGN:{
            char* left = print_operand(code->u.assign.left, false);
            char* right = print_operand(code->u.assign.right, false);
            if (left == NULL || right == NULL) break;
            fprintf(out_file, "%s := %s\n", left, right);
            break;
        }
        case ADD:
        case SUB:
        case MUL:
        case DIV:{
            char* result = print_operand(code->u.binop.result, false);
            char* op1 = print_operand(code->u.binop.op1, false);
            char* op2 = print_operand(code->u.binop.op2, false);
            printf_binop(kind, result, op1, op2);
            break;
        } 
        case TAG:{
            char* label = print_operand(code->u.label, false);
            fprintf(out_file, "LABEL %s :\n", label);
            break;
        }  
        case GOTO:{
            char* label = print_operand(code->u.label, false);
            fprintf(out_file, "GOTO %s\n", label);
            break; 
        }
        case IF:{
            Operand candidiate1 = code->u.ifnop.candidate1;
            Operand candidiate2 = code->u.ifnop.candidate2;
            Operand op = code->u.ifnop.op;
            Operand label = code->u.ifnop.label;
            char* label_if = print_operand(label, false);
            char* op_ir = print_operand(op, false);
            char* can_ir_1 = print_operand(candidiate1, false);
            char* can_ir_2 = print_operand(candidiate2, false);
            fprintf(out_file, "IF %s %s %s GOTO %s\n", can_ir_1, op_ir, can_ir_2, label_if);
            break;
        }
        case READ:{
            Operand read_var = code->u.variable;
            char* var = print_operand(read_var, false);
            fprintf(out_file, "READ %s\n", var);
            break;
        }
        case WRITE:{
            Operand read_var = code->u.variable;
            char* var = print_operand(read_var, false);
            fprintf(out_file, "WRITE %s\n", var);
            break;
        }
        case CALL:{
            Operand res = code->u.callf.res;
            Operand function = code->u.callf.function;
            char* res_f = print_operand(res, false);
            char* func = print_operand(function, false);
            fprintf(out_file, "%s := CALL %s\n", res_f, func);
            break;
        }
        case ARG:{
            Operand arg = code->u.arg;
            char* arg_f = print_operand(arg, false);
            fprintf(out_file, "ARG %s\n", arg_f);
            break;
        }
        case FUNCTION:{
            Operand func = code->u.func;
            char* fun_f = print_operand(func, false);
            fprintf(out_file, "FUNCTION %s :\n", fun_f);
            break;
        }
        case PARAM:{
            Operand param = code->u.param;
            char* param_f = print_operand(param, false);
            fprintf(out_file, "PARAM %s\n", param_f);
            break;
        }
        case DEC:{
            Operand var = code->u.dec_ar.var;
            Operand size = code->u.dec_ar.size;
            char* var_f = print_operand(var, true);
            char* size_f = print_operand(size, true);
            fprintf(out_file, "DEC %s %s\n", var_f, size_f);
            break;
        }
        default:
            break;
        }
        cur = cur->next;
    }
}

char* new_temp(int count) {
    char* tmp = malloc(sizeof(char)*10);
    sprintf(tmp, "t%d", count);
    return tmp;
}

char* new_label(int count) {
    char* tmp = malloc(sizeof(char)*10);
    sprintf(tmp, "L%d", count);
    return tmp;
}

char* compute_array_length(struct ast* tree_node) {

}
 
char* find_array_name(struct ast* vardec) {
    struct ast* fir = vardec->left;
    if (!strcmp(fir->token_name, "ID")) {
        return fir->name;
    } else {
        char* name = find_array_name(fir);
        return name;
    }
}

void translate_array(struct ast* vardec) {
    char* array_name = find_array_name(vardec);
    Sym_node array_node = find(array_name, false);
    int size = 1;
    Type array_t = array_node->type;
    while (array_t->kind == T_AR) {
        size *= array_t->u.array.size;
        array_t = array_t->u.array.elem;
    }
    size *= 4;
    Operand array_size = construct_const(size);
    Operand array_var = construct_var_name(array_name);
    InterCodes ir_array = construct_inter_code_dec(array_var, array_size);
    insert_inter_code(ir_array);
}

char* translate_VarDec(struct ast* vardec) {
    struct ast* fir = vardec->left;
    if (!strcmp(fir->token_name, "ID")) {
        return fir->name;
    } else {
        translate_array(vardec);
        return NULL;
    }
}

void translate_Dec(struct ast* dec) {
    struct ast* vardec = dec->left;
    char* var_name = translate_VarDec(vardec);
    struct ast* assig = vardec->right;
    if (assig != NULL) {
        translate_Exp(assig->right, var_name);
    }
}

void translate_DecList(struct ast* declist) {
    struct ast* dec = declist->left;
    struct ast* comma = dec->right;
    if (comma == NULL) {
        translate_Dec(dec);
    } else {
        translate_Dec(dec);
        translate_DecList(comma->right);
    }
}

void translate_Def(struct ast* def) {
    struct ast* declist = def->left->right;
    translate_DecList(declist);
}

void translate_DefList(struct ast* deflist) {
    if (deflist->line_num == -1) return;
    struct ast* def = deflist->left;
    translate_Def(def);
    translate_DefList(def->right);
}

void translate_FunDec(struct ast* fundec) {
    struct ast* f_ast = fundec->left;
    Sym_node function = find(f_ast->name, true);
    Operand f_node = construct_function(f_ast->name);
    InterCodes ir_f = construct_inter_code_func(f_node);
    insert_inter_code(ir_f);
    ParamList params = function->type->u.function->param_list;
    while (params != NULL) {
        char* param_name = params->name;
        Operand param_var = construct_var_name(param_name);
        InterCodes ir_param = construct_inter_code_paramlist(param_var);
        params = params->tail;
    }
}

void translate_StmtList(struct ast* stmtlist) {
    if (stmtlist->line_num == -1) return;
    struct ast* stm = stmtlist->left;
    translate_Stmt(stm);
    translate_StmtList(stm->right);
}

void translate_CompSt(struct ast* compst) {
    struct ast* deflist = compst->left->right;
    struct ast* stmtlist = deflist->right;
    translate_DefList(deflist);
    translate_StmtList(stmtlist);
}

void translate_ExtDef(struct ast* extdef) {
    struct ast* specif = extdef->left;
    struct ast* func = specif->right;
    struct ast* compst = func->right;
    translate_FunDec(func);
    translate_CompSt(compst);
}

ArgList translate_Args(struct ast* args, ArgList arg_list) {
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
        re_arg = translate_Args(exp->right->right, arg);
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
                arg_list = translate_Args(trd, NULL);
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
            Operand assign_var = construct_var_name(fir->left->name);//TODO:struct array
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
            Operand tmp_var_1 = construct_var_name(t_1);
            Operand tmp_var_2 = construct_var_name(t_2);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(ADD, place_var, tmp_var_1, tmp_var_2);
            insert_inter_code(ir_plus);
        } else if (!strcmp(op->token_name, "MINUS")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(t_1);
            Operand tmp_var_2 = construct_var_name(t_2);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(SUB, place_var, tmp_var_1, tmp_var_2);
            insert_inter_code(ir_plus);
        } else if (!strcmp(op->token_name, "STAR")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(t_1);
            Operand tmp_var_2 = construct_var_name(t_2);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(MUL, place_var, tmp_var_1, tmp_var_2);
            insert_inter_code(ir_plus);
        } else if (!strcmp(op->token_name, "DIV")) {
            struct ast* sec = op->right;
            char* t_1 = new_temp(count_tmp ++);
            char* t_2 = new_temp(count_tmp ++);
            translate_Exp(fir, t_1);
            translate_Exp(sec, t_2);
            Operand tmp_var_1 = construct_var_name(t_1);
            Operand tmp_var_2 = construct_var_name(t_2);
            Operand place_var = construct_var_name(place);
            InterCodes ir_plus = construct_inter_code_binop(DIV, place_var, tmp_var_1, tmp_var_2);
            insert_inter_code(ir_plus);
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
    } else if (!strcmp(fir->token_name, "LP")) {
        struct ast* sec = fir->right;
        translate_Exp(sec, place);
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
        translate_Cond(fir->right->right, label_2, label_3);
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
            translate_Cond(exp, label_1, label_2);
            InterCodes ir_label_1 = construct_inter_code_label(op_label_1);
            insert_inter_code(ir_label_1);
            translate_Stmt(stmt_1);
            InterCodes ir_label_2 = construct_inter_code_label(op_label_2);
            insert_inter_code(ir_label_2);
        } else {
            char* label_3 = new_label(count_label ++);
            Operand op_label_3 = construct_label(label_3);
            translate_Cond(exp, label_1, label_2);
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
    struct ast* extdeclist = root_->left;
    while (extdeclist->line_num != -1) {
        struct ast* extdef = extdeclist->left;
        translate_ExtDef(extdef);
        extdeclist = extdef->right;
    }
}