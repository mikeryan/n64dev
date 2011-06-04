; This is designed like a bunch of "if" statements, a jump table would
; probably be more efficient.

Write6502:
        andi    A6502_maskedaddr,A6502_regaddr,0x4007
        andi    t7,A6502_regaddr,0x8000
        bnez    t7,writemapper
        slti    t5,A6502_maskedaddr,8
        beqz    t5,writehigh
        li      A6502_compaddr,0
        bne     A6502_maskedaddr,A6502_compaddr,notwppu1
        li      A6502_compaddr,1

        ; PPU Control Register 1
        
        _tpa(sw,A6502_regval,rspdata+PPUControl1)
        _tpa(sw,A6502_regval,_PPUControl1)

        ; here we change the name table
        _tpa(lw,t4,_VRAM_T)
        andi    t4,~0xC00
        andi    t5,A6502_regval,3
        sll     t5,10
        or      t4,t5
        _tpa(lw,t5,lastppu1)
        _tpa(sw,t4,_VRAM_T)
        
        jr      A6502_return
        nop
notwppu1
        bne     A6502_maskedaddr,A6502_compaddr,notwppu2
        li      A6502_compaddr,3
        
        ; PPU Control Register 2

        _tpa(lw,t4,_PPUControl2)
        andi    t5,t4,1
        andi    t6,A6502_regval,1   ; monochrome bit
        beq     t6,t5,monono        ; has it changed?
        nop
        ; Palette will take care of mono updates to the palette with masking
        ; by itself, but here we have to rewrite the whole palette.

        ; This (unrolled) loop is for both turning on and off monochrome, we
        ; need to decide which mask to use.
        li      t4,0x3f
        beqz    t6,fullcolor
        nop
        li      t4,0x30 ; only bits 4 and 5 used in mono mode.
fullcolor

        monomac($01)
        monomac($02)
        monomac($03)
        monomac($05)
        monomac($06)
        monomac($07)
        monomac($09)
        monomac($0a)
        monomac($0b)
        monomac($0d)
        monomac($0e)
        monomac($0f)
        monomac($11)
        monomac($12)
        monomac($13)
        monomac($15)
        monomac($16)
        monomac($17)
        monomac($19)
        monomac($1a)
        monomac($1b)
        monomac($1d)
        monomac($1e)
        monomac($1f)

        ; fill in the background
        _tpa(lbu,t5,realpal+$00)
        and     t5,t4
        sll     t5,1
        _vtpa(lhu,t5,nes_pal,t5)
        sll     s8,t5,16
        or      t5,s8
        _tpa(sw,t5,bgcolor+rspdata)

        lui     t7,sp_set_palchg>16
        _tpa(sw,t7,sp_base_reg+sp_status_reg)

        _tpa(lw,t4,_PPUControl2)    ; so emphasis has it
monono  ; If I'm not mistaken emphasis should work in mono mode, too.


        ; has emphasis been changed?
        andi    t5,A6502_regval,$e0
        andi    t4,$e0
        beq     t4,t5,changeemphasis_no
        nop

        ; calculate new color filter (unfortunately I don't think this
        ; should affect the full background color, but not a problem in the
        ; few games that use it)
        la      t6,$ffffffff    ; rgba

        andi    t5,A6502_regval,$c0 ; red is decreased
        beqz    t5,emph_notred
        nop
        la      t7,$60000000
        subu    t6,t7
emph_notred
        andi    t5,A6502_regval,$a0 ; green is decreased
        beqz    t5,emph_notgreen
        nop
        la      t7,$00600000
        subu    t6,t7
emph_notgreen
        andi    t5,A6502_regval,$60 ; blue is decreased
        beqz    t5,emph_notblue
        nop
        la      t7,$00006000
        subu    t6,t7
emph_notblue
        _tpa(sw,t6,rspdata+primcolor)
changeemphasis_no

        _tpa(sw,A6502_regval,rspdata+PPUControl2)
        _tpa(sw,A6502_regval,_PPUControl2)
        jr      A6502_return
        nop
notwppu2
        bne     A6502_maskedaddr,A6502_compaddr,notsprramaddr
        li      A6502_compaddr,4

        ; SPRRAM Address register

        _tpa(sw,A6502_regval,sprramaddr)
        jr      A6502_return
        nop
notsprramaddr
        bne     A6502_maskedaddr,A6502_compaddr,notsprram
        li      A6502_compaddr,5
        
        ; SPRRAM write register
        
        _tpa(lw,t4,sprramaddr)
        _vtpa(sb,A6502_regval,_SPRRAM,t4)
        addi    t4,1
        andi    t4,0xff
        _tpa(sw,t4,sprramaddr)

        li      t4,sp_set_sprchg
        _tpa(sw,t4,sp_base_reg+sp_status_reg)

        jr      A6502_return
        nop
notsprram
        bne     A6502_maskedaddr,A6502_compaddr,notvaddr5
        li      A6502_compaddr,6
        
        ; VRAM Address Register #1
        
        ; a write to 2005, as described by loopy
        _tpa(lw,t4,pputoggle)
        _tpa(lw,t5,_VRAM_T)
        bnez    t4,second5write
        andi    t4,A6502_regval,0xf8
        srl     t4,3
        andi    t5,~0x1f
        or      t5,t4
        andi    A6502_regval,7
        _tpa(sw,t5,_VRAM_T)
        _tpa(sw,A6502_regval,_VRAM_X)
        _tpa(cache,25,_VRAM_X)
        li      t4,1
        _tpa(sw,t4,pputoggle)
        jr      A6502_return
        nop
second5write
        sll     t4,2
        andi    t5,~0x73e0
        or      t5,t4
        andi    A6502_regval,7
        sll     A6502_regval,12
        or      t5,A6502_regval
        _tpa(sw,t5,_VRAM_T)
        _tpa(sw,r0,pputoggle)
        jr      A6502_return
        nop
notvaddr5
        bne     A6502_maskedaddr,A6502_compaddr,notvaddr6
        li      A6502_compaddr,7
        
        ; VRAM Address Register #2
        
        ; a write to 2006, as described by loopy
        _tpa(lw,t4,pputoggle)
        _tpa(lw,t5,_VRAM_T)
        bnez    t4,second6write
        nop
        andi    A6502_regval,0x3f
        sll     A6502_regval,8
        andi    t5,0x00ff
        or      t5,A6502_regval
        _tpa(sw,t5,_VRAM_T)
        li      t4,1
        _tpa(sw,t4,pputoggle)
        jr      A6502_return
        nop
second6write
        andi    t5,0xff00
        or      t5,A6502_regval
        _tpa(sw,t5,_VRAM_T)

        _tpa(sh,t5,_VRAM_V)

        _tpa(sw,r0,pputoggle)

        jr      A6502_return
        nop
notvaddr6
        bne     A6502_maskedaddr,A6502_compaddr,notwritevram
        nop     ; no more low regs to write

        ; VRAM I/O Register        
        ; This is a VRAM write

        _tpa(lw,t5,_PPUControl1)
        _tpa(lhu,t4,_VRAM_V)
        andi    t5,4

        bnez    t5,winc32
        addi    t6,t4,32
        addi    t6,-31
winc32:
        _tpa(sh,t6,_VRAM_V)
        andi    t5,t4,0x3fff
        slti    t6,t5,0x3f00
        beqz    t6,writepal
        andi    t6,t4,0x3c00

        _tpa(lbu,t7,chrromsize)
        srl     t6,10-3
        _vtpa(lw,t4,_VRAMPages,t6)
        slti    t6,t5,0x2000
        slti    t7,t7,1
        xori    t7,1    ; if >= 1 CHRROM page
        and     t7,t6
        bnez    t7,notpattern   ; if a write is attempted to CHRROM
        add     t4,t5
        sb      A6502_regval,(t4)

        beqz    t6,notpattern
        nop
        _tpa(sd,ra,writera)
        jal     conebyte
        nop
        _tpa(ld,ra,writera)
notpattern
        jr      A6502_return
        nop
writepal:
        andi    t5,0x1f         ; t5 holds the palette address (mirrored)
        _tpa(lwu,t4,_PPUControl2)
        andi    a6502_regval,0x3f       ; ignore upper two bits
        andi    t4,1
        beqz    t4,monopalno
        sll     t7,a6502_regval,1   ; colors are halfwords
        andi    t7,a6502_regval,0x30    ; mono ignores lower four bits
        sll     t7,1                ; colors are halfwords
monopalno
        ; Lookup in palette to convert to the real color.
        _vtpa(lhu,t4,nes_pal,t7)        ; t4 holds the real color

        andi    t6,t5,$1c
        sll     t6,2
        andi    t7,t5,3
        or      t6,t7
        sll     t6,1

        andi    t7,t5,0x3   ; do not store xparent colors
        beqz    t7,azero
        nop

        _vtpa(sh,t4,_bgpal,t6)
        ; only colors that acutally matter set the pal changed flag
        lui     t7,sp_set_palchg>16
        _tpa(sw,t7,sp_base_reg+sp_status_reg)

        _vtpa(sb,A6502_regval,realpal,t5)   ; even in realpal
azero
        andi    t7,t5,0xf       ; THE zero?
        bnez    t7,notthezero
        nop
        ; all local zeroes (in the real value palette) are set to this one
        _tpa(sb,A6502_regval,realpal+0x0)
        _tpa(sb,A6502_regval,realpal+0x4)
        _tpa(sb,A6502_regval,realpal+0x8)
        _tpa(sb,A6502_regval,realpal+0xc)
        _tpa(sb,A6502_regval,realpal+0x10)
        _tpa(sb,A6502_regval,realpal+0x14)
        _tpa(sb,A6502_regval,realpal+0x18)
        _tpa(sb,A6502_regval,realpal+0x1c)

        ; fill in the background
        sll     s8,t4,16
        or      t4,s8
        _tpa(sw,t4,bgcolor+rspdata)

notthezero
notwritevram
        jr      A6502_return
        nop

writehigh:
        slti    A6502_compaddr,A6502_regaddr,0x6000
        bnez    A6502_compaddr,notsram_offw
        slti    A6502_compaddr,A6502_regaddr,0x4010

        ; mapper 66 uses writes to SRAM
        _tpa(lbu,t4,mapperusessram)
        bnez    t4,writemapper
        nop

        jr      A6502_return    ; write nothing, SRAM is disabled
        nop
notsram_offw

        beqz    A6502_compaddr,not_apu
        li      A6502_compaddr,0x4015

        andi    t4,A6502_regaddr,0xf
        _vtpa(sb,A6502_regval,asq1,t4)
        andi    t4,$c
        srl     t4,2
        li      t5,1
        _vtpa(sb,t5,sq1en,t4)

        andi    t4,A6502_regaddr,3
        xori    t4,3
        bnez    t4,apulength_not
        nop
        andi    t4,A6502_regval,$f8
        srl     t4,3
        _vtpa(lb,t4,lentab,t4)
        andi    t5,A6502_regaddr,$c
        srl     t5,2
        _tpa(lbu,t7,aenable)
        srlv    t7,t5
        andi    t7,1
        bnez    t7,chanen
        nop
        li      t4,0
chanen
        _vtpa(sb,t4,sq1len,t5)
        li      t4,$f
        _vtpa(sb,t4,sq1decvol,t5)
        li      t4,$400B
        bne     t4,A6502_regaddr,trilen_not
        ; on write to $400b halt=1
        li      t4,1
        _tpa(sb,t4,lincnthalt)
trilen_not
        jr      A6502_return
        nop
apulength_not
        andi    t4,A6502_regaddr,$3
        bnez    t4,apuvol_not
        andi    t4,A6502_regval,$10
        beqz    t4,apuvol_not
        andi    t4,A6502_regval,$0f
        andi    t5,A6502_regaddr,$c
        srl     t5,2
        _vtpa(sb,t4,sq1vol,t5)
apuvol_not
        jr      A6502_return
        nop
not_apu

        bne     A6502_compaddr,A6502_regaddr,not_apulen
        nop
        andi    t4,A6502_regval,1
        bnez    t4,enable_sq1
        nop
        _tpa(sb,r0,sq1len)
enable_sq1 andi t4,A6502_regval,2
        bnez t4,enable_sq2
        nop
        _tpa(sb,r0,sq2len)
enable_sq2 andi t4,A6502_regval,4
        bnez t4,enable_tri
        nop
        _tpa(sb,r0,trilen)
enable_tri andi t4,A6502_regval,8
        bnez t4,enable_rnd
        nop
        _tpa(sb,r0,rndlen)
enable_rnd andi t4,A6502_regval,$10
        beqz t4,disable_dmc
        nop
        _tpa(lbu,t4,aenable)
        andi    t5,t4,$10
        bnez    t5,enable_dmc  ; not just on, don't reload stuff
        nop

        ; Reload DMC regs
        _tpa(lbu,t4,dmcadrload)
        _tpa(lbu,t5,dmclenload)
        sll     t4,6
        ori     t4,$4000    ; all accesses should be ORed with $8000
        _tpa(sh,t4,dmcadr)
        sll     t5,4
        addi    t5,1
        _tpa(sh,t5,dmclen)
        _tpa(sb,r0,dmcbitcnt)

        j       enable_dmc
        nop
disable_dmc
        _tpa(sh,r0,dmclen)
enable_dmc
        _tpa(lbu,t4,astatus)
        _tpa(sb,A6502_regval,aenable)
        andi    t4,$7f  ; clear DMA IRQ flag
        _tpa(sb,t4,astatus)
        jr      A6502_return
        nop

not_apulen
        slti    t4,A6502_regaddr,0x4014
        beqz    t4,notDMC
        li      A6502_compaddr,0x4014
        andi    A6502_regaddr,3
        _vtpa(sb,A6502_regval,dmcfreq,A6502_regaddr)
        bnez    A6502_regaddr,dmcirqflag_not
        nop
        andi    A6502_regval,$80
        bnez    A6502_regval,dmcirqflag_not
        nop
        _tpa(lbu,t4,astatus)
        andi    t4,$7f  ; clear IRQ flag
        _tpa(sb,t4,astatus)
dmcirqflag_not
        jr      A6502_return
        nop
notDMC
        bne     A6502_compaddr,A6502_regaddr,notSPRDMA
        li      A6502_compaddr,0x4016
        
        ; SPRRAM DMA                

        addi    A6502_count,-sprdmalatency

        sll     A6502_regval,8
        srl     s8,A6502_regval,6
        addu    s8, A6502_nespage
        lw      s8,(s8)   ; The read table
        addu    A6502_regval,s8

        la      s8,_SPRRAM

        sprdma_ppu($00)
        sprdma_ppu($01)
        sprdma_ppu($02)
        sprdma_ppu($03)
        sprdma_ppu($04)
        sprdma_ppu($05)
        sprdma_ppu($06)
        sprdma_ppu($07)
        sprdma_ppu($08)
        sprdma_ppu($09)
        sprdma_ppu($0a)
        sprdma_ppu($0b)
        sprdma_ppu($0c)
        sprdma_ppu($0d)
        sprdma_ppu($0e)
        sprdma_ppu($0f)

        li      t4,sp_set_sprchg
        _tpa(sw,t4,sp_base_reg+sp_status_reg)

        jr      A6502_return
        nop
notSPRDMA
        bne     A6502_compaddr,A6502_regaddr,wnotcon1
        li      A6502_compaddr,$4017

        _tpa(lw,t4,controlstrobe)
        andi    A6502_regval,1
        bnez    A6502_regval,wnotcon10 ; there is only one strobe
        li      t5,1
        bne     t5,t4,wnotcon10
        nop
        _tpa(sw,r0,controller1cnt)
        _tpa(sw,r0,controller2cnt)
        _tpa(sb,r0,arkcnt)
wnotcon10
        _tpa(sw,A6502_regval,controlstrobe)

        jr      A6502_return
        nop
wnotcon1
        bne     A6502_compaddr,A6502_regaddr,wnotframeirq
        nop
        _tpa(sb,A6502_regval,soundirq)
        jr      A6502_return
        nop
wnotframeirq
        jr      A6502_return
        nop

; Everything past here is a mapper write

writemapper

        _tpa(lbu,t4,mapper)

        sll     t4,2
        _vtpa(lw,t4,mappertable,t4)
        jr      t4
        nop

map1
#include mmc1.inc

map2
        _tpa(lbu,t5,prgromsize)
        div     A6502_regval,t5
        nop
        nop
        mfhi    A6502_regval
        nop
        
        sll     A6502_regval,10+4 ; 16K
        la      t5,prgrom
        addu    t5,A6502_regval
        li      t4,0x8000
        subu    t5,t4

        li      t6,0x40-1
        la      t7,nespagetable+(0x80*4)
map2PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map2PRGloop
        addi    t6,-1
        
        jr      A6502_return
        nop

map3
        _tpa(lbu,t5,chrromsize)
        div     A6502_regval,t5
        nop
        nop
        mfhi    t5
        nop

        _tpa(sw,ra,writera)
        jal     vrom8
        nop
        _tpa(lw,ra,writera)

        jr      A6502_return
        nop
map4
#include mmc3.inc
map7
        ;_tpa(ld,t4,mmc1instrcnt)
        ;beq     t4,A6502_instrcnt,map7end
        ;nop
        ;_tpa(sd,A6502_instrcnt,mmc1instrcnt)

        andi    t4,A6502_regval,0x0f
        sll     t4,15
        la      t5,prgrom-0x8000
        addu    t5,t4

        li      t6,0x80-1
        la      t7,nespagetable+(0x80*4)
map7PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map7PRGloop
        addi    t6,-1

        andi    A6502_regval,0x10
map7_onescreen
        bnez    A6502_regval,map7_2400
        nop

        ; One-screen mirroring (0x2000)
        la      t4,vram
        _tpa(sw,t4,8*8+_VRAMPages)
        _tpa(cache,25,8*8+_VRAMPages)
        la      t4,vram-0x400
        _tpa(sw,t4,9*8+_VRAMPages)
        _tpa(cache,25,9*8+_VRAMPages)
        la      t4,vram-0x800
        _tpa(sw,t4,10*8+_VRAMPages)
        _tpa(cache,25,10*8+_VRAMPages)
        la      t4,vram-0xc00
        _tpa(sw,t4,11*8+_VRAMPages)
        _tpa(cache,25,11*8+_VRAMPages)
        jr      A6502_return
        nop

map7_2400

        ; One-screen mirroring (0x2400)
        la      t4,vram+0x400
        _tpa(sw,t4,8*8+_VRAMPages)
        _tpa(cache,25,8*8+_VRAMPages)
        la      t4,vram
        _tpa(sw,t4,9*8+_VRAMPages)
        _tpa(cache,25,9*8+_VRAMPages)
        la      t4,vram-0x400
        _tpa(sw,t4,10*8+_VRAMPages)
        _tpa(cache,25,10*8+_VRAMPages)
        la      t4,vram-0x800
        _tpa(sw,t4,11*8+_VRAMPages)
        _tpa(cache,25,11*8+_VRAMPages)
map7end
        jr      A6502_return
        nop

map9
#include mmc2.inc

map11
        _tpa(lbu,t5,prgromsize)
        andi    t4,A6502_regval,0x0f
        sll     t4,1
        div     t4,t5
        mfhi    t4

        sll     t4,14
        la      t5,prgrom-0x8000
        addu    t5,t4

        li      t6,0x80-1
        la      t7,nespagetable+(0x80*4)
map11PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map11PRGloop
        addi    t6,-1

        andi    A6502_regval,0xf0
        srl     A6502_regval,4

        _tpa(lbu,t4,prgromsize)
        la      t5,prog
        sll     t4,14
        add     t4,t5
        _tpa(lbu,t5,chrromsize)
        div     A6502_regval,t5
        mfhi    t5
        sll     t5,13
        add     t4,t5

        ; Pattern table 0
        _tpa(sw,t4,0*8+_vrampages)
        _tpa(cache,25,0*8+_vrampages)
        _tpa(sw,t4,1*8+_vrampages)
        _tpa(cache,25,1*8+_vrampages)
        _tpa(sw,t4,2*8+_vrampages)
        _tpa(cache,25,2*8+_vrampages)
        _tpa(sw,t4,3*8+_vrampages)
        _tpa(cache,25,3*8+_vrampages)
                
        ; Pattern table 1
        _tpa(sw,t4,4*8+_vrampages)
        _tpa(cache,25,4*8+_vrampages)
        _tpa(sw,t4,5*8+_vrampages)
        _tpa(cache,25,5*8+_vrampages)
        _tpa(sw,t4,6*8+_vrampages)
        _tpa(cache,25,6*8+_vrampages)
        _tpa(sw,t4,7*8+_vrampages)
        _tpa(cache,25,7*8+_vrampages)

        _tpa(sd,ra,writera)
        addiu   t5,t4,0x0000
        jal     gcrequest
        li      t6,0x0000
        addiu   t5,t4,0x0400
        jal     gcrequest
        li      t6,0x0400
        addiu   t5,t4,0x0800
        jal     gcrequest
        li      t6,0x0800
        addiu   t5,t4,0x0c00
        jal     gcrequest
        li      t6,0x0c00
        addiu   t5,t4,0x1000
        jal     gcrequest
        li      t6,0x1000
        addiu   t5,t4,0x1400
        jal     gcrequest
        li      t6,0x1400
        addiu   t5,t4,0x1800
        jal     gcrequest
        li      t6,0x1800
        addiu   t5,t4,0x1c00
        jal     gcrequest
        li      t6,0x1c00
        _tpa(ld,ra,writera)

        jr      A6502_return
        nop
map34   _tpa(lbu,t5,prgromsize)
        srl     t5,1
        div     A6502_regval,t5
        nop
        nop
        mfhi    A6502_regval
        nop
        
        sll     A6502_regval,10+5 ; 32K
        la      t5,prgrom
        addu    t5,A6502_regval
        li      t4,0x8000
        subu    t5,t4

        li      t6,0x80-1
        la      t7,nespagetable+(0x80*4)
map34PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map34PRGloop
        addi    t6,-1

        jr      A6502_return
        nop

map66
        jr      A6502_return
        nop
map71
        andi    t4,A6502_regaddr,$c000
        xori    t4,$c000
        bnez    t4,map71notROMswitch
        nop

        _tpa(lbu,t5,prgromsize)
        div     A6502_regval,t5
        mfhi    t4

        sll     t4,14
        la      t5,prgrom-0x8000
        addu    t5,t4

        li      t6,0x40-1
        la      t7,nespagetable+(0x80*4)
map71PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map71PRGloop
        addi    t6,-1

        jr      A6502_return
        nop

map71notROMswitch
        li      t5,$9000
        andi    A6502_regaddr,$f000
        bne     A6502_regaddr,t5,map71end
        nop
        andi    A6502_regval,$10
        j       map7_onescreen  ; let map7 take care of it
        nop
map71end
        jr      A6502_return
        nop

map0
        jr      A6502_return
        nop

; page # in t5
vrom8   _tpa(lbu,t4,prgromsize)
        la      t6,prog
        sll     t4,14
        add     t4,t6

        sll     t5,13
        add     t4,t5

        ; Pattern table 0
        _tpa(sw,t4,0*8+_vrampages)
        _tpa(cache,25,0*8+_vrampages)
        _tpa(sw,t4,1*8+_vrampages)
        _tpa(cache,25,1*8+_vrampages)
        _tpa(sw,t4,2*8+_vrampages)
        _tpa(cache,25,2*8+_vrampages)
        _tpa(sw,t4,3*8+_vrampages)
        _tpa(cache,25,3*8+_vrampages)
                
        ; Pattern table 1
        _tpa(sw,t4,4*8+_vrampages)
        _tpa(cache,25,4*8+_vrampages)
        _tpa(sw,t4,5*8+_vrampages)
        _tpa(cache,25,5*8+_vrampages)
        _tpa(sw,t4,6*8+_vrampages)
        _tpa(cache,25,6*8+_vrampages)
        _tpa(sw,t4,7*8+_vrampages)
        _tpa(cache,25,7*8+_vrampages)

        _tpa(sw,ra,vrom8ra)
        addiu   t5,t4,0x0000
        jal     gcrequest
        li      t6,0x0000
        addiu   t5,t4,0x0400
        jal     gcrequest
        li      t6,0x0400
        addiu   t5,t4,0x0800
        jal     gcrequest
        li      t6,0x0800
        addiu   t5,t4,0x0c00
        jal     gcrequest
        li      t6,0x0c00
        addiu   t5,t4,0x1000
        jal     gcrequest
        li      t6,0x1000
        addiu   t5,t4,0x1400
        jal     gcrequest
        li      t6,0x1400
        addiu   t5,t4,0x1800
        jal     gcrequest
        li      t6,0x1800
        addiu   t5,t4,0x1c00
        jal     gcrequest
        li      t6,0x1c00
        _tpa(lw,ra,vrom8ra)
        jr      ra
        nop

vrom8ra dw  0
 _align(8)
writera         dw      0,0
mappertable     dw      map0,map1,map2,map3,map4,0,0,map7
                dw      0,map9,0,map11,0,0,0,0
                dcw     8,0
                dcw     8,0
                dw      0,0,map34,0,0,0,0,0
                dcw     8,0
                dcw     8,0
                dcw     8,0
                dw      0,0,map66,0,0,0,0,map71

mmc1table       dw      mmc1reg0,mmc1reg1,mmc1reg2,mmc1reg3
mmc3command     db      0
mmc3reg0        db      0
mmc3reg1        db      1
 _align(4)

; Light speed is too slow.
