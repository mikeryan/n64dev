        ; Screen capture 64
        ; t0: screen height (pixels)
        ; t1: target ROM location
        ; wastes s8 and t2 regs
        ; Format of buffer:
        ; 0-1: "AG" ; check code (my initials...)
        ; 2-3: height
        ; 4-5: width
        ; 6-7: color depth

screen_capture
        lui     s8, $a440       ; VI
        lw      t2, $0004(s8)   ; get screen buffer addr
        lui     s8, $a000
        or      t2, s8          ; uncached
        addi    t2, -8          ; back up for extra data
        lui     s8, $a460       ; PI
        sw      t2, $0000(s8)   ; PI DMA RAM addr
        sw      t1, $0004(s8)   ; PI DMA ROM (ha!) addr

        ld      t1, (t2)        ; preserve what's at our temp dword
        sh      t0, 2(t2)       ; write height
        lui     s8, $a440       ; VI
        lw      s8, $0008(s8)   ; get width
        sh      s8, 4(t2)       ; write width
        mult    s8, t0          ; multiply to get total pixels ...

        lui     s8, $a440       ; VI
        lw      s8, (s8)        ; control reg (for color depth)
        andi    s8, 1           ; assuming valid mode 0=16, 1=32
        li      t0, 1
        sll     t0, 4
        sllv    t0, s8          ; an extra shift if 32 bit
        sh      t0, 6(t2)
        la      t0, $4147       ; a check code
        sh      t0, 0(t2)

        mflo    t0
        sll     t0, 1           ; 2 bytes pp with 16-bit
        sllv    t0, s8          ; 4 bytes pp with 32-bit
        addi    t0, 8-1         ; extra info, -1 for DMA
        lui     s8, $a460
        sw      t0, $0008(s8)   ; transfer however many bytes needed

        lw      t0, $0010(s8)
        nop
        andi    t0, 3
        bne     t0, zero, pc-24
        nop

        sd      t1, (t2)        ; restore what was at the temp dword

        jr      ra
        nop
