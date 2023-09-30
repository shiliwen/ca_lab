        # Advanced branch test
        .text

        # J, JR, JAL, JALR, BEQ, BNE, BLEZ, BGTZ, BLTZ, BGEZ, BLTZAL, BGEZAL
        # BLTZAL, BGEZAL
main:
        addiu $v0, $zero, 0xa

        # Set up some comparison values in registers
        addiu $t0, $zero, 5
        addiu $t1, $zero, 10

        # Checksum register
        addiu $t2, $zero, 0x5678

        # Test jump
        j l_1
l_0:
        addu $t2, $t2, $ra
        beq   $t0, $t1, l_2
l_1:
        addiu $t2, $t2, 7
        jal l_0
        j l_8
l_2:    
        addiu $t2, $t2, 9
        bne $t0, $t1, l_4
l_3:
        # Taken
        addiu $t2, $t2, 5
        bgez $t0, l_6
l_4:
        # Not taken
        addiu $t2, $t2, 11
        blez  $t0, l_3
l_5:
        # Taken
        addiu $t2, $t2, 99
        bgtz  $t0, l_3
l_6:
        # here
        addiu $t2, $t2, 111
        jr $ra
        # Should go to l_1, then go to l_8
l_7:
        # Should not get here
        addiu $t2, $t2, 200
        
        syscall
l_8:    
        addiu $t2, $t2, 215
        jal l_10
l_9:
        # Should not get here
        addiu $t2, $t2, 1
        syscall        
l_10:    
        addu $t2, $t2, $t0
        bltzal $t1, l_12
l_11:
        # Should not get here
        addiu $t2, $t2, 400
        syscall
l_12:    
        addu $t2, $t2, $t0
        bgezal $t1, l_11
        
l_13:    
        addiu $t2, $t2, 0xbeb0063d
        syscall

