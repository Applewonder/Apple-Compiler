# include <stdio.h>
# include <stdbool.h>
# include "data_hash.h"
# include "ast.h" 

static Sym_node map[20000];

static struct {
    int top;
    Sym_node stack[10000];
} STACK;


bool equiv_field(FieldList a, FieldList b) {
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
        if (point_1->type->kind != point_2->type->kind) {
            return false;
        }
        point_1 = point_1->basic_tail;
        point_2 = point_2->basic_tail;
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
    if ((a->kind == b->kind) && a->kind != T_STRU) return true; 
    if (a->kind != b->kind) return false;
    return equiv_field(a->u.structure, b->u.structure);
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
    // if (is_global) {
    //     bool is_func = false;
    //     if (type->kind == T_FUNC) {
    //         is_func = true;
    //     } 
    //     if (insert(type) == -1) return false;
    //     Type pointer = find(type->name, is_func);
    //     assert(pointer != NULL);
    //     if (STACK.stack[0] == NULL) {
    //         STACK.stack[0] = pointer;
    //         return true;
    //     }
    //     pointer->stack_next =  STACK.stack[0];
    //     STACK.stack[0] = pointer;
    //     return true;
    // }
    // bool is_func = false;
    // if (type->kind == T_FUNC) {
    //     is_func = true;
    // } 
    // if (insert(type) == -1) return false;
    // Type pointer = find(type->name, is_func);
    // assert(pointer != NULL);
    // int top = STACK.top;
    // if (STACK.stack[top] == NULL) {
    //     STACK.stack[top] = pointer;
    //     return true;
    // }
    // pointer->stack_next =  STACK.stack[top];
    // STACK.stack[top] = pointer;
    // return true;
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


