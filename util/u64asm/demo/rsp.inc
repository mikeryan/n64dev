; RSP DMA and otherwise functions
; (c) 2002-2003 Halley's Comet Software

        _align(4)
        
        ; address to boot from is in a1
boot_rsp:
        _tpa(sd,ra,rspreturn)   ; save return address
        
        la      a0,$1000        ; set the RSP's PC
        jal     SPsetPC
        nop
        li      a0,0x1000   ; address in RSP to write to (0x1000 is start of
        ; address in a1     ; IMEM)
        li      a2,$fff
        li      a3,1
        jal     SPrawDMA        ; transfer the code
        nop

        jal     SPdeviceBUSY    ; wait for transfer to complete
        nop

        li      a0,$25          ; start the RSP
        jal     SPsetSTATUS
        nop
        
        _tpa(ld,ra,rspreturn)
        jr      ra
        nop

SPsetSTATUS:
        _tpa(sw,a0,0xa4040010)
        jr      ra
        nop

SPsetPC:
        _tpa(lw,t0,$a4040010)
        andi    t0,1
        beqz    t0,SPsetPC
        nop
        _tpa(sw,a0,$a4080000)
        jr      ra
        nop

SPrawDMA:
        la      t0,$a4040000    ; if a3=1 then rdram->rsp
        sw      a0,0(t0)        ; a0=rspdest
        sw      a1,4(t0)        ; a1=dram
        beqzl   a3,@RSP_DRAM    ; else rsp->rdram
        sw      a2,$c(t0)       ; a2=size
        sw      a2,8(t0)
@RSP_DRAM: jr   ra
        nop

SPdeviceBUSY:
        _tpa(lw,t0,$a4040010)
        andi    t0,$1c
        bnez    t0,SPdeviceBUSY
        nop
        jr      ra
        nop
        
        _align(8)
rspreturn       dw      0,0
