; MMC3

#define mmc3mapify(_bank,_prgadr,_temp) \
        sll     _bank,13\
        la      _temp,prgrom\
        addu    _bank,_temp\
        subu    _bank,_prgadr\
        srl     _prgadr,8-2\
        la      _temp,nespagetable\
        addu    _prgadr,_temp\
        li      _temp,0x20-1\
        sw      _bank,(_prgadr)\
        addi    _prgadr,4\
        bnez    _temp,pc-8\
        addi    _temp,-1


        andi    t4,A6502_regaddr,0x6001
        bnez    t4,notmmc3command
        addi    t4,-1
        _tpa(sb,A6502_regval,mmc3command)

        move    t6,A6502_regval
        j       mmc3prgfix
        nop
notmmc3command
        bnez    t4,notmmc3pgnum
        addi    t4,-0x1fff

        _tpa(lbu,t6,mmc3command)

        li      t4,0
        andi    t5,t6,7   ; mask out command #
mmc3cmd0
        bne     t4,t5,mmc3cmd1
        addi    t4,1
        j       mmc3vrom2k
        li      t4,0x0000
mmc3cmd1
        bne     t4,t5,mmc3cmd2
        addi    t4,1
        j       mmc3vrom2k
        li      t4,0x0800
mmc3cmd2
        bne     t4,t5,mmc3cmd3
        addi    t4,1
        j       mmc3vrom1k
        li      t4,0x1000
mmc3cmd3
        bne     t4,t5,mmc3cmd4
        addi    t4,1
        j       mmc3vrom1k
        li      t4,0x1400
mmc3cmd4
        bne     t4,t5,mmc3cmd5
        addi    t4,1
        j       mmc3vrom1k
        li      t4,0x1800
mmc3cmd5
        bne     t4,t5,mmc3cmd6or7
        nop
        j       mmc3vrom1k
        li      t4,0x1c00
mmc3cmd6or7

        andi    t4,t6,1

        _tpa(lbu,t5,prgromsize)
        sll     t5,1    ; iNES specifies 16k banks
        div     A6502_regval,t5
        nop
        nop ; just for safety, I really should check the manual about this
        mfhi    A6502_regval
        nop

        _vtpa(sb,A6502_regval,mmc3prg,t4)
mmc3prgfix
        _tpa(lbu,A6502_regval,prgromsize)
        sll     A6502_regval,1  ; iNES specifies 16k banks
        andi    t6,$40
        bnez    t6,mmc3AandC
        nop
        ; $8000 and $a000 are switched
        _tpa(lbu,t4,mmc3prg)
        _tpa(lbu,t5,mmc3prg+1)
        addi    t6,A6502_regval,-2
        j       mmc38andA
        nop
mmc3AandC
        addi    t4,A6502_regval,-2
        _tpa(lbu,t5,mmc3prg+1)
        _tpa(lbu,t6,mmc3prg)
mmc38andA

        li      t7,0x8000
        mmc3mapify(t4,t7,A6502_regval)
        li      t7,0xa000
        mmc3mapify(t5,t7,A6502_regval)
        li      t7,0xc000
        mmc3mapify(t6,t7,A6502_regval)
        ; last bank never changes

        ;_tpa(lhu,t6,mmc3prg)
        ;_tpa(sh,t6,mmc3watch)

        jr      A6502_return
        nop

mmc3vrom2k
        _tpa(lbu,t5,chrromsize)
        sll     t5,3    ; 8k->1k
        div     A6502_regval,t5
        nop
        nop ; just for safety, I really should check the manual about this
        mfhi    A6502_regval

        andi    t6,0x80
        sll     t6,5
        xor     t4,t6
        _tpa(lbu,t6,prgromsize)
        la      t5,prog
        sll     t6,14
        addu    t6,t5
        andi    A6502_regval,0xfe
        sll     A6502_regval,10 ; refers to 2 1k pages, not 1 2k
        addu    t6,A6502_regval
        subu    t5,t6,t4
        ; calc vrampages adr
        srl     t7,t4,10-3
        _vtpa(sw,t5,_vrampages,t7)
        _vtpa(sw,t5,_vrampages+8,t7)
        _vtpa(cache,25,_vrampages,t7)
        _vtpa(cache,25,_vrampages+8,t7)
        ; request cache update
        _tpa(sd,ra,writera)
        move    t5,t6
        move    t7,t6
        move    t6,t4
        jal     gcrequest
        nop
        addiu   t5,t7,0x0400
        addiu   t6,t4,0x0400
        jal     gcrequest
        nop

        _tpa(ld,ra,writera)

        jr      A6502_return
        nop

mmc3vrom1k
        _tpa(lbu,t5,chrromsize)
        sll     t5,3    ; 8k->1k
        div     A6502_regval,t5
        nop
        nop ; just for safety, I really should check the manual about this
        mfhi    A6502_regval

        andi    t6,0x80
        sll     t6,5
        xor     t4,t6
        _tpa(lbu,t6,prgromsize)
        la      t5,prog
        sll     t6,14
        addu    t6,t5
        sll     A6502_regval,10
        addu    t6,A6502_regval
        subu    t5,t6,t4
        ; calc vrampages adr
        srl     t7,t4,10-3
        _vtpa(sw,t5,_vrampages,t7)
        _vtpa(cache,25,_vrampages,t7)
        ; request cache update
        _tpa(sd,ra,writera)
        move    t5,t6
        move    t6,t4
        jal     gcrequest
        nop
        _tpa(ld,ra,writera)
        
        jr      A6502_return
        nop

notmmc3pgnum

        bnez    t4,notmmc3mirror
        addi    t4,-1

;        _tpa(lbu,t4,romc1)
;        andi    t4,8    ; 4 screen
;        bnez    t4,mmc3mirrorend
;        nop

        andi    A6502_regval,1 ; v/h
        beqz    A6502_regval,vmirrormmc3
        nop

        ; Horizontal mirroring
        la      t5,vram
        _tpa(sw,t5,8*8+_VRAMPages)
        _tpa(cache,25,8*8+_vrampages)
        la      t5,vram-0x400
        _tpa(sw,t5,9*8+_VRAMPages)
        _tpa(sw,t5,10*8+_VRAMPages)
        _tpa(cache,25,9*8+_vrampages)
        _tpa(cache,25,10*8+_vrampages)
        la      t5,vram-0x800
        _tpa(sw,t5,11*8+_VRAMPages)
        _tpa(cache,25,11*8+_vrampages)

        jr      A6502_return
        nop
vmirrormmc3:
        ; Vertical mirroring
        la      t5,vram
        _tpa(sw,t5,8*8+_VRAMPages)
        _tpa(sw,t5,9*8+_VRAMPages)
        _tpa(cache,25,8*8+_vrampages)
        _tpa(cache,25,9*8+_vrampages)
        la      t5,vram-0x800
        _tpa(sw,t5,10*8+_VRAMPages)
        _tpa(sw,t5,11*8+_VRAMPages)
        _tpa(cache,25,10*8+_vrampages)
        _tpa(cache,25,11*8+_vrampages)
mmc3mirrorend
        jr      A6502_return
        nop
notmmc3mirror
        bnez    t4,notmmc3sram
        addi    t4,-0x1fff

        andi    A6502_regval,0x80
        bnez    A6502_regval,mmc3sramen
        nop

        _tpa(sw,ra,mmc3ra)
        jal     enable_sram
        nop
        _tpa(lw,ra,mmc3ra)

        jr      A6502_return
        nop

mmc3sramen
        _tpa(sw,ra,mmc3ra)
        jal     enable_sram
        nop
        _tpa(lw,ra,mmc3ra)

        jr      A6502_return
        nop

notmmc3sram
        bnez    t4,notmmc3IRQcnt
        addi    t4,-1

        _tpa(sh,A6502_regval,mmc3irqcnt)
        jr      A6502_return
        nop
notmmc3IRQcnt
        bnez    t4,notmmc3IRQlatch
        addi    t4,-0x1fff

        _tpa(sb,A6502_regval,mmc3irqlatch)
        jr      A6502_return
        nop
notmmc3IRQlatch
        bnez    t4,notmmc3IRQc0
        addi    t4,-1

        _tpa(sb,r0,mmc3irqen)           ; disable
        _tpa(sb,r0,mmc3irqon)
        jr      A6502_return
        nop
notmmc3IRQc0
        bnez    t4,notmmc3IRQc1
        nop

        _tpa(sb,r0,mmc3irqon)
        li      A6502_regval,1
        _tpa(sb,A6502_regval,mmc3irqen) ; enable
notmmc3IRQc1
        jr      A6502_return
        nop

; Figure out MMC3 IRQ
; first IRQ in a frame
mmc3irq         _tpa(lwu,t4,_PPUControl2)
                _tpa(lbu,t1,mmc3irqen)
                _tpa(lh,t2,mmc3irqcnt)
                beqz    t1,mmc3return
                andi    t4,$18
                beqz    t4,nommc3dec
                nop
                beqz    t2,nommc3dec
                nop
                addiu   t2,-1
                _tpa(sh,t2,mmc3irqcnt)
nommc3dec       bgtz    t2,mmc3return
                nop

                jal     enable_sram
                nop

                _tpa(lbu,t0,mmc3irqlatch)
                _tpa(sh,t0,mmc3irqcnt)
                li      t1,1
                _tpa(sb,t1,mmc3irqon)
                j       mmc3return
                nop

enable_sram
        la      t4,nespagetable+(0x60*4)        ; enable SRAM
        lwu     t6,(t4)
        li      t5,0x20-1
        bnez    t6,not_sral2
        la      t6,sram-0x6000
sral2   sw      t6,(t4)     ; read table
        sw      t6,1024(t4) ; write table
        addi    t4,4
        bnez    t5,sral2
        addi    t5,-1
not_sral2
        jr      ra
        nop

disable_sram
        ; disable SRAM by giving control to the handler
        la      t4,nespagetable+(0x60*4)
        lwu     t6,(t4)
        li      t5,0x20-1
        beqz    t6,not_sral1
sral1   sw      r0,(t4)     ; read table
        sw      r0,1024(t4) ; write table
        addi    t4,4
        bnez    t5,sral1
        addi    t5,-1
not_sral1
        jr      ra
        nop

mmc3prg        db       0,1  ; switchable pages
 _align(4)
