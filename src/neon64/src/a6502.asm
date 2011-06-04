; A 6502 emulator, A6502
; (c) 2002-2003 Halley's Comet Software
; I cannot deny the influence that M6502 has had on the way that this is
; written, but it is not a direct port, unlike the version in Neon64 beta 1.
; One big reason is that M6502 does not write back read values in
; Read-Modify-Write type instructions before the modification phase, like the
; actual 6502 does. It also does not add extra cycles for page traversal...

; The stack overflow detection is a debugging tool, but for some reason when
; it was removed the emu ran slower, so it stays for now.

#include 6502defs.inc
#include nespages.inc

Init6502:

        li      A6502_PC, 0xfffc  ; reset vector
        readword_A6502(A6502_PC,A6502_PC)

        li      A6502_S,0xff
        li      A6502_A,0
        li      A6502_X,0
        li      A6502_Y,0
        jr      ra
        li      A6502_flags,0x20|F_I    ; Marat said so, interrupt added by Ki

Int6502:
        andi    t0,A6502_flags,F_I
        bnez    t0,notint
        srl     t0,A6502_PC,8

        wstack_A6502(A6502_S,t0)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        andi    A6502_PC,0xff
        wstack_A6502(A6502_S,A6502_PC)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        andi    t0,A6502_flags,~F_B
        wstack_A6502(A6502_S,t0)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        li      A6502_PC,0xfffe
        readword_A6502(A6502_PC,A6502_PC)
        ori     A6502_flags, F_I
        andi    A6502_flags, ~F_D
        jr      ra
        addi    A6502_count,-interruptlatency
notint: jr      ra
        nop

NMI6502:
        srl     t0,A6502_PC,8
        wstack_A6502(A6502_S,t0)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        andi    A6502_PC,0xff
        wstack_A6502(A6502_S,A6502_PC)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        andi    t0,A6502_flags,~F_B
        wstack_A6502(A6502_S,t0)
        addi    A6502_S,-1
        andi    A6502_S,0xff

        li      A6502_PC,0xfffa
        readword_A6502(A6502_PC,A6502_PC)
        andi    A6502_flags,~F_D
notnmi
        jr      ra
        addi    A6502_count,-interruptlatency

; butt-ass
opcodejumptable
        DW      BRK,ORAIDX,FEX,FEX,FEX,ORAZP,ASLZP,FEX          ; 00-07
        DW      PHP,ORAIMM,ASLA,FEX,FEX,ORAABS,ASLABS,FEX       ; 08-0f
        DW      BPLREL,ORAIDY,FEX,FEX,FEX,ORAZPX,ASLZPX,FEX     ; 10-17
        DW      CLC,ORAABY,FEX,FEX,FEX,ORAABX,ASLABX,FEX        ; 18-1f
        
        DW      JSRABS,ANDIDX,FEX,FEX,BITZP,ANDZP,ROLZP,FEX     ; 20-27
        DW      PLP,ANDIMM,ROLA,FEX,BITABS,ANDABS,ROLABS,FEX    ; 28-2f
        DW      BMIREL,ANDIDY,FEX,FEX,FEX,ANDZPX,ROLZPX,FEX     ; 30-37
        DW      SEC,ANDABY,FEX,FEX,FEX,ANDABX,ROLABX,FEX        ; 38-3f

        DW      RTI,EORIDX,FEX,FEX,FEX,EORZP,LSRZP,FEX          ; 40-47
        DW      PHA,EORIMM,LSRA,FEX,JMPABS,EORABS,LSRABS,FEX    ; 48-4f
        DW      BVCREL,EORIDY,FEX,FEX,FEX,EORZPX,LSRZPX,FEX     ; 50-57
        DW      CLI,EORABY,FEX,FEX,FEX,EORABX,LSRABX,FEX        ; 58-5f

        DW      RTS,ADCIDX,FEX,FEX,FEX,ADCZP,RORZP,FEX          ; 60-67
        DW      PLA,ADCIMM,RORA,FEX,JMPIND,ADCABS,RORABS,FEX    ; 68-6f
        DW      BVSREL,ADCIDY,FEX,FEX,FEX,ADCZPX,RORZPX,FEX     ; 70-77
        DW      SEI,ADCABY,FEX,FEX,FEX,ADCABX,RORABX,FEX        ; 78-7f

        DW      FEX,STAIDX,FEX,FEX,STYZP,STAZP,STXZP,FEX        ; 80-87
        DW      DEY,FEX,TXA,FEX,STYABS,STAABS,STXABS,FEX        ; 88-8f
        DW      BCCREL,STAIDY,FEX,FEX,STYZPX,STAZPX,STXZPY,FEX  ; 90-97
        DW      TYA,STAABY,TXS,FEX,FEX,STAABX,FEX,FEX           ; 98-9f

        DW      LDYIMM,LDAIDX,LDXIMM,FEX,LDYZP,LDAZP,LDXZP,FEX  ; a0-a7
        DW      TAY,LDAIMM,TAX,FEX,LDYABS,LDAABS,LDXABS,FEX     ; a8-af
        DW      BCSREL,LDAIDY,FEX,FEX,LDYZPX,LDAZPX,LDXZPY,FEX  ; b0-b7
        DW      CLV,LDAABY,TSX,FEX,LDYABX,LDAABX,LDXABY,FEX     ; b8-bf

        DW      CPYIMM,CMPIDX,FEX,FEX,CPYZP,CMPZP,DECZP,FEX     ; c0-c7
        DW      INY,CMPIMM,DEX,FEX,CPYABS,CMPABS,DECABS,FEX     ; c8-cf
        DW      BNEREL,CMPIDY,FEX,FEX,FEX,CMPZPX,DECZPX,FEX     ; d0-d7
        DW      CLD,CMPABY,FEX,FEX,FEX,CMPABX,DECABX,FEX        ; d8-df
        
        DW      CPXIMM,SBCIDX,FEX,FEX,CPXZP,SBCZP,INCZP,FEX     ; e0-e7
        DW      INX,SBCIMM,NOP,FEX,CPXABS,SBCABS,INCABS,FEX     ; e8-ef
        DW      BEQREL,SBCIDY,FEX,FEX,FEX,SBCZPX,INCZPX,FEX     ; f0-f7
        DW      SED,SBCABY,FEX,FEX,FEX,SBCABX,INCABX,FEX        ; f8-ff

FEX     _tpa(lbu,t0,traperrors)
        beqz    t0,donttrap
        nop

        li      t1,1
        _tpa(sw,t1,debugflag)   ; so screen doesn't switch

        _text_value(117,68,t0,1)
        _text_string(117,60,bopcode)
        jal     A6502_status
        nop

        li      t0,240
        la      t1, $10400000
        jal     screen_capture
        nop

        deadend

donttrap    ; just skip the opcode if bad
        jr      ra
        nop
bopcode db      "Bad Opcode:",0
        _align(4)

; Thine program dost art screwed up.
; Er, I mean, Bad Opcode.
