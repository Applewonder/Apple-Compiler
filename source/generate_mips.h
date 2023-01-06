# ifndef GMIPS_H
# define GMIPS_H
#include "ast.h"
#include "ir_code.h"
#include "generate_ir_sym.h"
#include "generate_ir.h"

typedef enum {
    ZERO = 0,
    AT,
    V0,
    V1,
    A0,
    A1,
    A2,
    A3,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    T8,
    T9,
    K0,
    K1,
    GP,
    SP,
    FP,
    RA
} RegNum;

typedef struct Op_* Op;
typedef struct Assign_Space_Op_* Assign_Space_Op;

struct Op_ {
    int off_set;
    int param_num;
    Operand origin_op;
};

struct Assign_Space_Op_ {
    bool is_param;
    Op operand;
    Assign_Space_Op prev;
    Assign_Space_Op next;
};

void generate_init();
RegNum reg();
void resume_regs();
void load_from_mem(Assign_Space_Op operand, RegNum regis);
void load_from_mem_for_arg(Assign_Space_Op operand, RegNum regis);
void store_to_mem(RegNum regis, Assign_Space_Op operand);
void insert_alloc_operand(Assign_Space_Op operand);
Assign_Space_Op find_allocated(Operand op);
Assign_Space_Op construct_operand_need_alloc(Operand origin_op, int offset, bool is_param);
void generate_read_mips(Assign_Space_Op dest, RegNum dest_reg);
void generate_write_mips(Assign_Space_Op dest, RegNum dest_reg);
void malloc_op();
void generate_intercode_mips(InterCode ir);
void generate_module();

# endif