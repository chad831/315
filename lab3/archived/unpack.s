
# Nghia Nguyen
# Chad Benson
# Lab 3

.data

   bias: .word -127
   val: .word 0x081f0000
.text

main:

   lw $a0, val
   jal unpack

   li $v0, 10
   syscall

###############################################4
# Function Unpack 
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
   lui $t3, 0x8000      # use mask to add hidden one
   add $t2, $t2, $t3    
   neg $t2, $t2         # signed bit set, so negate    ????????????? now or before hidden one ?????????????
   j done

unsigned:

   sll $t2, $a0, 9     # parse faction by clearing upper bits
   srl $t2, $t2, 9
   lui $t3, 0x8000      # use mask to add hidden one
   add $t2, $t2, $t3    

done:
                                                                                                                     
   lw $fp, 0($sp)    # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   
   jr $ra

   .end

