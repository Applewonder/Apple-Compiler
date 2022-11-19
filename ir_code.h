#ifndef IR_H
#define IR_H

typedef struct Operand_* Operand;
typedef struct InterCodes_ * InterCodes;
typedef struct InterCode_ * InterCode;
typedef struct Arg_ * ArgList;

struct Operand_ {
    enum { VARIABLE, CONSTANT, ADDRESS, LABEl, OP, FUNC, ARGUE} kind;
    union {
        int value;
        char* var_name;
        char* op;
        char* f_name;
        char* arg_name;
    } u;
};

struct InterCode_
{
    enum { ASSIGN, ADD, SUB, MUL, DIV, TAG, GOTO, IF, READ, WRITE, CALL, ARG, FUNCTION, PARAM, DEC} kind;
    union {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
        struct { Operand candidate1, op, candidate2, label; } ifnop;
        struct { Operand result, op1; } snop;
        struct { Operand res, function; } callf;
        struct { Operand var, size; } dec_ar;
        Operand label;
        Operand variable;
        Operand arg;
        Operand func;
        Operand param;
    } u;
};

struct InterCodes_ { 
    InterCode code; 
    InterCodes prev, next; 
};

struct Arg_ {
    char* name;
    ArgList tail;
};

Operand construct_const(int value);
Operand construct_label(char* label);
Operand construct_op(char* op);
Operand construct_function(char* func_name);
Operand construct_arg(char* arg);
InterCodes construct_inter_code_dec(Operand var_name, Operand size);
InterCodes construct_inter_code_paramlist(Operand param);
InterCodes construct_inter_code_func(Operand func);
InterCodes construct_inter_code_arg(Operand arg);
InterCodes construct_inter_code_write_function(Operand place_var);
InterCodes construct_inter_code_call_function(Operand place_var, Operand function);
InterCodes construct_inter_code_read_function(Operand place_var);
InterCodes construct_inter_code_if(Operand candidate1, Operand op, Operand candidate2, Operand label);
InterCodes construct_inter_code_binop(int kind, Operand result, Operand op1, Operand op2);
InterCodes construct_inter_code_assign(Operand left, Operand right);
InterCodes construct_inter_code_label(Operand label);
InterCodes construct_inter_code_goto(Operand label);
void insert_inter_code(InterCodes inter_code);
Operand construct_var_name(char* name);

#endif