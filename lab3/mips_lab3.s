
# Nghia Nguyen
# Chad Benson
# Lab 3 - Floating Point Numbers
# Funct 1, unpack
# Funct 2, normalize

.data

   # messages
   newLine: .asciiz "\n"
   hex: .asciiz "0x"
   msg1: .asciiz "If the value is: "
   msg2: .asciiz "The sign is: "
   msg3: .asciiz "The exponent is: "
   msg4: .asciiz "The fraction is: "
   msg5: .asciiz "The exponent after normalizing: "
   msg6: .asciiz "The fraction after normalizing: "
   

   # data for unpack.s
   bias: .word 127
   # TEST VALUE #########################
   val: .word 0xc1480000

   # data for bintohex
   # buff: .word 0, 0, 0  # option 2, storage for result, 12 bytes
   buff1: .space 9 # storage for 9 bytes, less spaces
   buff2: .space 9 # storage for 9 bytes, less spaces
   buff3: .space 9 # storage for 9 bytes, less spaces
   buff4: .space 9 # storage for 9 bytes, less spaces
   buff5: .space 9 # storage for 9 bytes, less spaces
   buff6: .space 9 # storage for 9 bytes, less spaces

   table: .byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46  # table of character values

.text

main:

   # test unpack 
   li $v0, 4      
   la $a0, msg1
   syscall

   # Load predefined contants and tables
   la $a1, buff1
   la $s0, table
   la $a0, hex 
   li $v0, 4                  
   syscall

   # print out the test value in hex
   lw, $a0, val
   la $a1, buff1
   jal bintohex      

   la $a0, buff1
   li $v0, 4
   syscall
   li $v0, 4      
   la $a0, newLine 
   syscall

   # unpack value, and print the result
   lw $a0, val
   jal unpack

   # load exponent and fraction to normalize
   move $a0, $v0
   move $a1, $v1
   jal normalize

   # save result
   move $s1, $v0
   move $s2, $v1
   
   # print normalized exponent
   li $v0, 4                   
   la $a0, msg5
   syscall
   la $a0, hex 
   li $v0, 4                  
   syscall

   move $a0, $s1
   la $a1, buff5
   jal bintohex
 
   la $a0, buff5
   li $v0, 4
   syscall
   
   li $v0, 4      
   la $a0, newLine 
   syscall

   # print normalized fraction
   li $v0, 4                   
   la $a0, msg6
   syscall
   la $a0, hex 
   li $v0, 4                  
   syscall

   move $a0, $s2
   la $a1, buff6
   jal bintohex
 
   la $a0, buff6
   li $v0, 4
   syscall
   
   li $v0, 4      
   la $a0, newLine 
   syscall

   li $v0, 4      
   la $a0, newLine 
   syscall

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
# bugs: working for positives but not all negatives
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
   srl $t1, $t1, 23     # move exponent to lower bits 
   lw $t3, bias
   sub $t1, $t1, $t3    # substract bias to exponent
   li $t3, 0x00FF
   and $t1, $t1, $t3    # clear upper bits 

   sll $t2, $a0, 9            # parse faction by clearing upper bits
   srl $t2, $t2, 2            # shift back for sign bit and hidden one bit, format: sh.fraction
   lui $t3, 0x4000            
   add $t2, $t2, $t3          # add hidden one bit in
   beq $t0, $zero, unsigned   # if sign bit is unsigned, skip addin and negation
   neg $t2, $t2               # signed bit set, so use 2s complement 
   #addiu $t2, 1 

unsigned:

   li $v0, 4                  # print sign                  
   la $a0, msg2
   syscall
   la $a0, hex 
   li $v0, 4                  
   syscall

   move $a0, $t0              
   la $a1, buff2
   jal bintohex      

   la $a0, buff2
   li $v0, 4
   syscall
   li $v0, 4      
   la $a0, newLine 
   syscall

   li $v0, 4                   # print exponent 
   la $a0, msg3
   syscall
   la $a0, hex                
   li $v0, 4                  
   syscall
  
   move $a0, $t1 
   la $a1, buff3
   jal bintohex
   
   la $a0, buff3
   li $v0, 4
   syscall
   li $v0, 4      
   la $a0, newLine 
   syscall

   li $v0, 4                   # print fraction 
   la $a0, msg4
   syscall
   la $a0, hex                
   li $v0, 4                  
   syscall
  
   move $a0, $t2 
   la $a1, buff4
   jal bintohex
   
   la $a0, buff4
   li $v0, 4
   syscall
   li $v0, 4      
   la $a0, newLine 
   syscall

   move $v0, $t1                 # return exponent and fraction
   move $v1, $t2

   lw $fp, 0($sp)                # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   
   jr $ra

###############################################
# Function Normalize 
# assumes exponent and fraction in t1 and t2
# t1 -> exponent
# t2 -> fraction
# t3 -> bit mask
# t4 -> temp reg

normalize:  

   addiu $sp, $sp, -4   # push stack
   sw $ra, 0($sp)
   addiu $sp, $sp, -4   
   sw $fp, 0($sp)
   move $fp, $sp

   move $t1, $a0           # load arguments
   move $t2, $a1 
   
   beq $t2, $zero, done2    # special case: fraction is zero

   lui $t3, 0x8000          # mask to test 31 bit of fraction
	and $t3, $t2, $t3
	beq $t3, $zero, CheckBit30
	srl $t2, $t2, 1			# shift fract 1 bit to the right
	addi $t1, $t1, 1
	j done2
	
CheckBit30:
	lui $t3, 0x4000	      # loop until bit 30 is set
   and $t3, $t2, $t3
   bne $t3, $zero, done2
   sll $t2, $t2, 1         # shift fraction left
   addi $t1, $t1, -1       # decrement exponent (scale)
   j CheckBit30

done2:

   lw $fp, 0($sp)       # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   move $v0, $t1        # ret exp   
   move $v1, $t2        # ret fration 

   jr $ra

##############################################
# function bintohex
# function makes the following assumptions: 
# a0 => value
# s0 -> table
# a1 => buffer location
# t5 => loop counter

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
   srl $t0, $a0, 28 	# get the first 4 bits of a0 into t0
	sll $a0, $a0, 4 	 	# shift left 4 bits to delete the first 4 bits we got above
	add $t0, $t0, $s0 	# t0 will not be at the right index at the table
	
	lb  $t0, 0($t0)		# the hex value (character) is now save in t0
	sb  $t0, 0($a1)		# save the hex value (character) into the buff
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





