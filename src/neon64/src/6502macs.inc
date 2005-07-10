; MACROS!

#define sprdma_ppu(page) ld t4,page<4(A6502_regval)\
        ld      t5,page<4+8(A6502_regval)\
        sd      t4,page<4(s8)\
        sd      t5,page<4+8(s8)

#define monomac(addr) _tpa(lbu,t5,realpal+addr)\
        and     t5,t4\
        sll     t5,1\
        _vtpa(lhu,t5,nes_pal,t5)\
        _tpa(sh,t5,addr&$1c<2|(addr&3)<1+_bgpal)

; Various memory access
#define read_A6502(_addr,_target) srl       s8,_addr,8\
 sll    s8,2\
 addu   s8, A6502_nespage\
 lw     s8,(s8)\ ; The read table
 bnez   s8,pc+24\
 addu   s8,_addr\
 jalr   A6502_readfcn,A6502_return\
 move   A6502_regaddr,_addr\
 j      pc+12\
 move   _target,A6502_regval\
 lbu    _target,(s8)

#define write_A6502(_addr,_towrite) srl       s8,_addr,8\
 sll    s8,2\
 addu   s8, A6502_nespage\
 lw     s8,1024(s8)\ ; The write table
 bnez   s8,pc+28\
 addu   s8,_addr\
 move   A6502_regval,_towrite\
 jalr   A6502_writefcn,A6502_return\
 move   A6502_regaddr,_addr\
 j      pc+12\
 nop\
 sb     _towrite,(s8)

; rRAM and wRAM are used for zero page access
#define rRAM_A6502(_addr,_target) add    s8,A6502_mempage,_addr\
 lbu    _target, (s8)
#define wRAM_A6502(_addr,_target) add    s8,A6502_mempage,_addr\
 sb     _target, (s8)
#define rwordRAM_A6502(_addr,_target) add    s8,A6502_mempage,_addr\
 lbu    _target, (s8)\
 lbu    s8, 1(s8)\
 sll    s8, 8\
 or     _target,s8

; rstack and wstack are used for stack page access
#define rstack_A6502(_addr,_target) addu   s8,A6502_mempage,_addr\
 lbu    _target, 0x100(s8)
#define wstack_A6502(_addr,_target) addu   s8,A6502_mempage,_addr\
 sb     _target, 0x100(s8)

#define updatepcpage srl    s8,A6502_PC,6\
 andi   s8,~3\
 addu   s8,A6502_nespage\
 lw     A6502_PCpage,(s8)

; it is safe to use the same reg as both _addr and _target, as _target is
; only overwritten after _addr has been used the last time (in readop, readsop,
; and readword)

; paged access, but no register checking
#define readop_A6502(_addr,_target) srl       s8,_addr,6\
 andi   s8,~3\
 addu   s8, A6502_nespage\
 lw     s8,(s8)\
 beqz   s8,badopcoderead\
 addu   s8,_addr\
 lbu    _target,(s8)

; signed
#define readsop_A6502(_addr,_target) srl       s8,_addr,6\
 andi   s8,~3\
 addu   s8, A6502_nespage\
 lw     s8,(s8)\
 beqz   s8,badopcoderead\
 addu   s8,_addr\
 lb     _target,(s8)

; readword should work same as readop
; could be a problem at the end of an NES page... bah!
#define readword_A6502(_addr,_target) srl       s8,_addr,6\
 andi   s8,~3\
 addu   s8, A6502_nespage\
 lw     s8,(s8)\
 beqz   s8,badopcoderead\
 addu   s8,_addr\
 lbu    _target,(s8)\
 lbu    s8,1(s8)\
 sll    s8,8\
 or     _target,s8

; I now discard with clearing the bits here.
#define FL_A6502(_result) sltiu  s8, _result, 1\
 sll    s8, 1\
 or     A6502_flags, s8\
 andi   s8, _result, 0x80\
 or     A6502_flags, s8

; Addressing modes
; Any increment in PC is left to the code for the opcode itself, usually
;  it can take place during the return jump, in the delay slot
#define ABS_A6502(_target) readword_A6502(A6502_PC,_target)
#define ABSZP_A6502(_target) readop_A6502(A6502_PC,_target)
; INDEXXS and INDEXYS are for instructions that do a store, where the page
; boundary crossing isn't counted as another cycle (is always done)
#define INDEXXS_A6502(_target) readword_A6502(A6502_PC,_target)\
 add    _target,A6502_X\
 andi   _target,0xffff
#define INDEXYS_A6502(_target) readword_A6502(A6502_PC,_target)\
 add    _target,A6502_Y\
 andi   _target,0xffff
#define INDEXX_A6502(_target) readword_A6502(A6502_PC,_target)\
 andi   s8,_target,$ff\
 add    s8,A6502_X\
 srl    s8,8\   ; overflow bit
 sub    A6502_count,s8\
 add    _target,A6502_X\
 andi   _target,0xffff
#define INDEXY_A6502(_target) readword_A6502(A6502_PC,_target)\
 andi   s8,_target,$ff\
 add    s8,A6502_Y\
 srl    s8,8\
 sub    A6502_count,s8\
 add    _target,A6502_Y\
 andi   _target,0xffff
#define INDEXXZP_A6502(_target) readop_A6502(A6502_PC,_target)\
 add    _target,A6502_X\
 andi   _target,0xff
#define INDEXYZP_A6502(_target) readop_A6502(A6502_PC,_target)\
 add    _target,A6502_Y\
 andi   _target,0xff
; Does it need to be this complicated to incorporate the bug?
; This actually does the jump, BTW
#define INDIRECT_A6502(_2temp,_temp) readword_A6502(A6502_PC,_temp)\
 srl    s8,_temp,6\
 andi   s8,~3\
 addu   s8,A6502_nespage\
 lw     s8,(s8)\
 addu   s8,_temp\
 lbu    _2temp,(s8)\
 addi   s8, _temp, 1\
 andi   s8, 0xff\
 andi   _temp, ~0xff\
 or     _temp, s8\
 srl    s8,_temp,6\
 andi   s8,~3\
 addu   s8,A6502_nespage\
 lw     s8,(s8)\
 addu   s8,_temp\
 lbu    _temp,(s8)\
 sll    _temp, 8\
 or     A6502_PC,_2temp, _temp
#define INDIRECTX_A6502(_target) readop_A6502(A6502_PC,_target)\
 add    _target, A6502_X\
 andi   _target,0xff\
 rwordRAM_A6502(_target,_target)
;#define INDIRECTY_A6502(_target) readop_A6502(A6502_PC,_target)\
; rwordRAM_A6502(_target,_target)\
; add    _target, A6502_Y\
; andi   _target,0xffff
#define INDIRECTY_A6502(_target) readop_A6502(A6502_PC,_target)\
 rwordRAM_A6502(_target,_target)\
 andi   s8,_target,$ff\
 add    s8,A6502_Y\
 srl    s8,8\
 sub    A6502_count,s8\
 add    _target, A6502_Y\
 andi   _target,0xffff
; Actually does the branch
; The increment will be done elsewhere
#define BRANCH_A6502 andi    t0,A6502_PC,$ff00\
 readsop_A6502(A6502_PC,s8)\
 add     A6502_PC,s8\
 andi    t1,A6502_PC,$ff00\
 sub     t0,t1\
 andi    t0,1\   ; either 1 or -1, both have low bit set
 sub     A6502_count,t0

;#define BRANCH_A6502 readsop_A6502(A6502_PC,s8)\
; add     A6502_PC,s8

; Macros for operations
#define ADC_A6502(_addend,_temp,_2temp) xor    _temp,A6502_A,_addend\
 xori   _2temp,_temp,0x80\
 andi   _temp,A6502_flags,F_C\
 add    A6502_A,_temp\
 add    A6502_A,_addend\
 andi   A6502_flags,~(F_C|F_Z|F_S|F_V)\
 xor    _temp, _addend, A6502_A\
 and    _temp,_2temp\
 andi   _temp,0x80\
 srl    _temp,1\
 or     A6502_flags,_temp\
 sltiu  _temp, A6502_A, 0x100\
 xori   _temp,F_C\
 or     A6502_flags,_temp\
 andi   A6502_A,0xff\
 FL_A6502(A6502_A)

#define SBC_A6502(_sub,_temp,_2temp) xor _2temp,_sub,A6502_A\
 andi   _temp,A6502_flags,F_C\
 xori   _temp,F_C\
 sub    _temp,A6502_A,_temp\
 sub    _temp,_sub\
 andi   A6502_flags,~(F_C|F_S|F_V|F_Z)\
 xor    A6502_A,A6502_A,_temp\
 and    _2temp,A6502_A\
 andi   _2temp,0x80\
 srl    _2temp,1\
 or     A6502_flags,_2temp\
 sltiu  _2temp,_temp,0x100\
 or     A6502_flags,_2temp\
 andi   A6502_A,_temp,0xff\
 FL_A6502(A6502_A)

#define CMP_A6502(_reg,_sub,_temp,_2temp) sub  _temp,_reg,_sub\
 andi   A6502_flags,~(F_S|F_Z|F_C)\
 andi   _2temp,_temp,0xff\
 FL_A6502(_2temp)\
 sltiu  _temp,_temp,0x100\
 or     A6502_flags,_temp

#define BIT_A6502(_reg) andi A6502_flags,~(F_S|F_V|F_Z)\
 andi   s8,_reg,0xC0\
 or     A6502_flags,s8\
 and    s8,A6502_A,_reg\
 sltiu  s8,s8,1\
 sll    s8,1\
 or     A6502_flags,s8

#define AND_A6502(_mem) andi    A6502_flags,~(F_Z|F_S)\
 and     A6502_A,_mem\
 FL_A6502(A6502_A)

#define ASL_A6502(_mem) andi    A6502_flags,~(F_Z|F_S|F_C)\
 sll     _mem,1\
 andi   s8,_mem,0x100\
 srl    s8, 8\
 or     A6502_flags,s8\
 andi   _mem,0xff\
 FL_A6502(_mem)

; Can be used for DEX and DEY, too.
#define DEC_A6502(_mem) andi    A6502_flags,~(F_Z|F_S)\
 addi   _mem,-1\
 andi   _mem,0xff\
 FL_A6502(_mem)

#define EOR_A6502(_mem) andi    A6502_flags,~(F_Z|F_S)\
 xor     A6502_A,_mem\
 FL_A6502(A6502_A)

; Can be used for INX and INY, too.
#define INC_A6502(_mem) andi    A6502_flags,~(F_Z|F_S)\
 addi   _mem,1\
 andi   _mem,0xff\
 FL_A6502(_mem)

; logical shift right.
#define LSR_A6502(_mem) andi    A6502_flags,~(F_Z|F_S|F_C)\
 andi   s8,_mem,1\
 or     A6502_flags,s8\
 srl    _mem,1\
 sltiu  s8,_mem,1\
 sll    s8,1\
 or     A6502_flags,s8

#define ORA_A6502(_mem) andi    A6502_flags,~(F_Z|F_S)\
 or     A6502_A,_mem\
 FL_A6502(A6502_A)

#define PHA_A6502 wstack_A6502(A6502_S,A6502_A)\
 addi   A6502_S,-1\
 andi   A6502_S,0xff
 
#define PHP_A6502 ori A6502_flags,$30\
 wstack_A6502(A6502_S,A6502_flags)\
 addi   A6502_S,-1\
 andi   A6502_S,0xff

#define PLA_A6502 andi    A6502_flags,~(F_Z|F_S)\
 addi   A6502_S,1\
 andi   A6502_S,0xff\
 rstack_A6502(A6502_S,A6502_A)\
 FL_A6502(A6502_A)

#define PLP_A6502 addi   A6502_S,1\
 andi   A6502_S,0xff\
 rstack_A6502(A6502_S,A6502_flags)

#define ROL_A6502(_mem) sll     _mem,1\
 andi   s8, A6502_flags,1\
 or     _mem,s8\
 andi   A6502_flags,~(F_Z|F_S|F_C)\
 andi   s8,_mem,0x100\
 srl    s8,8\
 or     A6502_flags,s8\
 andi   _mem,0xff\
 FL_A6502(_mem)

#define ROR_A6502(_mem) andi    s8, A6502_flags, F_C\
 sll    s8, 8\
 or     _mem,s8\
 andi   A6502_flags,~(F_Z|F_S|F_C)\
 andi   s8, _mem, 1\
 or     A6502_flags,s8\
 srl    _mem, 1\
 FL_A6502(_mem)
