#
# Nghia Nguyen
# Chad Benson
# Lab 2 - Creating Functions in mips
# Functions takes a 32 bit value and
# shifts it according to a bit field format,
# from   fff0 0nn0 0000 x000 yyyy 0000 0000 0000
# to     0000 0000 0000 0000 yyyy 000x 0fff 00nn 
# 

.data

   val1: .word 0x6608C000
   val2: .word 0xC2008000
   inputMask: .word 0x19F70FFF
   shiftMask1: .word 0x0000F000  # used for bits 15 - 12
   shiftMask2: .word 0x00080000  # used for bit 19
   msg: .asciiz   "Sorry, format of input is not valid."
   format: .asciiz "\n"
	buff: .space 9 # storage for 9 bytes, less spaces
   table: .byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46  # table of character values

.text

################################################
main:

   lw $a0, val1    # load first input value
   jal shifter
	
	la $t1 table # load address of table
   la $a1 buff  # load address of buffer
   move $a0, $v0  # load result to print
	
   # call bintohex func
   jal bintohex
	
	# a1 will be at the last index in buffer when ret
	sb $zero 0($a1)
   la $a0 buff
	li $v0 4
	syscall

   li $v0, 4
   la $a0, format
   syscall

   lw $a0, val2   # load second input value
   jal shifter
   
	la $t1 table # load address of table
   la $a1 buff  # load address of buffer
   move $a0, $v0  # load result to print
	
   # call bintohex func
   jal bintohex

   # a1 will be at the last index in buffer when ret
	sb $zero 0($a1)
   la $a0 buff
	li $v0 4
	syscall
	
   li $v0 10
   syscall

  
 #  lw $a0, val2
 #  jal shifter



###############################################
# function valid checks whether input matches format
# assumes an input value in a0
# does not return anything

validate:

   addi $sp, $sp, -4       # push stack
   sw $ra, 0($sp)
   addi $sp, $sp, -4    
   sw $fp, 0($sp)
   move $fp, $sp
   addi $sp, $sp, -4    
   sw $a0, 0($sp)

   lw $t1, inputMask       # load mask
   and $a0, $a0, $t1       # mod input to test format
   beq $a0, $zero, valid   # branch to valid if a0 is zero
   
   la $a0, msg             # if not valid, alert user, and halt program
   li $v0, 4               # print error
   syscall

   li $v0, 10
   syscall

valid:

   lw $a0, 0($sp)          # pop stack
   addi $sp, $sp, 4    
   lw $fp, 0($sp)       
   addi $sp, $sp, 4    
   lw $ra, 0($sp)
   addi $sp, $sp, 4    
   jr $ra

#################################################
# function shifter assumes value in a0, and then validates
# t0 for result
# t1 handles top 7 bits
# t2 handles bits 12 - 15
# t3 handles bit 18
# t4 stores bit masks
# returns shifted word in v0

shifter:

   addi $sp, $sp, -4       # push stack
   sw $ra, 0($sp)
   addi $sp, $sp, -4    
   sw $fp, 0($sp)
   move $fp, $sp
   addi $sp, $sp, -4    
   sw $a0, 0($sp)

   jal validate         # test input format, halt if incorrect
                        # input ok, compute shift sequence

   srl $t1, $a0, 25     # shift high 7 bits to bottom
   lw $t4, shiftMask1
   and $t2, $t4, $a0    # save bits 12 through 15
   lw $t4, shiftMask2
   and $t3, $t4, $a0    # save bit 19
   srl $t3, $t3, 11     # move bit 19 down
   add $t1, $t2, $t1    # sum registers   
   add $t1, $t3, $t1    
   move $v0, $t1        # store result

   lw $a0, 0($sp)
   addi $sp, $sp, 4    
   lw $fp, 0($sp)       # pop stack
   addi $sp, $sp, 4    
   lw $ra, 0($sp)
   addi $sp, $sp, 4    
   jr $ra

##############################################
# function bintohex
# function makes the following assumptions: 
# a0 => value
# a1 => buffer location
# t1 => table
# t5 => loop counter
# ther is no return value

bintohex:
   addi $sp $sp -4 # build stack, ra, fp, vars, etc
   sw $ra 0($sp)
   addi $sp $sp -4    
   sw $fp 0($sp)
   move $fp $sp
   addi $sp $sp -4 # save temp register $t0
	sw $t0 0($sp)
	
	
   li $t5 8 			# set counter to 8
loop:
   srl $t0 $a0 28 	# get the first 4 bits of a0 into t0
	sll $a0 $a0 4 	 	# shift left 4 bits to delete the first 4 bits we got above
	add $t0 $t0 $t1 	# t0 will not be at the right index at the table
	
	lb  $t0 0($t0)		# the hex value (character) is now save in t0
	sb  $t0 0($a1)		# save the hex value (character) into the buff
	addi $a1 $a1 1		# increase to the next location in the buff
	addi $t5, $t5, -1 # decrease the counter by 1
	bne $t5 $zero loop	# run until the counter reach 0 (run 8 times)
	
   lw $t0 0($sp)		# restore the temp t0
	addi $sp $sp 4
	lw $fp 0($sp)    	# restore frame
	addi $sp $sp 4
	lw $ra 0($sp)		# retore the ret address
	addi $sp $sp 4    # stack is now has nothing
	jr $ra 				
   
.end
