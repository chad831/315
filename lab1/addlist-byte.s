.data
number: .byte 40, 33, -127, 122, 4, 0, 16, 24, 32, -5, 123 

.text
main:
   la $t4, number 		#load the address of number in to t4
   li $t3, 10			#set counter to 10
   li $t1, 0			#total
   
loop:
   lb $t0, 0($t4) 		#load the number into t0
   add $t1, $t0, $t1	#t1 = t1 + t0
   addi $t4, $t4, 1		#get to the next address of number - 1 byte with byte
   addi $t3, $t3, -1 	#decrease the counter
   bne $t3, $zero, loop #jump back to loop if t3 is not 0
   move $a0, $t1		#move t1 into a0 for printing
   li $v0, 1 			#print int
   syscall 
   
   li $v0, 10 			#stop program
   syscall

.end