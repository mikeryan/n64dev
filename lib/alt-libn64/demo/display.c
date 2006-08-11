/*
 * system for managing buffered video
 * 320x250, 15-bit color
 *
 * The logic will handle either double or triple buffering.
 */

#include <libn64.h>
#include "display.h"

#define BUFFER_COUNT 3

#define XRES 320
#define YRES 250

// frame buffers
unsigned short int fb[BUFFER_COUNT][XRES*YRES];

// display contexts
struct display_context dc[BUFFER_COUNT];

int nowshowing; // currently displayed buffer
int shownext; // complete drawn buffer to display next
int nowdrawing; // buffer currently being drawn on

void display_VIcallback();

void initDisplay() {
    // Set up the display contexts
    for (int c=0;c<BUFFER_COUNT;c++) {
	dc[c].conf.bitdepth=16;
	dc[c].conf.xres=XRES;
	dc[c].conf.yres=YRES;
	dc[c].conf.xscale=0.5;
	dc[c].conf.yscale=0.25;

	dc[c].conf.framebuffer=UncachedAddr(fb[c]);
	dc[c].idx=c;
    }

    VI_FillScreen(&dc[0].conf,0);

    VI_WriteRegs(&dc[0].conf);

    nowshowing=0;
    nowdrawing=-1;
    shownext=-1;

    registerVIhandler(display_VIcallback);
    set_VI_interrupt(1,0x200);
}

// Request a display context to write upon.
// Will return null pointer if nothing is available right away.
struct display_context * lockDisplay() {
    struct display_context * retval=0;
    disable_interrupts();
    for (int c=0;c<BUFFER_COUNT;c++) {
	if (c!=nowshowing && c!=nowdrawing && c!=shownext) {
	    nowdrawing=c;
	    retval = &dc[c];
	    break;
	}
    }
    enable_interrupts();
    return retval;
}

// Say that you are done, display the image.
// This will replace an existing complete image to "shownext"
void showDisplay(struct display_context * dc_to_show) {
    disable_interrupts(); // can't have the exception handler blowing this...

    if (dc_to_show->idx == nowdrawing) nowdrawing=-1; // if not something is wrong
    shownext=dc_to_show->idx;

    enable_interrupts();
}

void display_VIcallback() {
    if (shownext >= 0 && shownext != nowdrawing) {
	VI_WriteRegs(&dc[shownext].conf);
	nowshowing=shownext;
	shownext=-1;
    }
    // otherwise just leave the current one up
}
