        ; initializes timing for reading from SRAM
        ; cribbed from LaC's docs
init_sram
        lui     s8, $a460       ; PI

        li      t0, $05
        sw      t0, $0024(s8)   ; latency

        li      t0, $0c
        sw      t0, $0028(s8)   ; R/W strobe pulse width

        li      t0, $0d
        sw      t0, $002c(s8)   ; page size

        li      t0, $02
        sw      t0, $0030(s8)   ; R/W release duration

        jr      ra
        nop

        ; Read from SRAM
        ; t0: pointer to destination
        ; t1: page # (32 byte-wise) to read from
read_sram
        la      s8, $1fffffff
        and     t0, t0, s8      ; uncached RAM address

        ; SRAM starts at $0800:0000
        lui     s8, $0800

        ; multiply t1 by 32 to get offset in bytes
        li      t2, 32
        mult    t1, t2
        mflo    t1
        or      t1, s8          ; SRAM address

        lui     s8, $a460       ; PI
        sw      t0, $0000(s8)   ; PI DMA RAM address
        sw      t1, $0004(s8)   ; PI DMA SRAM address

        li      t0, 31
        sw      t0, $000c(s8)   ; read 32 bytes (-1 for DMA, don't ask)

        lw      t0, $0010(s8)   ; status reg
        nop
        andi    t0, 3
        bne     t0, zero, pc-12 ; loop until done
        nop

        jr      ra
        nop

        ; Write to SRAM
        ; t0: pointer to 32 bytes to write
        ; t1: page # (32 byte-wise) to write to
write_sram
        la      s8, $1fffffff
        and     t0, t0, s8      ; uncached RAM address

        ; SRAM starts at $0800:0000
        lui     s8, $0800

        ; multiply t1 by 32 to get offset in bytes
        li      t2, 32
        mult    t1, t2
        mflo    t1
        or      t1, s8          ; SRAM address

        lui     s8, $a460       ; PI
        sw      t0, $0000(s8)   ; PI DMA RAM address
        sw      t1, $0004(s8)   ; PI DMA SRAM address

        li      t0, 31
        sw      t0, $0008(s8)   ; write 32 bytes (-1 for DMA, don't ask)

        lw      t0, $0010(s8)   ; status reg
        nop
        andi    t0, 3
        bne     t0, zero, pc-12 ; loop until done
        nop

        jr      ra
        nop
