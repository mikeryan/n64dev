; General-purpose macros for use with U64ASM

; Because it's sign extended I need an extra instruction to deal with
;  bit 15. Grr. And I have no conditional assembly, so it happens everywhere.
#define _tpa(instr,reg,addr) lui     s8,addr>16&0xffff\
                             ori     s8,addr&0x8000\
                             instr   reg,addr&0x7fff(s8)
#define _vtpa(instr,_reg,addr,_2reg) lui        s8,addr>16&0xffff\
                                add     s8,_2reg\
                                ori     s8,addr&0x8000\
                                instr   _reg,addr&0x7fff(s8)
#define _align(_size) dcb _size-(pc%_size),0

#define deadend j       pc\
        nop

; Macros specific to Neon64

; Made to fill something needed from gbi.h and a useful macro to have around
; in any case.
#define _SHIFTL(_valtoshift,_shiftamt,_masksize) (_valtoshift&(2^_masksize-1)<_shiftamt)

#define rsp_go          _tpa(lhu,t0,_VRAM_V)\
                        li      t1,sp_set_ppugo\
                        _tpa(sw,t0,_VRAM_V2)\
                        _tpa(cache,25,_VRAM_V2)\
                        lui     s8,sp_base_reg>16\
                        sw      t1,sp_status_reg(s8)

#define rsp_done_wait   lui     s8,sp_base_reg>16\
                        lw      t1,sp_status_reg(s8)\
                        andi    t1,0x100\       ; signal 1, ppu done
                        beqz    t1,pc-8\
                        nop\
                        li      t1,0x800\       ; clear signal 1
                        sw      t1,sp_status_reg(s8)

; For use with the text renderer in text.asm

#define _text_string(_x_,_y_,_char_) \
                _tpa(sd,t0,text_regs)\
                _tpa(sd,ra,12*8+text_regs)\
                li      t0,_x_\
                _tpa(sw,t0,text_x)\
                li      t0,_y_\
                _tpa(sw,t0,text_y)\
                la      t0,_char_\
                _tpa(sw,t0,text_msg)\
                _tpa(sw,r0,text_type)\
                jal     text_blit\
                nop\
                _tpa(ld,t0,text_regs)\
                _tpa(ld,ra,12*8+text_regs)

#define _text_reg_string(_x_,_y_,_char_) \
                _tpa(sw,_char_,text_msg)\
                _tpa(sd,t0,text_regs)\
                _tpa(sd,ra,12*8+text_regs)\
                li      t0,_x_\
                _tpa(sw,t0,text_x)\
                li      t0,_y_\
                _tpa(sw,t0,text_y)\
                _tpa(sw,r0,text_type)\
                jal     text_blit\
                nop\
                _tpa(ld,t0,text_regs)\
                _tpa(ld,ra,12*8+text_regs)

; val is a register
#define _text_value(_x_,_y_,_value_,_size_) \
                _tpa(sw,_value_,text_msg)\
                _tpa(sd,t0,text_regs)\
                _tpa(sd,ra,12*8+text_regs)\
                li      t0,_x_\
                _tpa(sw,t0,text_x)\
                li      t0,_y_\
                _tpa(sw,t0,text_y)\
                li      t0,_size_\
                _tpa(sw,t0,text_type)\
                jal     text_blit\
                nop\
                _tpa(ld,t0,text_regs)\
                _tpa(ld,ra,12*8+text_regs)
