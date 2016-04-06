
# Chad Benson and Nghia Nguyen
# Lab 2 - Creating subroutines
# Uses 32 bit value to print characters

.data
   
   # buff: .word 0, 0, 0  # option 2, storage for result, 12 bytes
   buff: .space 9 # storage for 9 bytes, less spaces
   table: .byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46  # table of character values


.text

main:
	
   la $t1 table # load address of table
   la $a1 buff  # load address of buffer
   li $a0 0x1A0B8F03 # test value
 
	
	# call bintohex func
   jal bintohex
	  
	# a1 will be at the last index in buffer when ret
	sb $zero 0($a1)
   la $a0 buff
	li $v0 4
	syscall

   li $v0 10 # halt program
   syscall


# function bintohex
# a0 => value
# a1 => buffer location
# t1 => table
# t5 => loop counter
#

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
