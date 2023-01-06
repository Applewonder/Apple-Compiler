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

func:
	addi $t1, $sp, 8
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	addi $sp, $sp, -12
	lw $t1, 16($sp)
	move $t2, $t1
	sw $t2, 0($sp)
	lw $t1, 20($sp)
	move $t2, $t1
	sw $t2, 4($sp)
	lw $t1, 0($sp)
	lw $t2, 4($sp)
	add $t3, $t1, $t2
	sw $t3, 8($sp)
	lw $v0, 8($sp)
	lw $t1, 12($sp)
	move $sp, $t1
	jr $ra

main:
	addi $t1, $sp, 0
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	addi $sp, $sp, -24
	li $t1, 1
	sw $t1, 0($sp)
	li $t1, 2
	sw $t1, 4($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	lw $t1, 8($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	lw $t1, 8($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	jal func
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	sw $v0, 8($sp)
	li $t1, 3
	sw $t1, 12($sp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	lw $t1, 16($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	lw $t1, 16($sp)
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	jal func
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	sw $v0, 16($sp)
	lw $t1, 16($sp)
	move $a0, $t1
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t1, 0
	sw $t1, 20($sp)
	lw $v0, 20($sp)
	lw $t1, 24($sp)
	move $sp, $t1
	jr $ra

