/*
   N64 init code for GNU as
   by hcs
*/

#include "regs.S"

	.globl _start
_start:
	la sp, 0x803ffff0	/* init stack (assumes no mem pack for now) */
	la gp, _gp		/* init data pointer */
        li v0, 8
        sw v0,(0xbfc007fc)	/* magic N64 hardware init */

    	/* a bit from libgloss so we start at a known state */
    	li v0,SR_CU1|SR_PE|SR_FR|SR_KX|SR_SX|SR_UX
	mtc0 v0,C0_SR
	mtc0 $00,C0_CAUSE

/* fill .bss with 0s */
	/* Causes problems with programs that have framebuffer uninitialized,
	   when writing uncached and cache must be dumped this init'ed data
	   gets written back. Best leave uninitialized that way.
    	la $08, __bss_start
	la $09, __bss_end-8
bss_init:
	sd $0,($08)
    	addiu $08,8
	bltu $08,$09, bss_init
	*/
        jal UltraMain
