.data
number: .word 4155543, 3112 , -2, 1054, -33543, 1233, -433433, 10101, 16384
#number: .word 2
.text
main:
   la $t4, number #load the address of number in to t4
   li $t3, 9	#set counter to 10
   li $t1, 0 #total
   
loop:
   lw $t0, 0($t4) #load the number into t0
   add $t1, $t0, $t1 #t1 = t1 + t0
   addi $t4, $t4, 4 #get to the next address of number - 4 bytes with word
   addi $t3, $t3, -1 #decrease the counter
   bne $t3, $zero, loop #jump back to loop if t3 is not 0
   move $a0, $t1
   li $v0, 1 #print int
   syscall 
   
   li $v0, 10 #stop program
   syscall

.end