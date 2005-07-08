/* ai.h - Audio Interface */

#ifndef _AIDECL_H
#define _AIDECL_H

void initAudio();
void playAudio(u32* sample,u32 len);

/* AI Registers */

#define AI_BASE_REG		0x04500000
#define AI_DRAM_ADDR_REG	(AI_BASE_REG+0x00)     /* R0: DRAM address */
#define AI_LEN_REG		(AI_BASE_REG+0x04)     /* R1: Length */
#define AI_CONTROL_REG		(AI_BASE_REG+0x08)     /* R2: DMA Control */
#define AI_STATUS_REG		(AI_BASE_REG+0x0C)     /* R3: Status */
#define AI_DACRATE_REG		(AI_BASE_REG+0x10)     /* R4: DAC rate 14-lsb*/
#define AI_BITRATE_REG		(AI_BASE_REG+0x14)     /* R5: Bit rate 4-lsb */
#define AI_CONTROL_DMA_ON	0x01		       /* LSB = 1: DMA enable*/
#define AI_CONTROL_DMA_OFF	0x00		       /* LSB = 1: DMA enable*/
#define AI_STATUS_FIFO_FULL	0x80000000     	       /* Bit 31: full */
#define AI_STATUS_DMA_BUSY	0x40000000     	       /* Bit 30: busy */

#define VI_NTSC_CLOCK		48681812        /* Hz = 48.681812 MHz */
#define VI_PAL_CLOCK		49656530        /* Hz = 49.656530 MHz */
#define VI_MPAL_CLOCK		48628316        /* Hz = 48.628316 MHz */

#endif /* _AIDECL_H */
