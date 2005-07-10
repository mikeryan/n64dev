; Controller and controller pack access
; Information almost entirely from LaC's hardware dox v0.8

        ; Send command block to PI, address of block in t0
        ; Returns result in PI_result
exec_PI
        lui     s8,0xa480
        la      t1,0x1fc007c0
        sw      t0,0(s8)
        sw      t1,0x10(s8)        ; cause DRAM->PIF
siwaitw1
        lw      t0,0x18(s8)
        andi    t0,3            ; DMA or I/O busy
        bnez    t0,siwaitw1
        nop

        la      t0,PI_result_block&0x1fffffff
        la      t1,0x1fc007c0
        sw      t0,0(s8)
        sw      t1,4(s8)        ; cause PIF->DRAM
siwaitr
        lw      t0,0x18(s8)
        andi    t0,3            ; DMA or I/O busy
        bnez    t0,siwaitr
        nop
        jr      ra
        nop

        ; Get controller status
        ; If there's a controller in port 1 PI_result_block+4 != $ff
        ; If there's a controller pack in controller 1 PI_result_block+6 = 1
getconstatus
        _tpa(sw,ra,pack_ra)
        la      t0,get_status_block
        jal     exec_PI
        nop
        _tpa(lw,ra,pack_ra)
        jr      ra
        nop

resetcon
        _tpa(sw,ra,pack_ra)
        la      t0,reset_con_block
        jal     exec_PI
        nop
        _tpa(lw,ra,pack_ra)
        jr      ra
        nop

        ; Read from controller pack in controller 1
        ; t0: pointer to destination
        ; t1: page # (32 byte-wise) to read from
read_conpack
        _tpa(sw,ra,pack_ra)
        _tpa(sw,t0,pack_t)
        move    t0,t1
        jal     calcadrchecksum
        nop
        andi    t2,t0,0xff00
        srl     t2,8
        _tpa(sb,t2,read_cart_pack_block+3|0xa0000000)
        _tpa(sb,t0,read_cart_pack_block+4|0xa0000000)

        la      t0,read_cart_pack_block&0x1fffffff
        jal     exec_PI
        nop

        ; Copy into destination
        _tpa(lw,t0,pack_t)
        li      t1,31
        la      t2,PI_result_block+5|0xa0000000
readpackloop
        lb      t3,(t2)
        sb      t3,(t0)
        addi    t2,1
        addi    t0,1
        bnez    t1,readpackloop
        addi    t1,-1

        _tpa(lw,ra,pack_ra)

        jr      ra
        nop

        ; Write to controller pack in controller 1
        ; t0: pointer to 32 bytes to write
        ; t1: page # (32 byte-wise) to write to
write_conpack
        _tpa(sw,ra,pack_ra)
        _tpa(sw,t1,pack_t)

        ; Copy into block
        li      t1,31
        la      t2,write_cart_pack_block+5|0xa0000000
writepackloop
        lb      t3,(t0)
        sb      t3,(t2)
        addi    t2,1
        addi    t0,1
        bnez    t1,writepackloop
        addi    t1,-1

        _tpa(lw,t0,pack_t)
        jal     calcadrchecksum
        nop
        andi    t2,t0,0xff00
        srl     t2,8
        _tpa(sb,t2,write_cart_pack_block+3|0xa0000000)
        _tpa(sb,t0,write_cart_pack_block+4|0xa0000000)

        la      t0,write_cart_pack_block&0x1fffffff
        jal     exec_PI
        nop

        _tpa(lw,ra,pack_ra)
        
        jr      ra
        nop

        ; Controller Pack Address CRC
        ; address is in t0
        ; checksummed address in t0
calcadrchecksum
        li      t1,15
        li      s8,0
adrchkloop
        andi    t2,s8,$10
        beqz    t2,noxor_addr
        sll     s8,1
        li      t2,$15
noxor_addr
        andi    t3,t0,$400
        srl     t3,10
        or      s8,t3
        sll     t0,1
        xor     s8,t2

        bnez    t1,adrchkloop
        addi    t1,-1

        srl     t0,16-5
        andi    s8,$1f

        jr      ra
        or      t0,s8

        ; returns button values in t0,t1
read_con
        _tpa(sw,ra,pack_ra)
        la      t0,read_con_block&0x1fffffff
        jal     exec_PI
        nop
        _tpa(lbu,t0,PI_result_block+2|0xa0000000)
        andi    t0,0xc0
        bnez    t0,nocon1
        li      t0,0
        _tpa(lwu,t0,PI_result_block+4|0xa0000000)
nocon1
        _tpa(lbu,t1,PI_result_block+10|0xa0000000)
        andi    t1,0xc0
        bnez    t1,nocon2
        li      t1,0
        _tpa(lwu,t1,PI_result_block+12|0xa0000000)
nocon2
        _tpa(lw,ra,pack_ra)
        jr      ra
        nop

; Also prevents reset!
; returns which button was pressed in t0
Pause   _tpa(sw,ra,pausera)
ploop   jal     read_con
        nop
        srl     t0,16
        bnez    t0,ploop
        nop
ploop2  jal     read_con
        nop
        srl     t0,16
        beqz    t0,ploop2
        nop
        _tpa(lw,ra,pausera)
        jr      ra
        nop

pack_ra dw      0
pack_t  dw      0
conadr  dw      0x1f0
pausera dw      0
        _align(8)
read_con_block
        dw      0xff010401,0xffffffff
        dw      0xff010401,0xffffffff
        dw      0xfe000000,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,1
read_cart_pack_block
        dw      0x03210200,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0xfe000000,1
write_cart_pack_block
        dw      0x23010300,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      0xfe000000,1
get_status_block
        dw      0xff010300,0xffffffff
        dw      0xff010300,0xffffffff
        dw      0xff010300,0xffffffff
        dw      0xff010300,0xffffffff
        dw      0xfe000000,0
        dw      0,0
        dw      0,0
        dw      0,1
reset_con_block
        dw      0xff0103ff,0xffffffff
        dw      0xff0103ff,0xffffffff
        dw      0xff0103ff,0xffffffff
        dw      0xff0103ff,0xffffffff
        dw      0xfe00000,0
        dw      0,0
        dw      0,0
        dw      0,1

PI_result_block
        dcb     64,0xff
