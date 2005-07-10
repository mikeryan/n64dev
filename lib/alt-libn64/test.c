#include <math.h>
#include <stdio.h>
#include <string.h>
#include "N64sys.h"
#include "VI.h"
#include "SI.h"

unsigned short int screenbuffer[320*250];
unsigned short int screenbuffer2[320*250];

int UltraMain(void) {
   struct VI_config conf,conf2,*curdrawconf,*curdispconf;
   struct controller_data cd;
   char buf[1024];
   int tog=0;
   float offs=0;
   unsigned long time,lasttime,time2;

   conf.bitdepth=16;
   conf.xres=320;
   conf.yres=250;
   conf.xscale=0.5;
   conf.yscale=0.25;
   conf.framebuffer=UncachedAddr(screenbuffer);
   memcpy(&conf2,&conf,sizeof(conf));
   conf2.framebuffer=UncachedAddr(screenbuffer2);
   VI_WriteRegs(&conf);

   VI_FillScreen(&conf,0);
   
   while (1) {
       curdispconf=tog?&conf:&conf2;
       curdrawconf=tog?&conf2:&conf;
       tog ^= 1;

       time2=read_count();

       VI_RetraceStartWait();
       
       time=read_count(); // we want time spent *doing* stuff

       time2=(time-time2)*2;

       controller_Read(&cd);
       VI_WriteRegs(curdispconf);

       VI_FillScreen(curdrawconf,cd.c[0].err?0:(cd.c[0].start?0xf800:0));

       for (int c=0; c < 4; c++) {
	   if (cd.c[c].err) // if controller not present
	       sprintf(buf,"controller %d not present",c+1);
	   else
	       sprintf(buf,"controller %d present\nbuttons=0x%04X x=%d y=%d",c+1,
		       cd.c[c].buttons,
		       cd.c[c].x,
		       cd.c[c].y);

	   VI_DrawText(curdrawconf,50,50+2*8*c,buf);
       }

       for (float x=50; x < 250; x++) {
	   VI_DrawPixel(curdrawconf,x,sin((x-50)/200*M_TWOPI*4+offs)*250.0/8+175,0x7c0);
       }

       if (!cd.c[0].err) offs-=cd.c[0].x/32.0;

       sprintf(buf,"%lu cycles last frame\n%lu cycles waiting",lasttime,time2);
       VI_DrawText(curdrawconf,50,34,buf);
       lasttime=(read_count()-time)*2;
   }

   return 0;
}
