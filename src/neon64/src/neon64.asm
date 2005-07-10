; Neon64: An NES emulator for the N64
; (c) 2001-2004 Halley's Comet Software
; Brought to you by themind (halleyscometsoftware@hotmail.com)
; Certain elements derived from M6502, FCEU, and Nester.
; Written for Halley's Comet Software's U64ASM assembler.

        org     0x80000400
#imacros macros.inc
                beq r0,r0,start ; not absolute, in inits.inc
                nop
#include codes.h
#include sound.asm
#include a6502.asm
#include gcompile.inc
#include read.inc
#include write.inc

backtotop
                ; initialize pattern cache age
                la      t0,metacache
                li      t1,7
                li      t2,1
ageinit         sw      t2,(t0)
                addi    t0,8
                bnez    t1,ageinit
                addi    t1,-1

                ; Load the ROM
                jal     loadROM
                nop

                ; Initialize pointers that are in registers
                la      A6502_nespage,nespagetable
                la      A6502_mempage,mem
                la      A6502_readfcn,Read6502
                la      A6502_writefcn,Write6502
                la      A6502_instrcnt,1
                              
                li      A6502_count,0   ; Initialize periodic count.

                jal     Init6502        ; Reset the 6502.
                nop

                jal     initaudio
                nop

                ;la      t0,_VRAMpages&0x3ffffff8|1
                ;mtc0    t0,watchlo
                ;mtc0    r0,watchhi

                ;la      t0,pchcnt
                _tpa(sw,r0,pchcnt)

                jal     BootRSP         ; A. Boot RSP.
                nop

                ; two dummy frames
                jal     Run6502
                addi    A6502_count,cyclesperscanline+cyclesperhretrace*262*2

; BEGINNING OF ONCE PER FRAME LOOP
frameloop:      li      A6502_count,0   ; reset periodic count each frame

                _tpa(lb,t0,killframe)
                _tpa(lwu,t2,framecount)
                _tpa(lb,t3,fbfflag)
                la      t1,sp_clr_nogfx
                beqzl   t0,nokillframe
                addi    t2,1
                bnez    t3,nokillframe
                nop
                la      t1,sp_set_nogfx
nokillframe     _tpa(sw,t1,0xa4040010)
                _tpa(sw,t2,framecount)

                ; F. 1 idle scanline (for the PPU)
                
                jal     Run6502         ; 1. Do a scanline of CPU cycles.
                addi    A6502_count,cyclesperscanline+cyclesperhretrace

                _tpa(lbu,t0,ppustatus)
                ori     t0,$80
                ;andi    t0,~$60
                _tpa(sb,t0,ppustatus)

                ; G. Vertical Retrace (VINT) period

                li      t0,1
                _tpa(sb,t0,invblank)

                jal     alendec
                nop

                ; a few cycles before NMI
                jal     Run6502
                addi    A6502_count,cyclesbeforenmi

                _tpa(lwu,t1,_PPUControl1)
                andi    t1,$80
                beqz    t1,notvblankinterrupt
                nop
                jal     NMI6502         ; 2. Trigger NMI if enabled
                nop
notvblankinterrupt

                ; a partial line (some cycles taken out above)

                jal     lineofsound
                nop
                jal     Run6502
                addi    A6502_count,cyclesperscanline+cyclesperhretrace-cyclesbeforenmi

            _tpa(lbu,t0,vidmode)
            beqz    t0,ntscmode
            li      t0,20-2
            li      t0,70-2
ntscmode
vretraceloop                
                _tpa(sw,t0,scanleft)

                jal     lineofsound
                nop
                jal     Run6502
                addi    A6502_count,cyclesperscanline+cyclesperhretrace

                _tpa(lw,t0,scanleft)
                bnez    t0,vretraceloop
                addi    t0,-1

                ; clear PPU status
                _tpa(sb,r0,ppustatus)

                _tpa(sb,r0,invblank)

                ; B. Junk scanline for loading line 0 sprites

                rsp_go                  ; 1. Tell RSP to do a scanline.

                jal     Run6502         ; 2. Do a scanline of CPU cycles.
                addi    A6502_count,cyclesperscanline

                rsp_done_wait           ; 3. Wait for RSP to finish.

                jal     Run6502         ; 4. Do an hblank of CPU cycles.
                addi    A6502_count,cyclesperhretrace

                ; Da Cheat!
                _tpa(lbu,t0,mapper)
                li      t1,9
                bne     t0,t1,mmc2not_1
                nop
                li      t0,1
                _tpa(sb,t0,mmc2_toggle)
                jal     mmc2latchgfx
                nop
mmc2not_1

                _tpa(lw,t1,_PPUControl2) ; 4. If screen or sprites active v=t
                _tpa(lw,t0,_VRAM_T)

                andi    t1,0x18
                beqz    t1,notvt
                nop

                _tpa(sh,t0,_VRAM_V)
notvt:

                ; C. Do 240 scanlines, each consisting of:
                li      t0,240
                _tpa(sw,t0,scanleft)
                _tpa(sw,r0,scanline)

outer_loop
                _tpa(lbu,t0,mapper)
                li      t1,9
                beq     t0,t1,mmc2
                nop
mmc2return

                ; check if sp0 could be set this line
                ; If so fiddle timing so it occurs just when it should on
                ; the line (actually only where the sprite starts...)
                _tpa(lwu,t2,_PPUControl1)
                _tpa(lbu,t0,_SPRRAM)
                _tpa(lwu,t1,scanline)
                addi    t0,1
                andi    t2,0x20
                srl     t2,2
                addi    t2,8
                sub     t1,t0
                slti    t3,t1,0
                slt     t4,t1,t2
                xori    t3,1
                and     t3,t4
                bnez    t3,checksp0
                nop
forgetsp0

                rsp_go                  ; 1. Tell RSP to do a scanline.

                jal     lineofsound
                nop

                jal     Run6502         ; 2. Do a scanline of CPU cycles.
                addi    A6502_count,cyclesperscanline-cyclesafterfixit

                rsp_done_wait

                _tpa(lw,t0,sp_base_reg+sp_status_reg)
                srl     t0,7
                andi    t0,0x60   ; sp0 hit and 8-sprite
                _tpa(sb,t0,ppustatus)

returnfromsp0

                ; ii. if screen or sprites are on, do fixit.
                ; (this specific interpretation of loopy's observation was
                ;  found in FCEU).
                ; This increments the v so it points at the start of the
                ; next line.

                _tpa(lw,t0,_PPUControl2) ; Check if graphics are on
                andi    t0,0x18
                beqz    t0,notfixit
                nop
                _tpa(lhu,t1,_VRAM_V)
                ;andi    t1,0x041f
                ;andi    t2,0xfbe0
                ;or      t1,t2
                andi    t0,t1,0x7000    ; if fine y scroll=7, we'll
                li      t2,0x7000       ; need to increment the coarse scroll
                bne     t0,t2,fineyinc
                andi    t0,t1,0x03e0    ; check coarse y scroll
                xori    t1,0x7000       ; since they're set, this clears them
                li      t2,0x03a0
                bne     t0,t2,checkotheryscroll ; if y scroll=29
                li      t2,0x03e0
                andi    t1,~0x03e0
                j       storevramv
                xori    t1,0x0800               ; coarsey=0, flip y nt bit
checkotheryscroll bne   t0,t2,coarseyinc        ; if y scroll=31
                nop
                j       storevramv
                xori    t1,0x03e0       ; reset y scroll to 0, not flip y nt
coarseyinc      j       storevramv
                addi    t1,0x0020       ; just increment coarse y scroll
fineyinc        addi    t1,0x1000       ; just increment fine y scroll
storevramv      _tpa(sh,t1,_VRAM_V)
                ;_tpa(lw,t0,_VRAM_X)
                ;_tpa(sw,t0,rspdata+VRAM_X)

notfixit:

                jal     Run6502
                addi    A6502_count,cyclesafterfixit

                _tpa(lbu,t1,mapper)
                li      t0,4
                beq     t0,t1,mmc3irq
                nop
mmc3return

                _tpa(lw,t0,_PPUControl2) ; Check if graphics are on
                andi    t0,0x18
                beqz    t0,notloadv
                nop

                _tpa(lwu,t0,_VRAM_T)
                _tpa(lhu,t1,_VRAM_V)
                andi    t0,0x041f
                andi    t1,0xfbe0
                or      t1,t0
                _tpa(sh,t1,_VRAM_V)
notloadv

                ; 4. Do an hblank
                jal     Run6502
                addi    A6502_count,cyclesperhretrace ;-8

                ; End of the C loop
                _tpa(lw,t4,scanleft)
                _tpa(lw,t5,scanline)
                addi    t4,-1
                addi    t5,1
                _tpa(sw,t4,scanleft)
                _tpa(sw,t5,scanline)
                bgtz    t4,outer_loop
                nop

                ; E. Switch buffers
                _tpa(lw,t0,$a4040010)
                andi    t0,$800 ; no gfx
                bnez    t0,nobufferswap ; if nothing was drawn don't swap
                nop

                _tpa(lbu,t0,tribuf)
                li      t1,3
                addi    t0,1
                bne     t0,t1,tribufwrap
                nop
                li      t0,0
tribufwrap      _tpa(sb,t0,tribuf)
                la      t1,screenbuffer ; to draw to next frame
                la      t2,screenbuffer3 ; to display next frame
                beqz    t0,buf0
                addi    t0,-1
                la      t1,screenbuffer2
                la      t2,screenbuffer
                beqz    t0,buf0
                nop
                la      t1,screenbuffer3
                la      t2,screenbuffer2
buf0

                ; buffer for RDP output
                _tpa(sw,t1,rspdata+SetCimg)
                ; buffer read by VI (actually changed in vblank)
               _tpa(sw,t2,scrbuf)
nobufferswap

                rsp_go                  ; 2. Tell RSP to do a scanline.
                                        ; (this should kill it)

                ; D. Get controller input.
                jal     read_con
                nop
                srl     t0,16   ; get buttons
                srl     t1,16
                move    t6,t1
                _tpa(lb,t2,fbfflag) ; frame-by-frame
                andi    t1,t0,0x30
                xori    t1,0x30
                bnez    t2,menuyes
                nop
                bnez    t1,menuno
                nop
menuyes         ;move    t5,t0

                jal     menu
                nop

                ;move    t0,t5
                li      t0,0    ; don't bother saving through menu
                li      t6,0
menuno

                ; controller 1
                li      t2,0
                jal     analogtodigital
                nop
                andi    t0,0xff00
                sll     t0,8
                _tpa(sw,t0,controller1)

                ; controller 2
                move    t0,t6
                li      t2,8
                jal     analogtodigital
                nop
                andi    t0,0xff00
                sll     t0,8
                _tpa(sw,t0,controller2)

                jal     arkanoidpad
                nop

                j       frameloop
                nop

; ************** end of main loop

; Convert analog stick to D-pad
; changes t0
; offset in t2
; t1 used as temp

analogtodigital
        li     t1,0
        _vtpa(lb,t1,PI_result_block+7|0xa0000000,t2)
        addi   t1,-a2dthreshold
        bltz   t1,noup
        nop
        ori    t0,0x0800
        j      nodown
        nop
noup
        addi   t1,a2dthreshold*2
        bgtz   t1,nodown
        nop
        ori    t0,0x0400
nodown

        _vtpa(lb,t1,PI_result_block+6|0xa0000000,t2)
        addi   t1,-a2dthreshold
        bltz   t1,noright
        nop
        ori    t0,0x0100
        j      noleft
        nop
noright
        addi  t1,a2dthreshold*2
        bgtz  t1,noleft
        nop
        ori   t0,0x0200
noleft  jr    ra
        nop

; get analog stick x value and convert to arkanoid paddle data
; Paddle range is 98 to 242, 98 fully counter clockwise (which will be
; emulated with the control stick to the left)
arkanoidpad
        _tpa(lb,t0,PI_result_block+6|0xa0000000)
        addi    t0,70
        bgez    t0,minarknot
        li      t1,144  ; range, 242-98=144
        li      t0,0
minarknot
        mult    t0,t1
        mflo    t0
        li      t1,66+70    ; range
        li      t2,144
        div     t0,t1
        mflo    t0
        sub     t2,t0
        bgez    t2,maxarknot
        nop
        li      t0,144
maxarknot
        addi    t0,98   ; base value
        xori    t0,$ff  ; inverted for some reason
        _tpa(sb,t0,arkpad)
        jr      ra
        nop

; 6502 emulation

Run6502: _tpa(sw,ra,runra)
        blez    A6502_count, exitrun
        nop
        _tpa(lbu,t1,mmc3irqon)
        _tpa(lbu,t0,astatus)
        bnez    t1,irq_1
        andi    t0,$80  ; dmc irq status
        bnez    t0,irq_1
        nop

inner_loop:
;#include trace.inc

                readop_A6502(A6502_PC,t0)
                addi    A6502_PC,1
                la      t2,opcodejumptable
                sll     t1,t0,2
                addu    t1,t2
                lw      t1,(t1)
                addiu   A6502_instrcnt,1

                jalr    t1,ra
                nop     ; ditto

                bgtz    A6502_count, inner_loop
                nop
exitrun
                _tpa(lw,ra,runra)
                jr      ra
                nop
irq_1
        jal     Int6502
        nop
        j       inner_loop
        nop

BootRSP:
                li      t0,$1000
                _tpa(sw,t0,$a4080000)   ; set PC

                li      t0,%100101101 ; clear all, set int on break
                _tpa(sw,t0,0xa4040010) ; set status

                jr      ra
                nop

checksp0        
                _tpa(lb,t1,ppustatus)
                _tpa(lbu,t0,_SPRRAM+3)
                andi    t1,0x40
                bnez    t1,forgetsp0

                ; 3 pixels per CPU clock, according to Brad Taylor
                li      t2,3
                div     t0,t2
                mflo    t0
                li      t1,cyclesperscanline-cyclesafterfixit
                sub     t1,t0
                _tpa(sw,t1,cyclesaftersp0)

                jal     Run6502
                add     A6502_count,t0

                rsp_go  ; for more precision do it all here.
                jal     lineofsound ; to not totally waste this time
                nop
                rsp_done_wait

                _tpa(lw,t0,sp_base_reg+sp_status_reg)
                srl     t0,7
                andi    t0,0x60   ; sp0 hit and 8-sprite
                _tpa(sb,t0,ppustatus)

                _tpa(lwu,t1,cyclesaftersp0)

                jal     Run6502
                add     A6502_count,t1

                j       returnfromsp0
                nop

lastvramaddr    dh  0,0

; Initialize hardware and interrupt handlers.
#include inits.inc
 j backtotop
 nop
#include menu.inc
#include ppu.asm
#include save.inc
#include cap.asm
#include pal.inc
#include control.inc
#include rom.inc
#include "pattab.inc"
#include "atables.inc"
logo incbin "logo.bin"
 _align(8)
endoffile
