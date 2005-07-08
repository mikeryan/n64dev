/*

    Author:  Garth Elgar
    Compile: gcc -Wall -O2 -ltiff -o tiff2rgb tiff2rgb.c
    Purpose: Convert a TIFF image into N64 format RGB 
    Usage:   tiff2rgb [-32] filename.tiff

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

*/

/*
  OUTPUT FORMAT
  Writes a 64 bit header:
  u16 width
  u16 height
  u32 to force 64 bit alignment for DMA
  
  TODO: It would be nice to specify an the mask colour on the command line,
  like -mask 0xffffff to make all white pixels transparent
  The default is to use the msb's of the alpha channel as the mask

*/

#include <tiffio.h>
#include <string.h>

#define TIFFGetR(abgr)   ((abgr) & 0xff)
#define TIFFGetG(abgr)   (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr)   (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr)   (((abgr) >> 24) & 0xff)

#define PACK_RGBA5551(r,g,b,a)   ((((r)<<8) & 0xf800) | \
                                  (((g)<<3) & 0x7c0)  | \
                                  (((b)>>2) & 0x3e)   | \
                                  (((a)>>7) & 0x1))

#define PACK_RGBA8888(r,g,b,a)   ((((uint32)r<<24 & 0xff000000) | \
                                   ((uint32)g<<16 & 0x00ff0000) | \
                                   ((uint32)b<<8 & 0x0000ff00) | \
                                   ((uint32)a & 0xff)))
main(int argc, char* argv[])
{
  TIFF* tif;
  uint8 r,g,b,a,i,bpp;
  FILE *rgbf;
  char rgbfile[50];

  if (argc!=2 && argc!=3) {
    printf("Usage: %s [-32] filename.tiff\n",argv[0]);
    exit(1);
  }

  if (argc==2) bpp=16; else bpp=32;

  tif = TIFFOpen(argv[1], "r");
  if (tif) {
    uint32 w, h, pixel,rgba32_pixel;
    uint16 rgba16_pixel;
    size_t npixels;
    uint32* raster;
    int n;
    
    strcpy(rgbfile,argv[1]);
    strcpy((char*)strrchr(rgbfile,'.'),".rgb");
    rgbf=fopen(rgbfile,"wb");
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
      if (TIFFReadRGBAImage(tif,w,h,raster,0)) {
	int mw,mh;
	// save rgb data to file
	fputc((w>>8)&0xFF,rgbf); fputc(w&0xFF,rgbf); // u16 w,h header
	fputc((h>>8)&0xFF,rgbf); fputc(h&0xFF,rgbf);
	for (n=0; n<4; n++) fputc(0x0,rgbf); // alignment
	for (mh=h-1; mh>=0; mh--)
	  for (mw=0; mw<w; mw++) 
	    {
	      pixel=raster[w*mh+mw];
	      r=TIFFGetR(pixel);
	      g=TIFFGetG(pixel);
	      b=TIFFGetB(pixel);
	      a=TIFFGetA(pixel);
	      if (bpp==16)
		{
		  rgba16_pixel=PACK_RGBA5551(r,g,b,a);
	      // use white as the mask
	      // if (r==0xff && g==0xff && b==0xff) a|=1; else a&=~(uint16)1;
	      // if (a==0xcff) rgba16_pixel|=1; else rgba16_pixel&=~(uint16)1;
		  fputc((rgba16_pixel>>8)&0xFF,rgbf);
		  fputc(rgba16_pixel&0xFF,rgbf);
		}
	      else
		{
		  // rgba32_pixel=PACK_RGBA8888(r,g,b,a);
		  fputc(r,rgbf);
		  fputc(g,rgbf);
		  fputc(b,rgbf);
		  fputc(a,rgbf);
		}
	    }
      }
      _TIFFfree(raster);
      fclose(rgbf);
      printf("Converted %s to % dbpp RGBA using alpha channel as mask\n",
	     argv[1],bpp);
    }
    TIFFClose(tif);
  }
  exit(0);
}
