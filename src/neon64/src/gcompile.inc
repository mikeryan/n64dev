; NES Graphics Compiler v12
; (c) 2002-2004 Halley's Comet Software
; v9 - beginning cache
; v10 - switched to 4bpp texture generation
; v11 - back to v9 base, but write R4000 code to draw 8bpp texture
; v12 - only create 8 bytes of unpacked data per tile

; Structure of cache entry (8 bytes)
; bytes 0-3: age, incremented on each request (for any page)
; bytes 4-7: address of source page

; Compiler core, only does one line
; Input:
; t0: destination (incremented to point at next destination)
; t1: 1st pattern byte (low bits) (destroyed)
; t2: 2nd pattern byte (high bits) (destroyed)
; t3: pattbl address

; t4,t5: temp

#define gcmacro andi    t4,t1,$f0\
        andi    t5,t2,$f0\
        srl     t4,4-2\
        sll     t5,2\
        or      t4,t5\
        addu    t4,t3\
        lw      t5,(t4)\
        andi    t1,$0f\
        sll     t1,2\
        sw      t5,(t0)\
        andi    t2,$0f\
        sll     t2,6\
        or      t1,t2\
        addu    t1,t3\
        lw      t5,(t1)\
        sw      t5,4(t0)

; Request that a page be compiled, and stuck in gcpages (mapped to VRAM as
;  far as anything but the bg engine needs to know)
; A page is 0x0400 bytes of raw pattern data, no particular alignment required
; Parameters:
; t5: Location of page to compile
; t6: Address to map to
gcrequest _tpa(sd,ra,gcra)
        _tpa(sd,t0,gct0)

        ; 1. If there's only VRAM compilation is done real time

        _tpa(lbu,t0,chrromsize)
        beqz    t0,gcreqend2
        nop

        _tpa(sd,t1,gct1)
        _tpa(sd,t2,gct2)
        _tpa(sd,t3,gct3)
        _tpa(sd,t4,gct4)
        _tpa(sd,t7,gct7)
        _tpa(sd,s0,gcs0)

notsosimple
        ; stuff gets older
        la      t0,metacache
        li      t1,cachepages-1
ageloop lwu     t2,0(t0)
        addi    t0,8
        addi    t2,1
        sw      t2,-8(t0)
        bnez    t1,ageloop
        addi    t1,-1

        ; 2. Look for the page presently in cache
        la      t0,metacache-8
        li      t1,cachepages
cachesearch
        addi    t0,8
        lw      t2,4(t0)
        addi    t1,-1
        beq     t2,t5,cachesearchfound
        nop
        bnez    t1,cachesearch
        nop
cachesearchnotfound
        ; 2b. If not found, look for space for a new page
        la      t0,metacache
        li      t1,cachepages-2  ; first dealt with outside of loop
        lwu     s0,0(t0)        ; greatest age
        move    t7,t0           ; oldest address
emptysearch
        addi    t0,8
        lwu     t2,0(t0)
        sub     t3,s0,t2
        bgtz    t3,noteldest
        nop
        move    t7,t0
        move    s0,t2
noteldest
        bnez    t1,emptysearch
        addi    t1,-1
emptyfound

        ; address of place to write is in t7
        ; update cache entry
        ;li      t0,1
        sw      r0,0(t7)    ; first use of this page
        sw      t5,4(t7)    ; initialize

        la      t0,metacache
        subu    t7,t0   ; calculate offset
        sll     t7,12-3 ; 0x1000 bytes each page, 8 bytes each cache entry
        la      t0,pattblc
        addu    t0,t7

        ; update gcpages entry
        andi    t6,0x1c00
        srl     t4,t6,10-3
        sll     t6,2
        subu    t0,t6
        la      t6,_gcpages
        addu    t4,t6

        sw      t0,(t4)
        cache   25,0(t4)

        jal     cpage
        move    t6,t7
        j       gcreqend
        nop

cachesearchfound
        ; 2a. If found note that it has been found
        sw      r0,0(t0)

        ; update gcpages entry
        andi    t6,0x1c00
        srl     t5,t6,10-3
        sll     t6,2
        la      t4,_gcpages
        addu    t4,t5

        la      t1,metacache
        subu    t1,t0,t1
        sll     t1,12-3
        la      t0,pattblc
        addu    t0,t1
        subu    t0,t6
        sw      t0,(t4)
        cache   25,0(t4)
gcreqend

        _tpa(ld,ra,gcra)
        _tpa(ld,t1,gct1)
        _tpa(ld,t2,gct2)
        _tpa(ld,t3,gct3)
        _tpa(ld,t4,gct4)
        _tpa(ld,t7,gct7)
        _tpa(ld,s0,gcs0)
gcreqend2
        _tpa(ld,t0,gct0)

        jr      ra
        nop

; Compile a page (0x0400 bytes -> 0x1000 bytes)
; t5 = location to compile from
; t6 = location to compile to (-pattblc)
cpage
        la      t3,pattab
        la      t0,pattblc
        addu    t0,t6
        move    s0,t5
        
        li      t7,0x400-2      ; bytes to compile
cpttileloop1
cptlineloop1
        lbu     t1,(s0)
        lbu     t2,8(s0)

        gcmacro
        addi    t0,8

        addi    s0,1

        andi    t6,t7,0xf
        bnez    t6,cptlineloop1
        addi    t7,-2

        addi    s0,8

        bgtz    t7,cpttileloop1
        nop

        jr      ra
        nop

; Compile one byte (actually must use two)
; Parameters:
; t4=source byte address
; t5=vram address to compile
conebyte
        _tpa(sd,ra,gcra)
        _tpa(sd,t0,gct0)
        _tpa(sd,t1,gct1)
        _tpa(sd,t2,gct2)
        _tpa(sd,t3,gct3)
        _tpa(sd,t4,gct4)

        andi    t2,t4,8 ; clear bit 3
        xor     t4,t2
        andi    t2,t5,0x7
        sll     t2,1
        andi    t5,0x1ff0
        or      t5,t2
        sll     t5,2
        la      t0,pattblc
        addu    t0,t5
        lbu     t1,(t4)
        lbu     t2,8(t4)
        la      t3,pattab
        gcmacro

        _tpa(ld,ra,gcra)
        _tpa(ld,t0,gct0)
        _tpa(ld,t1,gct1)
        _tpa(ld,t2,gct2)
        _tpa(ld,t3,gct3)
        _tpa(ld,t4,gct4)
oneend
        jr      ra
        nop

gclookat        dw      0
 _align(8)
gcra    dw      0,0
gct0    dw      0,0
gct1    dw      0,0
gct2    dw      0,0
gct3    dw      0,0
gct4    dw      0,0
gct5    dw      0,0
gct6    dw      0,0
gct7    dw      0,0
gcs0    dw      0,0

; A flute without holes is not a flute.
; A donut without a hole is a danish.
