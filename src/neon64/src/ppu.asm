; NES PPU Emulator v3 for the Nintendo 64 Reality Signal & Display Processors
; (c) 2003 Halley's Comet Software
; halleyscometsoftware@hotmail.com
; http://here.is/halleyscomet

 _align(8)
rsptext:
    obj 0x1000 ; IMEM

; ********** Main PPU Program **********
ppu_temp        equr    s8
ppu_scanline    equr    a0
ppu_scanleft    equr    a1
ppu_linepos     equr    t0
ppu_sp0check    equr    t8

ppu_sprcnt      equr    t0
ppu_sprloc      equr    t1

; Waits for DMA not full (can start a new one)
#define dmawait mfc0    at,pagemask\
                bnez    at,pc-4\
                nop

; Waits for DMA not busy (DMAs finished)
#define dmarealwait mfc0    at,wired\
                bnez    at,pc-4\
                nop

#define rdpsubmit(_start,_end) mfc0 ppu_temp,compare\
    andi    ppu_temp,0x600\ ; wait for start/end not valid
    bnez    ppu_temp,pc-8\
    nop\
    li      ppu_temp,_start\
    mtc0    ppu_temp,badvaddr\
    li      ppu_temp,_end\
    mtc0    ppu_temp,count

; A. initialize
rsp_start
    la      ppu_temp,sp_clr_ppudone|sp_clr_ppugo|sp_clr_8sprite|sp_clr_spr0hit|sp_clr_vblank
    mtc0    ppu_temp,context            ; 1. Clear flags
    move    ppu_sp0check,r0

    li      ppu_scanline,-1             ; 4. Initialize scaline counters
    li      ppu_scanleft,239            ; 239 scanlines to go

; B. Load for line 0
    jal     RSP_wait
    nop
    dmawait
    li      ppu_temp,VRAM_V          ; 3. DMA in VRAM pages
    mtc0    ppu_temp,index
    la      ppu_temp,_VRAM_V2
    mtc0    ppu_temp,random
    li      ppu_temp,8*26-8   ; include VRAM_V,VRAM_X,VRAMpages,and GCpages
    mtc0    ppu_temp,entrylo0
    dmarealwait
    jal     getnewsprites
    nop
    jal     getsprites
    nop
    jal     RSP_done
    nop

    mfc0    ppu_temp, context
    andi    ppu_temp,0x800
    bnez    ppu_temp,ppu_nogfx
    nop

    li      ppu_temp,2
    mtc0    ppu_temp,compare    ; XDMA set (RDP reads from DMEM instead of DRAM)

; C. Scanline
scanlineloop
    jal     RSP_wait
    nop

    addi    ppu_scanline,1

    dmawait
    li      ppu_temp,VRAM_V          ; 3. DMA in VRAM pages
    mtc0    ppu_temp,index
    la      ppu_temp,_VRAM_V2
    mtc0    ppu_temp,random
    li      ppu_temp,8*26-8   ; include VRAM_V,VRAM_X,VRAMpages,and GCpages
    mtc0    ppu_temp,entrylo0
    ;dmarealwait

; adjust fillrectangle for drawing background color
    li      t0,8<2
    sll     t0,10
    or      t0,ppu_scanline
    sll     t0,2
    sw      t0,fillrect+4

    li      t0,0x36 ; fillrect command
    sll     t0,12
    ori     t0,255+8<2
    sll     t0,10
    or      t0,ppu_scanline
    addi    t0,1<2
    sll     t0,2
    sw      t0,fillrect
    rdpsubmit(fillstart,fillend)

    ; LOAD A NEW PALETTE

    mfc0    ppu_temp,context
    andi    ppu_temp,0x400      ; palette changed bit
    beqz    ppu_temp,samepal
    nop
    li      ppu_temp,sp_clr_palchg  ; agknowledge (just barely fits li)
    mtc0    ppu_temp,context
    rdpsubmit(loadpalstart,loadpalend)
samepal

    ; Draw sprites
    jal     drawsprites
    nop

    dmawait
    li      ppu_temp,bgsprline    ; send line
    mtc0    ppu_temp,index
    la      ppu_temp,bgsprbuffer
    mtc0    ppu_temp,random
    li      ppu_temp,256 ;+8-8
    mtc0    ppu_temp,entrylo1

    jal     changey
    li      ppu_temp,bg_spr_rect
    rdpsubmit(bgsprstart,bgsprend)

    dmawait
    li      ppu_temp,bgsprline   ; Get blank line
    mtc0    ppu_temp,index
    la      ppu_temp,zerobuffers
    mtc0    ppu_temp,random
    li      ppu_temp,256 ;+8-8
    mtc0    ppu_temp,entrylo0

    ; Draw BG
    lw      ppu_temp,VRAM_X     ; Load draw location, adjust fine x scroll
    li      ppu_linepos,PPUline
    sub     ppu_linepos,ppu_temp

    jal     drawbg
    nop

    ; Check sprite 0 (before new sprites loaded, after bg drawn)

    bnez    ppu_sp0check,sp0check
    nop
nosp0check

    jal     getnewsprites   ; for next scanline
    nop

    dmawait
    li      ppu_temp,tppuline   ; send line
    mtc0    ppu_temp,index
    la      ppu_temp,bgbuffer
    mtc0    ppu_temp,random
    li      ppu_temp,256+16-8
    mtc0    ppu_temp,entrylo1

    jal     changey
    li      ppu_temp,bg_rect
    rdpsubmit(bgstart,bgend)

    dmawait
    li      ppu_temp,tppuline   ; Get blank line
    mtc0    ppu_temp,index
    la      ppu_temp,zerobuffers
    mtc0    ppu_temp,random
    li      ppu_temp,256+8 ;+8-8
    mtc0    ppu_temp,entrylo0

    jal     getsprites  ; for next scanline
    nop

    ; do fg sprites after playfield
    dmawait
    li      ppu_temp,fgsprline    ; send line
    mtc0    ppu_temp,index
    la      ppu_temp,fgsprbuffer
    mtc0    ppu_temp,random
    li      ppu_temp,256 ;+8-8
    mtc0    ppu_temp,entrylo1

    jal     changey
    li      ppu_temp,fg_spr_rect
    rdpsubmit(fgsprstart,fgsprend)

    dmawait
    li      ppu_temp,fgsprline    ; Get blank line
    mtc0    ppu_temp,index
    la      ppu_temp,zerobuffers
    mtc0    ppu_temp,random
    li      ppu_temp,256 ;+8-8
    mtc0    ppu_temp,entrylo0

    jal     RSP_done
    nop

    ;addi    ppu_scanline,1
    bgtz    ppu_scanleft,scanlineloop
    addi    ppu_scanleft,-1
endtime:
    jal     RSP_wait
    nop

    ;lui     ppu_temp,sp_set_vblank>16     ; Set vblank flag
    ;mtc0    ppu_temp,context

    ;li      ppu_temp,2                    ; Halt RSP
    ;mtc0    ppu_temp,context
    ;nop
    ;nop     ; actually needed...
    ;nop
    ;deadend
    j       rsp_start
    nop

; ************** end ************

; only sets flags
ppu_nogfx
    jal     RSP_wait
    nop

    addi    ppu_scanline,1

    lwu     ppu_temp,PPUControl2
    andi    ppu_temp,$10    ; sprites on?
    beqz    ppu_temp,ppu_nocheatsp0
    lwu     ppu_temp,sp0line
    bne     ppu_scanline,ppu_temp,ppu_nocheatsp0
    nop
    lui     ppu_temp,sp_set_spr0hit>16
    mtc0    ppu_temp,context
ppu_nocheatsp0

    jal     getnewsprites   ; for next scanline
    nop

    jal     RSP_done
    nop

    bgtz    ppu_scanleft,ppu_nogfx
    addi    ppu_scanleft,-1

    j   endtime
    nop

; ************** Check sprite 0
; Doesn't check against BG because for some reason Battletoads timing isn't
; close enough for it.

sp0check
    lbu     ppu_temp,SPRRAM ; y position
    sub     ppu_temp,ppu_scanline,ppu_temp
    addi    ppu_temp,-1 ; ppu_scanline has inc'd since sp0 was loaded
    andi    ppu_temp,7  ; for 8x16
    lbu     t0,sprpat(ppu_temp)
    lbu     ppu_temp,sprpat+8(ppu_temp)
    or      t0,ppu_temp
    bnez    t0,sp0hit
    li      ppu_sp0check,0

    j       nosp0check  ; NOP not here intentionally
    nop
sp0hit
    ; et viola, a sprite 0 hit
    mfc0    ppu_temp,context
    andi    ppu_temp,$40<7
    bnez    ppu_temp,sp0hitnotthisline
    lui     ppu_temp,sp_set_spr0hit>16
    sw      ppu_scanline,sp0line
sp0hitnotthisline
    mtc0    ppu_temp,context

    j nosp0check
    nop

; change the y coordinate of a rectange to the current scanline
; address of rect is in ppu_temp
changey
    ; It is interesting (and important) to note that this does not affect the
    ; x coordinate unless x%4 != 0
    sll     t0,ppu_scanline,2
    sh      t0,6(ppu_temp)    ; upper left y
    addi    t0,4
    sh      t0,2(ppu_temp)    ; lower right y
    jr      ra
    nop


; ********** DMA Sprites From RAM (if changed) **********
getnewsprites
    mfc0    ppu_temp,context
    andi    ppu_temp,0x200      ; sprites changed bit
    beqz    ppu_temp,samesprites
    nop
    li      ppu_temp,sp_clr_sprchg  ; agknowledge
    mtc0    ppu_temp,context
    dmawait
    li      ppu_temp,SPRRAM
    mtc0    ppu_temp,index
    la      ppu_temp,_SPRRAM
    ;lw      ppu_temp,sprdmaadr
    mtc0    ppu_temp,random
    li      ppu_temp,0x100-8
    mtc0    ppu_temp,entrylo0

samesprites
    jr      ra
    nop

; ********** Load Sprites **********

#define loadspr(_which) andi    _which,7\
    li      ppu_temp,sprpat\
    sll     _which,4\
    add     ppu_temp,_which\
    mtc0    ppu_temp,index\
    srl     _which,2\
    lw      ppu_temp,sprdramadr(_which)\
    mtc0    ppu_temp,random\
    li      ppu_temp,16-8\
    mtc0    ppu_temp,entrylo0\
    srl     _which,2

ls_ssize        equr    t0
ls_sprram       equr    t1
ls_buf          equr    t2
ls_smax         equr    t4
ls_sleft        equr    t5
ls_sy           equr    t6
ls_temp         equr    t7
ls_cnt          equr    s0
ls_temp2        equr    s2
ls_temp3        equr    s3
ls_pt           equr    s4
ls_temp4        equr    s5
ls_spcnt        equr    s7
ls_spclip       equr    v0
ls_sx           equr    v1

getsprites
    dmarealwait

    ; Initialization
    lw     ls_pt,PPUControl1
    li      ls_sprram,SPRRAM
    andi    ppu_temp,ls_pt,0x20
    beqz    ppu_temp,not16
    li      ls_ssize,8
    li      ls_ssize,16
not16 li      ls_buf,spritebuffer
    li      ls_smax,8
    li      ls_sleft,63
    li      ls_cnt,0
    lw      ppu_temp,PPUControl2
    li      ls_spcnt,0
    andi    ls_temp,ppu_temp,0x10
    beqz    ls_temp,no_spld
    nop

    andi    ls_pt,0x08
    sll     ls_pt,12-3  ; 0x1000

loadloop:
    lbu     ls_sy,0(ls_sprram)
    lbu     ls_sx,3(ls_sprram)
;    addi    ls_sy,1                 ; will appear on next line
    sub     ls_sy,ppu_scanline,ls_sy
    bltz    ls_sy,next_sprite       ; sprite occurs after this line
    nop
    slt     ppu_temp,ls_sy,ls_ssize
    beqz    ppu_temp,next_sprite    ; sprite occurs too far before this line
    slti    ppu_temp,ls_sprram,SPRRAM+1 ; (check for sp0 before clipping)
    or      ppu_sp0check,ppu_temp
    beqz    ls_smax,maxsprites      ; too many sprites
    addi    ls_smax,-1

    lbu     ls_temp,1(ls_sprram)
    lbu     ls_temp2,2(ls_sprram)

    ; Sprite Pattern Loading
    li      ls_temp3,8
    beq     ls_ssize,ls_temp3,spriteload8
    nop

    ; 8x16 sprites

    andi    ppu_temp,ls_temp,1  ; odds are in 0x1000
    sll     ppu_temp,12          ; 0x1000
    andi    ls_temp,0xfe

    sub     ls_temp3,ls_sy      ; high or low
    slti    ls_temp3,ls_temp3,1
    ;sll     ls_temp3,7

    andi    ls_temp4,ls_temp2,0x80  ; check vflip
    srl     ls_temp4,7
    xor     ls_temp4,ls_temp3

    add     ls_temp,ls_temp4    ; the lo/hi tile, but on vflip the hi/lo tile
;    beqz    ls_temp4,not_vflip16
;    nop
;    addi    ls_temp,1       ; the lo/hi tile, but on vflip the hi/lo tile
;not_vflip16

    sll     ls_temp,4
    addu    ls_temp,ppu_temp

    ; Lookup in pages
    srl     ppu_temp,ls_temp,10-3        ; >> 10, 8 bytes each
    andi    ppu_temp,~7
    lw      ppu_temp,vrampages(ppu_temp); get base address
    addu    ls_temp,ppu_temp

    ; DMA in the pattern
    ;dmawait
    ;mtc0    ls_patbuf,index
    ;mtc0    ls_temp,random
    ;li      ppu_temp,16-8
    ;mtc0    ppu_temp,entrylo0

    sll     ppu_temp,ls_cnt,2
    sw      ls_temp,sprdramadr(ppu_temp)

    j       sp_load_done
    nop

spriteload8
    sll     ls_temp4,ls_temp,4
    addu    ls_temp4,ls_pt
    srl     ppu_temp,ls_temp4,10-3
    andi    ppu_temp,~7
    lw      ppu_temp,vrampages(ppu_temp)    ; get base address
    addu    ls_temp4,ppu_temp

    ; 8x8 sprite tile load
    ;dmawait
    ;mtc0    ls_patbuf,index
    ;mtc0    ls_temp4,random
    ;li      ppu_temp,16-8
    ;mtc0    ppu_temp,entrylo0

    sll     ppu_temp,ls_cnt,2
    sw      ls_temp4,sprdramadr(ppu_temp)

sp_load_done
    andi    ls_sy,7
    move    ls_temp,ls_spcnt
    addi    ls_spcnt,1

    sb      ls_sy,0(ls_buf)
    sb      ls_temp2,2(ls_buf)
    sb      ls_temp,1(ls_buf)
    lbu     ls_temp,3(ls_sprram)
    sb      ls_temp,3(ls_buf)
    addi    ls_cnt,1
    addi    ls_buf,4
next_sprite:
    addi    ls_sprram,4
    bnez    ls_sleft,loadloop
    addi    ls_sleft,-1
no_spld

    j       spendstuff
    nop

maxsprites
    lui     ppu_temp,sp_set_8sprite>16
    mtc0    ppu_temp,context

spendstuff

    sw      ls_cnt,spritecount

    dmawait
    addi    ls_cnt,-1
    loadspr(ls_cnt)
    dmawait
    addi    ls_cnt,-1
    loadspr(ls_cnt)

    jr      ra
    nop

; ********** Draw Sprites **********
; Input: ppu_sprcnt, number of sprites to draw
;        ppu_sprloc, location of sprite buffer to draw from

ds_temp     equr     a3
ds_y        equr     t3
ds_idx      equr     t4
ds_atr      equr     t5
ds_x        equr     t6
ds_clr      equr     t7
ds_pathi    equr     s1
ds_patlo    equr     s0
ds_cnt      equr     s2

drawsprites
    ; Initialize
    sw      ra,ppu_ra
    lw      ppu_sprcnt,spritecount

    lw     ppu_temp,PPUControl2        ; Check if sprites are active
    andi    ppu_temp,0x10
    beqz    ppu_temp,nosprites
    nop
;    li      ppu_temp,8
;    sub     ppu_temp,ppu_sprcnt
;timingloop
;    dcw     24,0    ; 24 NOPs
;    bnez    ppu_temp,timingloop
;    addi    ppu_temp,-1

    beqz    ppu_sprcnt,nosprites
    nop

    addi    ppu_sprloc,ppu_sprcnt,-1      ; We work backwards (for accuracy)
    sll     ppu_sprloc,2

    dmawait
    addi    ppu_sprcnt,-3   ; first two already loaded
    loadspr(ppu_sprcnt)     ; load sprite 2
    jal     ds_sub  ; draw sprite 0 (first on line)
    nop

    dmarealwait
    addi    ppu_sprcnt,-1
    loadspr(ppu_sprcnt)     ; load sprite 3
    jal     ds_sub  ; draw sprite 1
    nop

    dmawait
    addi    ppu_sprcnt,-1
    loadspr(ppu_sprcnt)     ; load sprite 4
    jal     ds_sub  ; draw sprite 2
    nop

    dmawait
    addi    ppu_sprcnt,-1
    loadspr(ppu_sprcnt)     ; load sprite 5
    jal     ds_sub  ; draw sprite 3
    nop

    dmawait
    addi    ppu_sprcnt,-1
    loadspr(ppu_sprcnt)     ; load sprite 6
    jal     ds_sub  ; draw sprite 4
    nop

    dmawait
    addi    ppu_sprcnt,-1
    loadspr(ppu_sprcnt)     ; load sprite 7
    jal     ds_sub  ; draw sprite 5
    nop

    dmawait
    jal     ds_sub  ; draw sprite 6
    nop

    dmarealwait
    jal     ds_sub  ; draw sprite 7
    nop

    ; SHOULD NEVER REACH THIS POINT
    break

    ; main loop

ds_sub
    lbu     ds_y,spritebuffer+0(ppu_sprloc)      ; y
    lbu     ds_idx,spritebuffer+1(ppu_sprloc)    ; index
    lbu     ds_atr,spritebuffer+2(ppu_sprloc)    ; attributes
    lbu     ds_x,spritebuffer+3(ppu_sprloc)      ; x

    andi    ds_clr,ds_atr,3 ; upper two color bits
    sll     ds_clr,4
    ori     ds_clr,$40  ; sprite palette starts here

    andi    ppu_temp,ds_atr,0x80    ; check for vflip
    beqz    ppu_temp,novflip
    li      ppu_temp,7
    sub     ds_y,ppu_temp,ds_y
novflip

    sll     ds_idx,4
    add     ds_y,ds_idx

    lbu     ds_pathi,sprpat+8(ds_y)
    sll     ds_pathi,1
    lbu     ds_patlo,sprpat(ds_y)

    andi    ppu_temp,ds_atr,0x40 ; horizontal flip
    bnez    ppu_temp,hflip
    nop

    andi    ppu_temp,ds_atr,0x20 ; background?
    bnez    ppu_temp,unflipped_bg_loop
    addi    ds_x,7
unflipped_loop
    andi    ppu_temp,ds_pathi,2
    andi    ds_temp,ds_patlo,1
    or      ppu_temp,ds_temp
    srl     ds_pathi,1
    srl     ds_patlo,1
    andi    ds_temp,ds_pathi,$fe
    or      ds_temp,ds_patlo
    beqz    ppu_temp,unflipped_trans
    or      ppu_temp,ds_clr
    sb      ppu_temp,fgsprline(ds_x)
    sb      r0,bgsprline(ds_x)
unflipped_trans
    bnez    ds_temp,unflipped_loop
    addi    ds_x,-1
    beqz    ppu_sprloc,nosprites
    addi    ppu_sprloc,-4

    jr      ra
    nop

unflipped_bg_loop
    andi    ppu_temp,ds_pathi,2
    andi    ds_temp,ds_patlo,1
    or      ppu_temp,ds_temp
    srl     ds_pathi,1
    srl     ds_patlo,1
    andi    ds_temp,ds_pathi,$fe
    or      ds_temp,ds_patlo
    beqz    ppu_temp,unflipped_bg_trans
    or      ppu_temp,ds_clr
    sb      ppu_temp,bgsprline(ds_x)
    sb      r0,fgsprline(ds_x)  ; this line makes SMB3 sprite priority work
unflipped_bg_trans
    bnez    ds_temp,unflipped_bg_loop
    addi    ds_x,-1
    beqz    ppu_sprloc,nosprites
    addi    ppu_sprloc,-4

    jr      ra
    nop

hflip
    andi    ppu_temp,ds_atr,0x20 ; background?
    bnez    ppu_temp,hflip_bg_loop
    nop
hflip_loop
    andi    ppu_temp,ds_pathi,2
    andi    ds_temp,ds_patlo,1
    or      ppu_temp,ds_temp
    srl     ds_pathi,1
    srl     ds_patlo,1
    andi    ds_temp,ds_pathi,$fe
    or      ds_temp,ds_patlo
    beqz    ppu_temp,hflip_trans
    or      ppu_temp,ds_clr
    sb      ppu_temp,fgsprline(ds_x)
    sb      r0,bgsprline(ds_x)
hflip_trans
    bnez    ds_temp,hflip_loop
    addi    ds_x,1
    beqz    ppu_sprloc,nosprites
    addi    ppu_sprloc,-4

    jr      ra
    nop

hflip_bg_loop
    andi    ppu_temp,ds_pathi,2
    andi    ds_temp,ds_patlo,1
    or      ppu_temp,ds_temp
    srl     ds_pathi,1
    srl     ds_patlo,1
    andi    ds_temp,ds_pathi,$fe
    or      ds_temp,ds_patlo
    beqz    ppu_temp,hflip_bg_trans
    or      ppu_temp,ds_clr
    sb      ppu_temp,bgsprline(ds_x)
    sb      r0,fgsprline(ds_x)
hflip_bg_trans
    bnez    ds_temp,hflip_bg_loop
    addi    ds_x,1
    beqz    ppu_sprloc,nosprites
    addi    ppu_sprloc,-4

    jr      ra
    nop
nosprites

    lw      ppu_temp,PPUControl2
    andi    ppu_temp,4
    bnez    ppu_temp,spnoclip
    nop
    sw      r0,bgsprline
    sw      r0,bgsprline+4
    sw      r0,fgsprline
    sw      r0,fgsprline+4
spnoclip

        lw      ra,ppu_ra
        jr      ra
        nop

; ********** Draw Background **********
; (aka bgzilla)
;ppu_linepos equr   t0  ; \
bg_c1       equr    t1  ;  |___ as required by the graphics compiler
bg_c2       equr    t2  ;  |
bg_c3       equr    t3  ; /
bg_v        equr    t4
bg_atrb     equr    t5
bg_y        equr    t6
bg_nt       equr    s0
bg_at       equr    s1
bg_pt       equr    s2
bg_temp     equr    s3
bg_cnt      equr    s4
bg_nta      equr    s5
bg_ata      equr    s6
bg_at1      equr    v0
bg_at2      equr    v1
bg_at3      equr    t7
bg_at4      equr    s7

#define get_nt(_slot) lbu  ppu_temp,namtemp(bg_nta)\    ; get tile #
    addi    bg_nta,1\       ; point at next nt byte
    sll     ppu_temp,4\     ; 16 bytes per tile
    or      ppu_temp,bg_pt\ ; which pt
    srl     bg_temp,ppu_temp,10\ ; get page #
    sll     bg_temp,3\      ; cvt to page addr
    lw      bg_temp,gcpages(bg_temp)\
    sll     ppu_temp,2\     ; 4x bigger
    addu    ppu_temp,bg_temp\
    addu    ppu_temp,bg_y\
    dmawait\
    li      bg_temp,slot__slot\ ; DMA in that tile
    mtc0    bg_temp,index\
    mtc0    ppu_temp,random\
    mtc0    r0,entrylo0\    ; 8 bytes
    andi    ppu_temp,bg_v,3\    ; get attribute bits
    bnez    ppu_temp,pc+28\
    nop\
    lbu     bg_atrb,atrtemp(bg_ata)\
    addi    bg_ata,1\
    andi    ppu_temp,bg_v,$40\
    srl     ppu_temp,4\
    srlv    bg_atrb,ppu_temp\
    andi    ppu_temp,bg_v,2\
    srlv    bg_temp,bg_atrb,ppu_temp\
    andi    bg_temp,3\
    sll     bg_temp,2\
    lw      bg_at_slot,atrtab(bg_temp)\
    addi    ppu_temp,bg_v,1\    ; increment v
    andi    ppu_temp,$1f\
    slti    bg_temp,ppu_temp,1\
    sll     bg_temp,10\
    xor     bg_v,bg_temp\
    andi    bg_v,0xffe0\
    sub     bg_temp,r0,ppu_temp\ ; negate for bgezal
    bgezal  bg_temp,nta\     ; addresses must be recalculated
    or      bg_v,ppu_temp

#define drawpt(_slot) lw      bg_temp,slot__slot\
    or      bg_temp,bg_at_slot\
    sw      bg_temp,_slot-1*8(ppu_linepos)\
    lw      bg_temp,slot__slot+4\
    or      bg_temp,bg_at_slot\
    sw      bg_temp,_slot-1*8+4(ppu_linepos)

drawbg
    sw      ra,ppu_ra
    ; Initialize
    lw      bg_v,VRAM_V
    li      bg_cnt,8
    srl     bg_y,bg_v,12
    ;andi    bg_y,7 ; no need
    sll     bg_y,3   ; 8 bytes per line

    ; 3. Check if BG should be drawn at all
    lw     ppu_temp,PPUControl2
    andi    ppu_temp,0x8
    beqz    ppu_temp,nobg
    nop

    ; 1. DMA in entire name table line (should almost always work)
    andi    bg_nt,bg_v,0x03e0
    addi    bg_nt,0x2000

    ;srl     ppu_temp,bg_nt,10
    ;sll     ppu_temp,3
    li      ppu_temp,$2000>10<3
    lw      ppu_temp,vrampages(ppu_temp)
    addu    bg_nt,ppu_temp

    dmawait
    li      ppu_temp,namtemp
    mtc0    ppu_temp,index
    mtc0    bg_nt,random
    la      ppu_temp,(32-8)|(4-1<12)|(32*31<20)
    mtc0    ppu_temp,entrylo0

    ; 2. DMA in entire attribute table line (should work when 1 works)
    andi    bg_at,bg_v,0x0380
    srl     bg_at,4
    addi    bg_at,0x23c0

    ;srl     ppu_temp,bg_at,10
    ;sll     ppu_temp,3
    li      ppu_temp,$23c0>10<3
    lw      ppu_temp,vrampages(ppu_temp)
    add     bg_at,ppu_temp

    dmawait
    li      ppu_temp,atrtemp
    mtc0    ppu_temp,index
    mtc0    bg_at,random
    la      ppu_temp,(8-8)|(4-1<12)|(32*32-8<20)
    mtc0    ppu_temp,entrylo0

    lw     bg_pt,PPUControl1
    andi    bg_pt,0x10
    sll     bg_pt,12-4

    andi    bg_v,0xfff
    ;lw      ppu_temp,PPUControl1    ; load name table
    ;andi    ppu_temp,3
    ;sll     ppu_temp,10
    ;or      bg_v,ppu_temp

    ; set up initial addresses
    jal     nta
    nop
    dmarealwait

    ; get initial attribute byte
    andi    ppu_temp,bg_v,3
    beqz    ppu_temp,no1statr
    nop
    lbu     bg_atrb,atrtemp(bg_ata)
    addi    bg_ata,1
    andi    ppu_temp,bg_v,$40
    srl     ppu_temp,4
    srlv    bg_atrb,ppu_temp
no1statr
    get_nt(1)
    get_nt(2)

    ; 5. Tile loop
bgloop
    get_nt(3)
    drawpt(1)
    beqz    bg_cnt,nobg ; for the last tile
    nop

    get_nt(4)
    drawpt(2)

    get_nt(1)
    drawpt(3)

    get_nt(2)
    drawpt(4)
    addi    ppu_linepos,32

    bnez    bg_cnt,bgloop
    addi    bg_cnt,-1

nobg

    ; bgclipping
    lw     ppu_temp,PPUControl2
    andi    ppu_temp,2
    bnez    ppu_temp,nobg_clip2
    nop
    sw      r0,ppuline
    sw      r0,ppuline+4

nobg_clip2

    lw      ra,ppu_ra
    jr  ra
    nop

; ********** Get info that only changes on a name table change **********
nta
    ; Attribute Table Address
    andi    ppu_temp,bg_v,0x380     ; calculate address
    srl     ppu_temp,2
    andi    bg_temp,bg_v,0x1c
    or      bg_temp,ppu_temp
    srl     bg_temp,2
    andi    ppu_temp,bg_v,0x0c00
    or      bg_temp,ppu_temp
    addi    bg_temp,0x23c0
    srl     ppu_temp,bg_temp,10     ; lookup in pages
    sll     ppu_temp,3
    lw      ppu_temp,vrampages(ppu_temp)
    addu    ppu_temp,bg_temp

    subu    ppu_temp,bg_at
    andi    bg_temp,ppu_temp,0x0c00
    srl     bg_temp,10-3
    andi    ppu_temp,0x7
    or      bg_ata,ppu_temp,bg_temp

    ; Name Table Address
    addi    bg_temp,bg_v,0x2000
    
    srl     ppu_temp,bg_temp,10     ; get actual address
    sll     ppu_temp,3
    lw      ppu_temp,vrampages(ppu_temp)
    addu    bg_temp,ppu_temp

    subu    bg_temp,bg_nt       ; subtract off base name table address

    andi    ppu_temp,bg_temp,0xc00  ; adjust (NTs are compacted now)
    srl     ppu_temp,10-5
    andi    bg_temp,0x1f
    or      bg_nta,bg_temp,ppu_temp

    jr      ra
    nop


; ****** CPU<->RSP semaphore method ******
;  signal 0 is set by the CPU to make RSP run, cleared by the
;  RSP to agknowledge.
;  signal 1 is set by the RSP to indicate it's done,
;  cleared by the CPU to agknowledge.

RSP_wait:
    mfc0    ppu_temp,context
    ;nop
    andi    ppu_temp,0x80         ; check signal 0
    beqz    ppu_temp,RSP_wait
    nop
    li      ppu_temp,0x200
    mtc0    ppu_temp,context      ; clear signal 0

    jr      ra
    nop
        
RSP_done:
    li      ppu_temp,0x1000     ; set signal 1
    mtc0    ppu_temp,context
    jr      ra
    nop
    objend

 _align(8)
rspdata         equ     0xa4000000
rspdataa
        obj     0x0     ; DMEM
PPUControl1     dw      0,0     ; 0x2000 write only
PPUControl2     dw      0,0     ; 0x2001 write only
SPRRAM          dcb     256,0
bgsprline       dcb     256+8,0
fgsprline       dcb     256+8,0 ; buffer at end for sprites off screen
tppuline        dcb     256+16,0    ; buffers on each side for scroll
ppuline         equ     tppuline+8
#include "staticdl.h"
VRAM_V          dw      0,0     ; the VRAM address, managed by the CPU
VRAM_X          dw      0,0
VRAMPages       ; addr >> 10
                dw      VRAM,0  ; pattern table #1 (probably CHRROM)
                dw      VRAM,0
                dw      VRAM,0
                dw      VRAM,0
                dw      VRAM,0  ; pattern table #2
                dw      VRAM,0
                dw      VRAM,0
                dw      VRAM,0
                dw      VRAM,0   ; #0 The name/attribute tables
                dw      VRAM,0   ; #1 This will be changed to fit whatever
                dw      VRAM,0   ; #2 type of mirroring is used
                dw      VRAM,0   ; #3
                dw      VRAM-0x1000,0 ; the end of this is palette
                dw      VRAM-0x1000,0
                dw      VRAM-0x1000,0
                dw      VRAM-0x1000,0
GCPages         dw      pattblc,0   ; 0x0000
                dw      pattblc,0   ; 0x0400
                dw      pattblc,0   ; 0x0800
                dw      pattblc,0   ; 0x0c00
                dw      pattblc,0   ; 0x1000
                dw      pattblc,0   ; 0x1400
                dw      pattblc,0   ; 0x1800
                dw      pattblc,0   ; 0x1c00
slot_1          dw      0,0
slot_2          dw      0,0
slot_3          dw      0,0
slot_4          dw      0,0
namtemp         dcb     4*32,0
atrtemp         dcb     4*8,0
spritebuffer    dcb     8*4,0   ; 4 bytes per sprite
sprpat          dcb     16*8,0 ; pattern for each sprite
sprdramadr      dcw     8,VRAM ; init'ed to a valid address
sprdmaadr       dw      _sprram,0
spritecount     dw      0
sp0line         dw      0
ppu_ra          dw      0
atrtab          dw      $00000000,$10101010,$20202020,$30303030
        objend

; Thou art dead.
