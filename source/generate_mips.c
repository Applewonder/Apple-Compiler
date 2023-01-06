# include "generate_mips.h"
# include <stdlib.h>

extern InterCodes start;
extern FILE *out_file;

Assign_Space_Op begin_alloc;
Assign_Space_Op end_alloc;
bool begin_arg = 0;
int t1_ocupy = 0;
int t2_occupy = 0;
int t3_occupy = 0;
int f_offset = 0;
int arguement_num = 0;

char* regnam[] = {
    "$zero",
    "$at",
    "$v0",
    "$v1",
    "$a0",
    "$a1",
    "$a2",
    "$a3",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$s0",
    "$s1",
    "$s2",
    "$s3",
    "$s4",
    "$s5",
    "$s6",
    "$s7",
    "$t8",
    "$t9",
    "$k0",
    "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};


void generate_init() {
    fprintf(out_file, ".data\n");
    fprintf(out_file, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(out_file, "_ret: .asciiz \"\\n\"\n");
    fprintf(out_file, ".globl main\n");
    fprintf(out_file, ".text\n");
    fprintf(out_file, "read:\n");
    fprintf(out_file, "\tli $v0, 4\n");
    fprintf(out_file, "\tla $a0, _prompt\n");
    fprintf(out_file, "\tsyscall\n");
    fprintf(out_file, "\tli $v0, 5\n");
    fprintf(out_file, "\tsyscall\n");
    fprintf(out_file, "\tjr $ra\n");
    fprintf(out_file, "\n");
    fprintf(out_file, "write:\n");
    fprintf(out_file, "\tli $v0, 1\n");
    fprintf(out_file, "\tsyscall\n");
    fprintf(out_file, "\tli $v0, 4\n");
    fprintf(out_file, "\tla $a0, _ret\n");
    fprintf(out_file, "\tsyscall\n");
    fprintf(out_file, "\tmove $v0, $0\n");
    fprintf(out_file, "\tjr $ra\n");
    fprintf(out_file, "\n");
}

RegNum reg() {
    if (!t1_ocupy) {
        t1_ocupy = 1;
        return T1;
    }
    if (!t2_occupy) {
        t2_occupy = 1;
        return T2;
    }
    if (!t3_occupy) {
        t3_occupy = 1;
        return T3;
    }
    return T4;
}

void resume_regs() {
    t1_ocupy = 0;
    t2_occupy = 0;
    t3_occupy = 0;
}

void load_from_mem(Assign_Space_Op operand, RegNum regis) {
    int offset = operand->operand->off_set;
    int param_offset = 0;
    if (operand->is_param) {
        param_offset = f_offset + 4;
    } 
    offset += param_offset;
    fprintf(out_file, "\tlw %s, %d($sp)\n", regnam[regis], offset);
}

void store_to_mem(RegNum regis, Assign_Space_Op operand) {
    int offset = operand->operand->off_set;
    int param_offset = 0;
    if (operand->is_param) {
        param_offset = f_offset + 4;
    }
    offset += param_offset;
    fprintf(out_file, "\tsw %s, %d($sp)\n", regnam[regis], offset);
}

void insert_alloc_operand(Assign_Space_Op operand) {
    if (begin_alloc == NULL) {
        begin_alloc = operand;
        end_alloc = operand;
        return;
    }
    operand->prev = end_alloc;
    end_alloc->next = operand;
    end_alloc = operand;
}

Assign_Space_Op find_allocated(Operand op) {
    char* name = op->u.var_name;
    Assign_Space_Op ir_start = begin_alloc;
    while (ir_start != NULL) {
        if (!strcmp(name, ir_start->operand->origin_op->u.var_name)) {
            return ir_start;
        }
        ir_start = ir_start->next;
    }
    return NULL;
}

Assign_Space_Op construct_operand_need_alloc(Operand origin_op, int offset, bool is_param) {
    Assign_Space_Op alloc_operand = malloc(sizeof(struct Assign_Space_Op_));
    Op op = malloc(sizeof(struct Op_));
    op->origin_op = origin_op;
    alloc_operand->operand = op;
    alloc_operand->is_param = is_param;
    alloc_operand->next = NULL;
    alloc_operand->prev = NULL;
    alloc_operand->operand->off_set = offset;
    return alloc_operand;
}

void generate_read_mips(Assign_Space_Op dest, RegNum dest_reg) {
    int offset = dest->operand->off_set;
    int param_offset = 0;
    if (!dest->is_param) {
        param_offset = f_offset;
    } 
    offset += param_offset;
    fprintf(out_file, "\taddi $sp, $sp, -4\n");
    fprintf(out_file, "\tsw $ra, 0($sp)\n");
    fprintf(out_file, "\tjal read\n");
    fprintf(out_file, "\tlw $ra, 0($sp)\n");
    fprintf(out_file, "\taddi $sp, $sp, 4\n");
    fprintf(out_file, "\tmove %s, $v0\n", regnam[dest_reg]);
    fprintf(out_file, "\tsw %s, %d($sp)\n", regnam[dest_reg], offset);
}

void generate_write_mips(Assign_Space_Op dest, RegNum dest_reg) {
    fprintf(out_file, "\taddi $sp, $sp, -4\n");
    fprintf(out_file, "\tsw $ra, 0($sp)\n");
    fprintf(out_file, "\tjal write\n");
    fprintf(out_file, "\tlw $ra, 0($sp)\n");
    fprintf(out_file, "\taddi $sp, $sp, 4\n");
}

void malloc_op() {
    InterCodes ir = start;
    int offset = 0;
    Operand func = NULL;
    while (ir != NULL) {
        switch (ir->code->kind)
        {
        case ASSIGN: 
        {
            // Operand right = ir->code->u.assign.right;
            Operand left = ir->code->u.assign.left;
            // if (right->kind != CONSTANT) {
            //     if (find_allocated(right) == NULL) {
            //         Assign_Space_Op need_alloc = construct_operand_need_alloc(right, offset);
            //         insert_alloc_operand(need_alloc);
            //         offset += 4;
            //     }
            // }
            if (find_allocated(left) == NULL) {
                Assign_Space_Op need_alloc = construct_operand_need_alloc(left, offset, false);
                insert_alloc_operand(need_alloc);
                offset += 4;
            }
            break;
        }
        case ADD: 
        case SUB:
        case MUL:
        case DIV:
        {
            // Operand op1 = ir->code->u.binop.op1;
            // Operand op2 = ir->code->u.binop.op2;
            Operand res = ir->code->u.binop.result;
            // if (op1->kind != CONSTANT) {
            //     if (find_allocated(op1) == NULL) {
            //         Assign_Space_Op need_alloc = construct_operand_need_alloc(op1, offset);
            //         insert_alloc_operand(need_alloc);
            //         offset += 4;
            //     }
            // }
            // if (op2->kind != CONSTANT) {
            //     if (find_allocated(op2) == NULL) {
            //         Assign_Space_Op need_alloc = construct_operand_need_alloc(op2, offset);
            //         insert_alloc_operand(need_alloc);
            //         offset += 4;
            //     }
            // }
            if (find_allocated(res) == NULL) {
                Assign_Space_Op need_alloc = construct_operand_need_alloc(res, offset, false);
                insert_alloc_operand(need_alloc);
                offset += 4;
            }
            break;
        }
        case READ:
        {
            Operand var = ir->code->u.variable;
            if (var->kind != CONSTANT) {
                if (find_allocated(var) == NULL) {
                    Assign_Space_Op need_alloc = construct_operand_need_alloc(var, offset, false);
                    insert_alloc_operand(need_alloc);
                    offset += 4;
                }
            }
            break;
        }
        case CALL:
        {
            Operand var = ir->code->u.callf.res;
            if (var->kind != CONSTANT) {
                if (find_allocated(var) == NULL) {
                    Assign_Space_Op need_alloc = construct_operand_need_alloc(var, offset, false);
                    insert_alloc_operand(need_alloc);
                    offset += 4;
                }
            }
            break;
        }
        case FUNCTION:
        {
            f_offset = 0;
            if (func == NULL) {
                func = ir->code->u.func;
            } else {
                Assign_Space_Op need_alloc = construct_operand_need_alloc(func, offset, false);
                insert_alloc_operand(need_alloc);
            }
            offset = 0;
            break;
        }
        case DEC:
        {
            Operand var = ir->code->u.dec_ar.var;
            int size = ir->code->u.dec_ar.size->u.value;
            if (var->kind != CONSTANT) {
                if (find_allocated(var) == NULL) {
                    Assign_Space_Op need_alloc = construct_operand_need_alloc(var, offset, false);
                    insert_alloc_operand(need_alloc);
                    offset += size;
                }
            }
            break;
        }
        case ADDR:
        {
            Operand dest = ir->code->u.addr.dest;
            if (dest->kind != CONSTANT) {
                if (find_allocated(dest) == NULL) {
                    Assign_Space_Op need_alloc = construct_operand_need_alloc(dest, offset, false);
                    insert_alloc_operand(need_alloc);
                    offset += 4;
                }
            }
            break;
        }
        case GETADDR:
        {
            Operand dest = ir->code->u.get_addr.dest;
            if (dest->kind != CONSTANT) {
                if (find_allocated(dest) == NULL) {
                    Assign_Space_Op need_alloc = construct_operand_need_alloc(dest, offset, false);
                    insert_alloc_operand(need_alloc);
                    offset += 4;
                }
            }
            break;
        }
        case MEM:
        case RETURN:
        {
            break;
        }
        case PARAM:
        {
            Operand dest = ir->code->u.param;
            Assign_Space_Op need_alloc = construct_operand_need_alloc(dest, f_offset, true);
            insert_alloc_operand(need_alloc);
            f_offset += 4;
        }
        case ARG:
        case TAG:
        case GOTO:
        case IF:
        case WRITE:
        {
            break;
        }
        default:
            break;
        }
        
        ir = ir->next;
    }
    Assign_Space_Op need_alloc = construct_operand_need_alloc(func, offset, false);
    insert_alloc_operand(need_alloc);
}

void generate_intercode_mips(InterCode ir) {
    int kind = ir->kind;
    switch (kind)
    {
    case ASSIGN: {
        Operand right = ir->u.assign.right;
        Operand left = ir->u.assign.left;
        Assign_Space_Op left_op = find_allocated(left);
        if (right->kind == CONSTANT) {
            int const_num = right->u.value;
            RegNum the_reg = reg();
            fprintf(out_file, "\tli %s, %d\n", regnam[the_reg], const_num);
            store_to_mem(the_reg, left_op);
            resume_regs();
        } else {
            Assign_Space_Op right_op = find_allocated(right);
            RegNum right_reg = reg();
            load_from_mem(right_op, right_reg);
            RegNum left_reg = reg();
            fprintf(out_file, "\tmove %s, %s\n", regnam[left_reg], regnam[right_reg]);
            store_to_mem(left_reg, left_op);
            resume_regs();
        }
        break;
    }
    case ADD: 
    case SUB:
    case MUL:
    case DIV:
    {
        Operand op1 = ir->u.binop.op1;
        Operand op2 = ir->u.binop.op2;
        Operand res = ir->u.binop.result;
        RegNum op1_reg = reg();
        RegNum op2_reg = reg();
        RegNum res_reg = reg();
        Assign_Space_Op res_op = find_allocated(res);
        if (op1->kind == CONSTANT) {
            fprintf(out_file, "\tli %s, %d\n", regnam[op1_reg], op1->u.value);
        } else {
            Assign_Space_Op op1_op = find_allocated(op1);
            load_from_mem(op1_op, op1_reg);
        }
        if (op2->kind == CONSTANT) {
            fprintf(out_file, "\tli %s, %d\n", regnam[op2_reg], op2->u.value);
        } else {
            Assign_Space_Op op2_op = find_allocated(op2);
            load_from_mem(op2_op, op2_reg);
        }
        if (kind == ADD) {
            fprintf(out_file, "\tadd %s, %s, %s\n", regnam[res_reg], regnam[op1_reg], regnam[op2_reg]);
        } else if (kind == SUB) {
            fprintf(out_file, "\tsub %s, %s, %s\n", regnam[res_reg], regnam[op1_reg], regnam[op2_reg]);
        } else if (kind == MUL) {
            fprintf(out_file, "\tmul %s, %s, %s\n", regnam[res_reg], regnam[op1_reg], regnam[op2_reg]);
        } else if (kind == DIV) {
            fprintf(out_file, "\tdiv %s, %s\n", regnam[op1_reg], regnam[op2_reg]);
            fprintf(out_file, "\tmflo %s\n", regnam[res_reg]);
        } 
        store_to_mem(res_reg, res_op);
        resume_regs();
        break;
    }
    case READ:
    {
        Operand var = ir->u.variable;
        Assign_Space_Op var_alloced = find_allocated(var);
        RegNum dist_reg = reg();
        generate_read_mips(var_alloced, dist_reg);
        resume_regs();
        break;
    }
    case CALL:
    {
        Operand call_r = ir->u.callf.res;
        Operand call_f = ir->u.callf.function;
        fprintf(out_file, "\tjal %s", call_f->u.f_name);
        Assign_Space_Op res_op = find_allocated(call_r);
        store_to_mem(V0, res_op);
        fprintf(out_file, "\tlw $ra, 0($sp)\n");
        fprintf(out_file, "\taddi $sp, $sp, 4\n");
        resume_regs();
        break;
    }
    case FUNCTION:
    {
        Operand func_op = ir->u.func;
        Assign_Space_Op func_alloc = find_allocated(func_op);
        RegNum sp_reg = reg();
        int arg_offset = arguement_num * 4;
        fprintf(out_file, "%s:\n", func_op->u.f_name);
        fprintf(out_file, "\taddi %s, $sp, %d\n", regnam[sp_reg], arg_offset);
        fprintf(out_file, "\taddi $sp, $sp, -4\n");
        fprintf(out_file, "\tsw %s, 0($sp)\n", regnam[sp_reg]);
        f_offset = func_alloc->operand->off_set;
        fprintf(out_file, "\taddi $sp, $sp, -%d\n", f_offset);
        resume_regs();
        break;
    }
    case DEC:
    {
        break;
    }
    case ADDR:
    {
        Operand dest = ir->u.addr.dest;
        Operand src = ir->u.addr.src;
        Assign_Space_Op dest_op = find_allocated(dest);
        Assign_Space_Op src_op = find_allocated(src);
        RegNum dest_reg = reg();
        RegNum src_reg = reg();
        load_from_mem(src_op, src_reg);
        fprintf(out_file, "\tlw %s, 0(%s)\n", regnam[dest_reg], regnam[src_reg]);
        store_to_mem(dest_reg, dest_op);
        resume_regs();
        break;
    }
    case GETADDR:
    {
        Operand dest = ir->u.get_addr.dest;
        Operand src = ir->u.get_addr.src;
        Assign_Space_Op dest_op = find_allocated(dest);
        Assign_Space_Op src_op = find_allocated(src);
        RegNum dest_reg = reg();
        RegNum src_reg = reg();
        int offset = src_op->operand->off_set;
        fprintf(out_file, "\taddi %s, %s, %d\n", regnam[src_reg], regnam[SP], offset);
        fprintf(out_file, "\tmove %s, %s\n", regnam[dest_reg], regnam[src_reg]);
        store_to_mem(dest_reg, dest_op);
        resume_regs();
        break;
    }
    case MEM:
    {
        Operand dest = ir->u.mem.dest;
        Operand src = ir->u.mem.src;
        Assign_Space_Op dest_op = find_allocated(dest);
        Assign_Space_Op src_op = find_allocated(src);
        RegNum dest_reg = reg();
        RegNum src_reg = reg();
        load_from_mem(src_op, src_reg);
        load_from_mem(dest_op, dest_reg);
        fprintf(out_file, "\tsw %s, 0(%s)\n", regnam[src_reg], regnam[dest_reg]);
        resume_regs();
        break;
    }
    case RETURN:
    {
        Operand var = ir->u.variable;
        Assign_Space_Op var_op = find_allocated(var);
        load_from_mem(var_op, V0);
        RegNum sp_reg = reg();
        fprintf(out_file, "\tlw %s, %d($sp)\n", regnam[sp_reg], f_offset);
        fprintf(out_file, "\tmove $sp, %s\n", regnam[sp_reg]);
        f_offset = 0;
        arguement_num = 0;
        fprintf(out_file, "\tjr $ra\n");
        fprintf(out_file, "\n");
        resume_regs();
        break;
    }
    case PARAM:
    {
        break;
    }
    case ARG:
    {
        if (!begin_arg) {
            fprintf(out_file, "\taddi $sp, $sp, -4\n");
            fprintf(out_file, "\tsw $ra, 0($sp)\n");
            begin_arg = true;
        }
        f_offset += 4;
        arguement_num += 1;
        Operand arg_op = ir->u.arg;
        Assign_Space_Op var_op = find_allocated(arg_op);
        RegNum arg_reg = reg();
        load_from_mem(var_op, arg_reg);
        fprintf(out_file, "\taddi $sp, $sp, -4\n");
        fprintf(out_file, "\tsw %s, 0($sp)\n", regnam[arg_reg]);
        resume_regs();
        break;
    }
    case TAG:
    {
        Operand label = ir->u.label;
        fprintf(out_file, "%s:\n", label->u.var_name);
        break;
    }
    case GOTO:
    {
        Operand label = ir->u.label;
        fprintf(out_file, "\tj %s:\n", label->u.var_name);
        break;
    }
    case IF:
    {
        Operand candidiate1 = ir->u.ifnop.candidate1;
        Operand candidiate2 = ir->u.ifnop.candidate2;
        Operand op = ir->u.ifnop.op;
        Operand label = ir->u.ifnop.label;
        Assign_Space_Op candidate_1_op = find_allocated(candidiate1);
        Assign_Space_Op candidate_2_op = find_allocated(candidiate2);
        RegNum candidate1_reg = reg();
        RegNum candidate2_reg = reg();
        load_from_mem(candidate_1_op, candidate1_reg);
        load_from_mem(candidate_2_op, candidate2_reg);
        char* label_if = label->u.var_name;
        char* op_if = op->u.op;
        if (!strcmp(op_if, "==")) {
            fprintf(out_file, "\tbeq %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        } else if (!strcmp(op_if, ">=")) {
            fprintf(out_file, "\tbge %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        } else if (!strcmp(op_if, "<=")) {
            fprintf(out_file, "\tble %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        } else if (!strcmp(op_if, ">")) {
            fprintf(out_file, "\tbgt %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        } else if (!strcmp(op_if, "<")) {
            fprintf(out_file, "\tblt %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        } else if (!strcmp(op_if, "!=")) {
            fprintf(out_file, "\tbne %s, %s, %s\n", regnam[candidate1_reg], regnam[candidate2_reg], label_if);
        }
        resume_regs();
        break;
    }
    case WRITE:
    {
        Operand var = ir->u.variable;
        Assign_Space_Op var_alloced = find_allocated(var);
        RegNum dist_reg = reg();
        load_from_mem(var_alloced, dist_reg);
        fprintf(out_file, "\tmove $a0, %s\n", regnam[dist_reg]);
        generate_write_mips(var_alloced, dist_reg);
        resume_regs();
        break;
    }
    
    default:
        break;
    }
}

void generate_module() {
    generate_init();
    malloc_op();
    InterCodes ir = start;
    while (ir != NULL) {
        generate_intercode_mips(ir->code);
        ir = ir->next;
    }
}
