.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "
"
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
	jr $ra

	addi $t1, $sp, 0
	addi $sp, $sp, -4
	sw $t1, 0($sp)
	addi $sp, $sp, -4
main:
	lw $t1, 0($sp)
	li $t1, 0
	sw $t1, 0($sp)
	lw $v0, 0($sp)
	lw $t1, 4($sp)
	move $sp, $t1
	jr $ra
