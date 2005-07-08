; RSP text blitting routine
; (c) 2002-2003 Halley's Comet Software
; lwu changed to lw 5/14/03 for emu compatibility
                
                _align(8)
dosbitmap:
        incbin "dos.raw"    ; the DOS font bitmap
        db      "0123456789ABCDEF"

; variable addresses
rspdata         equ     0xa4000000
rspline         equ     rspdata
rspfont         equ     rspdata+640
rspnumcharset   equ     rspfont+1024
rspclock        equ     rspnumcharset+16
rspchar         equ     rspclock+4 ; pointer or value
rspx            equ     rspchar+4
rspy            equ     rspx+4
rspbusy         equ     rspy+4
rsptype         equ     rspbusy+4 ; 0=string, 1=byte, 3=half word, 7=word
rspscrbuf       equ     rsptype+4 ; where the screen buffer is
rsptemp         equ     rspdata+2048

txtscrbuf       equr    a0
txtpixcnt       equr    t0
txtcolor        equr    t1
txtbuf          equr    t2
txtleft         equr    t3
txtbmpbyte      equr    t4
txtcurchar      equr    t5
txtline         equr    t6
txtoldclock     equr    a1
txtcharcnt      equr    a2

                _align(8)
rsptext:
                ; RSP execution begins here
                obj     0x1000
                sw      r0, rspclock-rspdata
                sw      r0, rspbusy-rspdata
                move    txtoldclock,r0
main_text_loop:
                lw     s0, rspclock
                beq     s0,txtoldclock,main_text_loop
                nop
                move    txtoldclock,s0
                li      s0, 1
                sw      s0, rspbusy-rspdata

                lw     s1, rspy-rspdata
                lw     s0, rspx-rspdata
                sll     s2, s1, 2
                add     s1, s2
                sll     s1, 6
                add     s1, s0
                sll     s1, 1
                lw     txtscrbuf, rspscrbuf-rspdata
                li      txtcharcnt,0
                add     txtscrbuf, s1
                
                lw     s0, rsptype-rspdata
                nop
                bnez    s0, txt_number
                nop
                
                ; load the string
                li      s0, 0
                li      s1, 63
                li      s2, rsptemp-rspdata
                lw     s3, rspchar-rspdata
                jal     rspinternaldma
                nop
                
returnpoint:
charloop:
                
                lbu     txtcurchar,rsptemp-rspdata(txtcharcnt)
                beqz    txtcurchar,finalend
                addi    txtcharcnt,1
                li      txtleft, 8   ; # of lines
                li      txtline, 0   ; current line

; top of loop through all lines
line_loop:
                jal     RSPinternalWAIT
                nop

                ; DMA what's already there
                li      s0, 0
                li      s1, 8*2+8-1
                li      s2, rspline-rspdata
                move    s3, txtscrbuf
                jal     RSPinternalDMA
                nop

                li      txtbuf, rspline-rspdata ; mem pointer for line buffer
                
                andi    s0, txtscrbuf, 6
                add     txtbuf, s0
                
                li      txtpixcnt, 8    ; # of pixels to draw
                sll     s0, txtcurchar,3
                add     s0, txtline
                lbu     txtbmpbyte, rspfont-rspdata(s0)

                jal     RSPinternalWAIT
                nop
                
; loop through the line
fill_loop:
                andi    s0, txtbmpbyte, 0x80
                beqz    s0, black
                li      txtcolor, 0
                li      txtcolor, $fffe
                ;li      txtcolor, $07C0 ;green
black:          sh      txtcolor, 0(txtbuf)

                sll     txtbmpbyte, 1

                addi    txtpixcnt, -1
                bnez    txtpixcnt, fill_loop
                addi    txtbuf, 2
                
                ; DMA onto screen buffer
                li      s0, 1
                li      s1, 8*2+8-1
                li      s2, rspline-rspdata
                move    s3, txtscrbuf
                jal     RSPinternalDMA
                nop

                addi    txtscrbuf, 320*2
                addi    txtleft, -1
                bnez    txtleft, line_loop
                addi    txtline, 1

                j       charloop
                addi    txtscrbuf,(-320*2*8)+16
finalend:
                sw      r0, rspbusy-rspdata
                j       main_text_loop
                nop

txtnumcharcnt   equr    s0 ; this is set by the type and loaded in main proc.
txtnumoff       equr    s1
txtnumval       equr    s2
txtnumrefval    equr    s3
txtnumshift     equr    s5

; *************** number writing routine
txt_number:
                li      s6, 48
                sb      s6,(rsptemp+0)-rspdata
                li      s7, 120
                sb      s7,(rsptemp+1)-rspdata
                li      txtnumoff, 2
                lw     txtnumval, rspchar-rspdata
                la      txtnumrefval, 0x0fffffff
                li      s6, 7
                sub     s6, txtnumcharcnt
                sll     s6, 2
                srlv    txtnumrefval, s6
                li      txtnumshift, 28
                sub     txtnumshift, s6
number_loop:
                srlv    s6, txtnumval, txtnumshift
                lbu     s7, rspnumcharset-rspdata(s6)
                and     txtnumval, txtnumrefval
                sb      s7, rsptemp-rspdata(txtnumoff)
                
                srl     txtnumrefval, 4
                addi    txtnumshift, -4
                addi    txtnumoff, 1
                bnez    txtnumcharcnt, number_loop
                addi    txtnumcharcnt, -1

                sb      r0, rsptemp-rspdata(txtnumoff)
                
                j       returnpoint
                nop

RSPinternalDMA:
                mfc0    s8,WIRED    ; wait
                nop
                bnez    s8,RSPinternalDMA
                nop
                mtc0    s2, INDEX
                mtc0    s3, RANDOM
                bnez    s0, not_entryl0
                nop
                mtc0    s1, ENTRYLO0
                jr      ra
                nop
not_entryl0
                mtc0    s1, ENTRYLO1
                jr      ra
                nop

RSPinternalWAIT:
                mfc0    s8,WIRED
                nop
                bnez    s8,RSPinternalWAIT
                nop
                jr      ra
                nop

endoftheworld:
                break
                objend
