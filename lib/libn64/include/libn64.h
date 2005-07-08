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
/* cache.h */

#ifndef _CACHEDECL_H
#define _CACHEDECL_H

extern void writeBackInvalDCache();
extern void __cpu_flush();

#endif /* _CACHEDECL_H */
/* cd64.h - CD64 specific functions */

extern void cd64_runcart();
extern void cd64_init();
extern void cd64_sendbyte(char c);
extern char cd64_recvbyte();
extern void cd64_pc_connect();
extern void cd64_mode(int mode);
int cd64_read(int fd,void *buf,size_t nbytes);
int cd64_write(int fd,const void *buf,size_t nbytes);
/* contpad.h  - Control Pad and Joystick */

#ifndef _CONTPADDECL_H
#define _CONTPADDECL_H

enum contPadButton {A_BUTTON=15,B_BUTTON=14,Z_TRIG=13,START_BUTTON=12,U_JPAD=11,D_JPAD=10,L_JPAD=9,R_JPAD=8,L_TRIG=5,R_TRIG=4,U_CBUTTONS=3,D_CBUTTONS=2,L_CBUTTONS=1,R_CBUTTONS=0};
typedef enum contPadButton contPadButton;

/* external functions */

void initContPads();
void readContPads();
int contPadToggle(int pad,contPadButton button);

#define PIF_ROM_START		0x1FC00000
#define PIF_ROM_END		0x1FC007BF
#define PIF_RAM_START		0x1FC007C0
#define PIF_RAM_END		0x1FC007FF

typedef struct {
  u32 cmd;
  unsigned A_BUTTON : 1;
  unsigned B_BUTTON : 1;
  unsigned Z_TRIG : 1;
  unsigned START_BUTTON : 1;
  unsigned U_JPAD : 1;
  unsigned D_JPAD : 1;
  unsigned L_JPAD : 1;
  unsigned R_JPAD : 1;
  unsigned empty : 2;
  unsigned L_TRIG : 1;
  unsigned R_TRIG : 1;
  unsigned U_CBUTTONS : 1;
  unsigned D_CBUTTONS : 1;
  unsigned L_CBUTTONS : 1;
  unsigned R_CBUTTONS : 1;
  s8      stick_x;		/* -80 <= stick_x <= 80 */
  s8      stick_y;		/* -80 <= stick_y <= 80 */
} PifContPad_t;

typedef union {
  PifContPad_t cont[8];
  u64 boundary_alignment;
} Pif;

/*
Each game controller channel has 4 error bits that are defined in bit
6-7 of cmd and res 
*/

#define CHNL_ERR_NORESP         0x80    /* Bit 7 (Rx): No response error */
#define CHNL_ERR_OVERRUN        0x40    /* Bit 6 (Rx): Overrun error */
#define CHNL_ERR_FRAME          0x80    /* Bit 7 (Tx): Frame error */
#define CHNL_ERR_COLLISION      0x40    /* Bit 6 (Tx): Collision error */
#define CHNL_ERR_MASK           0xC0    /* Bit 6-7: channel errors */


/* controller errors */
#define CONT_NO_RESPONSE_ERROR          0x8
#define CONT_OVERRUN_ERROR              0x4

/* Controller type */

#define CONT_ABSOLUTE           0x0001
#define CONT_RELATIVE           0x0002
#define CONT_JOYPORT            0x0004
#define CONT_EEPROM		0x8000

/* Controller status */

#define CONT_CARD_ON            0x01
#define CONT_CARD_PULL          0x02
#define CONT_ADDR_CRC_ER        0x04
#define CONT_EEPROM_BUSY	0x80

/* global pif, should get rid of this and have only functions in contpad.c */

extern Pif *pifr;

#define CONT(x) pifr->cont[x]
#define CONT_EXISTS(x) (pifr->cont[x].cmd&CONT_NO_RESPONSE_ERROR)

#endif /* _CONTPADDECL_H */
/* jpegd.h */

#ifndef _JPEGDDECL_H_
#define _JPEGDDECL_H_

#define PACK_RGBA5551(r,g,b,a)   ((((r)<<8) & 0xf800) | \
                                  (((g)<<3) & 0x7c0)  | \
                                  (((b)>>2) & 0x3e)   | \
                                  (((a)>>7) & 0x1))

#define PACK_RGBA8888(r,g,b,a)   ((((u32)r<<24 & 0xff000000) | \
                                   ((u32)g<<16 & 0x00ff0000) | \
                                   ((u32)b<<8 & 0x0000ff00) | \
                                   ((u32)a & 0xff)))

typedef struct jpegfile {
  void* jpeg_image;
  void* rgb_image;
  int jpeg_length;
  u16 w,h,rgb_bpp,autoscale;
} jpegfile;

extern void read_JPEG_file (jpegfile *jpgstruct);
#endif

#ifndef _DEBUGDECL_H
#define _DEBUGDECL_H

typedef struct cartemu {
  void (*runCart)();
  void (*sendByte)(char c);
  char (*recvByte)();
  int (*read)(int fd,void *buf,size_t nbytes);
  int (*write)(int fd,const void *buf,size_t nbytes);
} cartemu;

/* keep this a multiple of 4, align all variables with a 4 byte boundary.
   The n64 doesn't like reading across the end of a 16 byte boundary like
   0xb008d93e (u16 read) 
*/
#define FILENAME_LEN 24
typedef struct fs { // sizeof=2*16
  u8 filename[FILENAME_LEN];
  u32 length;
  u32 next_file;
} fs_struct;

typedef enum debugType { NONE,ONSCREEN,CD64_PROCOMM,V64JR,UPDOWNLOADER } debugType;

extern cartemu debug;

void initDebug(debugType type);
void print(char *string);
int putnum (unsigned int num);
void send_hex_u32(u32 h);
void dump_mem(u32 *ptr,int n);

#endif /* _DEBUGDECL_H */
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
/* vi.h */

#ifndef _VIDECL_H_
#define _VIDECL_H_

void initVideo(void *cfb,u32 *pal,u32 *ntsc,u32 *mpal);
void drawSprite(u16 *cfb,u16 *sprite,int x,int y);

/* primary colours */

#define RED 0xF00
#define GREEN 0x07c0
#define BLUE 0x00F0

/* info set up by boot code */

#define tvType   *((u32*)0x80000300)
#define TV_PAL  0
#define TV_NTSC 1
#define TV_MPAL 2

/* VI Registers */

#define VI_BASE_REG             0x04400000
#define VI_STATUS_REG           VI_BASE_REG
#define VI_DRAM_ADDR_REG        (VI_BASE_REG+0x04)
#define VI_H_WIDTH_REG          (VI_BASE_REG+0x08)
#define VI_V_INTR_REG           (VI_BASE_REG+0x0C)
#define VI_CURRENT_REG          (VI_BASE_REG+0x10)
#define VI_TIMING_REG           (VI_BASE_REG+0x14)
#define VI_V_SYNC_REG           (VI_BASE_REG+0x18)
#define VI_H_SYNC_REG           (VI_BASE_REG+0x1C)
#define VI_LEAP_REG             (VI_BASE_REG+0x20)
#define VI_H_VIDEO_REG          (VI_BASE_REG+0x24)
#define VI_V_VIDEO_REG          (VI_BASE_REG+0x28)
#define VI_V_BURST_REG          (VI_BASE_REG+0x2C)
#define VI_X_SCALE_REG          (VI_BASE_REG+0x30)
#define VI_Y_SCALE_REG          (VI_BASE_REG+0x34)

extern u32 VI_NTSC_LPN1[]; extern u32 VI_PAL_LPN1[]; extern u32 VI_MPAL_LPN1[];
extern u32 VI_NTSC_LPF1[]; extern u32 VI_PAL_LPF1[]; extern u32 VI_MPAL_LPF1[];extern u32 VI_NTSC_LAN1[]; extern u32 VI_PAL_LAN1[]; extern u32 VI_MPAL_LAN1[];
extern u32 VI_NTSC_LAF1[]; extern u32 VI_PAL_LAF1[]; extern u32 VI_MPAL_LAF1[];
extern u32 VI_NTSC_LPN2[]; extern u32 VI_PAL_LPN2[]; extern u32 VI_MPAL_LPN2[];
extern u32 VI_NTSC_LPF2[]; extern u32 VI_PAL_LPF2[]; extern u32 VI_MPAL_LPF2[];
extern u32 VI_NTSC_LAN2[]; extern u32 VI_PAL_LAN2[]; extern u32 VI_MPAL_LAN2[];
extern u32 VI_NTSC_LAF2[]; extern u32 VI_PAL_LAF2[]; extern u32 VI_MPAL_LAF2[];
extern u32 VI_NTSC_HPN1[]; extern u32 VI_PAL_HPN1[]; extern u32 VI_MPAL_HPN1[];
extern u32 VI_NTSC_HPF1[]; extern u32 VI_PAL_HPF1[]; extern u32 VI_MPAL_HPF1[];
extern u32 VI_NTSC_HAN1[]; extern u32 VI_PAL_HAN1[]; extern u32 VI_MPAL_HAN1[];
extern u32 VI_NTSC_HAF1[]; extern u32 VI_PAL_HAF1[]; extern u32 VI_MPAL_HAF1[];
extern u32 VI_NTSC_HPN2[]; extern u32 VI_PAL_HPN2[]; extern u32 VI_MPAL_HPN2[];
extern u32 VI_NTSC_HPF2[]; extern u32 VI_PAL_HPF2[]; extern u32 VI_MPAL_HPF2[];

#endif /* _VIDECL_H_ */

