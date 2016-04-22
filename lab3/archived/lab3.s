

# Nghia Nguyen
# Chad Benson
# Lab 3 - Floating Point Numbers
# Funct 1, unpack
# Funct 2, normalize

.data

   # data for unpack.s
   bias: .word -127
   val: .word 0x081f0000

   # data for bintohex
   # buff: .word 0, 0, 0  # option 2, storage for result, 12 bytes
   buff: .space 9 # storage for 9 bytes, less spaces
   table: .byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46  # table of character values

.text

main:

 lw $a0, val
 jal unpack


   li $v0, 10
   syscall

###############################################
# Function Unpack 
# funct parses a a 32 bit value into its sign, exponent, and fractional components
# assumes 32 bit value in a0
# t0 -> sign bit
# t1 -> exponent
# t2 -> fration
# t3 -> mask
unpack:

   addiu $sp, $sp, -4   # push stack
   sw $ra, 0($sp)
   addiu $sp, $sp, -4   
   sw $fp, 0($sp)
   move $fp, $sp

   lui $t3, 0x8000  # parse sign bit using mask
   and $t0, $a0, $t3
   lui $t3, 0x7f80      # parse exponent using mask
   and $t1, $a0, $t3
   srl $t1, $t1, 23     # shift bits to lower 16 ??????????????????????????????


   beq $t0, $zero, unsigned   # sign bit is unsigned

signed:

   sll $t2, $a0, 10     # parse faction by clearing upper bits
   srl $t2, $t2, 10
   lui $t3, 0x0080      # use mask to add hidden one ?????????????? do u add hidden 1 regardless of sign bit????????????
   add $t2, $t2, $t3    
   neg $t2, $t2         # signed bit set, so negate    ????????????? now or before hidden one ?????????????
   j done

unsigned:

   sll $t2, $a0, 9     # parse faction by clearing upper bits
   srl $t2, $t2, 9
   lui $t3, 0x0080      # use mask to add hidden one
   add $t2, $t2, $t3    

done:

   lw $fp, 0($sp)    # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   
   jr $ra

###############################################
# Function Normalize 
# assumes exponent and fraction in t1 and t2
# t1 -> exponent
# t2 -> fration
# t3 -> bit mask
# t4 -> temp reg

normalize:  

   addiu $sp, $sp, -4   # push stack
   sw $ra, 0($sp)
   addiu $sp, $sp, -4   
   sw $fp, 0($sp)
   move $fp, $sp

   move $t1, $a0        # load arguments
   move $t2, $a1 
   
   sll $t2, $t2, 10     # remove hidden one from fraction 
   srl $t2, $t2, 10
   beq $t2, $zero, done2 # special case: fraction is zero

   lui $t3, 0x0040      # mask to test sig bit
loop2:                  # loop until significant bit is set
   
   and $t2, $t2, $t3
   beq $t2, $zero, done2
   sll $t2, $t2, 1      # shift fraction left
   addi $t1, $t1, -1     # decrement exponent (scale)
   j loop2

done2:

   lw $fp, 0($sp)       # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   move $v0, $t1        # ret exp
   lui $t3, 0x0080      # readd hidden one ???????????????????????????????????????
   add $t2, $t2, $t3    
   move $v1, $t2        # ret fration 

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





