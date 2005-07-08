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

