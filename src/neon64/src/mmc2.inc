; MMC2

        andi    t4,A6502_regaddr,$f000
        li      t5,$a000
        bne     t5,t4,map9notprgrom
        li      t5,$b000

        _tpa(lbu,t4,prgromsize)
        sll     t4,1    ; 16k->8k
        div     A6502_regval,t4
        mfhi    A6502_regval
        sll     A6502_regval,10+3   ; 8K

        la      t4,prgrom
        addu    A6502_regval,t4
        li      t4,0x8000
        subu    A6502_regval,t4

        li      t4,0x20-1
        la      t5,nespagetable+(0x80*4)
map9setPRGloop
        sw      A6502_regval,(t5)
        addi    t5,4
        bnez    t4,map9setPRGloop
        addi    t4,-1
        jr      A6502_return
        nop

map9notprgrom
        beq     t4,t5,map9chrrom0
        li      t5,$c000
        bne     t4,t5,map9notchrrom0
        li      t5,$d000
map9chrrom0
        
        _tpa(lbu,t5,chrromsize)
        sll     t5,1    ; 8k->4k
        div     A6502_regval,t5
        mfhi    A6502_regval

        _tpa(lbu,t7,prgromsize)
        sll     A6502_regval,12
        la      t4,prog
        sll     t7,14
        addu    t7,t4
        addu    t4,t7,A6502_regval

        ; Pattern table 0
        _tpa(sw,t4,0*8+_vrampages)
        _tpa(sw,t4,1*8+_vrampages)
        _tpa(sw,t4,2*8+_vrampages)
        _tpa(sw,t4,3*8+_vrampages)
        _tpa(cache,25,0*8+_vrampages)
        _tpa(cache,25,1*8+_vrampages)
        _tpa(cache,25,2*8+_vrampages)
        _tpa(cache,25,3*8+_vrampages)

        _tpa(sw,ra,mmc2gfxra)
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x0000
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x0400
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x0800
        move    t5,t4
        jal     gcrequest
        li      t6,0x0c00
        _tpa(lw,ra,mmc2gfxra)

        jr      A6502_return
        nop
        
map9notchrrom0
        bne     t4,t5,map9notchrrom1_fd
        li      t5,$e000
        _tpa(sb,A6502_regval,mmc2_1fd)
        _tpa(sw,ra,writera)
        jal     mmc2latchgfx
        nop
        _tpa(lw,ra,writera)
        jr      A6502_return
        nop
map9notchrrom1_fd
        bne     t4,t5,map9notchrrom1_fe
        nop
        _tpa(sb,A6502_regval,mmc2_1fe)
        _tpa(sw,ra,writera)
        jal     mmc2latchgfx
        nop
        _tpa(lw,ra,writera)
        jr      A6502_return
        nop
map9notchrrom1_fe
        ; mirroring

        andi    A6502_regval,1 ; v/h
        bnez    A6502_regval,hmirrormmc2
        nop

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

        jr      A6502_return
        nop
hmirrormmc2:
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

mmc2latchgfx    ; calculate latch stuff
        _tpa(lbu,t4,mmc2_toggle)
        ;li      t4,0
        _vtpa(lbu,t6,mmc2_1fe,t4)

        _tpa(lbu,t5,chrromsize)
        sll     t5,1    ; 8k->4k
        div     t6,t5
        mfhi    t6

        _tpa(lbu,t7,prgromsize)
        sll     t6,12
        la      t4,prog
        sll     t7,14
        addu    t7,t4
        addu    t4,t7,t6
        addi    t4,-$1000

        ; Pattern table 1 (the adjustible one)
        _tpa(sw,t4,4*8+_vrampages)
        _tpa(sw,t4,5*8+_vrampages)
        _tpa(sw,t4,6*8+_vrampages)
        _tpa(sw,t4,7*8+_vrampages)
        _tpa(cache,25,4*8+_vrampages)
        _tpa(cache,25,5*8+_vrampages)
        _tpa(cache,25,6*8+_vrampages)
        _tpa(cache,25,7*8+_vrampages)

        _tpa(sw,ra,mmc2gfxra)
        addi    t4,0x1000
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x1000
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x1400
        move    t5,t4
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x1800
        move    t5,t4
        jal     gcrequest
        li      t6,0x1c00
        _tpa(lw,ra,mmc2gfxra)
        jr      ra
        nop

; each line check for the addresses that will set the latch
mmc2            _tpa(lw,t0,_PPUControl2) ; Check if graphics are on
                andi    t0,0x18
                beqz    t0,mmc2return
                nop

                ; check this scanline's name table for $fd or $fe

                _tpa(lhu,t6,_VRAM_V) ; address of nt byte
                li      t1,16 ;32   ; tiles per line
                li      t3,$fd  ; 11111101
                li      t4,$fe  ; 11111110
                _tpa(lbu,t5,mmc2_toggle)
                move    t7,t5
mmc2checkscanlineloop
                andi    t0,t6,$0fff
                addi    t0,$2000
                srl     t2,t0,10
                sll     t2,3
                _vtpa(lw,t2,_vrampages,t2)
                addu    t0,t2
fastload
                lbu     t2,(t0)

                beq     t2,t3,mmc2goody
                nop
                bne     t2,t4,mmc2nogoody
                nop
mmc2goody       andi    t5,t2,1
mmc2nogoody
                addi    t1,-1
                beqz    t1,mmc2checkend

                addi    t0,1
                addi    t6,1
                andi    s8,t6,$1f
                bnezl   s8,fastload
                nop
                addi    t6,-1
                j       mmc2checkscanlineloop
                xori    t6,$041f ; swap name table, clear full x bits
mmc2checkend
                ; ignoring sprites

                beq     t7,t5,mmc2return   ; no toggle change
                nop
                _tpa(sb,t5,mmc2_toggle)
                jal     mmc2latchgfx
                nop
                j       mmc2return
                nop
        
mmc2gfxra   dw  0
