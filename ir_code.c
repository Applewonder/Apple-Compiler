# include "ir_code.h"
#include "ir_code.h"
#include <stdlib.h>

InterCodes start;
InterCodes end;

void insert_inter_code(InterCodes inter_code) {
    inter_code->prev = end;
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
