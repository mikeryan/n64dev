/*  
    App:     Jpeg Viewer for Nintendo 64
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

extern fs_struct* _romfs;
// these are defined by the linker
extern u32 _binary_logo_jpg_start;
extern u32 _binary_logo_jpg_size;

#define ALIGN_16BYTE(x) ((void*)(((u32)x+15)&(u32)0xFFFFFFF0))

int main(void)
{
  int i,display_logo=1;
  u32 dmalen;
  jpegfile jpgstruct;
  void *ScreenBuffer;
  fs_struct *header,*romfsptr,*romfs;
  u32 *s,*d;
  u32 *jpeg;
  void *mem1;;

  // The frame buffer has to be 4 byte aligned
  // But make it 16 to be dma and cache friendly
  // note: we use an uncached address so writes really go to the video
  ScreenBuffer=ALIGN_16BYTE(K0_TO_K1(malloc(640*480*4+15)));

  // Initialise the VI, high res 640x480 32 bpp mode
  initVideo(ScreenBuffer,VI_PAL_HPN2,VI_NTSC_HPN2,VI_MPAL_HPN2);

  // Initialise the Peripheral Interface
  initPi();

  // before anything else blank the screen, or get annoying static..
  for (i=0;i<640*480; i++)
    ((u32*)ScreenBuffer)[i]=(u32)0x0; // clear screen.

  // initialise the debugger and control pads
  initContPads();
  initDebug(NONE);

  // make romfs point to the cartridge
  // this allows us to insert and remove files after the rom is made
  romfs=(fs_struct*)(((u32)&_romfs+(u32)0xc00)|(u32)0xB0000000);
  romfsptr=romfs;

  // allocate some space for the fs_struct header on each jpeg image
  header=ALIGN_16BYTE(malloc(sizeof(fs_struct)+15));

  // display boot up logo
  // set up the jpg struct for calling jpeg decompression
  jpgstruct.jpeg_image=&_binary_logo_jpg_start;
  jpgstruct.rgb_image=ScreenBuffer;
  jpgstruct.jpeg_length=&_binary_logo_jpg_size;
  jpgstruct.rgb_bpp=32;
  jpgstruct.autoscale=640;
  read_JPEG_file(&jpgstruct);
  sleep(5);

  while (1) // main program loop
    {	  
      // transfer the fs_struct header from the cart
      printf("%p -> %p (%d)\n",romfsptr,header,sizeof(fs_struct));
      piDmaRead(header,romfsptr,sizeof(fs_struct));
      PI_WAIT; // wait for dma transfer to complete

      // print some info about the file
      printf("showing %s (length %d)\n",header->filename,header->length);

      // transfer the jpeg image from the cart
      s=(void*)((u32)romfsptr+sizeof(fs_struct));
      dmalen=header->length;
      while (dmalen%2) dmalen++; // ensure transfer of even number bytes
      mem1=malloc(dmalen+15);
      d=(void*)jpeg=ALIGN_16BYTE(mem1);
      printf("%p -> %p (%d)\n",s,d,dmalen);
      piDmaRead(d,s,dmalen);
      PI_WAIT; // wait for dma transfer to complete

      // set up the jpg struct for calling jpeg decompression
      jpgstruct.jpeg_image=jpeg;
      jpgstruct.rgb_image=ScreenBuffer;
      jpgstruct.jpeg_length=header->length;
      jpgstruct.rgb_bpp=32;
      jpgstruct.autoscale=640;

      while ((IO_READ(VI_CURRENT_REG)&0x3FE)<512); // wait for vertical retrace

      for (i=0;i<640*480; i++)
	((u32*)ScreenBuffer)[i]=0x0; // clear screen.
      
      // decompress the jpeg, because rgb_image is set its written direct
      // into ScreenBuffer
      read_JPEG_file(&jpgstruct);

      printf("next file offset %d\n",header->next_file);

      // update pointer to the next jpeg image, or repeat from start
      if (header->next_file)
	romfsptr=(fs_struct*)((u32)romfsptr+header->next_file);
      else
	romfsptr=romfs;

      // free the space taken by the jpeg image
      free(mem1); 

      // show image for 5 sec
      sleep(5);
    }
}

/* 

DMA WORKING! After lots of problems wrote some asm to writeback and
invalidate the data cache (cache op 1 INDEX_INVALIDATE<<2|CACHE_D).
Made sure all target addresses were *16* byte aligned - this ensures
other data isn't present on a DMAed cache line (each line is 16 bytes)
so the cpu won't read in any of the target DMA area by accident when
accessing neighbouring data. Also use DMA lengths that are a multiple
of 16. Oh and the screen buffer is 16 byte aligned. I'm still not
totally sure that this is all necessary...

PI DMA STUFF

Not sure whats going on here, wasted a lot of time trying to figure it
out by trial and eror :-(

Think:

1) The RDRAM address has to be 8 byte aligned
2) The length written to the PI_WR_LEN_REG register has to be odd,
   ie. the transfer must be an even number of bytes (maybe)
3) The source address in the cart space has to be even
4) 8 bit reads return the correct byte on an even address, but the
   data at byte+1 on an odd address
5) 16 bit reads work but crash if you try to read across a 16 byte boundary
6) 32 bit reads appear to work, but if I replace the dma above with
   *s++=*d++ it doesn't work somewhere

The CD64 is probably interfering with the PI DMA stuff; don't know
why because I make sure to PI_WAIT before doing anything.

The cd64 sendbyte recvbyte functions were changing mode on every byte,
maybe the cd64 just couldnt handle it.. reading directly from the cart
still stuffs up sometimes.

*/






