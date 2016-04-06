
# Chad Benson and Nghia Nguyen
# Lab 2 - Creating subroutines
# Use recursion to find Fibonacci value

.data
	prompt1:	.asciiz  "Enter number: "
   prompt2: .asciiz  "Result: "

.text

main:
	la $a0, prompt1	#load address of prompt1 into a0
   li $v0, 4			#set print_string mode
	syscall
	
	li $v0, 5			#  read in int
	syscall
	move $a0, $v0		# store argument

	jal fib
	# get the result from ret value and pop stack
	lw $t0, 0($sp)
	addi $sp, $sp, 8
	
	la $a0, prompt2
	li $v0, 4
	syscall
	#print out the result
	move $a0, $t0
	li $v0, 1
	syscall
	
	li $v0 10			# halt
	syscall

# function
# Stack - Activation Record
################################
# local t0, t1 <- top of stack
# caller frame pointer   <- new frame pointer
# caller return address
# Space for ret value
# $a0 contains fib number to find
#################################
fib: 
	#callee setup
	addi $sp, $sp, -4		# push argument
	sw $a0, 0($sp)
	addi $sp, $sp, -8 # save space for ret value and push ra
	sw $ra, 0($sp)
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp		# set frame ptr before any local
	addi $sp, $sp, -4 
	sw $t0, 0($sp)		# store t0 for local use
	addi $sp, $sp, -4
	sw $t1, 0($sp)		#store t1 for local use
	
	li $t0, 1
	bgt $a0, $t0, Recursive
	beq $a0, $t0, BaseCase1

BaseCase0:
	sw $zero, 8($fp)	# store 0 to ret value
	j Done
	
BaseCase1:
	sw $t0, 8($fp)	# store 1 to ret value
	j Done
	
Recursive:	
	#passing argument n-1
	addi $a0, $a0, -1
	jal fib
	#getting the return value
	lw $t0, 0($sp)		# ret value from callee is at top of stack
	addi $sp, $sp, 8	# pop the ret value and argument
	
	#passing argument n-2
#	lw $a0, 12($fp)	#load the argument because $a0 changed
	addi $a0, $a0, -2
	jal fib
	#getting return value
	lw $t1, 0($sp)
	addi $sp, $sp, 8
	
	#done with 2 argument
	add $t0, $t1, $t0	# add the 2 ret value together
	sw $t0, 8($fp)		# store sum into the ret value

Done:	
	#Callee teardown
	lw $t1, 0($sp)
	addi $sp, $sp, 4
	lw $t0, 0($sp)
	addi $sp, $sp, 4
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	
	j $ra
	# return to caller
	# top of stack will point to ret value



.end
