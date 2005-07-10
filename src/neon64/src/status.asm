; Status reports (take forever to process)

#include text.asm
unrecoverable_exception:
                _text_string(13,24,except)
                andi    k1,0xff
                srl     k1,2
                _text_value(8*10+13,24,k1,1)
                _text_string(8*14+13,24,errcode)
                mfc0    k1,epc
                _text_value(8*21+13,24,k1,7)
                ;mfc0    k1,epc
                ;nop
                ;lwu     k1,(k1)
                ;_text_value(13,32,k1,7)

                mfc0    k1,badvaddr
                _text_string(13,40,badvaddrval)
                _text_value(8*9+13,40,k1,7)
                _text_string(13,8*1+40,atval)
                _text_value(8*3+13,8*1+40,at,7)
                _text_string(13,8*2+40,v0val)
                _text_value(8*3+13,8*2+40,v0,7)
                _text_string(13,8*3+40,v1val)
                _text_value(8*3+13,8*3+40,v1,7)
                _text_string(13,8*4+40,a0val)
                _text_value(8*3+13,8*4+40,a0,7)
                _text_string(13,8*5+40,a1val)
                _text_value(8*3+13,8*5+40,a1,7)
                _text_string(13,8*6+40,a2val)
                _text_value(8*3+13,8*6+40,a2,7)
                _text_string(13,8*7+40,a3val)
                _text_value(8*3+13,8*7+40,a3,7)

                _text_string(13,8*8+40,t0val)
                _text_value(8*3+13,8*8+40,t0,7)
                _text_string(13,8*9+40,t1val)
                _text_value(8*3+13,8*9+40,t1,7)
                _text_string(13,8*10+40,t2val)
                _text_value(8*3+13,8*10+40,t2,7)
                _text_string(13,8*11+40,t3val)
                _text_value(8*3+13,8*11+40,t3,7)
                _text_string(13,8*12+40,t4val)
                _text_value(8*3+13,8*12+40,t4,7)
                _text_string(13,8*13+40,t5val)
                _text_value(8*3+13,8*13+40,t5,7)
                _text_string(13,8*14+40,t6val)
                _text_value(8*3+13,8*14+40,t6,7)
                _text_string(13,8*15+40,t7val)
                _text_value(8*3+13,8*15+40,t7,7)

                _text_string(13+108,8*1+40,s0val)
                _text_value(8*3+13+108,8*1+40,s0,7)
                _text_string(13+108,8*2+40,s1val)
                _text_value(8*3+13+108,8*2+40,s1,7)
                _text_string(13+108,8*3+40,s2val)
                _text_value(8*3+13+108,8*3+40,s2,7)
                _text_string(13+108,8*4+40,s3val)
                _text_value(8*3+13+108,8*4+40,s3,7)
                _text_string(13+108,8*5+40,s4val)
                _text_value(8*3+13+108,8*5+40,s4,7)
                _text_string(13+108,8*6+40,s5val)
                _text_value(8*3+13+108,8*6+40,s5,7)
                _text_string(13+108,8*7+40,s6val)
                _text_value(8*3+13+108,8*7+40,s6,7)
                _text_string(13+108,8*8+40,s7val)
                _text_value(8*3+13+108,8*8+40,s7,7)

                _text_string(13+108,8*9+40,t8val)
                _text_value(8*3+13+108,8*9+40,t8,7)
                _text_string(13+108,8*10+40,t9val)
                _text_value(8*3+13+108,8*10+40,t9,7)
                _text_string(13+108,8*11+40,k0val)
                _text_value(8*3+13+108,8*11+40,k0,7)
                _text_string(13+108,8*12+40,k1val)
                _text_value(8*3+13+108,8*12+40,k1,7)
                _text_string(13+108,8*13+40,gpval)
                _text_value(8*3+13+108,8*13+40,gp,7)
                _text_string(13+108,8*14+40,spval)
                _text_value(8*3+13+108,8*14+40,sp,7)

                _text_string(13+108,8*15+40,raval)
                _tpa(lw,t1,rasave)
                _text_value(8*3+13+108,8*15+40,t1,7)
                _text_string(13+108,8*16+40,s8val)
                _tpa(lw,t1,s8save)
                _text_value(8*3+13+108,8*16+40,t1,7)

                jr      ra
                nop

; Debug Status

A6502_status:
                _text_string(13,12,sptxt)
                _text_value(37,12,A6502_S,1)
                _text_string(13,20,pctxt)
                _text_value(45,20,A6502_PC,3)
                _text_string(13,28,ftxt)
                _text_value(37,28,A6502_flags,3)
                _text_string(13,36,xtxt)
                _text_value(37,36,A6502_X,1)
                _text_string(13,44,ytxt)
                _text_value(37,44,A6502_Y,1)
                _text_string(13,52,atxt)
                _text_value(37,52,A6502_A,1)

                _tpa(lwu,t1,pchcnt)
                _text_string(13,60,histtxt)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,60,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,68,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,76,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,84,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,92,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,100,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,108,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,116,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,124,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,132,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,140,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,148,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,156,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,164,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,172,t0,3)

                addi    t1,-2
                andi    t1,0x1e
                _vtpa(lh,t0,pchist,t1)
                _text_value(61,180,t0,3)

                _text_string(101,12,ppu1txt)
                _tpa(lw,t0,_PPUControl1)
                _text_value(149,12,t0,1)
                _text_string(101,20,ppu2txt)
                _tpa(lw,t0,_PPUControl2)
                _text_value(149,20,t0,1)
                _text_string(101,28,ppustxt)
                ;_tpa(lw,t0,sp_base_reg+sp_status_reg)
                ;srl     t0,7
                _tpa(lbu,t0,ppustatus)
                _text_value(173,28,t0,1)
                _text_string(101,36,pputtxt)
                _tpa(lw,t0,_VRAM_T)
                _text_value(149,36,t0,3)
                _text_string(101,44,ppuvtxt)
                _tpa(lhu,t0,_VRAM_V)
                _text_value(149,44,t0,3)
                _text_string(101,52,ctxt)
                srl t0,A6502_PC,8
                sll t0,2
                addu    t0,A6502_nespage
                lw      t0,(t0)
                beqz    t0,status_badpage
                addu    t0,A6502_PC
                _text_value(133,52,t0,7)
                _text_string(213,52,equtxt)
                lbu     t1,(t0)
                _text_value(221,52,t1,1)
                lbu     t1,1(t0)
                _text_value(221,60,t1,1)
                lbu     t1,2(t0)
                _text_value(221,68,t1,1)
status_badpage
                jr      ra
                nop

sptxt           db      "S: ",0
pctxt           db      "PC: ",0
ftxt            db      "F: ",0
xtxt            db      "X: ",0
ytxt            db      "Y: ",0
atxt            db      "A: ",0
ppu1txt         db      "PPU1: ",0
ppu2txt         db      "PPU2: ",0
ppustxt         db      "PPUSTAT: ",0
pputtxt         db      "PPUT: ",0
ppuvtxt         db      "PPUV: ",0
ctxt            db      "PC: ",0
histtxt         db      "DATA: ",0
equtxt          db      "=",0

except          db      "Exception "
errcode         db      " at PC ",0
badvaddrval     db      "badvaddr=          ",0

atval           db      "at=          ",0
v0val           db      "v0=          ",0
v1val           db      "v1=          ",0
a0val           db      "a0=          ",0
a1val           db      "a1=          ",0
a2val           db      "a2=          ",0
a3val           db      "a3=          ",0
t0val           db      "t0=          ",0
t1val           db      "t1=          ",0
t2val           db      "t2=          ",0
t3val           db      "t3=          ",0
t4val           db      "t4=          ",0
t5val           db      "t5=          ",0
t6val           db      "t6=          ",0
t7val           db      "t7=          ",0
s0val           db      "s0=          ",0
s1val           db      "s1=          ",0
s2val           db      "s2=          ",0
s3val           db      "s3=          ",0
s4val           db      "s4=          ",0
s5val           db      "s5=          ",0
s6val           db      "s6=          ",0
s7val           db      "s7=          ",0
t8val           db      "t8=          ",0
t9val           db      "t9=          ",0
k0val           db      "k0=          ",0
k1val           db      "k1=          ",0
gpval           db      "gp=          ",0
spval           db      "sp=          ",0
s8val           db      "s8=          ",0
raval           db      "ra=          ",0
                _align(4)
 report
