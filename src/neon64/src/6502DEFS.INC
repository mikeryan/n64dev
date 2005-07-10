#imacros 6502macs.inc 

A6502_flags     equr    s0
A6502_PC        equr    s1
A6502_X         equr    s2
A6502_Y         equr    s3
A6502_A         equr    s4
A6502_S         equr    s5
A6502_count     equr    s6
A6502_nespage   equr    s7
A6502_mempage   equr    a0
A6502_readfcn   equr    a1
A6502_writefcn  equr    a2
A6502_return    equr    a3
A6502_regaddr   equr    v0
A6502_regval    equr    v1
;A6502_opcodes   equr    k0
;A6502_PCpage    equr    k0
A6502_instrcnt  equr    k0

F_C             equne   0x01
F_Z             equne   0x02
F_I             equne   0x04
F_D             equne   0x08
F_B             equne   0x10
F_V             equne   0x40
F_S             equne   0x80
