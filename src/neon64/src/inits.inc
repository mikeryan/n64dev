; Neon64 initialization & exception handling routines

; What is the difference between a file with a .INC extension and one with
; .ASM? .INC is supposed to suggest that the file isn't a self-contained
; program. This is not a rule, and I don't even pay much attention to it, but
; that is supposed to be the idea.

#include defs.inc

; mbu.bat or mgs.bat will copy backup.inc or gs.inc to hardware.inc to
; ensure the proper version is built.
#include hardware.inc

start:

    mtc0    r0,watchhi  ; otherwise gs tries to trap my writes
    mtc0    r0,watchlo

    beq r0,r0,initPI
    nop
initPIreturn

    ; disable interrupts
    mfc0    t0,status
    move    sp,t0
    nop
    andi    t1,t0,$ff00 ; unmask all interrupts
    xor     t0,t1
    mtc0    t0,status

    beq r0,r0,loadcode
    nop

destination ; absolute target for loadcode (GS ver)

     jal     cls
     nop

     jal    setupVIhires
     nop

                _tpa(sw,r0,text_x)
                la      t0,screenbuffer|$A0000000
                _tpa(sw,t0,text_scr)
                _tpa(sw,r0,text_type)

                la      t0,logo
                li      t1,8
                _tpa(sw,t0,text_msg)
                _tpa(sw,t1,text_y)
                jal     text_blit
                nop

                li      t0,2            ; on boot we halt the RSP
                _tpa(sw,t0,0xa4040010)
                li      k1,1|4|$10|$40|$80|$100|$200 ; on boot we halt the RDP
                _tpa(sw,k1,0xa410000C)

SPdeviceFULL    _tpa(lw,t0,$a4040014)
                bnez    t0,SPdeviceFULL
                nop
        
                ; Load the PPU data (mostly the page table)
                la      a0,0x0
                la      a1,rspdataa
                li      a2,0x1000-8
                lui     t0,$a404
                sw      a0,0(t0)        ; a0=rspdest
                sw      a1,4(t0)        ; a1=dram
                sw      a2,8(t0)        ; a2=size

SPdeviceFULL1   _tpa(lw,t0,$a4040014)
                bnez    t0,SPdeviceFULL1
                nop
                
                ; Load the PPU microcode
                la      a0,0x1000
                la      a1,rsptext
                li      a2,0x1000-8
                lui     t0,$a404
                sw      a0,0(t0)        ; a0=rspdest
                sw      a1,4(t0)        ; a1=dram
                sw      a2,8(t0)        ; a2=size

SPdeviceBUSY
                _tpa(lw,t0,$a4040018)
                bnez    t0,SPdeviceBUSY
                nop

                ; Setup interrupts

                ; At each exception vector will be a small
                ; piece of code that jumps to the main exception handler.
                la      a0,intvector
                la      a1,0xa0000000
                la      a2,4-1    ; 4 words
storeloop       lw      a3,0(a0)
                addiu   a0,4
                sw      a3,0(a1)
                sw      a3,0x80(a1)
                sw      a3,0x100(a1)
                sw      a3,generalexceptvec(a1)
                sync
                cache   $10,0(a1)
                cache   $10,0x80(a1)
                cache   $10,0x100(a1)
                cache   $10,generalexceptvec(a1)
                addiu   a1,4
                bnez    a2,storeloop
                addi    a2,-1

                ; Unmask any interrupts in status and MIPS here.
                _tpa(sw,r0,0xa4500008)  ; disable AI DMA
                _tpa(sw,r0,0xa450000c)  ; clear AI interrupt
                _tpa(sw,r0,0xa4400010)  ; clear VI int line

                li      t0,$5a5  ; unmask VI,AI interrupt clear others
                _tpa(sw,t0,0xa430000c)

                mfc0    t0,status
                nop
                nop
                ori     t0,0x1401       ; reset button, MIPS
                mtc0    t0,status
                nop

                j       pastintstuff
                nop
                
; As long as k1 is never used in the rest of the program, this will work
intvector       la      k1,inthandler
                jr      k1
                nop

inthandler      la      k1,s8save
                sd      s8,(k1)
                mfc0    k1,status
                _tpa(sw,k1,statussave)
                andi    k1,~1 ; disable interrupts
                mtc0    k1,status
                nop

                mfc0    k1,cause
                andi    s8,k1,0xff ; msb and 2 lsbs are 0
                beqz    s8,justaninterrupt
                nop
                nop
                ; exception handler: print processor status
                _tpa(sw,ra,rasave)
                jal     unrecoverable_exception
                nop
                deadend
justaninterrupt:
                
                ; Any interrupt handler would go here.
                
                andi    s8,k1,0x1000    ; "pre-NMI"
                beqz    s8,notreset
                nop

                ; RESET!

                li      k1,2            ; on reset we halt the RSP
                _tpa(sw,k1,0xa4040010)  ; so we can actually reset
                li      k1,1|4|$10|$40|$80|$100|$200 ; on reset we halt the RDP
                _tpa(sw,k1,0xa410000C)  ; so we can actually reset

                ; A cool little per-scanline effect while waiting for reset,
                ;  not really supposed to look like anything.
                ; This level of messing with video hardware brought to you
                ;  courtesy of the NES.
                _tpa(lw,t2,0xa4400030)
                li      t3,0
resetlooptop    move    t4,t3
                addi    t3,12
                _tpa(sw,r0,0xa4400010)  ; clear int
resetloop       _tpa(lw,t0,0xa4400010)
resetwaitloop   _tpa(lw,t1,0xa4400010)
                beq     t0,t1,resetwaitloop
                nop
                beqz    t0,resetlooptop
                nop

                sub     t5,t2,t4
                _tpa(sw,t5,0xa4400030)

                beqz    t4,resetloop
                nop

                j       resetloop
                addi    t4,-1

notreset:
                andi    s8,k1,$8000
                beqz    s8,notcount
                nop

                _tpa(lb,k1,frametoggle)
                beqz    k1,framerate_off2
                nop
                mfhi    k1
                _tpa(sw,k1,savehi)
                mflo    k1
                _tpa(sw,k1,savelo)

                _tpa(lw,k1,framecount)
                li      s8,fpsrate
                mult    k1,s8
                mflo    k1
                _tpa(sw,k1,framerate)
                _tpa(sw,r0,framecount)

                _tpa(lw,k1,savehi)
                mthi    k1
                _tpa(lw,k1,savelo)
                mtlo    k1
framerate_off2

                mtc0    r0,count
                la      k1,50000000/fpsrate
                mtc0    k1,compare  ; has the "side effect" of clearing int

                j       endint
                nop
notcount:

                _tpa(lw,k1,0xa4300008)
                andi    k1,1    ; SP intr
                beqz    k1,notsp
                nop

                _text_string(22,22,rspval)
                _text_string(22,30,pcval)
                _tpa(lw,k1,0xa4080000)
                _text_value(8*5+22,30,k1,7)

                deadend

rspval          db  "SP Broke!",0
pcval           db  "PC: ",0
 _align(8)
ait0save        dw      0,0
airasave        dw      0,0
notsp
                _tpa(lw,k1,0xa4300008)
                andi    k1,4    ; AI intr
                beqz    k1,notai
                nop
                _tpa(sd,t0,ait0save)
                _tpa(sd,ra,airasave)
                jal     aihandler
                nop
                _tpa(ld,ra,airasave)
                _tpa(ld,t0,ait0save)
                j   endint
                nop

notai
                _tpa(lw,k1,0xa4300008)
                andi    k1,8    ; VI intr
                beqz    k1,notvi
                nop
                _tpa(sw,r0,0xa4400010)  ; clear VI int line
                _tpa(lwu,k1,debugflag)
                bnez    k1,endint
                nop
                _tpa(lw,k1,scrbuf)
                _tpa(sw,k1,0xa4400004)

                ; draw FPS meter to now-static screen
                _tpa(lb,k1,frametoggle)
                beqz    k1,framerate_off
                nop
                mfhi    k1
                _tpa(sw,k1,savehi)
                mflo    k1
                _tpa(sw,k1,savelo)
                _tpa(lwu,k1,framerate)
                _text_value(20,220,k1,1)
                _tpa(lw,k1,savehi)
                mthi    k1
                _tpa(lw,k1,savelo)
                mtlo    k1
framerate_off

                j   endint
                nop
notvi

endint          _tpa(lw,k1,statussave)
                mtc0    k1,status
                la      k1,s8save
                ld      s8,(k1)
                eret

cls
                la      t1, screenbuffer+(280*2*256*3)
                la      t0, screenbuffer
                li      t2, $0001
fill_screen_loop:
                sh      t2, (t0)
                addi    t0, 2
                bne     t1, t0, fill_screen_loop
                nop
                jr      ra
                nop

setupVIhires
    ; Set up VI (512x240x16bit) (high res horizontal, nice aspect ratio)
    lui     t0, 0xa440

    _tpa(lbu,t1,VIvidmode)
    nop
    bnez    t1,setupVIhiresPAL
    nop

    ; Control
    la      t1, 0x00013002  ; 16 bit/pixel
    sw      t1, 0(t0)
    ; framebuffer
    la      t1, screenbuffer
    sw      t1, 4(t0)
    _tpa(sw,t1,scrbuf)
    ; width
    li      t1, 0x280
    sw      t1, 8(t0)
    ; vertical interrupt
    li      t1, 0
    sw      t1, 12(t0)
    ; current line
    li      t1, 0
    sw      t1, 16(t0)
    ; video timing
    la      t1, 0x03e52227
    sw      t1, 20(t0)
    ; v_sync
    li      t1, 0x020d
    sw      t1, 24(t0)
    ; h_sync
    li      t1, 0x0c15
    sw      t1, 28(t0)
    ; h_sync again (twice)
    sll     t2, t1, 16
    or      t1, t1, t2
    sw      t1, 32(t0)
    ; h screen limits
    la      t1, 0x007602f4
    sw      t1, 36(t0)
    ; v screen limits
    la      t1, 0x001f01ff
    sw      t1, 40(t0)
    ; "color burst"
    la      t1, 0x000e0204
    sw      t1, 44(t0)
    ; h scale-up factor
    li      t1, 0x19f*$28/$10
    sw      t1, 48(t0)
    ; v scale-up factor
    li      t1, 0x400
    sw      t1, 52(t0)

    jr      ra
    nop

setupVIhiresPAL

    ; Control
    la      t1, 0x00013002  ; 16 bit/pixel
    sw      t1, 0(t0)
    ; framebuffer
    la      t1, screenbuffer
    sw      t1, 4(t0)
    _tpa(sw,t1,scrbuf)
    ; width
    li      t1, 0x318
    sw      t1, 8(t0)
    ; vertical interrupt
    li      t1, 0
    sw      t1, 12(t0)
    ; current line
    li      t1, 0
    sw      t1, 16(t0)
    ; video timing
    la      t1, 0x4541E3A
    sw      t1, 20(t0)
    ; v_sync
    li      t1, 0x271
    sw      t1, 24(t0)
    ; h_sync
    la      t1, 0x170c69
    sw      t1, 28(t0)
    ; h_sync again (twice)
    la      t1,0xc6f0c6d
    sw      t1, 32(t0)
    ; h screen limits
    la      t1, 0x009802f4
    sw      t1, 36(t0)
    ; v screen limits
    la      t1, 0x00220250
    sw      t1, 40(t0)
    ; "color burst"
    la      t1, 0x9026b
    sw      t1, 44(t0)
    ; h scale-up factor
    li      t1, 0x19f*$28/$10
    sw      t1, 48(t0)
    ; v scale-up factor
    li      t1, 0x350
    sw      t1, 52(t0)

    jr      ra
    nop

setupVINESres

                ; Set up VI (280x240x16bit)
                lui     t0,$a440

    _tpa(lbu,t1,VIvidmode)
    bnez    t1,setupVINESresPAL
    nop

                ; width
                li      t1, 0x118
                sw      t1, 8(t0)
                ; video timing
                la      t1, 0x03e52227
                sw      t1, 20(t0)
                ; v_sync
                li      t1, 0x020d
                sw      t1, 24(t0)
                ; h_sync
                li      t1, 0x0c15
                sw      t1, 28(t0)
                ; h_sync again (twice)
                sll     t2, t1, 16
                or      t1, t1, t2
                sw      t1, 32(t0)
                ; h screen limits
                la      t1, 0x007602f4
                sw      t1, 36(t0)
                ; v screen limits
                la      t1, 0x001f01ff
                sw      t1, 40(t0)
                ; "color burst"
                la      t1, 0x000e0204
                sw      t1, 44(t0)
                ; h scale-up factor
                li      t1, 0x1cc
                sw      t1, 48(t0)
                ; v scale-up factor
                li      t1, 0x400
                sw      t1, 52(t0)

                jr      ra
                nop

setupVINESresPAL:
            ; width
            li      t1, 0x118
            sw      t1, 8(t0)
    ; video timing
    la      t1, 0x4541E3A
    sw      t1, 20(t0)
    ; v_sync
    li      t1, 0x271
    sw      t1, 24(t0)
    ; h_sync
    la      t1, 0x170c69
    sw      t1, 28(t0)
    ; h_sync again (twice)
    la      t1,0xc6f0c6d
    sw      t1, 32(t0)
    ; h screen limits
    la      t1, 0x007202f4
    sw      t1, 36(t0)
    ; v screen limits
    la      t1, 0x001f0250
    sw      t1, 40(t0)
    ; "color burst"
    la      t1, 0x9026b
    sw      t1, 44(t0)
                ; h scale-up factor
                li      t1, 0x1aa
                sw      t1, 48(t0)
                ; v scale-up factor
                li      t1, 0x350
                sw      t1, 52(t0)

                jr      ra
                nop


pastintstuff:   _tpa(lbu,t0,resetflag)
                _tpa(sb,r0,resetflag)
                bnez    t0,nowait
                nop
                
                jal     Pause   ; wait for the user to load the ROM
                nop

                ; (button value is returned in t0)
                andi    t0,8 ; c-up
                beqz    t0, nowait
                nop
                _tpa(lbu,t0,VIvidmode)
                nop
                xori    t0,1
                _tpa(sb,t0,VIvidmode)
                j        destination
                nop
nowait
                ;li      t0,$40
                ;_tpa(sw,t0,0xa430000c)  ; disable VI interrupt

                jal detatchGS
                nop

    mtc0    r0,count
    la      t0,50000000/fpsrate
    mtc0    t0,compare
    
    mfc0    t0,status   ; activate count
    ori     t0,$8000
    mtc0    t0,status

                jal     cls
                nop

                jal     setupVINESres
                nop

                ; initialize variables to zero
                la      t0,lastvar-firstvar-1
                la      t1,firstvar
memclearloop    sb      r0,(t1)
                addi    t1,1
                bnez    t0,memclearloop
                addi    t0,-1

                la      t0,lastvar&$8fffffff-mem-1
                la      t1,mem
memclearloopc   sb      r0,(t1)
                addi    t1,1
                bnez    t0,memclearloopc
                addi    t0,-1

                ; needs to be init'd, or mapper errors get written over code!
                la      t0,screenbuffer
                _tpa(sw,t0,scrbuf)
    
                ; load default nespages
                la      t0,npt_init
                li      t1,0x800-1
                la      t3,nespagetable
nespageload     lbu     t2,(t0)
                addi    t0,1
                sb      t2,(t3)
                addi    t3,1
                bnez    t1,nespageload
                addi    t1,-1

                ; load VRAM table
                la      t2,_VRAMPages
                la      t0,VRAMPages+rspdataa
                li      t1,8*26-1   ; include VRAM_V,VRAM_X,VRAMpages,and GCpages

invloop         lbu     t3,(t0)
                addi    t0,1
                sb      t3,(t2)
                addi    t2,1
                bnez    t1,invloop
                addi    t1,-1

                ; init some mappers that need it (only loadnewrom)
                _tpa(sb,r0,mmc3command)
                _tpa(sb,r0,mmc3reg0)
                li      t0,1
                _tpa(sb,t0,mmc3reg1)
                la      t0,$00010000
                _tpa(sw,t0,mmc3prg) ; 0x00,0x01,0x00,0x00

                _tpa(lbu,t0,traperrors)
                _tpa(sb,t0,traperrors2)

                ; load tables for sound generation
                la      t0,atables
                la      t1,atablesend
                la      t2,lentab
atableload      lw      t3,(t0)
                addi    t0,4
                sw      t3,(t2)
                bne     t0,t1,atableload
                addi    t2,4

                ; load table for gfx compiler
                la      t0,pattabl
                la      t1,pattabl+1024
                la      t2,pattab
ptload          lw      t3,(t0)
                addi    t0,4
                sw      t3,(t2)
                bne     t0,t1,ptload
                addi    t2,4


                ; set for whatever video mode we're in (PAL/NTSC)
                jal     setscreenlimits
                nop
