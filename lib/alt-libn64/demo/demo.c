#include <math.h>
#include <stdio.h>
#include <string.h>
#include <libn64.h>
#include <malloc.h>
#include "display.h"
#include "audio.h"

/*
 * alt-libn64 demo
 * by hcs
 */

long int buffercount=0;
long int framecount=0;

void AIcallback() {
    buffercount++;
}

void VIcallback() {
    framecount++;
}

// execution begins here
int UltraMain(void) {
    struct display_context * dc;
    struct controller_data cd;

    int lasty=0,lastx=0;
    int d=0;

    // buffer for rendering text strings (only sprintf works at the moment)
    char buf[1024]; 

    // offset for the scrolling waveform
    float offs=0;

    // used to keep track of how many cycles various processes take
    unsigned long time,rendertime,lastrendertime,waittime,lastwaittime;

    // enable MI interrupts (on the CPU)
    set_MI_interrupt(1);

    initDisplay();
    initAudio();

    registerAIhandler(AIcallback);
    registerVIhandler(VIcallback);
   
    while (1) {
	short * sb;
	int sbsize;
	// Mark the value of the COUNT register (increments at half the CPU
	// rate) at the start of the main loop.
	time=read_count();

	while (! (dc=lockDisplay())) ;

	// Mark the time after waiting.
	waittime=time;
	waittime=((time=read_count())-waittime)*2;

	sb=getbuf();
	sbsize=getbufsize();

	for (int c=0;c<sbsize;c++,d++) {
       	    sb[c*2]=sin((d%22050)/22050.0*M_TWOPI*1000.0)*100*((cd.c[0].y-lasty)*c/sbsize+lasty)*(((cd.c[0].x-lastx)*c/sbsize+lastx)/128.0-1.0);
	    sb[c*2+1]=sin((d%22050)/22050.0*M_TWOPI*1000.0)*100*((cd.c[0].y-lasty)*c/sbsize+lasty)*(((cd.c[0].x-lastx)*c/sbsize+lastx)/128.0+1.0);
	}
	writebuffer(sb);

	lastx=cd.c[0].x;
	lasty=cd.c[0].y;

	// this is what really marks the beginning of the current frame

	// read controller data
	controller_Read(&cd);

	// start drawing on the other frame buffer, clear the screen (uses red
	// when the start button on controller 1 is pressed, black otherwise)
	VI_FillScreen(&dc->conf,cd.c[0].err?0:(cd.c[0].start?0xf800:0));

	// print a little info for each controller
	for (int c=0; c < 4; c++) {
	    if (cd.c[c].err) // if controller not present
		sprintf(buf,"controller %d not present",c+1);
	    else {
		sprintf(buf,"controller %d present\nbuttons=0x%04X x=%4d y=%4d",c+1,
		    	cd.c[c].buttons,
		     	cd.c[c].x,
		      	cd.c[c].y);
	    }

	    VI_DrawText(&dc->conf,50,58+2*8*c,buf);
	}

	// draw a waveform
	for (float x=50; x < 250; x++) {
	    VI_DrawPixel(&dc->conf,x,cbrt(sin((x-50)/200*M_TWOPI*4+offs))*250.0/8+175,0x7c0);
	}

	// the waveform can be scrolled by the x axis of the first controller
	if (!cd.c[0].err) offs-=(cd.c[0].x/2*2)/32.0;

	// print up some informative text, and report how long we waited for
	// this frame, and how long the last frame took to render
	sprintf(buf,"buffers: %5d frames: %5d\n%8lu cycles last frame\n%8lu cycles waiting\n%4.2f FPS",buffercount,framecount,lastrendertime,lastwaittime,100000000.0/(lastwaittime+lastrendertime));
	VI_DrawText(&dc->conf,50,26,buf);

	// calculate how many cycles this frame took to render
	rendertime=(read_count()-time)*2;
	lastwaittime=waittime;
	lastrendertime=rendertime;

	showDisplay(dc);
	dc=0;
    }

    return 0;
}

