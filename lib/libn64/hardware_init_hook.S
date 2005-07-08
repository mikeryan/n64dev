#include "mips.h"
	.set noreorder
	.text
	.align	4	
	.globl hardware_init_hook
	.ent hardware_init_hook

hardware_init_hook:			
	li   t0,8
        sw   t0,(0xBfc007fc) # Initialise PIF
	.end hardware_init_hook
	
