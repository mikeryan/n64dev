; Profile results are in: tracer is SLOW.

;                la      t0,$c1ca
;                bne     t0,A6502_PC,_c1ca_not
;                li      t0,1
;                _tpa(sw,t0,debugflag)
;_c1ca_not
                
                ;_tpa(lbu,t0,traperrors)
                ;beqz    t0,notrace
                ;nop
                _tpa(lw,t0,debugflag)
                beqz    t0,notrace
                nop
                
                jal     A6502_status
                nop
                jal     Pause
                nop
notrace         _tpa(sw,r0,debugflag)

                _tpa(lwu,t0,pchcnt)
                _vtpa(sh,A6502_PC,pchist,t0)
                addi    t0,2
                andi    t0,0x1e
                _tpa(sw,t0,pchcnt)
