; Management for battery backed RAM
; the term SRAM is more accurately WRAM

; 1. check for controller pack
; 2. check if controller pack is correctly formatted
; 3. calculate current game's CRC
; 4. look for current game in save header
; 5. write current save space variable with current game's save's location,
;    $ff if not found
; 6. if found load from controller pack, if not found fill sram with zeros
; 7. if found print game name and CRC, do short delay

loadSRAM
        _tpa(sw,ra,loadSRAMra)

        jal     crc
        nop
        _tpa(sh,t0,doyouseewhatisee)

        li      t0,$ff
        _tpa(sb,t0,savespace)
        jal checkforconpack
        nop
        bnez    t0,lsrconpackfound
        nop
        j       loadSRAMend
        nop
lsrconpackfound
; All other subroutines should be able to use the sramheader that this loads,
; and any changes will be made to both the one in RAM and on the pack, so this
; is the only point where we need to load sramheader.
        la      t0,sramheader
        jal     read_conpack
        li      t1,0
        la      t0,sramheader+32
        jal     read_conpack
        li      t1,1
        la      t0,sramheader+64
        jal     read_conpack
        li      t1,2
        la      t0,sramheader+96
        jal     read_conpack
        li      t1,3

        jal     checkformat
        nop
        bnez    t2,lsrfmtgood
        nop
        j       loadSRAMend
        nop
lsrfmtgood
        _tpa(lhu,t0,doyouseewhatisee)        
        _tpa(lhu,t1,sramheader+32)  ; save 0 CRC
        beq     t0,t1,SRAMfound
        li      t5,0
        _tpa(lhu,t1,sramheader+64)  ; save 1 CRC
        beq     t0,t1,SRAMfound
        li      t5,1
        _tpa(lhu,t1,sramheader+96)  ; save 2 CRC
        beq     t0,t1,SRAMfound
        li      t5,2

        ; not found
        la      t6,sram
        li      t7,$2000/8-1
sramclearloop
        sd      r0,(t6)
        addi    t6,8
        bnez    t7,sramclearloop
        addi    t7,-1

        j       loadSRAMend
        nop
        
SRAMfound

        _tpa(sb,t5,savespace)
;        ; write crc
;        _text_value(20,58,t0,3)
;        ; write name
;        sll     t4,t5,5
;        la      t3,sramheader+34
;        addu    t3,t4
;        _text_reg_string(20,50,t3)
;
;        jal     pause
;        nop

        addi    t5,1
        sll     t5,10+3-5 ; 8K/32
        la      t6,sramm
        li      t7,$2000/32-1
loadconpack
        move    t0,t6
        move    t1,t5
        jal     read_conpack
        nop
        addiu   t6,32
        addi    t5,1
        bnez    t7,loadconpack
        addi    t7,-1

        la      t6,sramm
        la      t7,sram
        li      t5,$2000/8-1
transsram
        ld      t0,(t6)
        sd      t0,(t7)
        addi    t6,8
        addi    t7,8
        bnez    t5,transsram
        addi    t5,-1

loadSRAMend
        _tpa(lw,ra,loadSRAMra)
        jr      ra
        nop

; 1. check for controller pack, if not present whine
; 2. check controller format, if not correct ask to reformat
; 3. check current save space variable, if $ff call newSRAM to make a new
;    space (if still $ff when we return we've backed out)
; 4. write sram to controller pack
saveSRAM
        _tpa(sw,ra,saveSRAMra)
        jal     cls
        nop
        jal     checkforconpack
        nop
        bnez    t0,ssrconpackfound
        nop
        _text_string(20,50,noconpack)
        jal     pause
        nop
        j       saveSRAMend
        nop
ssrconpackfound

        jal     checkformat
        nop
        bnez    t2,ssrfmtgood
        nop
        _text_string(20,50,formatmsg)
        li      t1,20
        li      t2,98
        jal     yesnotog
        nop
        beqz    t0,saveSRAMend
        nop

        jal     cls
        nop

        ; write blank header
        _tpa(ld,t0,saveheader)
        _tpa(sd,t0,sramheader)
        _tpa(ld,t0,saveheader+8)
        _tpa(sd,t0,sramheader+8)
        li      t0,128-16-1
        la      t1,sramheader+16
clearheader
        sb      r0,(t1)
        addi    t1,1
        bnez    t0,clearheader
        addi    t0,-1

        jal     writeheader
        nop

ssrfmtgood

        _tpa(lbu,t0,savespace)
        li      t1,$ff
        bne     t0,t1,ssrspacefound
        nop
        jal     newSRAM
        nop
        _tpa(lbu,t0,savespace)
        li      t1,$ff
        beq     t0,t1,saveSRAMend   ; backed out
        nop
ssrspacefound

        la      t6,sram
        la      t7,sramm
        li      t5,$2000/8-1
transsram2
        ld      t1,(t6)
        sd      t1,(t7)
        addi    t6,8
        addi    t7,8
        bnez    t5,transsram2
        addi    t5,-1

        addi    t5,t0,1
        sll     t5,10+3-5 ; 8K/32
        la      t6,sramm
        li      t7,$2000/32-1
saveconpack
        move    t0,t6
        move    t1,t5
        jal     write_conpack
        nop
        addiu   t6,32
        addi    t5,1
        bnez    t7,saveconpack
        addi    t7,-1        

saveSRAMend
        jal     cls
        nop
        _tpa(lw,ra,saveSRAMra)
        jr      ra
        nop

; 1. try to find an empty space (CRC=0) to write the new save
; 2. if not found ask if one should be deleted, call deleteSRAM
; 3. when empty space found, calculate CRC and get name
; 4. write data into header
; 5. write header to controller pack
; 6. set current save space variable to new space

newSRAM
        _tpa(sw,ra,newSRAMra)

        _tpa(lhu,t1,sramheader+32)  ; save 0 CRC
        beqz    t1,emptySRAMfound   ; I hope zero CRCs aren't common...
        li      t7,0
        _tpa(lhu,t1,sramheader+64)  ; save 1 CRC
        beqz    t1,emptySRAMfound
        li      t7,1
        _tpa(lhu,t1,sramheader+96)  ; save 2 CRC
        beqz    t1,emptySRAMfound
        li      t7,2

        ; no empty space found
        _text_string(20,50,deleteynmsg)
        li      t1,20
        li      t2,82
        jal     yesnotog
        nop
        beqz    t0,newSRAMend
        nop

        jal     cls
        nop
        jal     deleteSRAM
        nop

        li  t6,$ff
        beq t6,t7,newSRAMend    ; user chickened out
        nop

emptySRAMfound

        _tpa(sb,t7,savespace)
        _tpa(lhu,t0,doyouseewhatisee)

        addi    t7,1
        sll     t7,5
        la      t5,sramheader
        addu    t5,t7
        sh      t0,(t5)

        ; initialize name
        addi    t5,2
        move    t7,t5
        li      t0,25-1
        li      t1,32   ; space character
namespace
        sb      t1,(t5)
        addi    t5,1
        bnez    t0,namespace    ; assembly label jokes aren't funny
        addi    t0,-1
        sb      r0,(t5)

        li      t3,0    ; which character we're on
        _text_string(20,42,entername)
save_nameloop
        _text_reg_string(20,66,t7)

        li      t2,20
        sll     t1,t3,3
        add     t2,t1
        _tpa(sw,t2,text_x)
        li      t2,58
        _tpa(sw,t2,text_y)
        _tpa(sw,r0,text_type)
        la      t1,enterprompt
        _tpa(sw,t1,text_msg)
        jal     text_blit
        nop

        jal     pause
        nop

        move    t5,t0

        li      t2,20
        sll     t1,t3,3
        add     t2,t1
        _tpa(sw,t2,text_x)
        li      t2,58
        _tpa(sw,t2,text_y)
        _tpa(sw,r0,text_type)
        la      t1,enterpromptclear
        _tpa(sw,t1,text_msg)
        jal     text_blit
        nop

        move    t0,t5

        addu    t2,t3,t7
        lbu     t1,(t2)

        andi    t5,t0,$0400 ; down
        beqz    t5,save_nodown
        nop
        addi    t1,-1
        li      t4,$40  ; below A
        bne     t1,t4,save_noloalph
        nop
        li      t1,32
save_noloalph
        li      t4,31   ; below space
        bne     t1,t4,save_nolospace
        nop
        li      t1,$39  ; 9
save_nolospace
        li      t4,$2f  ; below 0
        bne     t1,t4,save_nolonum
        nop
        li      t1,$5a
save_nolonum
        sb      t1,(t2)
save_nodown

        andi    t5,t0,$0800 ; up
        beqz    t5,save_noup
        nop
        addi    t1,1
        li      t4,$5b  ; above Z
        bne     t1,t4,save_nohialph
        nop
        li      t1,$30  ; 0
save_nohialph
        li      t4,$3a  ; above 9
        bne     t1,t4,save_nohinum
        nop
        li      t1,32
save_nohinum
        li      t4,33   ; above space
        bne     t1,t4,save_nohispace
        nop
        li      t1,$41
save_nohispace
        sb      t1,(t2)
save_noup

;        andi    t5,t0,$2000
;        beqz    t5,save_noz
;        nop
;        xori    t1,$20
;        sb      t1,(t2)
;save_noz

        andi    t5,t0,$0100
        beqz    t5,save_noright
        nop
        addi    t3,1
        li      t4,25
        bne     t4,t3,save_noright
        nop
        li      t3,24
save_noright

        andi    t5,t0,$0200
        beqz    t5,save_noleft
        nop
        addi    t3,-1
        bgez    t3,save_noleft
        nop
        li      t3,0
save_noleft

        andi    t0,$1000
        beqz    t0,save_nameloop
        nop

        jal     writeheader
        nop

newSRAMend
        jal     cls
        nop
        _tpa(lw,ra,newSRAMra)
        jr      ra
        nop

; 1. display names of those currently on cart in menu style
; 2. get user selection
; 3. get user confirmation
; 4. write zeroes to the selected region of header
; 5. write header to controller pack
; return which save was deleted in t7

deleteSRAM
        _tpa(sw,ra,deleteSRAMra)

        _text_string(20,50,deletemsg)

        li      t7,0
delmenloop
        li      t0,44
        _tpa(sw,t0,text_x)
        _tpa(sw,r0,text_type)
        li      t5,66
        li      t6,2
        move    t4,t7
        la      t1,sramheader+34
delmenu
        li      t3,$0001
        bnez    t4,shdno
        li      t2,$ffff
        li      t2,$0001
        li      t3,$ffff
shdno   la      s8,text_color
        sw      t2,0(s8)
        sw      t3,4(s8)

        _tpa(sw,t1,text_msg)
        _tpa(sw,t5,text_y)

        jal     text_blit
        nop

        addi    t1,32
        addi    t4,-1
        addi    t5,8
        bnez    t6,delmenu
        addi    t6,-1

        li      t3,$0001
        bnez    t4,shdnonone
        li      t2,$ffff
        li      t2,$0001
        li      t3,$ffff
shdnonone la      s8,text_color
        sw      t2,0(s8)
        sw      t3,4(s8)
        _text_string(44,90,none)

        jal     pause
        nop

        andi    t1,t0,$0800
        beqz    t1,del_noup
        nop
        addi    t7,-1
        bgez    t7,del_noup
        nop
        li      t7,0
del_noup

        andi    t1,t0,$0400
        beqz    t1,del_nodown
        nop
        addi    t7,1
        li      t1,4
        bne     t7,t1,del_nodown
        nop
        li      t7,3
del_nodown

        andi    t0,$8000
        beqz    t0,delmenloop
        nop

        jal     cls
        nop

        li      t1,3
        bne     t7,t1,del_nonone
        nop
        li      t7,$ff
del_nonone

        ; doesn't actually have to do any deletion, as soon as this returns
        ; newSRAM will overwrite the value.

        li      t2,$ffff
        li      t3,$0001
        la      s8,text_color
        sw      t2,0(s8)
        sw      t3,4(s8)
deleteSRAMend
        _tpa(lw,ra,deleteSRAMra)
        jr      ra
        nop

; returns 0 or 1 (not found or found) in t0
; might also detect a rumble pack... oh well
checkforconpack
        _tpa(sw,ra,cfconpackra)
        ;jal     cls
        ;nop

        jal     getconstatus
        nop
        _tpa(lbu,t0,PI_result_block+6)
        li      t1,1    ; something is plugged into the mempack slot
        beq     t0,t1,conpackfound
        li      t0,1
        li      t0,0
conpackfound
        _tpa(lw,ra,cfconpackra)
        jr      ra
        nop
        
; t2=0 if wrong format, 1=right format
checkformat
        _tpa(sw,ra,checkformatra)
        _tpa(lbu,t0,sramheader+15)
        bnez    t0,savever1not
        nop
        ; Version 0 checked only first dword by mistake
        _tpa(ld,t0,saveheader)
        _tpa(ld,t1,sramheader)
        bne     t0,t1,wrongformat
        nop

        _tpa(ld,t0,saveheader)
        _tpa(sd,t0,sramheader)
        _tpa(ld,t0,saveheader+8)
        _tpa(sd,t0,sramheader+8)

        jal     writeheader
        nop

        j       rightformat
        li      t2,1
savever1not
        _tpa(ld,t0,saveheader)
        _tpa(ld,t1,sramheader)
        bne     t0,t1,wrongformat
        _tpa(ld,t0,saveheader+8)
        _tpa(ld,t1,sramheader+8)
        beq     t0,t1,rightformat
        li      t2,1
wrongformat
        li      t2,0
rightformat
        _tpa(lw,ra,checkformatra)
        jr      ra
        nop

; CRC routine from a post on Dextrose by Halcyon (converted from BASIC)
; CRC returned in t0
crc     li      t0,0
        _tpa(lbu,t1,prgromsize) ; 16K PRGROM pages
        _tpa(lbu,t2,chrromsize) ; 8K CHRROM pages
        sll     t1,14
        sll     t2,13
        add     t1,t2
        addi    t1,16-1  ; header
        la      t2,prgrom-16 ;proga
crcloop lbu     t3,(t2)
        li      t4,$80
crcloop2
        and     t5,t4,t3
        slti    t5,t5,1
        andi    t6,t0,32768
        slti    t6,t6,1
        xor     t5,t6   ; testbit
        andi    t0,32767
        sll     t0,1
        beqz    t5,nocrcxor
        srl     t4,1
        xori    t0,$8005
nocrcxor
        bnez    t4,crcloop2
        nop
        addi    t2,1
        bnez    t1,crcloop
        addi    t1,-1
        jr      ra
        andi    t0,$ffff

writeheader
        _tpa(sw,ra,writeheaderra)

        la      t0,sramheader
        jal     write_conpack
        li      t1,0

        la      t0,sramheader+32
        jal     write_conpack
        li      t1,1

        la      t0,sramheader+64
        jal     write_conpack
        li      t1,2

        la      t0,sramheader+96
        jal     write_conpack
        li      t1,3

        _tpa(lw,ra,writeheaderra)
        jr      ra
        nop

; Get the answer to a yes/no question
; t0: 0=no 1=yes
; parameters: t1=x coord, t2=y coord
; Start confirms
yesnotog
    _tpa(sw,ra,yesnotogra)
    _tpa(sw,t1,text_x)
    _tpa(sw,t2,text_y)
    _tpa(sw,r0,text_type)

    li  t7,1    ; decision (no initially)

yesnoloop
    xori    t7,1
    sll     t0,t7,2 ; no is 4 characters
    la      t3,no
    addu    t0,t3
    _tpa(sw,t0,text_msg)

    jal     text_blit
    nop

    jal     Pause
    nop
    andi    t0,$1000

    beqz    t0,yesnoloop
    nop

    move    t0,t7
    _tpa(lw,ra,yesnotogra)
    jr  ra
    nop


; The prototype save file header: (size=128 bytes)
 _align(8)
saveheader
    db  "HCS Neon64 v1.2",1  ; 16 bytes
                        ; ^ last byte is save file version #
                        ; Version 0: had an error, the header said only
                        ; "HCS Neon", not "HCS Neon64 v1.1",0. Otherwise the
                        ; format is the same.
                        ; Version 1: current

doyouseewhatisee
    dh  0
;    dcb 16,0
;save0crc
;    dh  0
;save0name
;    dcb 30,0
;save1crc
;    dh  0
;save1name
;    dcb 30,0
;save2crc
;    dh  0
;save2name
;    dcb 30,0

noconpack
    db      "Controller pack in controller 1",$0a,"not detected.",0
formatmsg
    db      "The current controller",$0a,"pack is not formatted",$0a
    db      "properly for Neon64.",$0a,"Format now? (This will",$0a
    db      "erase ANYTHING on the",$0a,"controller pack!)",$0a
    db      "    (Press START to confirm)",0
deleteynmsg
    db      "The controller pack is",$0a,"full (max 3 saves),",$0a
    db      "do you want to delete a",$0a,"save to make room?",$0a
    db      "    (Press START to confirm)",0
no  db      "No ",0
yes db      "Yes",0
entername
    db      "Enter a name for this game's",$0a
    db      "save (START to confirm)",0
enterprompt
    db      25,$0a,$0a,24,0
enterpromptclear
    db      " ",$0a,$0a," ",0
deletemsg
    db      "Choose a save to delete",$0a,"(Press A to confirm)",$0a
    db      "1.",$0a,"2.",$0a,"3.",0
none db "None",0

 _align(4)
deleteSRAMra    dw  0
newSRAMra       dw  0
saveSRAMra      dw  0
loadSRAMra      dw  0
cfconpackra     dw  0
checkformatra   dw  0
yesnotogra      dw  0
writeheaderra   dw  0
