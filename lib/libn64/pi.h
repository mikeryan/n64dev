/* pi.h */

#ifndef _PIDECL_H
#define _PIDECL_H

void initPi();
void piDmaRead(void *dest,void *src,u32 length);

/*
 * Peripheral Interface (PI) Registers 
 */

#define PI_BASE_REG		0x04600000

/* PI DRAM address (R/W): [23:0] starting RDRAM address */
#define PI_DRAM_ADDR_REG	(PI_BASE_REG+0x00)	/* DRAM address */

/* PI pbus (cartridge) address (R/W): [31:0] starting AD16 address */
#define PI_CART_ADDR_REG	(PI_BASE_REG+0x04)

/* PI read length (R/W): [23:0] read data length */
#define PI_RD_LEN_REG		(PI_BASE_REG+0x08)

/* PI write length (R/W): [23:0] write data length */
#define PI_WR_LEN_REG		(PI_BASE_REG+0x0C)

/* 
 * PI status (R): [0] DMA busy, [1] IO busy, [2], error
 *           (W): [0] reset controller (and abort current op), [1] clear intr
 */
#define PI_STATUS_REG		(PI_BASE_REG+0x10)

/*
 * PI status register has 3 bits active when read from (PI_STATUS_REG - read)
 *	Bit 0: DMA busy - set when DMA is in progress
 *	Bit 1: IO busy  - set when IO is in progress
 *	Bit 2: Error    - set when CPU issues IO request while DMA is busy
 */
#define	PI_STATUS_ERROR		0x04
#define	PI_STATUS_IO_BUSY	0x02
#define	PI_STATUS_DMA_BUSY	0x01

/* PI status register has 2 bits active when written to:
 *	Bit 0: When set, reset PIC
 *	Bit 1: When set, clear interrupt flag
 * The values of the two bits can be ORed together to both reset PIC and 
 * clear interrupt at the same time.
 *
 * Note: 
 *	- The PIC does generate an interrupt at the end of each DMA. CPU 
 *	needs to clear the interrupt flag explicitly (from an interrupt 
 *	handler) by writing into the STATUS register with bit 1 set.
 *
 *	- When a DMA completes, the interrupt flag is set.  CPU can issue
 *	another request even while the interrupt flag is set (as long as
 *	PIC is idle).  However, it is the CPU's responsibility for
 *	maintaining accurate correspondence between DMA completions and
 *	interrupts.
 *
 *	- When PIC is reset, if PIC happens to be busy, an interrupt will
 *	be generated as PIC returns to idle.  Otherwise, no interrupt will
 *	be generated and PIC remains idle.
 */
/*
 * Values to clear interrupt/reset PIC (PI_STATUS_REG - write)
 */
#define	PI_SET_RESET		0x01
#define	PI_CLR_INTR		0x02

#define PI_WAIT while (IO_READ(PI_STATUS_REG)&(PI_STATUS_IO_BUSY     \
					      |PI_STATUS_DMA_BUSY));
#endif /* _PIDECL_H */
