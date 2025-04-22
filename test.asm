.text

main:
    and $t0, $t0, $zero        # $t0 = 0
    and $t1, $t1, $zero        # $t1 = 10
    addi $t1, $t1, 10

loop:
    beq $t0, $t1, exit         # if ($t0 == 10) exit

    and $t2, $t2, $zero        # $t2 = 0
    lw $t3, 0($t2)             # $t3 = mem[$t2]
    addi $t3, $t3, 1           # $t3 += 1
    sw $t3, 0($t2)             # mem[$t2] = $t3

    addi $t0, $t0, 1           # $t0 += 1
    beq $zero, $zero, loop     # goto loop

pre_exit:
    lw $v0, 0($zero)           # $v0 = mem[0]

exit:
    beq $v0, $v0, exit         # goto exit