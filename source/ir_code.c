# include "ir_code.h"
#include "ir_code.h"
#include <stdlib.h>

InterCodes start;
InterCodes end;

void insert_inter_code(InterCodes inter_code) {
    if (start == NULL) {
        start = inter_code;
        end = inter_code;
        return;
    }
    inter_code->prev = end;
    end->next = inter_code;
    end = inter_code;
}

InterCodes construct_inter_code_assign(Operand left, Operand right) {
    InterCode inter_assign = malloc(sizeof(struct InterCode_));
    inter_assign->kind = ASSIGN;
    inter_assign->u.assign.left = left;
    inter_assign->u.assign.right = right;
    InterCodes assign_node = malloc(sizeof(struct InterCodes_));
    assign_node->code = inter_assign;
    assign_node->next = NULL;
    assign_node->prev = NULL;
    return assign_node;
}

InterCodes construct_inter_code_binop(int kind, Operand result, Operand op1, Operand op2) {
    InterCode inter_binop = malloc(sizeof(struct InterCode_));
    inter_binop->kind = kind;
    inter_binop->u.binop.result = result;
    inter_binop->u.binop.op1 = op1;
    inter_binop->u.binop.op2 = op2;
    InterCodes bi_node = malloc(sizeof(struct InterCodes_));
    bi_node->code = inter_binop;
    bi_node->next = NULL;
    bi_node->prev = NULL;
    return bi_node;
}

InterCodes construct_inter_code_label(Operand label) {
    InterCode inter_label = malloc(sizeof(struct InterCode_));
    inter_label->kind = TAG;
    inter_label->u.label = label;
    InterCodes label_node = malloc(sizeof(struct InterCodes_));
    label_node->code = inter_label;
    label_node->next = NULL;
    label_node->prev = NULL;
    return label_node;
}

InterCodes construct_inter_code_goto(Operand label) {
    InterCode inter_goto = malloc(sizeof(struct InterCode_));
    inter_goto->kind = GOTO;
    inter_goto->u.label = label;
    InterCodes goto_node = malloc(sizeof(struct InterCodes_));
    goto_node->code = inter_goto;
    goto_node->next = NULL;
    goto_node->prev = NULL;
    return goto_node;
}

InterCodes construct_inter_code_assign_address(Operand place_var, Operand addr) {
    InterCode inter_addr = malloc(sizeof(struct InterCode_));
    inter_addr->kind = ADDR;
    inter_addr->u.addr.dest = place_var;
    inter_addr->u.addr.src = addr;
    InterCodes addr_node = malloc(sizeof(struct InterCodes_));
    addr_node->code = inter_addr;
    addr_node->next = NULL;
    addr_node->prev = NULL;
    return addr_node;
}

InterCodes construct_inter_code_get_address(Operand dest, Operand src) {
    InterCode inter_get_addr = malloc(sizeof(struct InterCode_));
    inter_get_addr->kind = GETADDR;
    inter_get_addr->u.get_addr.dest = dest;
    inter_get_addr->u.get_addr.src = src;
    InterCodes get_addr_node = malloc(sizeof(struct InterCodes_));
    get_addr_node->code = inter_get_addr;
    get_addr_node->next = NULL;
    get_addr_node->prev = NULL;
    return get_addr_node;
}

Operand construct_arg(char* arg) {
    Operand argue = malloc(sizeof(struct Operand_));
    argue->kind = ARGUE;
    argue->u.arg_name = arg;
    return argue;
}

Operand construct_op(char* op) {
    Operand operand = malloc(sizeof(struct Operand_));
    operand->kind = OP;
    operand->u.op = op;
    return operand;
}

InterCodes construct_inter_code_return(Operand var_name) {
    InterCode inter_return = malloc(sizeof(struct InterCode_));
    inter_return->kind = RETURN;
    inter_return->u.variable = var_name;
    InterCodes return_node = malloc(sizeof(struct InterCodes_));
    return_node->code = inter_return;
    return_node->next = NULL;
    return_node->prev = NULL;
    return return_node;
}


InterCodes construct_inter_code_dec(Operand var_name, Operand size) {
    InterCode inter_dec = malloc(sizeof(struct InterCode_));
    inter_dec->kind = DEC;
    inter_dec->u.dec_ar.size = size;
    inter_dec->u.dec_ar.var = var_name;
    InterCodes dec_node = malloc(sizeof(struct InterCodes_));
    dec_node->code = inter_dec;
    dec_node->next = NULL;
    dec_node->prev = NULL;
    return dec_node;
}



InterCodes construct_inter_code_mem(Operand dest, Operand src) {
    InterCode inter_mem = malloc(sizeof(struct InterCode_));
    inter_mem->kind = MEM;
    inter_mem->u.mem.dest = dest;
    inter_mem->u.mem.src = src;
    InterCodes mem_node = malloc(sizeof(struct InterCodes_));
    mem_node->code = inter_mem;
    mem_node->next = NULL;
    mem_node->prev = NULL;
    return mem_node;
}

InterCodes construct_inter_code_paramlist(Operand param) {
    InterCode inter_param = malloc(sizeof(struct InterCode_));
    inter_param->kind = PARAM;
    inter_param->u.param = param;
    InterCodes param_node = malloc(sizeof(struct InterCodes_));
    param_node->code = inter_param;
    param_node->next = NULL;
    param_node->prev = NULL;
    return param_node;
}

InterCodes construct_inter_code_func(Operand func) {
    InterCode inter_f = malloc(sizeof(struct InterCode_));
    inter_f->kind = FUNCTION;
    inter_f->u.func = func;
    InterCodes f_node = malloc(sizeof(struct InterCodes_));
    f_node->code = inter_f;
    f_node->next = NULL;
    f_node->prev = NULL;
    return f_node;
}

InterCodes construct_inter_code_if(Operand candidate1, Operand op, Operand candidate2, Operand label) {
    InterCode inter_goto = malloc(sizeof(struct InterCode_));
    inter_goto->kind = IF;
    inter_goto->u.ifnop.label = label;
    inter_goto->u.ifnop.candidate1 = candidate1;
    inter_goto->u.ifnop.candidate2 = candidate2;
    inter_goto->u.ifnop.op = op;
    InterCodes goto_node = malloc(sizeof(struct InterCodes_));
    goto_node->code = inter_goto;
    goto_node->next = NULL;
    goto_node->prev = NULL;
    return goto_node;
}

InterCodes construct_inter_code_read_function(Operand place_var) {
    InterCode inter_read = malloc(sizeof(struct InterCode_));
    inter_read->kind = READ;
    inter_read->u.variable = place_var;
    InterCodes read_node = malloc(sizeof(struct InterCodes_));
    read_node->code = inter_read;
    read_node->prev = NULL;
    read_node->next = NULL;
    return read_node;
}

InterCodes construct_inter_code_write_function(Operand place_var) {
    InterCode inter_write = malloc(sizeof(struct InterCode_));
    inter_write->kind = WRITE;
    inter_write->u.variable = place_var;
    InterCodes write_node = malloc(sizeof(struct InterCodes_));
    write_node->code = inter_write;
    write_node->prev = NULL;
    write_node->next = NULL;
    return write_node;
}

InterCodes construct_inter_code_call_function(Operand place_var, Operand function) {
    InterCode inter_read = malloc(sizeof(struct InterCode_));
    inter_read->kind = CALL;
    inter_read->u.callf.function = function;
    inter_read->u.callf.res = place_var;
    InterCodes call_node = malloc(sizeof(struct InterCodes_));
    call_node->code = inter_read;
    call_node->prev = NULL;
    call_node->next = NULL;
    return call_node;
}

InterCodes construct_inter_code_arg(Operand arg) {
    InterCode inter_arg = malloc(sizeof(struct InterCode_));
    inter_arg->kind = ARG;
    inter_arg->u.arg = arg;
    InterCodes arg_node = malloc(sizeof(struct InterCodes_));
    arg_node->code = inter_arg;
    arg_node->prev = NULL;
    arg_node->next = NULL;
    return arg_node;
}

Operand construct_function(char* func_name) {
    Operand function = malloc(sizeof(struct Operand_));
    function->kind = FUNC;
    function->u.f_name = func_name;
    return function;
}

Operand construct_const(int value) {
    Operand op = malloc(sizeof(struct Operand_));
    op->kind = CONSTANT;
    op->u.value = value;
    return op;
}

Operand construct_var_name(char* name) {
    Operand op = malloc(sizeof(struct Operand_));
    op->kind = VARIABLE;
    op->u.var_name = name;
    return op;
}

Operand construct_label(char* label) {
    Operand op = malloc(sizeof(struct Operand_));
    op->kind = LABEl;
    op->u.var_name = label;
    return op;
}
