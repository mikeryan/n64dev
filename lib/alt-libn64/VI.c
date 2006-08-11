/*
 * VI (Video Interface)
 */

#include "VI.h"
#include "VI_font.c"

volatile struct VI_regs_s * const VI_regs = (struct VI_regs_s *)0xa4400000;

void VI_WriteRegs(struct VI_config * conf) {
   VI_regs->control=0x103000 | 
       (conf->bitdepth==16?2:(
	conf->bitdepth==32?3:0)); // 32 bpp not really supported too well
   VI_regs->framebuffer=conf->framebuffer;
   VI_regs->width=conf->xres;
   //VI_regs->v_int=0;	// used for VI interrupt
   //VI_regs->cur_line=0;  // tells current line, write to clear interrupt
   VI_regs->timing=0x3e52239; // fairly standard?
   VI_regs->v_sync=0x20d;
   VI_regs->h_sync=0xc15;
   VI_regs->h_sync2=0xc150c15;
   VI_regs->h_limits=0x6c02ec;
   VI_regs->v_limits=0x2501ff;
   VI_regs->color_burst=0xe0204; // likewise?
   VI_regs->h_scale=1.0/conf->xscale*0x100;
   VI_regs->v_scale=1.0/conf->yscale*0x100;
}

void VI_RetraceStartWait(void) {
    // if already in retrace will wait for the next one
    while (VI_regs->cur_line != 0x200) {}
}

void VI_RetraceEndWait(void); // TODO

void VI_FillScreen(struct VI_config * conf, unsigned short color) {
    for (int c=0; c < conf->xres*conf->yres; c++) conf->framebuffer[c]=color;
}

void inline VI_DrawPixel(struct VI_config * conf, unsigned int x, unsigned int y,
	unsigned short color) {
    conf->framebuffer[y*conf->xres+x]=color;
}

void VI_DrawText(struct VI_config * conf, int xstart, int y,
	unsigned char * message) {
    int x=xstart;

    while (*message) {
	if (*message=='\n') {
	    x=xstart;
	    y+=8;
	    message++;
	    continue;
	}
	for (int row=0; row < 8; row++) {
	    unsigned char c=VI_fontdata[(int)(*message)*8+row];
	    for (int col=0; col < 8; col++) {
		VI_DrawPixel(conf,x+col,y+row,(c&0x80)?0xfffe:0);
		c <<= 1;
	    }
	}
	x+=8;
	message++;
    }
}
