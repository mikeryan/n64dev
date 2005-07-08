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

