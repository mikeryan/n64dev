; MMC1
        _tpa(ld,t4,mmc1instrcnt)
        ; which register to use
        andi    A6502_regaddr,0x6000
        srl     A6502_regaddr,13

        beq     A6502_instrcnt,t4,mmc1end
        nop
        _tpa(sd,A6502_instrcnt,mmc1instrcnt)

        andi    t4,A6502_regval,0x80
        beqz    t4,nommc1reset
        andi    A6502_regval,1

        _tpa(sb,r0,mmc1regcnt)
        _tpa(sb,r0,mmc1regtmp)
        
        _tpa(lbu,t4,mmc1reg)
        ; bit 4 is unaffected, Matt Richey lists bit 0 and 1 as "unknown",
        ;  let's assume they are unaffected too
        ori     t4,0xc
        _tpa(sb,t4,mmc1reg)
        j       mmc1reg0
        nop
nommc1reset

        ; buffering
                
        ;_vtpa(lbu,t4,mmc1regtmp,A6502_regaddr)
        ;_vtpa(lbu,t5,mmc1regcnt,A6502_regaddr)
        _tpa(lbu,t4,mmc1regtmp)
        _tpa(lbu,t5,mmc1regcnt)
        sllv    A6502_regval,t5
        or      t4,A6502_regval
        addi    t5,1
        ;_vtpa(sb,t4,mmc1regtmp,A6502_regaddr)
        ;_vtpa(sb,t5,mmc1regcnt,A6502_regaddr)
        _tpa(sb,t4,mmc1regtmp)
        _tpa(sb,t5,mmc1regcnt)
        li      t6,5
        bne     t5,t6,mmc1end   ; if we've filled the buffer we act
        nop

        ;_vtpa(sb,r0,mmc1regtmp,A6502_regaddr)
        ;_vtpa(sb,r0,mmc1regcnt,A6502_regaddr)
        _tpa(sb,r0,mmc1regtmp)
        _tpa(sb,r0,mmc1regcnt)

        _vtpa(sb,t4,mmc1reg,A6502_regaddr)

        sll     A6502_regaddr,2
        _vtpa(lw,t5,mmc1table,A6502_regaddr)
        nop
        jr      t5
        nop

mmc1reg2
        li      t4,1
        _tpa(sb,t4,mmc14koverride)
        j       mmc1do
        nop
mmc1reg1
        _tpa(lbu,t4,mmc1reg+0)
        _tpa(lbu,t5,mmc14koverride)
        andi    t4,0x10 ;8k/4k
        bnez    t4,mmc1do
        nop
        bnez    t5,mmc1do
        nop
        _tpa(lbu,t4,mmc1reg+1)
        addi    t4,1
        _tpa(sb,t4,mmc1reg+2)
        j   mmc1do
        nop
mmc1reg3
        _tpa(lb,t5,mmc1reg)
        andi    t5,8
        srl     t5,3
        ori     t5,0x1e
        and     t4,t5
        _tpa(lbu,t6,prgromsize)
        div     t4,t6
        nop
        nop
        mfhi    t4
        nop
        _tpa(sb,t4,mmc1prgrom)
mmc1reg0
mmc1do  ; write tables

        ; set up PRGROM

        _tpa(lbu,t4,mmc1reg)
        ; take care of GIANT (512K) ROMs

        _tpa(lbu,t5,prgromsize)
        andi    t5,0x20
        beqz    t5,not512kmmc1
        nop
        _tpa(lbu,t5,mmc1reg+1)
        _tpa(lbu,t6,mmc1prgrom)
        andi    t5,0x10
        andi    t6,$f
        or      t6,t5
        _tpa(sb,t6,mmc1prgrom)
not512kmmc1

        andi    t6,t4,8    ; 32K/16K
        bnez    t6,mmc1_16k
        nop

        _tpa(lbu,t5,mmc1prgrom)

        _tpa(lbu,t6,prgromsize)
        div     t5,t6
        nop
        nop
        mfhi    t5
        nop

        li      t6,0x80-1
        andi    t5,0x1e
        la      t4,0x8000
        j       mmc1_32k
        nop
mmc1_16k
        andi    t4,4    ; $C000/$8000
        bnez    t4,mmc1_16k_8000
        nop
        li      t6,0x40-1

        _tpa(lbu,t4,mmc1prgrom)
        la      t5,prgrom
        andi    t4,$10
        sll     t4,14
        addu    t5,t4   ; 1st/2nd 256K bank affects hardwire, too
        li      t4,0x8000
        subu    t5,t4
        la      t7,nespagetable+(0x80*4)
map1PRGloop8000
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map1PRGloop8000
        addi    t6,-1
        
        la      t4,0xc000

        j       mmc1_16k_c000
        nop

mmc1_16k_8000
        li      t6,0x40-1

        _tpa(lbu,t4,prgromsize)
        _tpa(lbu,t5,mmc1prgrom)
        addi    t4,-1
        andi    t4,$f
        andi    t5,$10  ; 1st/2nd 256K bank affects hardwire, too
        or      t4,t5
        sll     t4,14
        la      t5,prgrom
        addu    t5,t4
        li      t4,0xc000
        subu    t5,t4
        la      t7,nespagetable+(0xc0*4)
map1PRGloopc000
        sw      t5,(t7)
        nop
        addi    t7,4
        bnez    t6,map1PRGloopc000
        addi    t6,-1

        la      t4,0x8000

mmc1_16k_c000
        _tpa(lbu,t5,mmc1prgrom)

        _tpa(lbu,t6,prgromsize)
        div     t5,t6
        nop
        nop
        mfhi    t5

        li      t6,0x40-1
mmc1_32k
        la      t7,prgrom

        sll     t5,14
        add     t5,t7
        subu    t5,t4

        srl     t4,8-2
        la      t7,nespagetable
        addu    t7,t4
map1PRGloop
        sw      t5,(t7)
        addi    t7,4
        bnez    t6,map1PRGloop
        addi    t6,-1

        ; mirroring

        _tpa(lb,t4,mmc1reg)
        nop
        andi    t5,t4,2 ; onescreen/not
        bnez    t5,noonescreen
        nop
        andi    t6,t4,1 ; 0=mirrored from $2000, 1=mirrored from $2400
        la      t5,vram
        sll     t6,10   ; $400
        add     t5,t6
        li      t6,$400
        _tpa(sw,t5,8*8+_VRAMPages)
        _tpa(cache,25,8*8+_vrampages)
        subu    t5,t6
        _tpa(sw,t5,9*8+_VRAMPages)
        _tpa(cache,25,9*8+_vrampages)
        subu    t5,t6
        _tpa(sw,t5,10*8+_VRAMPages)
        _tpa(cache,25,10*8+_vrampages)
        subu    t5,t6
        _tpa(sw,t5,11*8+_VRAMPages)
        _tpa(cache,25,11*8+_vrampages)

        j       aftermirrormmc1
        nop

noonescreen
        andi    t5,t4,1 ; v/h
        bnez    t5,hmirrormmc1
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

        j       aftermirrormmc1
        nop
hmirrormmc1:
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
aftermirrormmc1

        ; set up CHRROM

        _tpa(lbu,t5,chrromsize)
        beqz    t5,mmc1end

        andi    t4,0x10 ;8k/4k
        bnez    t4,mmc14k
        nop

        j   mmc14k
        nop

        _tpa(lbu,t6,mmc1reg+1)

        _tpa(lbu,t5,chrromsize)
        sll     t5,1    ; 8k->4k
        div     t6,t5
        nop
        nop
        mfhi    t6

        _tpa(lbu,t7,prgromsize)
        andi    t6,$1f
        sll     t6,12
        la      t4,prog
        sll     t7,14
        addu    t7,t4
        addu    t4,t7,t6

        ; Pattern table 0
        _tpa(sw,t4,0*8+_vrampages)
        _tpa(sw,t4,1*8+_vrampages)
        _tpa(sw,t4,2*8+_vrampages)
        _tpa(sw,t4,3*8+_vrampages)
        _tpa(cache,25,0*8+_vrampages)
        _tpa(cache,25,1*8+_vrampages)
        _tpa(cache,25,2*8+_vrampages)
        _tpa(cache,25,3*8+_vrampages)

        ; Pattern table 1
        _tpa(sw,t4,4*8+_vrampages)
        _tpa(sw,t4,5*8+_vrampages)
        _tpa(sw,t4,6*8+_vrampages)
        _tpa(sw,t4,7*8+_vrampages)
        _tpa(cache,25,4*8+_vrampages)
        _tpa(cache,25,5*8+_vrampages)
        _tpa(cache,25,6*8+_vrampages)
        _tpa(cache,25,7*8+_vrampages)

        _tpa(sd,ra,writera)
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
        addi    t4,0x0400
        jal     gcrequest
        li      t6,0x0c00
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
        _tpa(ld,ra,writera)

        jr      A6502_return
        nop
mmc14k:
        _tpa(lbu,t6,mmc1reg+1)

        _tpa(lbu,t5,chrromsize)
        sll     t5,1    ; 8k->4k
        div     t6,t5
        nop
        nop
        mfhi    t6

        _tpa(lbu,t7,prgromsize)
        andi    t6,$1f
        sll     t6,12
        la      t4,prog
        sll     t7,14
        addu    t7,t4
        addu    t4,t7,t6

        ; Pattern table 0
        _tpa(sw,t4,0*8+_vrampages)
        _tpa(sw,t4,1*8+_vrampages)
        _tpa(sw,t4,2*8+_vrampages)
        _tpa(sw,t4,3*8+_vrampages)
        _tpa(cache,25,0*8+_vrampages)
        _tpa(cache,25,1*8+_vrampages)
        _tpa(cache,25,2*8+_vrampages)
        _tpa(cache,25,3*8+_vrampages)

        _tpa(sd,ra,writera)
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

        _tpa(lbu,t6,mmc1reg+2)

        _tpa(lbu,t5,chrromsize)
        sll     t5,1    ; 8k->4k
        div     t6,t5
        nop
        nop
        mfhi    t6

        andi    t6,$1f
        sll     t6,12
        addu    t4,t7,t6
        addi    t4,-0x1000

        ; Pattern table 1
        _tpa(sw,t4,4*8+_vrampages)
        _tpa(sw,t4,5*8+_vrampages)
        _tpa(sw,t4,6*8+_vrampages)
        _tpa(sw,t4,7*8+_vrampages)
        _tpa(cache,25,4*8+_vrampages)
        _tpa(cache,25,5*8+_vrampages)
        _tpa(cache,25,6*8+_vrampages)
        _tpa(cache,25,7*8+_vrampages)

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
        _tpa(ld,ra,writera)

afterchrmmc1
        ;li      t4,1
        ;_tpa(sw,t4,vramchanged)
mmc1end
        jr      A6502_return
        nop
 _align(8)
mmc1instrcnt    dw  0,0
