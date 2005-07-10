; Initialization routines for Game Shark version

generalexceptvec    equ 0x120   ; relocated there by the GS

initPI
    beq r0,r0,initPIreturn
    nop
ROMload
    jr  ra
    nop

loadcode
    j       destination ; first absolute jump
    nop

detatchGS
                ; The ROM has been loaded, we can now detatch the GS.

                mtc0    r0,watchlo
                mtc0    r0,watchhi

                la      a0,intvector    ; take back the exception vector
                la      a1,0xa0000000
                la      a2,4-1    ; 4 words
                la      a3,gstemp
storeloop2      lw      t0,0x180(a1)
                lw      t1,0(a0)
                addiu   a0,4
                sw      t1,0x180(a1)
                sw      t0,0(a3)
                sync
                cache   $10,0x180(a1)
                addiu   a1,4
                addiu   a3,4
                bnez    a2,storeloop2
                addi    a2,-1

                ; make sure the ROM is there
                la      t0,$80300000
                la      t1,$80400000
                la      t2,$a0300000
wbloop2         ld      t3,(t0)
                sd      t3,(t2)
                cache   25,0(t0)
                addi    t0,8
                bne     t0,t1,wbloop2
                addi    t2,8
                nop
               
                jr  ra
                nop

loadnewrom      
    ; disable interrupts
    mfc0    t0,status
    move    sp,t0
    nop
    andi    t1,t0,$ff00 ; unmask all interrupts
    xor     t0,t1
    andi    t0,$fffe ; clear IE
    mtc0    t0,status

    ; put back the GS's exception vector
                la      a1,0xa0000000
                la      a2,4-1    ; 4 words
                la      a3,gstemp
storeloop3      lw      t1,0(a3)
                addiu   a3,4
                sw      t1,0x180(a1)
                sync
                cache   $10,0x180(a1)
                addiu   a1,4
                bnez    a2,storeloop3
                addi    a2,-1

    j   destination ; should handle all the stuff...
    nop

gstemp  dcw     4,0
 _align(4)
