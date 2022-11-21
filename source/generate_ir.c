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
static char* tmp_exp_array_name = NULL;
static char* tmp_exp_struc_name = NULL;

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
        case RETURN:{
            Operand var = code->u.variable;
            char* var_re = print_operand(var, false);
            fprintf(out_file, "RETURN %s\n", var_re);
            break;
        }
        case ADDR:{
            Operand dest = code->u.addr.dest;
            Operand src = code->u.addr.src;
            char* dest_f = print_operand(dest, false);
            char* src_f = print_operand(src, false);
            if (dest_f == NULL || src_f == NULL) break;
            fprintf(out_file, "%s := *%s\n", dest_f, src_f);
            break;
        }
        case MEM:{
            Operand dest = code->u.addr.dest;
            Operand src = code->u.addr.src;
            char* dest_f = print_operand(dest, false);
            char* src_f = print_operand(src, false);
            if (dest_f == NULL || src_f == NULL) break;
            fprintf(out_file, "*%s := %s\n", dest_f, src_f);
            break;
        }
        case GETADDR:{
            Operand dest = code->u.get_addr.dest;
            Operand src = code->u.get_addr.src;
            char* dest_f = print_operand(dest, false);
            char* src_f = print_operand(src, false);
            if (dest_f == NULL || src_f == NULL) break;
            fprintf(out_file, "%s := &%s\n", dest_f, src_f);
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
    sprintf(tmp, "tmp_value_%d", count);
    return tmp;
}

char* new_label(int count) {
    char* tmp = malloc(sizeof(char)*10);
    sprintf(tmp, "L%d", count);
    return tmp;
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

int calculate_array_size(Type type) {
    if (type->kind == T_AR) {
        int size = calculate_array_size(type->u.array.elem);
        size *= type->u.array.size;
        return size;
    } else {
        return calculate_type_size(type);
    }
}

int calculate_stru_size(FieldList field, int size) {
    if (field == NULL) return size;
    int cur_size = calculate_type_size(field->type);
    cur_size += size;
    int tail_size = calculate_stru_size(field->basic_tail, cur_size);
    return tail_size;
}

int calculate_type_size(Type type) {
    switch (type->kind)
    {
    case T_INT:
    case T_FLOAT:
        return 4;
    case T_AR:
        return calculate_array_size(type);
    case T_STRU:
        return calculate_stru_size(type->u.structure, 0);
    default:
        printf("real fun\n");
        return -1;
        break;
    }
}

void calculate_struc_mem_start(struct ast* exp) {
    if (!strcmp(exp->left->token_name, "ID")) {
        tmp_exp_struc_name = exp->left->name;
    } else if (!strcmp(exp->left->right->token_name, "LB")) {
        calculate_array_elem_size(exp->left);
        tmp_exp_struc_name = tmp_exp_array_name;
    }
}

Type calculate_array_elem_size(struct ast* exp) {
    Type type = NULL;
    struct ast* num = NULL;
    if (!strcmp(exp->left->token_name, "ID") || !strcmp(exp->left->right->token_name, "DOT")) {
        if (!strcmp(exp->left->token_name, "ID")) {
            Sym_node arr_node = find (exp->left->name, false);
            char* tmp_name = new_temp(count_tmp ++);
            tmp_exp_array_name = tmp_name;
            Operand tmp = construct_var_name(tmp_name);
            Operand addr_ar = construct_var_name(exp->left->name);
            InterCodes ir_pass_addr = construct_inter_code_assign(tmp, addr_ar);
            insert_inter_code(ir_pass_addr);
            type = arr_node->type;
        } else {
            // Sym_node arr_node = find (exp->left->right->right->name, false);
            // char* tmp_name = new_temp(count_tmp ++);
            // tmp_exp_array_name = tmp_name;
            // Operand tmp = construct_var_name(tmp_name);
            // Operand addr_ar = construct_var_name(arr_node->name);
            // InterCodes ir_pass_addr = construct_inter_code_assign(tmp, addr_ar);
            // insert_inter_code(ir_pass_addr);
            // type = arr_node->type;
            calculate_struc_mem_start(exp->left);
            struct ast* offset = exp->left->right->right;
            char* id_off = offset->name;
            int offs = find_offset(id_off);
            Operand const_num = construct_const(offs);
            char* tmp_name = new_temp(count_tmp ++);
            tmp_exp_array_name = tmp_name;
            Operand tmp = construct_var_name(tmp_name);
            Operand struc_node = construct_var_name(tmp_exp_struc_name);
            InterCodes ir_pass_addr = construct_inter_code_binop(ADD, tmp, struc_node, const_num);
            insert_inter_code(ir_pass_addr);
            type = find(id_off, false)->type;
        }   
    } else {
        type = calculate_array_elem_size(exp->left);
    }
    num = exp->right->right;
    char* t_1 = new_temp(count_tmp ++);
    translate_Exp(num, t_1);
    Operand count = construct_var_name(t_1);
    char* t_2 = new_temp(count_tmp ++);
    Operand tmp_2 = construct_var_name(t_2);
    int cur_size = calculate_type_size(type->u.array.elem);
    Operand size_op = construct_const(cur_size);
    InterCodes ir_mul = construct_inter_code_binop(MUL, tmp_2, count, size_op);
    insert_inter_code(ir_mul);
    Operand address = construct_var_name(tmp_exp_array_name);
    InterCodes ir_add = construct_inter_code_binop(ADD, address, address, tmp_2);
    insert_inter_code(ir_add);
    return type->u.array.elem;
}

char* translate_array(struct ast* vardec) {
    char* array_name = find_array_name(vardec);
    
    Sym_node array_node = find(array_name, false);
    int size = 1;
    Type array_t = array_node->type;
    while (array_t->kind == T_AR) {
        size *= array_t->u.array.size;
        array_t = array_t->u.array.elem;
    }
    size *= calculate_type_size(array_t);
    Operand array_size = construct_const(size);
    Operand array_var = construct_var_name(array_name);
    char* t_1 = new_temp(count_tmp ++);
    Operand tmp = construct_var_name(t_1);
    InterCodes ir_space = construct_inter_code_dec(tmp, array_size);
    insert_inter_code(ir_space);
    InterCodes ir_array = construct_inter_code_get_address(array_var, tmp);
    insert_inter_code(ir_array);
    return array_name;
}

char* translate_VarDec(struct ast* vardec) {
    struct ast* fir = vardec->left;
    if (!strcmp(fir->token_name, "ID")) {
        return fir->name;
    } else {
        return translate_array(vardec);
        
    }
}

void translate_struct_inside_struct(int start, Type type) {
    FieldList field = type->u.structure;
    int offset = start;
    while (field != NULL) {
        insert_offset(offset, field->name);
        if (field->type->kind == T_STRU) {
            translate_struct_inside_struct(offset, field->type);
        }
        int mem_size = calculate_type_size(field->type);
        offset += mem_size;
        field = field->basic_tail;
    }
}

void translate_Dec(struct ast* dec) {
    struct ast* vardec = dec->left;
    char* var_name = translate_VarDec(vardec);
    struct ast* assig = vardec->right;
    if (assig != NULL) {
        translate_Exp(assig->right, var_name);
    } else {
        Sym_node struct_node = find(var_name, false);
        if (struct_node->type->kind == T_STRU) {
            char* t_1 = new_temp(count_tmp ++);
            Operand tmp = construct_var_name(t_1);
            int size = calculate_stru_size(struct_node->type->u.structure, 0);
            Operand struc_size = construct_const(size);
            InterCodes ir_space = construct_inter_code_dec(tmp, struc_size);
            insert_inter_code(ir_space);
            Operand struc_name = construct_var_name(var_name);
            InterCodes ir_array = construct_inter_code_get_address(struc_name, tmp);
            insert_inter_code(ir_array);
            FieldList field = struct_node->type->u.structure;
            int offset = 0;
            while (field != NULL) {
                insert_offset(offset, field->name);
                if (field->type->kind == T_STRU) {
                    translate_struct_inside_struct(offset, field->type);
                }
                int mem_size = calculate_type_size(field->type);
                offset += mem_size;
                field = field->basic_tail;
            }
        }
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
        insert_inter_code(ir_param);
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
    if (!strcmp(specif->left->token_name, "StructSpecifier")) return;
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
            Operand assign_var = NULL;
            if (!strcmp(fir->left->token_name, "ID")) {
                assign_var = construct_var_name(fir->left->name);
                Operand tmp_var = construct_var_name(t_1);
                InterCodes ir_var = construct_inter_code_assign(assign_var, tmp_var);
                insert_inter_code(ir_var);
                InterCodes ir_place = construct_inter_code_assign(place_var, assign_var);
                insert_inter_code(ir_place);
            }else if (!strcmp(fir->left->token_name, "Exp")) {
                if (!strcmp(fir->left->right->token_name, "LB")) {
                    Type type = calculate_array_elem_size(fir->left);
                    assign_var = construct_var_name(tmp_exp_array_name);
                    Operand tmp_var = construct_var_name(t_1);
                    if (type->kind == T_AR || type->kind == T_STRU) {
                        InterCodes ir_var = construct_inter_code_assign(assign_var, tmp_var);
                        insert_inter_code(ir_var);
                    } else {
                        InterCodes ir_var = construct_inter_code_mem(assign_var, tmp_var);
                        insert_inter_code(ir_var);
                    }
                    InterCodes ir_place = construct_inter_code_assign_address(place_var, assign_var);
                    insert_inter_code(ir_place);
                } else {
                    Operand tmp_var = construct_var_name(t_1);
                    calculate_struc_mem_start(fir->left);
                    Operand base = construct_var_name(tmp_exp_struc_name);
                    struct ast* id = fir->left->right->right;
                    int offs = find_offset(id->name);
                    Operand id_s = construct_const(offs);
                    char* t_2 = new_temp(count_tmp ++);
                    Operand tmp_2 = construct_var_name(t_2);
                    InterCodes ir_addr = construct_inter_code_binop(ADD, tmp_2, base, id_s);
                    insert_inter_code(ir_addr);
                    InterCodes ir_var = construct_inter_code_mem(tmp_2, tmp_var);
                    insert_inter_code(ir_var);
                }
            }    
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
            Type type = calculate_array_elem_size(fir);
            Operand addr = construct_var_name(tmp_exp_array_name);
            Operand place_var = construct_var_name(place);
            if (type->kind == T_AR || type->kind == T_STRU) {
                InterCodes ir_addr = construct_inter_code_assign(place_var, addr);
                insert_inter_code(ir_addr);
            } else {
                InterCodes ir_addr = construct_inter_code_assign_address(place_var, addr);
                insert_inter_code(ir_addr);
            }
        } else if (!strcmp(op->token_name, "DOT")) {
            calculate_struc_mem_start(fir);
            Operand place_var = construct_var_name(place);
            Operand id_s = construct_const(find_offset(op->right->name));
            Operand base = construct_var_name(tmp_exp_struc_name);
            char* t_3 = new_temp(count_tmp ++);
            Operand tmp_3 = construct_var_name(t_3);
            InterCodes ir_base = construct_inter_code_binop(ADD, tmp_3, base, id_s);
            insert_inter_code(ir_base);
            InterCodes ir_addr = construct_inter_code_assign_address(place_var, tmp_3);
            insert_inter_code(ir_addr);
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
        InterCodes goto_label_1 = construct_inter_code_goto(op_label_1);
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
    } else if (!strcmp(fir->token_name, "RETURN")) {
        char* t_1 = new_temp(count_tmp ++);
        translate_Exp(fir->right, t_1);
        Operand return_var = construct_var_name(t_1);
        InterCodes ir_return = construct_inter_code_return(return_var);
        insert_inter_code(ir_return);
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