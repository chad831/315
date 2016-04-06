
# Chad Benson and Nghia Nguyen
# Lab 2 - Creating subroutines
# Use recursion to find Fibonacci value

.data
	asciiz prompt1 "Enter number: "
   asciiz prompt2 "Result: "

.text

main:
	la $a0, prompt1	#load address of prompt1 into a0
   li $v0, 4			#set print_string mode
	syscall
	
	li $v0, 1			#  read in int
	syscall
	move $a0, $v0		# store argument

	jal fib
	
	li $v0 10			# halt
	syscall

# function
# $a0 contains fib number to find
fib: 
	addi $sp, $sp, -4		# push argument
	sw $a0, 0($sp)
	addi $sp, $sp, -8 # save space for ret value and push ra
	sw $ra, 0($sp)
	addi $sp, $sp, -4
	sw $fp, 0($sp)
	move $fp, $sp		# set frame ptr
	
	
.end
