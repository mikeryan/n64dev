; Initialization routines for Backup Device version

generalexceptvec    equ 0x180

initPI
; Initialize 'SOMETHING'
; If this is not done, the N64 will freeze in about 5 seconds,
; and reset won't work at all.
    lui     t1, 0xbfc0
    lwu     t0, 0x07fc(t1)
    nop
    ori     t0, 8
    sw      t0, 0x07fc(t1)

    beq r0,r0,initPIreturn
    nop

loadcode
    j   destination
    nop

detatchGS
    ; invalidate cache... who knows what might try to overwrite our stuff?

    la      t0,$80300000
    la      t1,$80400000
invloop2    cache   17,0(t0)  ; data hit invalidate
    addi    t0,8
    bne     t0,t1,invloop2
    nop

    jr  ra
    nop

ROMload
    ; check for a ROM at the end of this file
    _tpa(lwu,t0,endoffile)      ; load header
    la      t1, $4E45531A   ; N,E,S,0x1A
    bne     t0,t1,romnotfound
    li      t0,0
    la      t0,$a0300000
    la      t1,$a0400000
    la      t2,endoffile
romcopyloop
    ld      t3,(t2)
    addi    t2,8
    sd      t3,(t0)
    addi    t0,8
    bne     t0,t1,romcopyloop
    nop
    j   endloadrom
    nop
romnotfound
                ; LOAD ROM FROM CART
                la      t3, $a4600000   ; PI

                ; wait beforehand
waitforPIbefore nop
                lw      t0, $0010(t3)
                nop
                andi    t0, 3
                bne     t0, zero, waitforPIbefore
                nop

                ; first DMA in the ROM

                la      t0, proga&$1fffffff
                sw      t0, $0000(t3)

                la      t2, $10200000   ; where the ROM is
                sw      t2, $0004(t3)

                la      t1, $fffff      ; 1 meg (big enough)
                sw      t1, $000c(t3)

waitforPI       nop
                lw      t0, $0010(t3)
                nop
                andi    t0, 3
                bne     t0, zero, waitforPI
                nop

                ; END LOAD ROM FROM CART
endloadrom
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

    j   destination
    nop
