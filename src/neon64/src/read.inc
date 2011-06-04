; t4 and up should be safe to use...
A6502_maskedaddr        equr    t4
A6502_compaddr          equr    t6
Read6502:

        li      A6502_regval,0 ; for unsupported registers
        ; the low registers are mirrored, there are only eight of them

        andi    A6502_maskedaddr,A6502_regaddr,0x6000
        xori    A6502_maskedaddr,0x6000
        bnez    A6502_maskedaddr,A6502_notsram
        nop
        li      A6502_regval,0
        jr      A6502_return
        nop
A6502_notsram
        andi    A6502_maskedaddr,A6502_regaddr,0x4017
        slti    t5,A6502_maskedaddr,0x18 ;8
        beqz    t5,readhigh
        li      A6502_compaddr,2
        andi    A6502_maskedaddr,7
        bne     A6502_maskedaddr,A6502_compaddr,notreadstatus
        ;li      A6502_compaddr,4
        li      A6502_compaddr,7
        ; 2002: This is the status of the PPU
        _tpa(lbu,A6502_regval,ppustatus)
        _tpa(sw,r0,pputoggle) ; this clears the first/second write toggle
        andi    t4,A6502_regval,$7f
        _tpa(sb,t4,ppustatus)
        jr      A6502_return
        nop
notreadstatus:
;        bne     A6502_maskedaddr,A6502_compaddr,notreadsprram
;        li      A6502_compaddr,7
;
;        ; SPRRAM I/O register
;        
;        _tpa(lw,t4,sprramaddr)
;        _vtpa(lb,A6502_regval,_SPRRAM,t4)
;        addi    t4,1
;        andi    t4,0xff
;        _tpa(sw,t4,sprramaddr)
;
;        jr      A6502_return
;        nop
;notreadsprram
        bne     A6502_maskedaddr,A6502_compaddr,notreadvram
        nop     ; no other low regs to read
        ; This is VRAM read

        _tpa(lw,t5,_PPUControl1)
        _tpa(lhu,t4,_VRAM_V)
        andi    t5,4
        beqz    t5,notrinc32
        addi    t6,t4,1
        addi    t6,31
notrinc32:
        _tpa(sh,t6,_VRAM_V)
        andi    t5,t4,0x3fff
        slti    t6,t5,0x3f00
        beqz    t6,readpal
        andi    t6,t4,0x3c00
        srl     t6,10-3
        _vtpa(lw,t4,_VRAMPages,t6)
        add     t4,t5
        _tpa(lw,A6502_regval,lastVRAM)
        lbu     t5,(t4)
        _tpa(sw,t5,lastVRAM)
        jr      A6502_return
        nop
readpal:
        andi    t5,0x1f
        ; The 'real' palette is what is written through 2007. The PPU uses
        ;  a version already translated to truecolor.
        _tpa(lw,A6502_regval,lastVRAM)
        _vtpa(lb,t5,realpal,t5)
        _tpa(sw,t5,lastVRAM)
        jr      A6502_return
        nop
notreadvram:

        ; a read from a valueless register...
        _tpa(sh,A6502_regaddr,readerraddr)
        j       readerr
        nop

        ; no other low regs to read
        jr      A6502_return
        nop
readhigh:
        li      A6502_compaddr,0x4015
        bne     A6502_compaddr,A6502_regaddr,rnotalen
        li      A6502_compaddr,0x4016
        _tpa(lbu,A6502_regval,astatus)
        ;andi    A6502_regval,~$40    ; clear bit 6, sound frame IRQ

        andi    A6502_regval,$c0 ; sound interrupts
        _tpa(lbu,t5,sq1len)
        beqz    t5,sq1off2
        nop
        ori     A6502_regval,1
sq1off2
        _tpa(lbu,t5,sq2len)
        beqz    t5,sq2off2
        nop
        ori     A6502_regval,2
sq2off2
        _tpa(lbu,t5,trilen)
        _tpa(lbu,t6,lincnt)
        beqz    t5,trioff2
        nop
        beqz    t6,trioff2
        nop
        ori     A6502_regval,4
trioff2

        _tpa(lbu,t5,rndlen)
        beqz    t5,rndoff2
        nop
        ori     A6502_regval,8
rndoff2
        _tpa(lhu,t5,dmclen)
        beqz    t5,dmcoff2
        nop
        ori     A6502_regval,$10
dmcoff2

        _tpa(sb,A6502_regval,astatus)
        jr      A6502_return
        nop

rnotalen
        bne     A6502_compaddr,A6502_regaddr,rnotcon1
        li      A6502_compaddr,0x4017
        _tpa(lw,t4,controller1cnt)
        _tpa(lw,t5,controller1)
        li      t6,23
        sub     t6,t4
        addi    t4,1
        bgez    t6,rc1_isntzero
        nop
        jr      A6502_return
        li      A6502_regval,$40
rc1_isntzero
        _tpa(sw,t4,controller1cnt)

        srlv    A6502_regval,t5,t6
        andi    A6502_regval,1
        ori     A6502_regval,$40 ; fixes Mad Max et al, thanks to Jsr!

        jr      A6502_return
        nop
rnotcon1:
        bne     A6502_compaddr,A6502_regaddr,rnotcon2
        nop

        _tpa(lbu,t4,arkflag)
        bnez    t4,arkproc
        li      A6502_regval,0

        _tpa(lw,t4,controller2cnt)
        _tpa(lw,t5,controller2)
        li      t6,23
        sub     t6,t4
        addi    t4,1
        bgez    t6,rc2_iszero
        nop
        jr      A6502_return
        li      A6502_regval,$00
rc2_iszero
        _tpa(sw,t4,controller2cnt)

        srlv    A6502_regval,t5,t6
        andi    A6502_regval,1

        jr      A6502_return
        nop
arkproc
        ; Exclusively Arkanoid (tm) stuff

        _tpa(lw,t5,controller1)
        srl     t5,16
        andi    t5,$f0
        beqz    t5,arkbutno2 ; button
        nop
        ori     A6502_regval,8
arkbutno2

        _tpa(lbu,t5,arkcnt) ; paddle position
        _tpa(lbu,t4,arkpad)
        li      t6,8
        bnel    t5,t6,arkoutofdata2
        addi    t6,t5,1
        ori     A6502_regval,$10
arkoutofdata2
        _tpa(sb,t6,arkcnt)
        sllv    t4,t5
        andi    t4,$80
        srl     t4,3
        or      A6502_regval,t4

        jr      A6502_return
        nop
rnotcon2:
        ; nothing else to read

        _tpa(sh,A6502_regaddr,readerraddr)
        j       readerr
        nop

        jr      A6502_return
        nop

readerr ;_tpa(lbu,t4,traperrors)
        ;beqz    t4,donttrapreads
        ;nop
        ;_tpa(lhu,t4,readerraddr)
        ;_text_value(61,188,t4,3)
        ;jal     a6502_status
        ;nop
        ;deadend
donttrapreads
        jr      A6502_return
        nop
        
readra  dw  0
readerraddr dh   0
 _align(4)

; For death awaits you all, with nasty big pointy teeth.
