; General purpose macros
; (c)2002-2003 Halley's Comet Software

#define _tpa(instr,reg,addr) lui     s8,addr>16&0xffff\
                             instr   reg,addr&0xffff(s8)
#define _align(_size) \
    dcb _size-(pc%_size),0

#define deadend \
        j        pc\
        nop
