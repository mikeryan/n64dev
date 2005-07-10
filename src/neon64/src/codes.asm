               org     0x80000408

#include defs.inc
#imacros macros.inc
; A6502 opcodes
; Format of labels: ADCIMM
;                   \_/\_/
;                    |  Addressing mode: IMM=immediate ZP=zero page abs
;                    |  ZPX=zero page abs x idx ZPY=zero page abs y idx
;                    |  ABS=abs ABX=abs x idx ADY=abs y idx
;                    |  IDX=preidx indirect IDY=postidx indirect
;                    |  A=accumulator, Y=y, X=x, REL=relative branch
;                    |  IND=indirect
;                    instruction name, 3 chars
;
; PC already points at byte after opcode
;
; I don't think that any register over t3 is used here, so those are not
;  preserved.

#include 6502defs.inc

; ADC 0x69
ADCIMM          readop_A6502(A6502_PC,t0)
                ADC_A6502(t0,t1,t2)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-2
; 0x65
ADCZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-3
; 0x75
ADCZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-4
; 0x6D
ADCABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x7D
ADCABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x79
ADCABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x61
ADCIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0x71
ADCIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                ADC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; AND 0x29
ANDIMM          readop_A6502(A6502_PC,t0)
                AND_A6502(t0)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-2
; 0x25
ANDZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-3
; 0x35
ANDZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-4
; 0x2D
ANDABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x3D
ANDABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x39
ANDABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x21
ANDIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0x31
ANDIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                AND_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; ASL 0x0A
ASLA            ASL_A6502(A6502_A)
                jr      ra
                addi    A6502_count,-2
; 0x06
ASLZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ASL_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; 0x16
ASLZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ASL_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0x0E
ASLABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ASL_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-6
; 0x1E
ASLABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ASL_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-7
; 0x90
BCCREL          andi    t0, A6502_flags, F_C
                bnez    t0, not_bcc
                addi    A6502_count,-2 ; always
                addi    A6502_count,-1 ; only if branch is taken
                BRANCH_A6502
not_bcc         jr      ra
                addi    A6502_PC,1
; 0xB0
BCSREL          andi    t0, A6502_flags, F_C
                beqz    t0, not_bcs
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bcs         jr      ra
                addi    A6502_PC,1
; 0x24
BITZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                BIT_A6502(t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0x2C
BITABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                BIT_A6502(t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xF0
BEQREL          andi    t0, A6502_flags, F_Z
                beqz    t0, not_beq
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_beq         jr      ra
                addi    A6502_PC,1

; 0xD0
BNEREL          andi    t0, A6502_flags, F_Z
                bnez    t0, not_bne
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bne         jr      ra
                addi    A6502_PC,1

; 0x30
BMIREL          andi    t0, A6502_flags, F_S
                beqz    t0, not_bmi
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bmi         jr      ra
                addi    A6502_PC,1

; 0x10
BPLREL          andi    t0, A6502_flags, F_S
                bnez    t0, not_bpl
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bpl         jr      ra
                addi    A6502_PC,1

; 0x00 (not currently emulated)
BRK    ori     t1,A6502_flags, F_B|$20
                addi    A6502_PC,1
                srl     t0,A6502_PC,8
                wstack_A6502(A6502_S,t0)
                addi    A6502_S,-1
                andi    A6502_S,0xff

                andi    A6502_PC,0xff
                wstack_A6502(A6502_S,A6502_PC)
                addi    A6502_S,-1
                andi    A6502_S,0xff

                wstack_A6502(A6502_S,t1)
                addi    A6502_S,-1
                andi    A6502_S,0xff

                li      A6502_PC, 0xfffe
                readword_A6502(A6502_PC,A6502_PC)
                ori     A6502_flags, F_I
                jr      ra
                addi    A6502_count,-7
; 0x50
BVCREL          andi    t0, A6502_flags, F_V
                bnez    t0, not_bvc
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bvc         jr      ra
                addi    A6502_PC,1
; 0x70
BVSREL          andi    t0, A6502_flags, F_V
                beqz    t0, not_bvs
                addi    A6502_count,-2
                addi    A6502_count,-1
                BRANCH_A6502
not_bvs         jr      ra
                addi    A6502_PC,1
; 0x18
CLC             andi    A6502_flags,~F_C
                jr      ra
                addi    A6502_count,-2
; 0xD8
CLD             andi    A6502_flags,~F_D
                jr      ra
                addi    A6502_count,-2
; 0x58
CLI             andi    A6502_flags,~F_I
                jr      ra
                addi    A6502_count,-2
; 0xB8
CLV             andi    A6502_flags,~F_V
                jr      ra
                addi    A6502_count,-2
; 0xC9
CMPIMM          readop_A6502(A6502_PC,t0)
                CMP_A6502(A6502_A,t0,t1,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xC5
CMPZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xD5
CMPZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0xCD
CMPABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xDD
CMPABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xD9
CMPABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xC1
CMPIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0xD1
CMPIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_A,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0xE0
CPXIMM          readop_A6502(A6502_PC,t0)
                CMP_A6502(A6502_X,t0,t1,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xE4
CPXZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                CMP_A6502(A6502_X,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xEC
CPXABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_X,t1,t0,t2)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xC0
CPYIMM          readop_A6502(A6502_PC,t0)
                CMP_A6502(A6502_Y,t0,t1,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xC4
CPYZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                CMP_A6502(A6502_Y,t1,t0,t2)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xCC
CPYABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                CMP_A6502(A6502_Y,t1,t0,t2)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xC6
DECZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                DEC_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0xD6
DECZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                DEC_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0xCE
DECABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                DEC_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-6
; 0xDE
DECABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                DEC_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-7
; 0xCA
DEX             DEC_A6502(A6502_X)
                jr      ra
                addi    A6502_count,-2
; 0x88
DEY             DEC_A6502(A6502_Y)
                jr      ra
                addi    A6502_count,-2
; 0x49
EORIMM          readop_A6502(A6502_PC,t0)
                EOR_A6502(t0)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-2
; 0x45
EORZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-3
; 0x55
EORZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-4
; 0x4D
EORABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x5D
EORABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x59
EORABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x41
EORIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0x51
EORIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                EOR_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; 0xE6
INCZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                INC_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0xF6
INCZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                INC_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0xEE
INCABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                INC_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-6
; 0xFE
INCABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                INC_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-7
; 0xE8
INX             INC_A6502(A6502_X)
                jr      ra
                addi    A6502_count,-2
; 0xC8
INY             INC_A6502(A6502_Y)
                jr      ra
                addi    A6502_count,-2
; 0x4C
JMPABS          readword_A6502(A6502_PC,A6502_PC)
                jr      ra
                addi    A6502_count,-3
; 0x6C
JMPIND          INDIRECT_A6502(t0,t2)
                jr      ra
                addi    A6502_count,-5
; 0x20
JSRABS          readword_A6502(A6502_PC,t0)

                addi    A6502_PC,1 ; not 2
                srl     t1,A6502_PC,8
                wstack_A6502(A6502_S,t1)
                addi    A6502_S,-1
                andi    A6502_S,0xff

                andi    A6502_PC,0xff
                wstack_A6502(A6502_S,A6502_PC)
                addi    A6502_S,-1
                andi    A6502_S,0xff

                move    A6502_PC,t0
                jr      ra
                addi    A6502_count,-6
; 0xA9
LDAIMM          readop_A6502(A6502_PC,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xA5
LDAZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xB5
LDAZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0xAD
LDAABS          ABS_A6502(t0)
                read_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xBD
LDAABX          INDEXX_A6502(t0)
                read_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xB9
LDAABY          INDEXY_A6502(t0)
                read_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xA1
LDAIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0xB1
LDAIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,A6502_A)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0xA2
LDXIMM          readop_A6502(A6502_PC,A6502_X)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xA6
LDXZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,A6502_X)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xB6
LDXZPY          INDEXYZP_A6502(t0)
                rRAM_A6502(t0,A6502_X)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0xAE
LDXABS          ABS_A6502(t0)
                read_A6502(t0,A6502_X)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xBE
LDXABY          INDEXY_A6502(t0)
                read_A6502(t0,A6502_X)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xA0
LDYIMM          readop_A6502(A6502_PC,A6502_Y)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-2
; 0xA4
LDYZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,A6502_Y)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0xB4
LDYZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,A6502_Y)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0xAC
LDYABS          ABS_A6502(t0)
                read_A6502(t0,A6502_Y)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xBC
LDYABX          INDEXX_A6502(t0)
                read_A6502(t0,A6502_Y)
                andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0x4A
LSRA            LSR_A6502(A6502_A)
                jr      ra
                addi    A6502_count,-2
; 0x46
LSRZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                LSR_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0x56
LSRZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                LSR_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0x4E
LSRABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                LSR_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-6
; 0x5E
LSRABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                LSR_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-7
; 0xEA
NOP             jr      ra
                addi    A6502_count,-2
; 0x09
ORAIMM          readop_A6502(A6502_PC,t0)
                ORA_A6502(t0)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-2
; 0x05
ORAZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-3
; 0x15
ORAZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-4
; 0x0D
ORAABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x1D
ORAABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x19
ORAABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0x01
ORAIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0x11
ORAIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                ORA_A6502(t1)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; 0x48
PHA             PHA_A6502
                jr      ra
                addi    A6502_count,-3
; 0x08
PHP             PHP_A6502
                jr      ra
                addi    A6502_count,-3
; 0x68
PLA             PLA_A6502
                jr      ra
                addi    A6502_count,-4
; 0x28
PLP             PLP_A6502
                jr      ra
                addi    A6502_count,-4
; 0x2A
ROLA            ROL_A6502(A6502_A)
                jr      ra
                addi    A6502_count,-2
; 0x26
ROLZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ROL_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0x36
ROLZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ROL_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0x2E
ROLABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ROL_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-6
; 0x3E
ROLABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ROL_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-7
; 0x6A
RORA            ROR_A6502(A6502_A)
                jr      ra
                addi    A6502_count,-2
; 0x66
RORZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ROR_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-5
; 0x76
RORZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                ROR_A6502(t1)
                wRAM_A6502(t0,t1)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0x6E
RORABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ROR_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-6
; 0x7E
RORABX          INDEXXS_A6502(t0)
                read_A6502(t0,t1)
                write_A6502(t0,t1)  ; technically correct
                ROR_A6502(t1)
                write_A6502(t0,t1)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-7
; 0x4D
RTI             PLP_A6502
                addi   A6502_S,1
                andi   A6502_S,0xff

                rstack_A6502(A6502_S,A6502_PC)
                addi   A6502_S,1
                andi   A6502_S,0xff

                rstack_A6502(A6502_S,t0)
                sll    t0,8
                or      A6502_PC,t0
                jr      ra
                addi    A6502_count,-6
; 0x60
RTS             addi   A6502_S,1
                andi   A6502_S,0xff

                rstack_A6502(A6502_S,A6502_PC)
                addi   A6502_S,1
                andi   A6502_S,0xff

                rstack_A6502(A6502_S,t0)
                sll    t0,8
                or      A6502_PC,t0
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; SBC 0xE9
SBCIMM          readop_A6502(A6502_PC,t0)
                SBC_A6502(t0,t1,t2)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-2
; 0xE5
SBCZP           ABSZP_A6502(t0)
                rRAM_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-3
; 0xF5
SBCZPX          INDEXXZP_A6502(t0)
                rRAM_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-4
; 0xED
SBCABS          ABS_A6502(t0)
                read_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0xFD
SBCABX          INDEXX_A6502(t0)
                read_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0xF9
SBCABY          INDEXY_A6502(t0)
                read_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 2
                jr      ra
                addi    A6502_count,-4
; 0xE1
SBCIDX          INDIRECTX_A6502(t0)
                read_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-6
; 0xF1
SBCIDY          INDIRECTY_A6502(t0)
                read_A6502(t0,t1)
                SBC_A6502(t1,t2,t3)
                addi    A6502_PC, 1
                jr      ra
                addi    A6502_count,-5
; 0x38
SEC             ori     A6502_flags,F_C
                jr      ra
                addi    A6502_count,-2
; 0xF8
SED             ori     A6502_flags,F_D
                jr      ra
                addi    A6502_count,-2
; 0x78
SEI             ori     A6502_flags,F_I
                jr      ra
                addi    A6502_count,-2
; 0x85
STAZP           ABSZP_A6502(t0)
                wRAM_A6502(t0,A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0x95
STAZPX          INDEXXZP_A6502(t0)
                wRAM_A6502(t0,A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0x8D
STAABS          ABS_A6502(t0)
                write_A6502(t0,A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0x9D
STAABX          INDEXXS_A6502(t0)
                write_A6502(t0,A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-5
; 0x99
STAABY          INDEXYS_A6502(t0)
                write_A6502(t0,A6502_A)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-5
; 0x81
STAIDX          INDIRECTX_A6502(t0)
                write_A6502(t0,A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0x91
STAIDY          INDIRECTY_A6502(t0)
                write_A6502(t0,A6502_A)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-6
; 0x86
STXZP           ABSZP_A6502(t0)
                wRAM_A6502(t0,A6502_X)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0x96
STXZPY          INDEXYZP_A6502(t0)
                wRAM_A6502(t0,A6502_X)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0x8E
STXABS          ABS_A6502(t0)
                write_A6502(t0,A6502_X)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0x84
STYZP           ABSZP_A6502(t0)
                wRAM_A6502(t0,A6502_Y)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-3
; 0x94
STYZPX          INDEXXZP_A6502(t0)
                wRAM_A6502(t0,A6502_Y)
                addi    A6502_PC,1
                jr      ra
                addi    A6502_count,-4
; 0x8C
STYABS          ABS_A6502(t0)
                write_A6502(t0,A6502_Y)
                addi    A6502_PC,2
                jr      ra
                addi    A6502_count,-4
; 0xAA
TAX             andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                move    A6502_X,A6502_A
                jr      ra
                addi    A6502_count,-2
; 0xA8
TAY             andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_A)
                move    A6502_Y,A6502_A
                jr      ra
                addi    A6502_count,-2
; 0xBA
TSX             andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_S)
                move    A6502_X,A6502_S
                jr      ra
                addi    A6502_count,-2
; 0x8A
TXA             andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_X)
                move    A6502_A,A6502_X
                jr      ra
                addi    A6502_count,-2
; 0x9A
TXS             ;andi    A6502_flags,~(F_S|F_Z)
                ;FL_A6502(A6502_X)
                move    A6502_S,A6502_X
                jr      ra
                addi    A6502_count,-2
; 0x98
TYA             andi    A6502_flags,~(F_S|F_Z)
                FL_A6502(A6502_Y)
                move    A6502_A,A6502_Y
                jr      ra
                addi    A6502_count,-2

badopcoderead
        _tpa(lbu,t0,traperrors2)
        beqz    t0,badopcoderead_return

        li      t1,1
        _tpa(sw,t1,debugflag)   ; so screen doesn't switch
        jal     A6502_status
        nop
        deadend
badopcoderead_return
        ; should be enough to prevent a total crash, but won't fix anything
        _tpa(lw,ra,runra)
        jr  ra
        nop

#include "status.asm"
