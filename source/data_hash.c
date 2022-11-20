# include <stdio.h>
# include <stdbool.h>
# include "data_hash.h"
# include "ast.h" 

static Sym_node map[20000];

static Chain_node f_dec;

static struct {
    int top;
    Sym_node stack[10000];
} STACK;



bool equiv_plist(ParamList plist_1, ParamList plist_2) {
    ParamList point_1 = plist_1;
    ParamList point_2 = plist_2;
    for (; (point_1 != NULL || point_2 != NULL);)
    {
        if (point_1 == NULL && point_2 != NULL) {
            return false;
        }
        if (point_1 != NULL && point_2 == NULL) {
            return false;
        }
        if (!equiv_type(point_1->type, point_2->type)) {
            return false;
        }
        point_1 = point_1->tail;
        point_2 = point_2->tail;
    }
    return true;
}

bool equiv_field(FieldList a, FieldList b) {
    if(a == b) return true;
    FieldList point_1 = a;
    FieldList point_2 = b;
    for (; (point_1 != NULL || point_2 != NULL);)
    {
        if (point_1 == NULL && point_2 != NULL) {
            return false;
        }
        if (point_1 != NULL && point_2 == NULL) {
            return false;
        }
        if (!equiv_type(point_1->type, point_2->type)) {
            return false;
        }
        point_1 = point_1->basic_tail;
        point_2 = point_2->basic_tail;
    }
    return true;
}

bool equiv_arr(Type a, Type b) {
    if (a == b) return true;
    Type point_1 = a;
    Type point_2 = b;
    for (; (point_1 != NULL || point_2 != NULL);)
    {
        if ((point_1->kind == point_2->kind) && (point_1->kind != T_AR)) {
            return true;
        }
        if (point_1 == NULL && point_2 != NULL) {
            return false;
        }
        if (point_1 != NULL && point_2 == NULL) {
            return false;
        }
        if (point_1->u.array.elem->kind != point_2->u.array.elem->kind) {
            return false;
        }
        point_1 = point_1->u.array.elem;
        point_2 = point_2->u.array.elem;
    }
    return true;
}

void scratch(Type type) {
    if (type->kind != T_STRU) return;
    FieldList pointer_backup = type->u.structure;
    FieldList pointer_out = type->u.structure->tail;
    for (; pointer_out != NULL; pointer_out = pointer_out->tail)
    {
        FieldList pointer_in = pointer_backup;
        for (; pointer_in != NULL; pointer_in = pointer_in->basic_tail)
        {
            if (pointer_in->basic_tail != NULL) continue;
            pointer_in->basic_tail = pointer_out;
            break;
        }
        pointer_backup = pointer_backup->tail;
    }
}

bool equiv_type(Type a, Type b) {
    //if (a == NULL || b == NULL) return false;
    if (a == b) return true;
    if (a->kind != b->kind) return false;
    if (a->kind == b->kind) {
        if ((a->kind != T_STRU && a->kind != T_AR) && a->kind != T_FUNC) {
            return true;
        }
        if (a->kind == T_STRU) {
            return equiv_field(a->u.structure, b->u.structure);
        }
        if (a->kind == T_AR) {
            return equiv_arr(a, b);
        }
        if (a->kind == T_FUNC) {
            Type ret_a = a->u.function->ret;
            Type ret_b = b->u.function->ret;
            if (!equiv_type(ret_a, ret_b)) return false;
            int param_a = a->u.function->param_num;
            int param_b = b->u.function->param_num;
            if (param_a != param_b) return false;
            ParamList plist_a = a->u.function->param_list;
            ParamList plist_b = b->u.function->param_list;
            return equiv_plist(plist_a, plist_b);
        }
    } 
    return false;
}

unsigned int hash_pjw(char* name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

int insert(Sym_node s_node) {
    // int index = 0;
    // char* name = type->name;
    // index = hash_pjw(name);
    // if (map[index] == NULL){
    //     Sym_node s_node = (Sym_node)malloc(sizeof(Sym_node));
    //     s_node
    //     map[index] = type;
    //     return index;
    // }
    // bool is_unique = true;
    // Type pointer = map[index];
    // for (; pointer != NULL; pointer = pointer->sym_next)
    // {
    //     if (!strcmp(pointer->name, name)) {
    //         if (type->kind == T_FUNC && pointer->kind != T_FUNC) {
    //             continue;
    //         }
    //         if (type->kind != T_FUNC && pointer->kind == T_FUNC) {
    //             continue;
    //         }
    //         return -1;
    //     }
    // }
    // type->sym_next = map[index];
    // map[index] = type;
    // return index;
    int index = 0;
    char* name = s_node->name;
    index = hash_pjw(name);
    if (map[index] == NULL) {
        map[index] = s_node;
        return index;
    }
    s_node->sym_next = map[index];
    map[index] = s_node;
    return index;
}

Sym_node find(char* name, bool is_f) {
    // if (is_f) {
    //     unsigned int index = 0;
    //     index = hash_pjw(name);
    //     if (!strcmp(map[index]->name, name) && map[index]->kind == T_FUNC) return map[index];
    //     Type pointer = map[index];
    //     for (; pointer != NULL; pointer = pointer->sym_next)
    //     {
    //         if (!strcmp(map[index]->name, name) && map[index]->kind == T_FUNC) return pointer;
    //     }
    // } else {
    //     unsigned int index = 0;
    //     index = hash_pjw(name);
    //     if (!strcmp(map[index]->name, name) && map[index]->kind != T_FUNC) return map[index];
    //     Type pointer = map[index];
    //     for (; pointer != NULL; pointer = pointer->sym_next)
    //     {
    //         if (!strcmp(map[index]->name, name) && map[index]->kind != T_FUNC) return pointer;
    //     }
    // }
    // return NULL;
    if (is_f) {
        Sym_node s_node = STACK.stack[0];
        for (; s_node != NULL; s_node = s_node->stack_next)
        {
            if (!strcmp(s_node->name, name) && s_node->type->kind == T_FUNC) return s_node;
        }
        return NULL;
    } else {
        int index = 0;
        index = hash_pjw(name);
        if (map[index] == NULL) return NULL;
        if (!strcmp(map[index]->name, name) && map[index]->type->kind != T_FUNC) return map[index];
        Sym_node pointer = map[index];
        for (; pointer != NULL; pointer = pointer->sym_next)
        {
            if (!strcmp(pointer->name, name) && pointer->type->kind != T_FUNC) return pointer;
        }
    }
    return NULL;
}

unsigned int erase(Sym_node s_node) {
    char* name = s_node->name;
    unsigned int index = hash_pjw(name);
    if (s_node->sym_next == NULL) {
        //free(s_node->type);
        // free(s_node);
        map[index] = NULL;
        return index;
    }
    Sym_node pointer = map[index]->sym_next;
    //free(s_node);
    map[index] = pointer;
    return index;
}

bool stack_insert(Type type, char* name) {
    Sym_node pointer = STACK.stack[STACK.top];
    if (type->kind != T_STRU) {
        for (; pointer != NULL; pointer = pointer->stack_next)
        {
            if (!strcmp(pointer->name, name)) {
                if (type->kind == T_FUNC && pointer->type->kind != T_FUNC) {
                    continue;
                }
                if (type->kind != T_FUNC && pointer->type->kind == T_FUNC) {
                    continue;
                }
                return false;
            }
        }
        for (int i = 0; i < STACK.top; i++)
        {
            Sym_node s_node = STACK.stack[i];
            for (; s_node != NULL; s_node = s_node->stack_next)
            {
                if (s_node->type->kind != T_STRU) continue;
                if (!strcmp(s_node->name, name)) {
                    return false;
                }
            } 
        }
    } else {
        for (int i = 0; i <= STACK.top; i++)
        {
            Sym_node s_node = STACK.stack[i];
            for (; s_node != NULL; s_node = s_node->stack_next)
            {
                if (s_node->type->kind != T_STRU) continue;
                if (!strcmp(s_node->name, name)) {
                    return false;
                }
            } 
        }
    }
    Sym_node s_node = (Sym_node)malloc(sizeof(struct sym_node_));
    s_node->type = type;
    s_node->name = name;
    s_node->stack_next = STACK.stack[STACK.top];
    STACK.stack[STACK.top] = s_node;
    insert(s_node);
    return true;
}

void stack_push() {
    STACK.top++;
}

void avail_only_for_func_stack_down() {
    STACK.top--;
}
void recover_immediatly() {
    STACK.top++;
}

void stack_pop() {
    Sym_node pointer = STACK.stack[STACK.top];
    while(pointer != NULL)
    {
        Sym_node tmp = pointer;
        pointer = pointer->stack_next;
        STACK.stack[STACK.top] = pointer;
        erase(tmp);
    }
    STACK.top--;
}

bool stack_init() {
    STACK.top = 0;
}

Type structure_find(Sym_node s_node, char* name) {
    FieldList s_pointer = s_node->type->u.structure;
    for (; s_pointer != NULL; s_pointer = s_pointer->basic_tail)
    {
        if (!strcmp(s_pointer->name, name)) {
            return s_pointer->type;
        }
    }
    return NULL;
}

bool chain_insert(Type type, char* name, int line_num) {
    Chain_node cur = (Chain_node)malloc(sizeof(struct chain_node_));
    cur->line_num = line_num;
    cur->name = name;
    cur->type = type;
    Chain_node pointer = f_dec;
    for (; pointer != NULL; pointer = pointer->next)
    {
        if (!strcmp(pointer->name, name)) {
            if (!equiv_type(pointer->type, type)) {
                printf("Error type 19 at Line %d: hh\n", line_num);
                return false;
            }
        }
    }
    Sym_node f_def = find(name, true);
    if (f_def == NULL) {
        cur->is_def = false;
    } else {
        Type f_deffed = f_def->type;
        if (!equiv_type(f_deffed, type)) {
            printf("Error type 19 at Line %d: hh\n", line_num);
            return false;
        } else {
            cur->is_def = true;
        }
    }

    cur->next = f_dec;
    f_dec = cur;

    return true;
}

void give_def(Type type, char* name) {
    Chain_node pointer = f_dec;
    for (; pointer != NULL; pointer = pointer->next)
    {
        if (!strcmp(pointer->name, name)) {
            if (!equiv_type(pointer->type, type)) {
                printf("Error type 19 at Line %d: hh\n", pointer->line_num);
            } else {
                pointer->is_def = true;
            }
        }
    }
}

void final_check() {
    Chain_node pointer = f_dec;
    for (; pointer != NULL; pointer = pointer->next)
    {
        if (!pointer->is_def) {
            printf("Error type 18 at Line %d: hh\n", pointer->line_num);
        }
    }
}

