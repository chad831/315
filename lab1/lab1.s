
# Chad Benson
# Nghia Nguyen
# 315
#
.data

list: .word 22 33 -10

# use lb for load byte
.text


main:

    la $t4 list
    li $t3 9
    li $t1 0

    loop: 
        lw $t0 0($t4)
        add $t1 $t0 $t1
        addi $t4 $t4 4
        addi $t3 $t3 -1
        bne $t3 $zero loop
    
    move $a0 $t1
    li $v0 1
    syscall


    set expandtab 
    set tabstop=3
    set softtabstop=3
    set shiftwidth=3


.end


