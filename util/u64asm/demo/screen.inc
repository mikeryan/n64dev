; Basic graphics functions.
; (c) 2002-2003 Halley's Comet Software

                _align(4)
init_PI:    ; doesn't belong in this file, sorry
; Initialize PI
; If this is not done, the N64 will freeze in about 5 seconds,
; and reset won't work at all.
                lui     t1, 0xbfc0
                lwu     t0, 0x07fc(t1)
                nop
                ori     t0, 8
                sw      t0, 0x07fc(t1)
                jr      ra
                nop

; Set up the framebuffer at 0x80200000, 16 bit/pixel mode, 320x250(ish) rez.
init_screen:
                ; Set up VI
screenbuffer    equ     0x80200000
screenbuffer2   equ     screenbuffer+(320*240*2)
                ; Control
                lui     t0, 0xa440
                la      t1, 0x00013002  ; 16 bit/pixel
                sw      t1, 0(t0)
                ; framebuffer
                la      t1, screenbuffer
                sw      t1, 4(t0)
                ; width
                li      t1, 0x140
                sw      t1, 8(t0)
                ; vertical interrupt
                li      t1, 2
                sw      t1, 12(t0)
                ; current line
                li      t1, 0
                sw      t1, 16(t0)
                ; video timing
                la      t1, 0x03e52239
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
                la      t1, 0x006c02ec
                sw      t1, 36(t0)
                ; v screen limits
                la      t1, 0x002501ff
                sw      t1, 40(t0)
                ; "color burst"
                la      t1, 0x000e0204
                sw      t1, 44(t0)
                ; h scale-up factor
                li      t1, 0x200
                sw      t1, 48(t0)
                ; v scale-up factor
                li      t1, 0x400
                sw      t1, 52(t0)
                jr      ra
                nop

; Wait for the vertical retrace
vblank:
                lui     t0,$a440
                li      t3,$200
                li      t4,$202
VSync:
                lw      t1,$0010(t0)
                sync
                beq     t1,t3,VSync_end
                nop
                bne     t1,t4,VSync
                nop
VSync_end:
                jr      ra
                nop
