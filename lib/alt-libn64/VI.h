#ifndef _VI_H
#define _VI_H

typedef struct VI_regs_s {
   unsigned long control;
   unsigned short int * framebuffer;
   unsigned long width;
   unsigned long v_int;
   unsigned long cur_line;
   unsigned long timing;
   unsigned long v_sync;
   unsigned long h_sync;
   unsigned long h_sync2;
   unsigned long h_limits;
   unsigned long v_limits;
   unsigned long color_burst;
   unsigned long h_scale;
   unsigned long v_scale;
} _VI_regs_s;

extern volatile struct VI_regs_s * const VI_regs;

typedef struct VI_config {
    int bitdepth;
    int xres,yres;
    float xscale,yscale;
    unsigned short int * framebuffer;
} _VI_config;

extern unsigned char VI_fontdata[2048];

void VI_WriteRegs(struct VI_config *);

void VI_RetraceStartWait(void);

void VI_FillScreen(struct VI_config *, unsigned short);

void VI_DrawPixel(struct VI_config *, unsigned int, unsigned int,
	unsigned short);

void VI_DrawText(struct VI_config *, int, int, unsigned char *);

#endif // _VI_H
