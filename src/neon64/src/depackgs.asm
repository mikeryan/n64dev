; The decompression stub for Neon64, GS version
#imacros "macros.inc"
 org    $80300000

    la      t0,$a0200000
    la      t1,depackdata_start
    la      t2,depackdata_end
copyloop
    ld      t3,(t1)
    addi    t1,8
    sd      t3,(t0)
    bne     t1,t2,copyloop
    addi    t0,8

    la      t0,$a0200000
    la      t1,$a0300000
fillloop    cache   20,0(t0) ; instruction cache fill
    addi    t0,8
    bne     t0,t1,fillloop
    nop

    j       $a0200000
    nop

 _align(8)
depackdata_start
 obj    $80200000
    la  a0,todepack
    la  a1,$a0000400

    la  sp,$801ffff8

    jal depackrnc
    nop

    la      t0,$80000000
    la      t1,$80100000
fillloop2   cache   20,0(t0)  ; instruction cache fill
    cache   17,0(t0)         ; data hit invalidate
    addi    t0,8
    bne     t0,t1,fillloop2
    nop
    
    j   $80000400
    nop

#include "rnc.asm"

todepack
 incbin "neon64.rnc"

 objend
 _align(8)
depackdata_end
