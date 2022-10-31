#ifndef SYM_H
#define SYM_H

#include "ast.h"

typedef struct Type_* Type;
typedef struct Func_* Func;
typedef struct FieldList_* FieldList;
typedef struct Param_* ParamList;

enum TYPE{
    T_INT,
    T_FLOAT,
    T_FUNC,
    T_AR,
    T_STRU
};

struct Type_
{
    enum TYPE kind;
    union
    {
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { 
            Type elem; 
            int size; 
        } array;
        // 结构体类型信息是一个链表
        FieldList structure;
        Func function;
    } u;
};

struct FieldList_
{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList basic_tail;// 连续定义
    FieldList tail; // 下一个域
};

struct Func_ 
{
    Type ret;
    int param_num;
    ParamList param_list;
};

struct Param_
{
    char* name;
    Type type;
    ParamList tail;
};

Type check_exp_type(struct ast* aim);
FieldList construct_array_node(struct ast* aim, Type type);
FieldList VarDec(struct ast* aim, Type type);
Type TYPE(struct ast* aim);
Type STRUCT();
char* OptTag(struct ast* aim, Type type);
char* Tag(struct ast* aim, Type type);
Type StructSpecifier(struct ast* aim);
FieldList DefList(struct ast* aim);
Type Specifier(struct ast* aim);
FieldList Dec(struct ast* aim, Type type);
FieldList DecList(struct ast* aim, Type type);
FieldList Def(struct ast* aim);
FieldList ExtDecList(struct ast* aim, Type type);
Type FunDec(struct ast* aim, Type type);
ParamList VarList(struct ast* aim);
ParamList ParamDec(struct ast* aim);
void construct_sym_table(struct ast* root_, int depth);
void Stmt(struct ast* aim);
void StmtList(struct ast* aim);
void CompSt(struct ast* aim);
ParamList Args(struct ast* aim);

#endif