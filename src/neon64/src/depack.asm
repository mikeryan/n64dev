; The decompression stub for Neon64, relocatable GS version

#imacros "macros.inc"
    org 0

    ; determine PC (jal to a known location and back, ra contains PC)
    la      t0,31<21|8 ; jr ra
    lui     t1,$a000
    sw      t0,0x400(t1)
    cache   20,0x400(t1) ; instruction cache fill
    sw      r0,0x404(t1)
    cache   20,0x404(t1) ; instruction cache fill
    jal     0x80000400
    nop
    addiu   v0,ra,-pc

    la      t0,$a0200000
    la      t1,depackdata_start
    addu   t1,v0
    la      t2,depackdata_end
    addu   t2,v0
copyloop
    ld      t3,(t1)
    addi    t1,8
    sd      t3,(t0)
    bne     t1,t2,copyloop
    addi    t0,8

    la      t0,$a0200000
    la      t1,$a02c0000
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
