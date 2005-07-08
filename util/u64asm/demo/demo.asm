; U64ASM Demo v2
; (c)2002-2003 Halley's Comet Software
; Writes the input from the controller 1 buttons, in hexadecimal, to
; the screen. Uses double buffering and custom RSP microcode.

; The _tpa (two part address) macro makes up for the fact that U64ASM doesn't
;  automatically put a lui where you need one.
; Note: There is a flaw in the macro, which does not affect this program.
;       Locating it is left as an exercise to the reader. :)

; The main problem with this program as it stands, other than its
;  uselessness, is that the RSP runs continuously from when it is booted.
;  This means that you cannot reboot the N64 to restart this program, it
;  will simply freeze (waiting for the RSP to BREAK, I assume.)
;  This could be fixed by having the RSP break after every string drawn,
;  and rebooting it to write the text, but I haven't gotten around to
;  implementing it yet.

                org     $80000400
                
                jal     init_PI
                nop

                ; 320x240, 16bpp
                jal     init_screen
                nop

                ; DMAs the font bitmap data into the RSP's DMEM.
                ; a0 = RSP Address
                ; a1 = RAM Address
                ; a2 = Length
                ; a3 = Direction, 1 = RAM->RSP
                la      a0, rspfont
                la      a1, dosbitmap
                li      a2, 1024+16
                li      a3, 1
                jal     SPrawDMA
                nop

                ; Wait for above transfer to finish.
                jal     SPdeviceBUSY
                nop

                ; This does quite a bit:
                ; 1. DMAs the RSP code (at address RSP text) into RSP IMEM.
                ; 2. Waits for that to finish.
                ; 3. Boots the RSP so the code begins to run.
                la      a1, rsptext
                jal     boot_rsp
                nop

                ; rsptype points to an address in RSP DMEM that the text
                ;  blitting function accesses. It indicates the type of
                ;  text that will be displayed. 0=a NULL (0) terminated
                ;  string. Any number greater than that is one less than the
                ;  number of hexadecimal digits to display the number as,
                ;  we're showing a halfword (2 bytes) here, so we need 4
                ;  digits, and 4-1=3.
                li      t3, 3
                _tpa(sw,t3,rsptype)

                ; For double buffering, s0 controls which buffer we're using.
                li      s0, 0
                
mainloop:
                ; This sends the request for controller data.
                ;  (Not the *right* way to do this, so sue me)
                jal     initcontroller                
                nop

                ; Wait for a vblank, should be enough time for the transfer
                ;  to complete.
                jal     vblank
                nop

                ; Figure out which video buffer we're using
                lui     t0, 0xa440
                la      t1, screenbuffer2
                la      s1, screenbuffer
                bnez    s0, not_sbuf1
                move    t3, t1
                move    t1, s1
                move    s1, t3
not_sbuf1:
                ; Set the screen buffer
                sw      t1, 4(t0)
                xori    s0,1    ; toggles the buffer

                ; Specifies the start of the screen buffer that the RSP
                ;  will use. It is the same one that will be displayed
                ;  after the vertical retrace.
                _tpa(sw,t1,rspscrbuf)

                ; if the controller is not plugged in, we don't draw
                ;  anything.
                jal     detectcontroller
                nop
                andi    t0, 1
                beqz    t0, notplugged
                nop

                ; Read button and joystick values.
                jal     readcontroller
                nop
                
                ; Convert the joystick values into offsets we can use
                ;  to move the text around.
                neg     t2
                li      t4, 4
                div     t1, t4
                _tpa(lhu,t3,c1x)
                mflo    t1
                div     t2, t4
                _tpa(lhu,t4,c1y)
                mflo    t2                

                ; check screen bounds (so the number can't go off screen)
                ; the instructions are a bit intertwined so that I use nop
                ;  as rarely as possible.
                add     t1, t3
                bgez    t1, xnotltz
                add     t2, t4
                li      t1, 0
xnotltz:        bgez    t2, ynotltz
                slti    a0, t1, 320-(8*6)
                li      t2, 0
ynotltz:        bnez    a0, xltmax
                slti    a1, t2, 240-8
                li      t1, 320-(8*6)-1
xltmax:         bnez    a1, yltmax
                nop
                li      t2, 240-8-1
yltmax:
                
                ; Tell the RSP where to write the text.
                _tpa(sh,t1,c1x)
                _tpa(sh,t2,c1y)
                _tpa(sw,t0,rspchar)  ; in this case (type=3), the value
                _tpa(sw,t1,rspx)        ; x coord
                _tpa(sw,t2,rspy)        ; y coord
                _tpa(lwu,t0,rspclock)   ; when the 'clock' is toggled the
                nop                     ;  microcode leaves a wait loop
                xori    t0,1            ;  and draws the text
                _tpa(sw,t0,rspclock)
                nop
                
notplugged:
                ; We work here with the other screen WHILE the rsp is
                ;  drawing the text on the screen to be displayed.
                ;  Remember how above the other screen's address was in s1.
                la      t0, (320*240)-1
clear_screen_loop:
                sh      r0, (s1)
                addi    s1, 2
                bnez    t0, clear_screen_loop
                addi    t0, -1

                ; The RSP sets the rspbusy flag while it is drawing,
                ;  we'll wait until it has finished (although it will
                ;  probably finish before we get done clearing the screen).
rspwait:        _tpa(lwu,t0,rspbusy)
                nop
                bnez    t0, rspwait
                nop

                j       mainloop
                nop

                deadend
; I was afraid to keep these in registers because I thought that I might
;  tamper with the values by accident, so here they are in RAM.
        _align(4)
c1x:    dh      160-((6*8)/2)   ; location of the numbers
c1y:    dh      120-(8/2)

#include macros.inc
#include screen.inc
#include rsp.inc
#include control.inc
#include text.inc
