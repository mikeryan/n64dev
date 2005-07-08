/*
    App:     Example demo using libn64
    Author:  Garth Elgar

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#include <mips.h>
#include <malloc.h>
#include <libn64.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

extern u16 _binary_mario_rgb_start;
extern u16 _binary_gotpost_pcm_start;
extern u32 *_binary_gotpost_pcm_size;

#define GOTPOST (&_binary_gotpost_pcm_start)
#define GOTPOST_LEN ((u32)(&_binary_gotpost_pcm_size))
#define MARIO (&_binary_mario_rgb_start)

int main(void)
{
  int x=0,y=0;
  u32 *ptrone,two;
  u16 *currentBuffer;
  int mariox=0,marioy=0;
  u32 *ScreenBuffer1,*ScreenBuffer2;

  initDebug(NONE);

  ScreenBuffer1=(u32*)(K0_TO_K1(malloc(320*240*2)));
  ScreenBuffer2=(u32*)(K0_TO_K1(malloc(320*240*2)));
  bzero(ScreenBuffer1,320*240*2);

  initContPads();
  currentBuffer=ScreenBuffer1;
  initVideo(ScreenBuffer1,VI_PAL_LAN1,VI_NTSC_LAN1,VI_MPAL_LAN1);  
  initAudio();
 
  while (1) { // Main Demo Loop

    // use double buffering to smooth the scrolling
    // tell n64 to output next buffer
    IO_WRITE(VI_DRAM_ADDR_REG,currentBuffer);

    // swap buffers
    if (currentBuffer==ScreenBuffer1) 
      currentBuffer=ScreenBuffer2;
    else 
      currentBuffer=ScreenBuffer1;

    readContPads();
    if (CONT(0).R_JPAD) mariox++;
    if (CONT(0).L_JPAD) mariox--;
    if (CONT(0).U_JPAD) marioy--;
    if (CONT(0).D_JPAD) marioy++ ;
    if (mariox<0) mariox=0;
    if (marioy<0) marioy=0;
    bzero(currentBuffer,320*240*2);
    drawSprite(currentBuffer,MARIO,mariox,marioy);
    if (contPadHit(0,START_BUTTON)) {
      playAudio(GOTPOST,GOTPOST_LEN);
    }
    while (IO_READ(VI_CURRENT_REG)<512); // wait for vertical retrace
  }
}



