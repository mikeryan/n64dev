/* si.h - Serial Interface */

#ifndef _SIDECL_H
#define _SIDECL_H

#define SI_BASE_REG		0x04800000

/* SI DRAM address (R/W): [23:0] starting RDRAM address */
#define SI_DRAM_ADDR_REG	(SI_BASE_REG+0x00)	/* R0: DRAM address */

/* SI address read 64B (W): [] any write causes a 64B DMA write */
#define SI_PIF_ADDR_RD64B_REG	(SI_BASE_REG+0x04)	/* R1: 64B PIF->DRAM */

/* Address SI_BASE_REG + (0x08, 0x0c, 0x14) are reserved */

/* SI address write 64B (W): [] any write causes a 64B DMA read */
#define SI_PIF_ADDR_WR64B_REG	(SI_BASE_REG+0x10)	/* R4: 64B DRAM->PIF */

/* 
 * SI status (W): [] any write clears interrupt
 *           (R): [0] DMA busy, [1] IO read busy, [2] reserved
 *                [3] DMA error, [12] interrupt
 */
#define SI_STATUS_REG		(SI_BASE_REG+0x18)	/* R6: Status */

/* SI status register has the following bits active:
 *	0:   DMA busy		- set when DMA is in progress
 *	1:   IO busy		- set when IO access is in progress
 *	3:   DMA error		- set when there are overlapping DMA requests
 *     12:   Interrupt		- Interrupt set
 */
#define	SI_STATUS_DMA_BUSY	0x0001
#define	SI_STATUS_RD_BUSY	0x0002
#define	SI_STATUS_DMA_ERROR	0x0008
#define	SI_STATUS_INTERRUPT	0x1000

#define SI_WAIT while (IO_READ(SI_STATUS_REG)&0x3)
#define SI_BUSY (IO_READ(SI_STATUS_REG)&0x3)

#endif /* _SIDECL_H */
