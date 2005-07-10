; Loading routines (ROM, SRAM)

loadROM:
                _tpa(sw,ra,loadromra)

                jal     cls
                nop

                _text_string(10,12,mmm0)

                jal ROMload
                nop

                _tpa(lwu,t0,proga)      ; load header
                la      t1, $4E45531A   ; N,E,S,0x1A
                beq     t0, t1, goodfmt
                nop
                _text_string(20,22,badfmt)
                jal     pause
                nop
                j       loadnewrom
                nop
goodfmt:        

                ;_tpa(lbu,t0,prgromsize)    ; 16K PRG-ROM page count
                ;sll     t0, 4
                ;_text_value(23,8*5+12,t0,3)
                ;_text_string(8*6+23,8*5+12,PRGCOUNTmsg)
                
                ;_tpa(lbu,t0,chrromsize)    ; 8K CHR-ROM page count
                ;sll     t0, 3
                ;_text_value(23,8*6+12,t0,3)
                ;_text_string(8*6+23,8*6+12,CHRCOUNTmsg)

                ; Mirroring type
                ;_text_string(23,8*7+12,mirrortype)
                _tpa(lbu,t0,romc1)    ; ROM Control byte #1
                andi    t1, t0, 1
                andi    t0, 8
                beq     t0, zero, not4screend
                nop
                ; 4 screen mirroring (extra VRAM on cart), the pages are
                ;  set up for this by default.
                ;_text_string(8*11+23,8*7+12,fourscreen)
                j       aftermirror
                nop
not4screend:
                beqz    t1, hmirrord
                nop
                ; Vertical mirroring
                la      t0,vram
                _tpa(sw,t0,8*8+_VRAMPages)
                _tpa(sw,t0,9*8+_VRAMPages)
                la      t0,vram-0x800
                _tpa(sw,t0,10*8+_VRAMPages)
                _tpa(sw,t0,11*8+_VRAMPages)
                ;_text_string(8*11+23,8*7+12,Vmirror)
                j       aftermirror
                nop
hmirrord:       
                ; Horizontal mirroring                
                la      t0,vram
                _tpa(sw,t0,8*8+_VRAMPages)
                la      t0,vram-0x400
                _tpa(sw,t0,9*8+_VRAMPages)
                _tpa(sw,t0,10*8+_VRAMPages)
                la      t0,vram-0x800
                _tpa(sw,t0,11*8+_VRAMPages)
                ;_text_string(8*11+23,8*7+12,Hmirror)
aftermirror:
                
                ; Mapper type
                ;_text_string(23,8*8+12,mappermsg)
                _tpa(lbu,t0,romc1)
                _tpa(lbu,t1,romc2)
                andi    t0, $F0
                andi    t1, $F0
                srl     t0, 4
                or      t0,t1
                _tpa(sb,t0,mapper)
                ;_text_value(8*7+23,8*8+12,t0,1)
                ;_text_string(8*11+23,8*8+12,mappermsg2)

                ;sll     t0, 2
                ;_vtpa(lwu,a0,mtypetable,t0)
                ;_text_reg_string(8*13+23,8*8+12,a0)

                ; Individual mapper initialization routines
                _tpa(lbu,t0,mapper)
                nop
                bnez    t0,notmapper0
                nop
                ; Mapper 0 (none)
                jal     nprgrom
                nop
                jal     nchrrom
                nop
                j       endofloadrom
                nop
notmapper0:
                li      t1,1
                bne     t0,t1,notmapper1
                nop
                ; Mapper 1 (MMC1)

                jal     unprgrom
                nop
                jal     nchrrom
                nop

                li      t0,$c   ; on reset based on matt richey
                _tpa(sb,t0,mmc1reg)

                j       endofloadrom
                nop

notmapper1

                li      t1,2
                bne     t0,t1,notmapper2
                nop
                ; Mapper 2 (UNROM)
                jal     unprgrom
                nop

                jal     nchrrom
                nop
                j       endofloadrom
                nop
notmapper2
                li      t1,3
                bne     t0,t1,notmapper3
                nop
                ; Mapper 3 (CNROM)
                ; Has switchable CHRROM banks, but nothing special on boot.
                jal     nprgrom
                nop
                jal     nchrrom
                nop
                j       endofloadrom
                nop
notmapper3:
                li      t1,4
                bne     t0,t1,notmapper4
                nop
                ; Mapper 4 (MMC3)
                ; Inits same as 2
                jal     unprgrom
                nop
                jal     nchrrom
                nop

                j       endofloadrom
                nop
notmapper4:
                li      t1,7
                bne     t0,t1,notmapper7
                nop
mapper7
                ; Mapper 7 (AOROM)
                jal     nprgrom    ; 1st 32k always loaded
                nop
                ; no CHRROM

                ; One-screen mirroring (0x2000, FCEU agrees)
                la      t0,vram
                _tpa(sw,t0,8*8+_VRAMPages)
                _tpa(cache,25,8*8+_VRAMPages)
                la      t0,vram-$400
                _tpa(sw,t0,9*8+_VRAMPages)
                _tpa(cache,25,9*8+_VRAMPages)
                la      t0,vram-$800
                _tpa(sw,t0,10*8+_VRAMPages)
                _tpa(cache,25,10*8+_VRAMPages)
                la      t0,vram-$c00
                _tpa(sw,t0,11*8+_VRAMPages)
                _tpa(cache,25,11*8+_VRAMPages)

                j       endofloadrom
                nop
notmapper7:
                li      t1,9
                bne     t0,t1,notmapper9
                nop
                ; Mapper 9 (MMC2)
                jal     mmc2prgrom
                nop
                jal     nchrrom
                nop

                ; not needed
                ;li      t0,$00  ; $fe on reset
                ;_tpa(sw,t0,mmc2toggle)

                j       endofloadrom
                nop
notmapper9
                li      t1,11
                bne     t0,t1,notmapper11
                nop
                ; Mapper 11 (Color Dreams)
                jal     nprgrom
                nop
                jal     nchrrom
                nop

                j       endofloadrom
                nop
notmapper11:    li      t1,34
                bne     t0,t1,notmapper34
                nop
                ; Mapper 34 (Nina-1)
                ; Only supporting the Deadly Towers portion ($8000-$FFFF)
                jal     nprgrom
                nop

                _tpa(lbu,t0,chrromsize)
                bnez    t0,badmapper    ; Deadly Towers has VRAM
                nop                     ; Impossible Mission 2 does not.

                j       endofloadrom
                nop
notmapper34

                li      t1,66
                bne     t0,t1,notmapper66
                nop
                ; Mapper 66
                ; Mega Man uses this and works fine with Mapper 2.
                li      t0,2
                _tpa(sb,t0,mapper)

                ; SMB + Duck Hunt has CHRROM and doesn't work.
                _tpa(lbu,t0,chrromsize)
                bnez    t0,badmapper
                nop

                jal     unprgrom
                nop

                jal     nchrrom
                nop

                j       endofloadrom
                nop
notmapper66

                li      t1,71
                bne     t0,t1,notmapper71
                nop
                ; Mapper 71
                _tpa(lbu,t0,romc1)
                andi    t0,1
                bnez    t0,mapper71
                nop
                ; Games which have this mapper and horizontal mirroring set
                ; (neither 7 nor 71 use the mapper bit, both have one screen
                ; mirroring) work better with mapper 7.
                li      t0,7
                _tpa(sb,t0,mapper)
                j       mapper7
                nop
mapper71
                ; A variation on AOROM
                jal     unprgrom
                nop
                j       endofloadrom
                nop
notmapper71
badmapper
                _text_string(20,22,badmap)
                jal     pause
                nop
                j       loadnewrom
                nop
endofloadrom:

                _tpa(lbu,t0,romc1)
                andi    t0,2
                beqz    t0,noSRAM
                nop
                ; read SRAM from controller pack
                jal     loadSRAM
                nop
noSRAM
                la      t0,_VRAMPages
                li      t1,20-1
wbloop          cache   25,0(t0)  ; data hit writeback
                addi    t0,8
                bnez    t1,wbloop
                addi    t1,-1

                ; compile tiles for the first time
                li      t0,0x1c00
comp_load
                srl     t5,t0,10-3
                _vtpa(lw,t5,_vrampages,t5)
                addu    t5,t0
                move    t6,t0
                jal     gcrequest
                nop
                bnez    t0,comp_load
                addi    t0,-0x400

                _tpa(lw,ra,loadromra)
                jr      ra
                nop
                
; Some modules for loadROM
                
nprgrom         ; No Mapper (NROM)-style PRGROM load
                ; The PRG-ROM pages are initially set up for 16K, if we have
                ;  32K we need to reset the PRG-ROM mirroring.
                _tpa(lbu,t0,prgromsize)
                nop
                slti    t0,t0,2
                bnez    t0,not32kPRGROMmap0
                li      t1,0x40-1
                la      t2,prgrom-0x8000
                la      t3,nespagetable+(0xC0*4)
map0resetPRGloop
                sw      t2,(t3)
                addi    t3,4
                bnez    t1,map0resetPRGloop
                addi    t1,-1
not32kPRGROMmap0
                jr      ra
                nop

nchrrom         ; No Mapper (NROM)-style CHRROM load
                ; The VRAM pages must be rewritten to point at the first
                ;  CHRROM page.
                _tpa(lbu,t0,prgromsize)
                la      t1,prog
                _tpa(lbu,t2,chrromsize)
                sll     t0,14
                beqz    t2,nochrrom
                addu    t0,t1
                
                ; Pattern table 0
                _tpa(sw,t0,0*8+_vrampages)
                _tpa(sw,t0,1*8+_vrampages)
                _tpa(sw,t0,2*8+_vrampages)
                _tpa(sw,t0,3*8+_vrampages)
                                
                _tpa(sw,t0,0*8+vrampages+rspdata)
                _tpa(sw,t0,1*8+vrampages+rspdata)
                _tpa(sw,t0,2*8+vrampages+rspdata)
                _tpa(sw,t0,3*8+vrampages+rspdata)

                ; Pattern table 1
                _tpa(sw,t0,4*8+_vrampages)
                _tpa(sw,t0,5*8+_vrampages)
                _tpa(sw,t0,6*8+_vrampages)
                _tpa(sw,t0,7*8+_vrampages)

                _tpa(sw,t0,4*8+vrampages+rspdata)
                _tpa(sw,t0,5*8+vrampages+rspdata)
                _tpa(sw,t0,6*8+vrampages+rspdata)
                _tpa(sw,t0,7*8+vrampages+rspdata)

nochrrom:                
                jr      ra
                nop

unprgrom        ; Mapper #2 (UNROM)-style PRGROM load
                ; Last PRGROM bank loaded into 0xc000, first into 0x8000.
                ; First is handled by default, we just need to change last.
                _tpa(lbu,t2,prgromsize)
                nop
                addi    t2,-1
                sll     t2,10+4 ; 16K
                la      t1,prgrom
                addu    t2,t1
                li      t0,0xC000
                subu    t2,t0

                li      t1,0x40-1
                la      t3,nespagetable+(0xC0*4)
map2resetPRGloop
                sw      t2,(t3)
                addi    t3,4
                bnez    t1,map2resetPRGloop
                addi    t1,-1
                jr      ra
                nop

mmc2prgrom      ; Mapper #9 (MMC2)-style PRGROM load
                ; first 8k ROM bank at $8000, last 8k ROM banks at
                ; $a000, $c000, $e000
                ; First bank is handled fine by default.
                _tpa(lbu,t2,prgromsize)
                nop
                sll     t2,1    ; convert to 8k page count
                addi    t2,-3   ; 3rd to last page #
                sll     t2,10+3 ; 8K
                la      t1,prgrom
                addu    t2,t1
                li      t1,0xA000
                subu    t2,t1

                li      t1,0x60-1
                la      t3,nespagetable+(0xA0*4)
map9resetPRGloop
                sw      t2,(t3)
                addi    t3,4
                bnez    t1,map9resetPRGloop
                addi    t1,-1
                jr      ra
                nop


; ROM info
prgromsize:     equ     proga+4
chrromsize:     equ     proga+5
romc1:          equ     proga+6
romc2:          equ     proga+7
intromsg5       db      "Loading...",0

badfmt          db      "ERROR: ROM is not in iNES",$0a
                db      "       format!",0
badmap          db      "ERROR: Unsupported mapper!",0
mmm0            db      "Loading...",0

 _align(4)
loadromra       dw      0
sramra          dw      0

; Swordchucks, yo!
