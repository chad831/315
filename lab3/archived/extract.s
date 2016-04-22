

# Nghia Nguyen
# Chad Benson
# Lab 3
#

.data


.text

main:






li $v0, 10
syscall

###############################################4
# Function Extract
# assumes 32 bit value in a0
# t0 -> sign bit
# t1 -> exponent
# t2 -> fration

extract:

   addiu $sp, $sp, -4   # push stack
   sw $ra, 0($sp)
   addiu $sp, $sp, -4   
   sw $fp, 0($sp)
   move $fp, $sp

   lui $t4, 0x80000000  # parse sign bit using mask
   add $t0, $a0, $t4
   
   lui $t4, 0x7f800000 # parse exponent using mask
   add $t1, $a0, $t4

   beq $t0, $zero, unsigned   # sign bit is unsigned


signed:


   j done:

unsigned:


done:

   lw $fp, 0($sp)    # pop stack
   addiu $sp, $sp, 4   
   lw $ra, 0($sp)
   addiu $sp, $sp, 4   
   
   .end
