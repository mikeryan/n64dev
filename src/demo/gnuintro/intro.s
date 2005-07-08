# It's one big thank you to CZN. This code is completley ripped from them
# This is just a 'rewrite' so that people can do the same in GNU asm.
	
	.equ SCREENBUFFER, czn

	.set noreorder
	
	.globl start
	.ent start
start:
        la  $sp,0x80400000-16
        jal  initialise                 # init n64 screen
        nop
loop:	beq $0,$0, loop
	nop
	.end start
	
#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
# INITIALISE N64
#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北

	.globl initialise
	.ent initialise
initialise:
        li   $8,8
        sw   $8,(0xbfc007fc)
#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
#  CLEAR LE SCREEN AERA
#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
#        la   $8,SCREENBUFFER
#        li   $9,320*245*2*4
#        li   $11, 0xffffffff
#loopclear:
#        sw   $11,0($8)
#        add  $8,4
#        bne  $9,$0,loopclear
#        sub  $9,4

#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
#  Init video and SCREEN DIPLAY ADRESS
#北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
        li   $8,0xa4400000
        li   $9,0x13006
        sw   $9,0($8)
        la   $9,SCREENBUFFER
        sw   $9,4($8)
        li   $9,320
        sw   $9,8($8)
        li   $9,0x2
        sw   $9,12($8)
        li   $9,0x0
        sw   $9,16($8)
        li   $9,0x3e52239
        sw   $9,20($8)
        li   $9,0x0000020d
        sw   $9,24($8)
        li   $9,0x00000c15
        sw   $9,28($8)
        li   $9,0x0c150c15
        sw   $9,32($8)
        li   $9,0x006c02ec
        sw   $9,36($8)
        li   $9,0x002501ff
        sw   $9,40($8)
        li   $9,0x000e0204
        sw   $9,44($8)
        li   $9,0x200
        sw   $9,48($8)
        li   $9,0x400
        sw   $9,52($8)
        jr   $31
        nop
	.end initialise
