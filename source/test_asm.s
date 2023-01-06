.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

fact:
	addi $t1, $sp, 4
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	addi $sp, $sp, -36
	lw $t1, 40($sp)
	move $t2, $t1
	sw $t2, 0($sp)
	li $t1, 1
	sw $t1, 4($sp)
	lw $t1, 0($sp)
	lw $t2, 4($sp)
	beq $t1, $t2, L0
	j L1
L0:
	lw $t1, 40($sp)
	move $t2, $t1
	sw $t2, 8($sp)
	lw $v0, 8($sp)
	lw $t1, 36($sp)
	move $sp, $t1
	jr $ra

	j L2
L1:
	lw $t1, 40($sp)
	move $t2, $t1
	sw $t2, 12($sp)
	lw $t1, 40($sp)
	move $t2, $t1
	sw $t2, 16($sp)
	li $t1, 1
	sw $t1, 20($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	sub $t3, $t1, $t2
	sw $t3, 24($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	lw $t1, 28($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	jal fact
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	sw $v0, 28($sp)
	lw $t1, 12($sp)
	lw $t2, 28($sp)
	mul $t3, $t1, $t2
	sw $t3, 32($sp)
	lw $v0, 32($sp)
	lw $t1, 36($sp)
	move $sp, $t1
	jr $ra

L2:
main:
	addi $t1, $sp, 0
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	addi $sp, $sp, -40
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $t1, $v0
	sw $t1, 0($sp)
	lw $t1, 0($sp)
	move $t2, $t1
	sw $t2, 4($sp)
	lw $t1, 4($sp)
	move $t2, $t1
	sw $t2, 8($sp)
	li $t1, 1
	sw $t1, 12($sp)
	lw $t1, 8($sp)
	lw $t2, 12($sp)
	bgt $t1, $t2, L3
	j L4
L3:
	lw $t1, 4($sp)
	move $t2, $t1
	sw $t2, 16($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	lw $t1, 20($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	jal fact
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	sw $v0, 20($sp)
	lw $t1, 20($sp)
	move $t2, $t1
	sw $t2, 24($sp)
	j L5
L4:
	li $t1, 1
	sw $t1, 28($sp)
	lw $t1, 28($sp)
	move $t2, $t1
	sw $t2, 24($sp)
L5:
	lw $t1, 24($sp)
	move $t2, $t1
	sw $t2, 32($sp)
	lw $t1, 32($sp)
	move $a0, $t1
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t1, 0
	sw $t1, 36($sp)
	lw $v0, 36($sp)
	lw $t1, 40($sp)
	move $sp, $t1
	jr $ra

