#ifndef GIR_H
#define GIR_H
#include "ast.h"
#include "ir_code.h"
#include "generate_ir_sym.h"

char* print_operand(Operand ope, bool is_dec);
void printf_binop(int kind, char* result, char* op1, char* op2);
void print_ir(InterCodes cur);
char* new_temp(int count);
char* new_label(int count);
char* find_array_name(struct ast* vardec);
void translate_array(struct ast* vardec);
char* translate_VarDec(struct ast* vardec);
void translate_Dec(struct ast* dec);
void translate_DecList(struct ast* declist);
void translate_Def(struct ast* def);
void translate_DefList(struct ast* deflist);
void translate_FunDec(struct ast* fundec);
void translate_StmtList(struct ast* stmtlist);
void translate_CompSt(struct ast* compst);
void translate_ExtDef(struct ast* extdef);
ArgList translate_Args(struct ast* args, ArgList arg_list);
void translate_Cond(struct ast* exp, char* label_true, char* label_false);
void translate_Stmt(struct ast* stmt);
void translate_Exp(struct ast* exp, char* place);
void generate_ir(struct ast* root_, int depth);
char* compute_array_length(struct ast* exp);
int calculate_array_size(Type type);
int calculate_stru_size(FieldList field, int size);
int calculate_type_size(Type type);
Type calculate_array_elem_size(struct ast* exp);

#endif