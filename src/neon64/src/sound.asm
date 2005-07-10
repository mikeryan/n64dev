; audio generation
; Almost every line in this file is drawn from data in dmc.txt and
; nessound.txt by Brad Taylor (btmine@hotmail.com)
; The only improvisations are the errors :)

; Linear counter logic and mixing based on work by blargg

lineofsound
        _tpa(lb,t0,awhichdraw)
        beqz    t0,d_notabuf2
        li      t0,0
        la      t0,abufsize
d_notabuf2
        la      t1,abuf1
        addu    t0,t1
        _tpa(lhu,t1,abufdrawpos)
        addu    t0,t1

        li      t6,samplesperline-1
asamploop
; Square channel 1
        _tpa(lbu,t2,sq1len)
        beqz    t2,sq1_silent
        li      t1,0

        _tpa(lhu,t2,sq1cnt)
        addi    t2,-soundrate
        bgtz    t2,sq1_nochange
        nop

        ; reload
        _tpa(lbu,t3,asq1+3)
        _tpa(lbu,t4,asq1+2)
        andi    t3,7
        sll     t3,8
        or      t4,t3

        _tpa(lbu,t7,sq1en)
        beqz    t7,sq1_silent
        li      t1,0
        slti    t7,t4,8   ; sweep end condition
        bnez    t7,sq1_silent
        nop

        sll     t4,5
        neg     t2
        div     t2,t4
        mflo    t3      ; # of cycles this encompasses
        mfhi    t2      ; left over
        sub     t4,t2
        _tpa(sh,t4,sq1cnt)

        _tpa(lbu,t2,sq1duty)
        add     t2,t3
        addi    t2,1
        andi    t2,$f
        _tpa(sb,t2,sq1duty)
        j       sq1_nolduty
        move    t1,t2
sq1_nochange
        _tpa(lb,t1,sq1duty)
        _tpa(sh,t2,sq1cnt)
sq1_nolduty
        _tpa(lb,t2,asq1)
        andi    t4,t2,$c0
        srl     t4,2
        or      t4,t1
        _vtpa(lbu,t3,sqtab,t4)
        li      t1,0
        beqz    t3,sq1_silent
        nop
        ; active, use current volume
        _tpa(lbu,t1,sq1vol)
sq1_silent
        andi    t5,t1,$0f

; Square channel 2
        _tpa(lbu,t2,sq2len)
        beqz    t2,sq2_silent
        li      t1,0

        _tpa(lhu,t2,sq2cnt)
        addi    t2,-soundrate
        bgtz    t2,sq2_nochange
        nop
        ; reload
        _tpa(lbu,t3,asq2+3)
        _tpa(lbu,t4,asq2+2)
        andi    t3,7
        sll     t3,8
        or      t4,t3

        _tpa(lbu,t7,sq2en)
        beqz    t7,sq2_silent
        li      t1,0
        slti    t7,t4,8   ; sweep end condition
        bnez    t7,sq2_silent
        nop

        sll     t4,5
        neg     t2
        div     t2,t4
        mflo    t3      ; # of cycles this encompasses
        mfhi    t2      ; left over
        sub     t4,t2
        _tpa(sh,t4,sq2cnt)

        _tpa(lbu,t2,sq2duty)
        add     t2,t3
        addi    t2,1
        andi    t2,$f
        _tpa(sb,t2,sq2duty)
        j       sq2_nolduty
        move    t1,t2
sq2_nochange
        _tpa(lb,t1,sq2duty)
        _tpa(sh,t2,sq2cnt)
sq2_nolduty
        _tpa(lb,t2,asq2)
        andi    t4,t2,$c0
        srl     t4,2
        or      t4,t1
        _vtpa(lbu,t3,sqtab,t4)
        li      t1,0
        beqz    t3,sq2_silent
        nop
        ; active, use current volume
        _tpa(lbu,t1,sq2vol)
sq2_silent
        andi    t1,$0f
        add     t5,t1

        _vtpa(lbu,t8,square_table,t5)

; Triangle channel
        _tpa(lbu,t2,trilen)
        _tpa(lbu,t3,lincnt)
        beqz    t2,tri_nochange2 ;_silent
        nop

        beqz    t3,tri_nochange2 ;_silent
        nop

        _tpa(lhu,t2,tricnt)
        addi    t2,-soundrate
        bgtz    t2,tri_nochange
        nop
        ; reload
        _tpa(lbu,t3,atri+3)
        _tpa(lbu,t4,atri+2)

        andi    t3,7
        sll     t3,8
        or      t4,t3

        slti    t3,t4,8
        bnez    t3,tri_nochange2 ;silent
        nop

        sll     t4,5
        neg     t2
        div     t2,t4
        mflo    t3      ; # of cycles this encompasses
        mfhi    t2      ; left over
        sub     t4,t2
        _tpa(sh,t4,tricnt)

        _tpa(lbu,t2,triduty)
        add     t2,t3
        addi    t2,1
        andi    t2,$1f
        _tpa(sb,t2,triduty)

        j       tri_go
        nop
tri_nochange
        _tpa(sh,t2,tricnt)
tri_nochange2
        _tpa(lb,t2,triduty)
tri_go  _vtpa(lbu,t1,tritab,t2)
        andi    t1,$0f
        li      t5,3
        mult    t1,t5
        mflo    t5

; Noise channel
        _tpa(lbu,t2,rndlen)
        beqz    t2,rnd_silent
        li      t1,0

        _tpa(lhu,t2,rndcnt)
        addi    t2,-soundrate/2 ; clocked at CPU/2
        bgtz    t2,rnd_nochange
        nop
        ; reload
        _tpa(lb,t1,arnd+2)
        andi    t4,t1,$f
        sll     t4,1
        _vtpa(lhu,t4,frqtab,t4)

        sll     t4,5
        neg     t2
        div     t2,t4
        mflo    t7      ; # of cycles this encompasses
        mfhi    t2      ; left over
        sub     t4,t2
        _tpa(sh,t4,rndcnt)

        _tpa(lhu,t4,rndcval)
        andi    t1,$80
rndloop
        sll     t4,1
        andi    t2,t4,$8000
        srl     t2,15
        andi    t3,t4,$0200
        bnez    t1,rnd_not_32k
        srl     t3,9
        andi    t3,t4,$4000
        srl     t3,14
rnd_not_32k
        xor     t2,t3
        or      t4,t2
        bnez    t7,rndloop
        addi    t7,-1

        andi    t1,t4,0x8000
        xori    t1,0x8000
        andi    t4,0x7fff
        _tpa(sh,t4,rndcval)
        j       rnd_nolduty
        nop
rnd_nochange
        _tpa(sh,t2,rndcnt)
        _tpa(lh,t1,rndcval)
        andi    t1,0x4000
        xori    t1,0x4000
rnd_nolduty
        bnez    t1,rnd_silent
        li      t1,0
        _tpa(lb,t2,arnd)
        ; active, use current volume
        _tpa(lbu,t1,rndvol)
        andi    t1,$0f
rnd_silent
        sll     t1,1
        add     t5,t1

; Delta Modulation Channel


        _tpa(lbu,t2,aenable)
        andi    t2,$10
        beqz    t2,dmc_nochange2
        nop
        _tpa(lhu,t2,dmccnt)
        addi    t2,-soundrate
        bgtz    t2,dmc_nochange
        nop

        ; reload
        _tpa(lbu,t3,dmcfreq)
        andi    t3,$f
        sll     t3,1
        _vtpa(lhu,t4,dmcfrqtab,t3)

        sll     t4,5
        neg     t2
        div     t2,t4
        mflo    t3      ; # of cycles this encompasses
        mfhi    t2      ; left over
        sub     t4,t2
        _tpa(sh,t4,dmccnt)

        move    t7,t3
        _tpa(lbu,t2,dmcbitcnt)
        _tpa(lbu,t3,dmccurbit)
        _tpa(lbu,t1,dmcdelta)
        andi    t1,$7e
        srl     t1,1
dmcloop addi    t4,t2,-1
        beqz    t4,dmc_load
        nop
        bnez    t2,dmc_notload
        move    t2,t4
dmc_load
        _tpa(lhu,t2,dmcadr)
        _tpa(lhu,t4,dmclen)
        bnez    t4,dmcloop_still
        nop
        _tpa(lbu,t4,dmcfreq)
        andi    t2,t4,$40
        beqz    t2,dmc_noloop
        nop

        ; loop
        _tpa(lbu,t2,dmcadrload)
        _tpa(lbu,t4,dmclenload)
        sll     t2,6
        ori     t2,$4000
        sll     t4,4
        addi    t4,1
        j       dmcloop_still
        nop
dmc_noloop
        _tpa(lbu,t2,aenable)    ; dmc goes off
        andi    t2,~$10
        _tpa(sb,t2,aenable)

        andi    t4,$80
        beqz    t4,dmcloop_end
        li      t2,0

        ; IRQ
        _tpa(lbu,t4,astatus)
        ori     t4,$80  ; set DMC IRQ flag
        _tpa(sb,t4,astatus)

        j       dmcloop_end
        li      t2,0

dmcloop_still
        ori     t3,t2,$8000
        addi    t4,-1
        _tpa(sh,t4,dmclen)
        addi    t2,1
        andi    t2,$7fff
        _tpa(sh,t2,dmcadr)

        srl    s8,t3,6
        andi   s8,~3
        addu   s8,A6502_nespage
        lw     s8,(s8)
        beqz   s8,baddmcread ; read from a register? valid, but...
        addu   s8,t3
        lbu    t3,(s8)

        addi    A6502_count,-1

        li      t2,7
dmc_notload
        andi    t4,t3,1
        srl     t3,1
        sll     t4,6
        or      t4,t1
        _vtpa(lbu,t1,dmcdectab,t4)
        bnez    t7,dmcloop
        addi    t7,-1
dmcloop_end
        _tpa(lbu,t4,dmcdelta)
        _tpa(sb,t3,dmccurbit)
        _tpa(sb,t2,dmcbitcnt)
        andi    t4,$01
        sll     t1,1
        or      t1,t4
        _tpa(sb,t1,dmcdelta)

        j       dmc_nolduty
        nop
dmc_nochange
        _tpa(sh,t2,dmccnt)
dmc_nochange2
        _tpa(lbu,t1,dmcdelta)
dmc_nolduty
        andi    t1,$7f
        
        _tpa(lbu,t2,dmcenable)
        bnez    t2,dmcon
        add     t1,t5
        move    t1,t5
dmcon
        _vtpa(lbu,t1,dtn_table,t1)
        add     t1,t8
        sll     t1,6
        sh      t1,0(t0)
        sh      t1,2(t0)

        addi    t0,4
        bnez    t6,asamploop
        addi    t6,-1

        _tpa(lb,t1,frame4)
        bnez    t1,no4th
        addi    t1,-1
        li      t1,65
        _tpa(sb,t1,frame4)

; Decay
        _tpa(lb,t1,sq1dec)
        _tpa(lb,t2,asq1)
        bnez    t1,nosq1decay2
        addi    t1,-1

        andi    t1,t2,$f
        _tpa(sb,t1,sq1dec)

        _tpa(lb,t1,sq1decvol)
        bnez    t1,normdecsq1
        addi    t1,-1
        andi    t3,t2,$20
        bnez    t3,normdecsq1
        li      t1,$f
        li      t1,0
normdecsq1  _tpa(sb,t1,sq1decvol)

        andi    t3,t2,$10
        bnez    t3,nosq1decay
        nop

        _tpa(sb,t1,sq1vol)

        j       nosq1decay
        nop
nosq1decay2
        _tpa(sb,t1,sq1dec)
nosq1decay

        _tpa(lb,t1,sq2dec)
        _tpa(lb,t2,asq2)
        bnez    t1,nosq2decay2
        addi    t1,-1

        andi    t1,t2,$f
        _tpa(sb,t1,sq2dec)

        _tpa(lb,t1,sq2decvol)
        bnez    t1,normdecsq2
        addi    t1,-1
        andi    t3,t2,$20
        bnez    t3,normdecsq2
        li      t1,$f
        li      t1,0
normdecsq2  _tpa(sb,t1,sq2decvol)

        andi    t3,t2,$10
        bnez    t3,nosq2decay
        nop

        _tpa(sb,t1,sq2vol)

        j       nosq2decay
        nop
nosq2decay2
        _tpa(sb,t1,sq2dec)
nosq2decay

        _tpa(lb,t1,rnddec)
        _tpa(lb,t2,arnd)
        bnez    t1,nornddecay2
        addi    t1,-1

        andi    t1,t2,$f
        _tpa(sb,t1,rnddec)

        _tpa(lb,t1,rnddecvol)
        bnez    t1,normdecrnd
        addi    t1,-1
        andi    t3,t2,$20
        bnez    t3,normdecrnd
        li      t1,$f
        li      t1,0
normdecrnd  _tpa(sb,t1,rnddecvol)

        andi    t3,t2,$10
        bnez    t3,nornddecay
        nop

        _tpa(sb,t1,rndvol)

        j       nornddecay
        nop
nornddecay2
        _tpa(sb,t1,rnddec)
nornddecay

; Linear counter
        _tpa(lbu,t3,lincnt)
        _tpa(lbu,t1,lincnthalt)
        _tpa(lbu,t2,atri)
        bnez    t1,lincnt_haltset
        nop
        beqz    t3,lincnt_nodec
        addi    t3,-1
        _tpa(sb,t3,lincnt)
        j       lincnt_nodec
        nop
lincnt_haltset
        andi    t4,t2,$7f
        _tpa(sb,t4,lincnt)
lincnt_nodec

        andi    t2,$80
        bnez    t2,lincnt_noclearhalt
        nop
        _tpa(sb,r0,lincnthalt)
lincnt_noclearhalt

        j       yes4th
        nop
no4th   _tpa(sb,t1,frame4)
yes4th

; Sweep
        _tpa(lb,t1,frame2)
        bnez    t1,no2th
        addi    t1,-1

        ;li      t1,1
        ;_tpa(sb,t1,sq1en)
        ;_tpa(sb,t1,sq2en)

        _tpa(lbu,t1,sq1en)
        _tpa(lb,t2,asq1+1)
        beqz    t1,nosq1sweep2
        andi    t1,t2,$80
        beqz    t1,nosq1sweep2
        nop

        _tpa(lb,t1,sq1swp)
        bnez    t1,nosq1sweep
        addi    t1,-1

        andi    t1,t2,$70
        srl     t1,4
        _tpa(sb,t1,sq1swp)

        _tpa(lbu,t3,asq1+3)
        _tpa(lbu,t4,asq1+2)
        andi    t3,7
        sll     t3,8
        or      t4,t3

        andi    t1,t2,8
        andi    t2,7
        beqz    t2,nosq1sweep2  ; fixes SMB2
        srlv    t3,t4,t2
        beqz    t1,sq1sweepup
        add     t7,t4,t3

        sub     t7,t4,t3
        addi    t7,-1   ; the difference

sq1sweepup

        slti    t2,t7,8
        slti    t3,t7,0x800   ; end conditions
        xori    t3,1
        or      t2,t3
        beqz    t2,swsq1_noprob
        nop
        _tpa(sb,r0,sq1en)
swsq1_noprob

        andi    t3,t7,$0ff
        andi    t4,t7,$700
        srl     t4,8
        _tpa(sb,t3,asq1+2)
        _tpa(sb,t4,asq1+3)

        j       nosq1sweep2
        nop
nosq1sweep
        _tpa(sb,t1,sq1swp)
nosq1sweep2

        _tpa(lbu,t1,sq2en)
        _tpa(lb,t2,asq2+1)
        beqz    t1,nosq2sweep2
        andi    t1,t2,$80
        beqz    t1,nosq2sweep2
        nop

        _tpa(lb,t1,sq2swp)
        bnez    t1,nosq2sweep
        addi    t1,-1

        andi    t1,t2,$70
        srl     t1,4
        _tpa(sb,t1,sq2swp)

        _tpa(lbu,t3,asq2+3)
        _tpa(lbu,t4,asq2+2)
        andi    t3,7
        sll     t3,8
        or      t4,t3

        andi    t1,t2,8
        andi    t2,7
        beqz    t2,nosq2sweep2
        srlv    t3,t4,t2
        beqz    t1,sq2sweepup
        add     t7,t4,t3

        sub     t7,t4,t3
        ;addi    t7,-1   ; the difference

sq2sweepup

        slti    t2,t7,8
        slti    t3,t7,0x800   ; end conditions
        xori    t3,1
        or      t2,t3
        beqz    t2,swsq2_noprob
        nop
        _tpa(sb,r0,sq2en)
swsq2_noprob

        andi    t3,t7,$0ff
        andi    t4,t7,$700
        srl     t4,8
        _tpa(sb,t3,asq2+2)
        _tpa(sb,t4,asq2+3)  ; destroy length reg here, don't need anyway
        j   nosq2sweep2
        nop
nosq2sweep
        _tpa(sb,t1,sq2swp)
nosq2sweep2

        li      t1,130
no2th   _tpa(sb,t1,frame2)

        _tpa(lh,t0,abufdrawpos)
        addi    t0,4*samplesperline
        _tpa(sh,t0,abufdrawpos)

        la      t1,abufsize
        bne     t0,t1,nextbuffer_not
        nop

        _tpa(lb,t0,awhichdraw)
        xori    t0,1
        _tpa(sb,t0,awhichdraw)

d_collide
        _tpa(lb,t1,awhichplay)
        bne t1,t0,d_nocollide
        nop
        j   d_collide
        nop
d_nocollide
        _tpa(sh,r0,abufdrawpos)

        _tpa(sb,r0,killframe)
nextbuffer_not
        jr      ra
        nop

baddmcread
        _tpa(sw,ra,runra)
        j       badopcoderead
        nop

aihandler
        _tpa(sw,r0,0xa450000c)  ; clear audio interrupt

        _tpa(lb,k1,awhichdraw)
        _tpa(lb,t0,awhichplay)
        beq     t0,k1,nocollide ; if current play != current draw
        li  k1,1
        _tpa(sb,k1,killframe)   ; disable gfx for the next frame
nocollide

        beqz    t0,notabuf2
        li      k1,0
        la      k1,abufsize
notabuf2
        la      t0,abuf1
        addu    t0,k1

        lui     s8,0xa450
        li      k1,soundperiod
        sw      k1,0x10(s8) ; freq (more properly period)
        li      k1,15 ;7
        sw      k1,0x14(s8) ; bit rate
        sw      t0,0x00(s8) ; address
        la      k1,abufsize ;-1
        sw      k1,0x04(s8) ; size
        li      k1,1
        sw      k1,0x08(s8) ; activate

        _tpa(lb,t0,awhichplay)
        xori    t0,1
        _tpa(sb,t0,awhichplay)

        jr      ra
        nop

initaudio
        li      t0,1
        _tpa(sb,t0,awhichplay)  ; start playing the 2nd buffer
        _tpa(sh,t0,rndcval)

        lui     s8,0xa450
        li      t0,soundperiod
        sw      t0,0x10(s8) ; freq
        li      t0,15 ;7
        sw      t0,0x14(s8) ; sample size
        la      t0,abuf1
        sw      t0,0x00(s8) ; address
        la      t0,abufsize ;-1
        sw      t0,0x04(s8) ; size
        li      t0,1
        sw      t0,0x08(s8) ; activate

        jr      ra
        nop

alendec
        _tpa(lbu,t0,sq1len)
        _tpa(lbu,t1,sq2len)
        _tpa(lbu,t2,trilen)
        _tpa(lbu,t3,rndlen)
        beqz    t0,sq1_lenoff
        _tpa(lbu,t5,asq1)
        andi    t5,$20
        bnez    t5,sq1_lenoff
        nop
        addi    t0,-1
sq1_lenoff
        beqz    t1,sq2_lenoff
        _tpa(lbu,t5,asq2)
        andi    t5,$20
        bnez    t5,sq2_lenoff
        nop
        addi    t1,-1
sq2_lenoff
;        _tpa(lbu,t6,lincnt)
;        bnez    t6,trion
;        nop
        beqz    t2,tri_lenoff
;trion
        _tpa(lbu,t5,atri)
        andi    t5,$80
        bnez    t5,tri_lenoff
        nop
        addi    t2,-1
tri_lenoff
        beqz    t3,rnd_lenoff
        _tpa(lbu,t5,arnd)
        andi    t5,$20
        bnez    t5,rnd_lenoff
        nop
        addi    t3,-1
rnd_lenoff

        _tpa(sb,t0,sq1len)
        _tpa(sb,t1,sq2len)
        _tpa(sb,t2,trilen)
        _tpa(sb,t3,rndlen)
        
        jr      ra
        nop
