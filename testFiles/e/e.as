.define sz = 2 
MAIN:       mov r3, LIST[sz]
LOOP:       jmp r2
            prn #-5
            mov STR[5],       STR[2] 
            sub r1   ,     r4
            cmp K,       #sz 
            bne STR
L1:         inc LIST
mcr mcr_m
    cmp r3, #sz 
    bne END
endmcr
prn #-5
mov STR[5], STR[2]  
sub r1, r4
mcr_m
inc K
kkk
bne LOOP
END: hlt 
.define len = 4
STR:   .string    "abcdef" 
LIST: .data 6   ,  -9 ,     len
K:  .data 22 
;mcr kkk
mcr kkk
hlt
cmp #3, r5
sub r4,r7
mov #3, r1
endmcr
