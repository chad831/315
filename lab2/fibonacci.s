
# Chad Benson and Nghia Nguyen
# Lab 2 - Creating subroutines
# Use recursion to find Fibonacci value

.data
	msg1: .asciiz "Enter number: "
   msg2: .asciiz "Result: "

.text

main:

	la $a0, msg1   	   #load address of prompt1 into a0
   li $v0, 4			   #set print_string mode
	syscall
	
	li $v0, 1			   #  read in int
	syscall
	move $a0, $v0		   # store argument
   and $s0, $s0, $zero  # register to add returned values

	jal fib
	
	li $v0 10			# halt
	syscall

# function
# $a0 contains fib number to find
# $s0 contains final sum
fib: 

# Yo Nghia- I implemented the stack the way Prof Retz does it
# I saved the way you are used to below

#  addi $sp, $sp, -4		# push argument
#	sw $a0, 0($sp)
#	addi $sp, $sp, -8 # save space for ret value and push ra
#	sw $ra, 0($sp)
#	addi $sp, $sp, -4
#	sw $fp, 0($sp)
#	move $fp, $sp		# set frame ptr
                     
                     # push stack
   addi $sp, $sp, -4		
   sw $ra, 0($sp)    # store ra
   addi $sp, $sp, -4		
   sw $fp, 0($sp)    # store and set frame ptr
   move $fp $sp  
   addi $sp, $sp, -4		
   sw $a0, 0($sp)    # store argument n
   addi $sp, $sp, -4		
   sw $s0, 0($sp)    # store sum

   and $t1, $t1, $zero     # clear t1  
   add $t1, $t1, $a0       # load n value in t1
   li $t2, 1               # load base case 1
   
   beq $t1, $t2, base1     # check base case, if n was 1
   beq $t1, $zero, base2   # check base case, if n was 0

   # add left side of tree
   addi $t3, $t1, -1       # calc n - 1
   move $a0, $t3           # load new argument
   jal fib                 # call fib again
   add $s0, $v0, $s0       # add to sum

   # add right side of tree
   addi $t4, $t1, -2       # calc n - 2
   move $a0, $t4           # load new argument
   jal fib                 # call fib again
   add $s0, $v0, $s0       # add to sum

   base1:
      li $v0, 1   # load return value
   
   base2:
      li $v0, 0   # load return value

                  # pop stack
   lw $s0 0($sp)  # restore sum???
   addi $sp, $sp, 4		
   lw $a0 0($sp)  # restore value   
   addi $sp, $sp, 4		
   lw $fp 0($sp)  # restore frame ptr 
   addi $sp, $sp, 4		
   lw $ra 0($sp)  # retore ra 
   addi $sp, $sp, 4		

   jr $ra

.end
