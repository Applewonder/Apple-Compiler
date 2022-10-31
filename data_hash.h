#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"

typedef struct sym_node_* Sym_node;
typedef struct chain_node_* Chain_node;
struct sym_node_{
    Type type;
    char* name;
    Sym_node sym_next;
    Sym_node stack_next;
};

struct chain_node_{
    Type type;
    char* name;
    int line_num;
    Chain_node next;
    bool is_def;
};

unsigned int hash_pjw(char* name);
int insert(Sym_node s_node);
Sym_node find(char* name, bool is_f);
unsigned int erase(Sym_node s_node);
bool stack_insert(Type type, char* name);
void stack_push();
void stack_pop();
bool stack_init();
Type structure_find(Sym_node s_node, char* name);
bool equiv_field(FieldList a, FieldList b);
void scratch(Type type);
bool equiv_type(Type a, Type b);
bool equiv_plist(ParamList plist_1, ParamList plist_2);
bool chain_insert(Type type, char* name, int line_num);
void give_def(Type type, char* name);
void final_check();
void avail_only_for_func_stack_down();
void recover_immediatly();
#endif