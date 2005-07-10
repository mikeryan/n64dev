; Text blitting routine
; (c) 2002-2003 Halley's Comet Software

; NOT DESIGNED FOR SPEED!

; WARNING!!
; This no longer saves t0! It has a space in memory for it (text_regs),
;  but it won't be written there automatically.

text_addr       equr    t0
text_buf        equr    t1

text_char       equr    t2
text_char8      equr    t3
text_bmp        equr    s1
text_line       equr    s2
text_lineleft   equr    s3
text_pixleft    equr    a1
text_fgcolor    equr    a2
text_bgcolor    equr    a3
text_numoff     equr    t2
text_numval     equr    t3
text_numrefval  equr    s1
text_numshift   equr    s2
text_numcharcnt equr    s3

text_blit:      ;_tpa(sd,t0,0*8+text_regs)
                _tpa(sd,t1,1*8+text_regs)
                _tpa(sd,t2,2*8+text_regs)
                _tpa(sd,t3,3*8+text_regs)
                _tpa(sd,s0,4*8+text_regs)
                _tpa(sd,s1,5*8+text_regs)
                _tpa(sd,s2,6*8+text_regs)
                _tpa(sd,s3,7*8+text_regs)
                _tpa(sd,a0,8*8+text_regs)
                _tpa(sd,a1,9*8+text_regs)
                _tpa(sd,a2,10*8+text_regs)
                _tpa(sd,a3,11*8+text_regs)

                _tpa(lw,text_numcharcnt,text_type)
                bnez    text_numcharcnt,text_number
                nop
                _tpa(lw,text_addr,text_msg)
return_from_text_number
                _tpa(lwu,text_fgcolor,text_color)
                _tpa(lwu,text_bgcolor,text_bg)
loadxy
                _tpa(lw,a0,text_y)
                _tpa(lw,s0,$a4400008)
                mult    a0,s0
                mflo    s0
                _tpa(lw,a0,text_x)
                add     s0,a0
                sll     s0,1
                _tpa(lw,a0,$a4400004)
                add     text_buf,s0,a0
                lui     a0,$a000
                or      text_buf,a0
charloop:       li      s0,$0a  ; end of line character
                lbu     text_char,(text_addr)
                addi    text_addr,1
                bne     text_char,s0,eol_not
                li      s0,$0b
                _tpa(lw,s0,text_y)
                addi    s0,8
                _tpa(sw,s0,text_y)
                j       loadxy
                nop
eol_not         bne     text_char,s0,grey_not ; half-intensity grey
                li      s0,$0c
                li      text_fgcolor,(8<11)|(8<6)|(8<1)|1
                j       charloop
                nop
grey_not        bne     text_char,s0,white_not ; full white
                nop
                li      text_fgcolor,$ffff
                j       charloop
                nop
white_not       beqz    text_char,end
                li      text_line,0
                li      text_lineleft,7
lineloop:       sll     text_char8,text_char,3
                lui     s0,text_font>16
                add     s0,text_char8
                add     s0,text_line
                lbu     text_bmp,text_font&0xffff(s0)
                addi    text_line,1
                li      text_pixleft,7
pixloop:        andi    s0,text_bmp,0x80
                beqz    s0,black
                move    a0,text_bgcolor
                move    a0,text_fgcolor
black:          sh      a0,(text_buf)
                sll     text_bmp,1
                addi    text_buf,2
                bnez    text_pixleft,pixloop
                addi    text_pixleft,-1         ; end of pixloop
                _tpa(lw,text_bmp,$a4400008)
                sll     text_bmp,1
                add     text_buf,text_bmp
                addi    text_buf,-8*2
                bnez    text_lineleft,lineloop  ; end of lineloop
                addi    text_lineleft,-1
                sll     text_bmp,3
                sub     text_buf,text_bmp
                addi    text_buf,8*2
                j       charloop                ; end of charloop
                nop

end:            ;_tpa(ld,t0,0*8+text_regs)
                _tpa(ld,t1,1*8+text_regs)
                _tpa(ld,t2,2*8+text_regs)
                _tpa(ld,t3,3*8+text_regs)
                _tpa(ld,s0,4*8+text_regs)
                _tpa(ld,s1,5*8+text_regs)
                _tpa(ld,s2,6*8+text_regs)
                _tpa(ld,s3,7*8+text_regs)
                _tpa(ld,a0,8*8+text_regs)
                _tpa(ld,a1,9*8+text_regs)
                _tpa(ld,a2,10*8+text_regs)
                _tpa(ld,a3,11*8+text_regs)
                
                jr      ra
                nop
text_number:    li      s0, 48
                _tpa(sb,s0,text_temp)
                li      s0, 120
                _tpa(sb,s0,text_temp+1)
                li      text_numoff, 2
                _tpa(lwu,text_numval,text_msg)
                la      text_numrefval, 0x0fffffff
                la      s0, 0xffffffff
                li      a0, 7
                sub     a0, text_numcharcnt
                sll     a0, 2
                srlv    text_numrefval, a0
                srlv    s0, a0
                and     text_numval, s0
                li      text_numshift, 28
                sub     text_numshift, a0
number_loop:    srlv    s0, text_numval, text_numshift
                _vtpa(lbu,a0,text_charset,s0)
                and     text_numval, text_numrefval
                _vtpa(sb,a0,text_temp,text_numoff)
                
                srl     text_numrefval, 4
                addi    text_numshift, -4
                addi    text_numoff, 1
                bnez    text_numcharcnt, number_loop
                addi    text_numcharcnt, -1

                lui     s0,text_temp>16
                addi    text_addr,s0,text_temp&0xffff
                add     s0,text_numoff
                sb      r0,text_temp&0xffff(s0)
                j       return_from_text_number
                nop

text_font       incbin "dos.raw"
text_charset    db      "0123456789ABCDEF"
                _align(8)
text_regs       dcw     13*2,0
text_x          dw      0
text_y          dw      0
text_msg        dw      0 ; pointer
text_scr        dw      0 ; also a pointer
text_type       dw      0
text_color      dw      0xffff
text_bg         dw      0x0001
text_temp       dcb     16,0 ; just to be even

; We demand rigidly defined areas of doubt and uncertainty!
