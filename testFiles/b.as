; file ps.as
.entry LIST 
.define sz = 2 
MAIN:       mov r3 LIST[ll]
LOOP:       jmp W
            prn #-5
            mov STR[5],       STR[2] 
            sub r1   ,     r4
            cmp K,       #sz 
            bne W
L1:         inc L3
