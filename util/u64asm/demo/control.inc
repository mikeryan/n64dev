; Controller access routines
; (c) 2002-2003 Halley's Comet Software

        _align(4)   ; all instructions must be word-aligned

; Requests that controller data be read
initcontroller:
        lui     s8, $a480
        lui     s7, $bfc0
        la      t0,datapad-$80000000
        sw      t0,(s8)
        la      t0,$1fc007c0
        sw      t0,$0010(s8)
        jr      ra
        nop
        
; Controller 1 buttons are returned in t0
readcontroller:
        lui     s8, $a480
        lui     s7, $bfc0
        li      t6, 0
        la      t2,datapad2-$80000000
        sw      t2,(s8)
        la      t2,$1fc007c0
        sw      t2,$0004(s8)
        lhu     t0,$07c4(s7)  ; buttons
        lb      t1,$07c6(s7)  ; analog x
        lb      t2,$07c7(s7)  ; analog y
        jr      ra
        nop

detectcontroller:
        lui     s8, $a480
        lui     s7, $bfc0
        li      t6, 0
        la      t2,datapad2-$80000000
        sw      t2,(s8)
        la      t2,$1fc007c0
        sw      t2,$0004(s8)
        li      t3, 0
        lbu     t0,$07c2(s7)  ; controller 1
        andi    t0, 0x80
        beqzl   t0, not_con1
        ori     t3, 1
not_con1:
        lbu     t0,$07c2+8(s7)  ; controller 2
        andi    t0, 0x80
        beqzl   t0, not_con2
        ori     t3, 2
not_con2:
        lbu     t0,$07c2+16(s7)  ; controller 3
        andi    t0, 0x80
        beqzl   t0, not_con3
        ori     t3, 4
not_con3:
        lbu     t0,$07c2+24(s7)  ; controller 4
        andi    t0, 0x80
        beqzl   t0, not_con4
        ori     t3, 8
not_con4:
        move    t0, t3
        jr      ra
        nop
         
        _align(64)
datapad
        dw      $ff010401,0
        dw      0,0
        dw      0,0
        dw      0,0
        dw      $fe000000,0
        dw      0,0
        dw      0,0
        dw      0,1
datapad2
        dcb  64,0
