#include <stdio.h>
#include <string.h>
#include "data_hash.h"
#include "generate_ir_sym.h"

extern struct ast* root;

Type funcret;
bool enter_func = false;
int enter_struc = 0;
bool in_struc;

bool find_func(struct ast* fir, bool is_arg) {
    char* name = fir->name;
    Sym_node s_node = find(name, 1);
    int p_num = s_node->type->u.function->param_num;
    if (!is_arg) {
        if (p_num == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        ParamList f_list = s_node->type->u.function->param_list;
        ParamList p_list = Args(fir->right->right);
        return equiv_plist(f_list, p_list);
    }
}

ParamList Args(struct ast* aim) {
    Type fir = check_exp_type(aim->left);
    ParamList plist = (ParamList)malloc(sizeof(struct Param_));
    plist->type = fir;
    if (aim->left->right != NULL) {
        plist->tail = Args(aim->left->right->right);
        return plist;
    } else {
        plist->tail = NULL;
    }
    return plist;
}

bool check_left(struct ast* aim) {
    struct ast* fir = aim->left;
    if (!strcmp(fir->token_name, "ID")) {
        if(fir->right == NULL) {
            return true;
        }
        return false;
    } else if (!strcmp(fir -> token_name, "OCT") || !strcmp(fir -> token_name, "HEX") || !strcmp(fir -> token_name, "INT")) {
        return false;
    } else if (!strcmp(fir->token_name, "FLOAT")) {
        return false;
    } else if (!strcmp(fir->token_name, "LP")) {
        return check_left(fir->right);
    } else if (!strcmp(fir->token_name, "MINUS")) {
        return false;
    } else if(!strcmp(fir->token_name, "NOT")) {
        return false;
    } else if(!strcmp(fir->right->token_name, "DOT")) {
        return true;
    } else if (!strcmp(fir->right->token_name, "AND") || !strcmp(fir->right->token_name, "OR")){
        return false;
    } else if (!strcmp(fir->right->token_name, "ASSIGNOP")) {
        return false;
    } else if (!strcmp(fir->right->token_name, "LB")) {
        return true;
    } else {
        return false;
    }
}

Type check_exp_type(struct ast* aim) {
    struct ast* fir = aim->left;
    if (!strcmp(fir->token_name, "ID")) {
        if (fir->right == NULL) {
            Sym_node vty = find(fir->name, 0);
            if (vty == NULL) {
                printf("Error type 1 at Line %d: hh\n", fir->line_num);
                return NULL;
            }
            return vty->type;
        } else {
            Sym_node fty = find(fir->name, 1);
            if (fty == NULL) {
                if (find(fir->name, 0) != NULL){
                    printf("Error type 11 at Line %d: hh\n", fir->line_num);
                    return NULL;
                } else {
                    printf("Error type 2 at Line %d: hh\n", fir->line_num);
                    return NULL;
                } 
            }
            bool is_arg = true;
            if (!strcmp(fir->right->right->token_name, "RP")) is_arg = false;
            if (!find_func(fir, is_arg)) {
                printf("Error type 9 at Line %d: hh\n", fir->line_num);
                return NULL;
            }
            return fty->type->u.function->ret;
        }
    } else if (!strcmp(fir -> token_name, "OCT") || !strcmp(fir -> token_name, "HEX") || !strcmp(fir -> token_name, "INT")) {
        Type i_ty = (Type)malloc(sizeof(struct Type_));
        i_ty->kind = T_INT;
        return i_ty;
    } else if (!strcmp(fir->token_name, "FLOAT")) {
        Type f_ty = (Type)malloc(sizeof(struct Type_));
        f_ty->kind = T_FLOAT;
        return f_ty;
    } else if (!strcmp(fir->token_name, "LP")) {
        return check_exp_type(fir->right);
    } else if (!strcmp(fir->token_name, "MINUS")) {
        Type tp = check_exp_type(fir->right);
        if (tp->kind != T_FLOAT && tp->kind != T_INT) {
            printf("Error type 7 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        return check_exp_type(fir->right);
    } else if(!strcmp(fir->token_name, "NOT")) {
        Type tp = check_exp_type(fir->right);
        if (tp->kind != T_INT) {
            printf("Error type 7 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        return tp;
    } else if(!strcmp(fir->right->token_name, "DOT")) {
        Type tp = check_exp_type(fir);
        if (tp == NULL) {
            printf("Error type 1 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        if (tp->kind != T_STRU) {
            printf("Error type 13 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        Sym_node snode = (Sym_node)malloc(sizeof(struct sym_node_));
        snode->type = tp;
        Type t_mem = structure_find(snode, fir->right->right->name);
        if (t_mem == NULL) {
            printf("Error type 14 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        return t_mem;
    } else if (!strcmp(fir->right->token_name, "AND") || !strcmp(fir->right->token_name, "OR")){
        Type l_ty = check_exp_type(fir);
        Type r_ty = check_exp_type(fir->right->right);
        if (l_ty == NULL) return NULL;
        if (r_ty == NULL) return NULL;
        if (l_ty->kind != T_INT || r_ty->kind != T_INT) {
            printf("Error type 7 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        Type t_e = (Type)malloc(sizeof(struct Type_));
        t_e->kind = T_INT;
        return t_e;
    } else if (!strcmp(fir->right->token_name, "ASSIGNOP")) {
        Type l_ty = check_exp_type(fir);
        Type r_ty = check_exp_type(fir->right->right);
        if (l_ty == NULL) return NULL;
        if (r_ty == NULL) return NULL;
        if (!equiv_type(l_ty, r_ty)) {
            printf("Error type 5 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        if (!check_left(fir)) {
            printf("Error type 6 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        return l_ty;
    } else if (!strcmp(fir->right->token_name, "LB")) {
        Type stu_type = check_exp_type(fir);
        if (stu_type == NULL) return NULL;
        if (stu_type->kind != T_AR) {
            printf("Error type 10 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        Type i_type = check_exp_type(fir->right->right);
        if(i_type == NULL) return NULL;
        if (i_type->kind != T_INT) {
            printf("Error type 12 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        //printf("%p\n", stu_type->u.array.elem);
        return stu_type->u.array.elem;
    } else {
        Type l_ty = check_exp_type(fir);
        Type r_ty = check_exp_type(fir->right->right);
        if (l_ty == NULL) return NULL;
        if (r_ty == NULL) return NULL;
        
        if (!equiv_type(l_ty, r_ty)) {
            printf("Error type 7 at Line %d: hh\n", fir->line_num);
            return NULL;
        } else if (l_ty->kind != T_INT && l_ty->kind != T_FLOAT) {
            printf("Error type 7 at Line %d: hh\n", fir->line_num);
            return NULL;
        }
        if (!strcmp(fir->right->token_name, "RELOP")) {
            Type t_e = (Type)malloc(sizeof(struct Type_));
            t_e->kind = T_INT;
            return t_e;
        }
        return l_ty;
    }
}

int calculate_sym_array_size(Type type) {
    if (type->kind == T_AR) {
        int size = calculate_sym_array_size(type->u.array.elem);
        size *= type->u.array.size;
        return size;
    } else {
        return calculate_sym_type_size(type);
    }
}

int calculate_sym_stru_size(FieldList field, int size) {
    if (field == NULL) return size;
    int cur_size = calculate_sym_type_size(field->type);
    cur_size += size;
    int tail_size = calculate_sym_stru_size(field->basic_tail, cur_size);
    return tail_size;
}

int calculate_sym_type_size(Type type) {
    switch (type->kind)
    {
    case T_INT:
    case T_FLOAT:
        return 4;
    case T_AR:
        return calculate_sym_array_size(type);
    case T_STRU:
        return calculate_sym_stru_size(type->u.structure, 0);
    default:
        printf("real fun\n");
        return -1;
        break;
    }
}

FieldList construct_array_node(struct ast* aim, Type type) {//the entry is: vard lb int rb
    if (!strcmp(aim->token_name, "ID")) {
        bool is_success = stack_insert(type, aim->name);
        if (!is_success) {
            printf("Error type 3 at Line %d: hh\n", aim->line_num);
        } 
        FieldList b = (FieldList)malloc(sizeof(struct FieldList_));
        b->type = type;
        b->name = aim->name;
        b->tail = NULL;
        b->basic_tail = NULL;
        return b;
    }
    Type vard = (Type)malloc(sizeof(struct Type_));
    vard->kind = T_AR;
    vard->u.array.elem = type;
    vard->u.array.size = aim->right->right->ival;
    FieldList arr = construct_array_node(aim->left, vard);
    return arr;
}

FieldList VarDec(struct ast* aim, Type type) {
    struct ast* tmp = aim->left;
    if(!strcmp(tmp->token_name, "ID")) {
        bool is_success = stack_insert(type, tmp->name);
        
        if (!is_success) {
            if (!enter_struc) {
                printf("Error type 3 at Line %d: hh\n", tmp->line_num);
            } else {
                printf("Error type 15 at Line %d: hh\n", tmp->line_num);
            }
            return NULL;
        } 
        FieldList vlist = (FieldList)malloc(sizeof(struct FieldList_));
        vlist->name = tmp->name;
        vlist->type = type;
        return vlist;
    }
    FieldList final = (FieldList)malloc(sizeof(struct FieldList_));
    final = construct_array_node(tmp, type);
    return final;
}

Type TYPE(struct ast* aim) {
    Type type = (Type)malloc(sizeof(struct Type_));
    if (!strcmp(aim->name, "int")) {
        type->kind = T_INT;
    } else {
        type->kind = T_FLOAT;
    }
    return type;
}

Type STRUCT() {
    Type type = (Type)malloc(sizeof(struct Type_));
    type->kind = T_STRU;
    return type;
}

char* OptTag(struct ast* aim, Type type) {
    if (aim->line_num == -1) return NULL;
    struct ast* id = aim->left;
    char* name = id->name;
    return name;
}

char* Tag(struct ast* aim, Type type) {
    char* name = aim->left->name;
    return name;
}

void give_offset(FieldList fields, int offset) {
    FieldList fl = fields;
    while (fl != NULL) {
        Type type = fl->type;
        if (type->kind == T_STRU) {
            give_offset(type->u.structure, offset);
        }
        int size = calculate_sym_type_size(type);
        insert_offset(offset, fl->name);
        offset += size;
        fl = fl->basic_tail;
    }
}

Type StructSpecifier(struct ast* aim) {
    Type type = STRUCT();
    struct ast* tag = aim->left->right;
    char* name = NULL;
    if (!strcmp(tag->token_name, "OptTag")) {
        name = OptTag(tag, type);
    } else {
        name = Tag(tag, type);
        //printf("--------------%s\n", name);
        Sym_node snode = find(name, 0);
        if (snode == NULL) {
            printf("Error type 17 at Line %d: hh\n", aim->line_num);
            return NULL;
        }
        type = snode->type;
        return type;
    }
    if (name != NULL) {
        //stack_push();
        enter_struc ++;
        struct ast* defl = tag->right->right;
        type->u.structure = DefList(defl);
        scratch(type);
        //stack_pop();
        enter_struc --;
        bool is_success = stack_insert(type, name);
        give_offset(type->u.structure, 0);
        if (!is_success) {
            printf("Error type 16 at Line %d: hh\n", aim->line_num);
        } 
        return type;
    } else {
        //stack_push();
        enter_struc ++;
        struct ast* defl = tag->right->right;
        type->u.structure = DefList(defl);
        scratch(type);
        //stack_pop();
        enter_struc --;
        give_offset(type->u.structure, 0);
        return type;
    }
    
}

FieldList DefList(struct ast* aim) {
    if (aim->line_num == -1) return NULL;
    struct ast* child = aim->left;
    FieldList def = Def(child);
    if (def == NULL) {
        return DefList(child->right);
    }
    def->tail = DefList(child->right);
    return def;
}

Type Specifier(struct ast* aim) {
    Type type = (Type)malloc(sizeof(struct Type_));
    struct ast* possi = aim->left;
    if (!strcmp(possi->token_name, "TYPE")) {
        type = TYPE(possi);
        return type;
    } else {
        type = StructSpecifier(aim->left);
        return type;
    }
} 

FieldList Dec(struct ast* aim, Type type) {
    struct ast* vdec = aim->left;
    FieldList tmp = VarDec(vdec, type);
    if (tmp == NULL) return NULL;
    if (vdec->right == NULL) {
        return tmp;
    }
    if (enter_struc) {
        printf("Error type 15 at Line %d: hh\n", vdec->line_num);
        return NULL;
    }
    struct ast* exp = vdec->right->right;
    //TODO:check assign type exp;
    Type ty = check_exp_type(exp);
    if (ty == NULL) return NULL;
    //if(ty->kind != tmp->kind) //TODO: you know
    if (!equiv_type(ty, tmp->type)) {
        printf("Error type 8 at Line %d: hh\n", vdec->line_num);
    }
    return tmp;
}

FieldList DecList(struct ast* aim, Type type) {
    struct ast* dec = aim->left;
    FieldList cur = Dec(dec, type);
    if (cur == NULL) return NULL;
    if (dec->right != NULL) {
        FieldList decl = DecList(dec->right->right, type);
        cur->basic_tail = decl;
    } else {
        cur->basic_tail = NULL;
    }
    return cur;
}

FieldList Def(struct ast* aim) {
    Type type = Specifier(aim->left);
    int offset = calculate_sym_type_size(type);
    if (type == NULL) return NULL;
    FieldList cur = DecList(aim->left->right, type);
    return cur;
}

FieldList ExtDecList(struct ast* aim, Type type) {
    struct ast* vdec = aim->left;
    FieldList cur = VarDec(vdec, type);
    if (vdec->right != NULL) {
        FieldList decl = ExtDecList(vdec->right->right, type);
        cur->basic_tail = decl;
    } else {
        cur->basic_tail = NULL;
    }
    return cur;
}

Type FunDec(struct ast* aim, Type type, bool is_def) {
    if (is_def) {
        funcret = type;
        Type f_type = (Type)malloc(sizeof(struct Type_));
        
        f_type->kind = T_FUNC;
        f_type->u.function = (Func)malloc(sizeof(struct Func_));
        f_type->u.function->param_num = 0;
        f_type->u.function->ret = type;
        struct ast* id = aim->left;
        
        if (!strcmp(id->right->right->token_name, "RP")) {
            bool is_success = stack_insert(f_type, id->name);
            if (!is_success) {
                printf("Error type 4 at Line %d: hh\n", id->line_num);
            } 
            give_def(f_type, id->name);
            return f_type;
        }
        struct ast* vlist = id->right->right;
        //stack_push();
        f_type->u.function->param_list = VarList(vlist);
        enter_func = true;
        
        ParamList plist = f_type->u.function->param_list;
        
        for (; plist != NULL; plist = plist->tail)
        {
            f_type->u.function->param_num ++;
        }
        //avail_only_for_func_stack_down();
        bool is_success = stack_insert(f_type, id->name);
        //recover_immediatly();
        if (!is_success) {
            printf("Error type 4 at Line %d: hh\n", id->line_num);
        } 
        give_def(f_type, id->name);
        return f_type;
    } else {
        Type f_type = (Type)malloc(sizeof(struct Type_));
        
        f_type->kind = T_FUNC;
        f_type->u.function = (Func)malloc(sizeof(struct Func_));
        f_type->u.function->param_num = 0;
        f_type->u.function->ret = type;
        struct ast* id = aim->left;

        if (!strcmp(id->right->right->token_name, "RP")) {
            bool is_success = chain_insert(f_type, id->name, id->line_num);
            return f_type;
        }

        struct ast* vlist = id->right->right;
        //stack_push();
        f_type->u.function->param_list = VarList(vlist);
        //stack_pop();

        ParamList plist = f_type->u.function->param_list;

        for (; plist != NULL; plist = plist->tail)
        {
            f_type->u.function->param_num ++;
        }
        bool is_success = chain_insert(f_type, id->name, id->line_num);
        return f_type;
    }
    
}

ParamList VarList(struct ast* aim) {
    struct ast* paramdec = aim->left;
    ParamList paraml = ParamDec(paramdec);
    if (paramdec->right != NULL) {
        struct ast* vlist = paramdec->right->right;
        ParamList plist = VarList(vlist);
        paraml->tail = plist;
        return paraml;
    } else {
        paraml->tail = NULL;
    }
    return paraml;
}

ParamList ParamDec(struct ast* aim) {
    struct ast* spec = aim->left;
    Type type = Specifier(spec);
    struct ast* vdec = spec->right;
    FieldList vtype = VarDec(vdec, type);
    ParamList plist = (ParamList)malloc(sizeof(struct Param_));
    plist->name = vtype->name;
    plist->type = vtype->type;
    plist->tail = NULL;
    return plist;
}

void Stmt(struct ast* aim) {
    struct ast* fir = aim->left;
    if (!strcmp(fir->token_name, "Exp")) {
        check_exp_type(fir);
    } else if (!strcmp(fir->token_name, "CompSt")) {
        CompSt(fir);
    } else if (!strcmp(fir->token_name, "RETURN")) {
        Type t_ret = check_exp_type(fir->right);
        if (t_ret == NULL) return;
        if (!equiv_type(t_ret, funcret)) {
            printf("Error type 8 at Line %d: hh\n", aim->line_num);
        }
    } else if (!strcmp(fir->token_name, "IF")){
        Type t_judge = check_exp_type(fir->right->right);
        struct ast* stm = fir->right->right->right->right;
        Stmt(stm);
        if (stm->right != NULL) {
            struct ast* e_stm = stm->right->right;
            Stmt(e_stm);
        }
    } else {
        Type t_judge = check_exp_type(fir->right->right);
        struct ast* stm = fir->right->right->right->right;
        Stmt(stm);
    }
}

void StmtList(struct ast* aim) {
    if (aim->line_num == -1) return;
    struct ast* stm = aim->left;
    Stmt(stm);
    StmtList(stm->right);
}

void CompSt(struct ast* aim) {
    //if (!enter_func) stack_push();
    if (enter_func) enter_func = false;
    struct ast* dlist = aim->left->right;
    DefList(dlist);
    struct ast* slist = dlist->right;
    StmtList(slist);
    //stack_pop();
    
}

void generate_ir_sym(struct ast* root_, int depth) {
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
            generate_ir_sym(itr, depth + 1);
            itr = itr -> right;
        }
    }
}