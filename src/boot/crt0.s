org  $80000400 ; entry point (the (standard) header loads your
                                  ; code to $80000400)

    ; always do this first thing, otherwise the N64
        ; crashes in approx. 5 seconds
        li   t1,8
        lui    t0,$bfc0
        sw   t1,$07fc(t0)

        ; Initialize video (I won't go into what all the
        ; values mean, but it initializes the screen to
        ; 320x240 16 bit color mode)
        lui   t0,$a440 ; VI register base
        li   t1, $103002
        sw   t1,0(t0)
        la   t1,0xa0200000 ; the frame buffer address
        sw   t1,4(t0)
        li   t1,320
        sw   t1,8(t0)
        li   t1,$2
        sw   t1,12(t0)
        li   t1,$0
        sw   t1,16(t0)
        li   t1,$3e52239
        sw   t1,20(t0)
        li   t1,$0000020d
        sw   t1,24(t0)
        li   t1,$00000c15
        sw   t1,28(t0)
        li   t1,$0c150c15
        sw   t1,32(t0)
        li   t1,$006c02ec
        sw   t1,36(t0)
        li   t1,$002501ff
        sw   t1,40(t0)
        li   t1,$000e0204
        sw   t1,44(t0)
        li   t1,$200
        sw   t1,48(t0)
        li   t1,$400
        sw   t1,52(t0)

        ; clear the screen
        la   t0,0xa0200000
        li   t1,320*240*2-2
        li   t2,0 ; black
loopclear:
        sh   t2,0(t0)
        add  t0,2
        bnez t1,loopclear
        sub  t1,2

deadend
        j   deadend
        nop
