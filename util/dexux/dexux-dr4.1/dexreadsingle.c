/* Dexux
 *
 * Copyright (C) 2000 William Ono <wmono@users.sourceforge.net>
 * Copyright (C) 2000 Ben Galliart <bgalllia@luc.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h> /* Standard i/o definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <string.h> /* String function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "libdexux.h"

/* 0=psx, 1=mcs, 2=xpm */

int dexreadsingle(int dexuxfd, int singlesaveformat, 
                   int blocknum, char *fname) {
  unsigned char blockdata[8193];
  unsigned char headerdata[129];
  char chunk[2], *pchunk;
  int midlink;
  FILE *fout;

  fprintf(stderr, "Writting block #%d to %s\n", blocknum, fname);

  if (strcmp(fname, "-") == 0) {
    fout=stdout;
  } else {
    fout=fopen(fname, "w");
  }

  pchunk = chunk;

  if (singlesaveformat < 2) {
    *(pchunk+0) = 0;
    *(pchunk+1) = blocknum;

    dexuxgetframe(dexuxfd, chunk, headerdata);
    dexuxgetblock(dexuxfd, blocknum, blockdata);

    if (singlesaveformat == 0) {
      unsigned char title_shiftjis[65], *ptitle_shiftjis, title_ascii[65];
      int psxcnt, titlecnt;

      ptitle_shiftjis=title_shiftjis;

      for (psxcnt = 0x0A; psxcnt <= 0x1D; psxcnt++) {
        fprintf(fout, "%c", headerdata[psxcnt]);
      }
      for (titlecnt=0x04; titlecnt <= 0x43;
           *(ptitle_shiftjis+titlecnt-4) = blockdata[titlecnt++]) ;
      dexuxjistoascii(title_shiftjis, title_ascii);
      for (titlecnt=0; titlecnt < 30; titlecnt++) {
        fprintf(fout, "%c", title_ascii[titlecnt]);
      }
      fprintf(fout, "%c%c%c%c", 0x1D, 0x0B, 0x0B, 0x00);
      fwrite(blockdata, 8192, 1, fout);
    } else {
      fwrite(headerdata, 128, 1, fout);
      fwrite(blockdata, 8192, 1, fout);
    }
    midlink=1;
    while (midlink) {
      if (headerdata[8] == 0xFF) {
        midlink=0;
      } else {
        blocknum = headerdata[8] + 1;
        *(pchunk+0) = 0;
        *(pchunk+1) = blocknum;

        dexuxgetframe(dexuxfd, chunk, headerdata);
        dexuxgetblock(dexuxfd, blocknum, blockdata);
        fwrite(blockdata, 8192, 1, fout);
      }
    } 
  } else {
    int colorcnt, cvalue, xcnt, ycnt;
    char palette[16]=" .XoO+@#$%&*=-;:", buf[129];

    *(pchunk+0) = dexuxframetop(blocknum * 0x40);
    *(pchunk+1) = dexuxframebottom(blocknum * 0x40);
    dexuxgetframe(dexuxfd, chunk, buf); 
    fprintf(fout, "/* XPM */\n");
    fprintf(fout, "static char *psx[] = {\n");
    fprintf(fout, "/* columns rows colors chars-per-pixel */\n");
    fprintf(fout, "\"16 16 16 1\",\n");
    for (colorcnt=0; colorcnt<16; colorcnt++) {
      fprintf(fout, "\"%c c #", palette[colorcnt]);
      /* Red */
      fprintf(fout, "%.2x", (buf[0x60 + 2*colorcnt] & 31) << 3);
      /* Green */
      cvalue  = (buf[0x60 + 2*colorcnt] & 224) >> 2;
      cvalue += (buf[0x60 + 2*colorcnt + 1] & 3) << 6;
      fprintf(fout, "%.2x", cvalue); 
      /* Blue */
      fprintf(fout, "%.2x\",\n", (buf[0x60 + 2*colorcnt + 1] & 124) << 1);
    }
    *(pchunk+0) = dexuxframetop(blocknum * 0x40);
    *(pchunk+1) = dexuxframebottom(blocknum * 0x40 + 1);
    dexuxgetframe(dexuxfd, chunk, buf);
    fprintf(fout, "/* pixels */\n\"");
    for (ycnt=0; ycnt < 16; ycnt++) {
      for (xcnt=0; xcnt < 8; xcnt++) {
        fprintf(fout, "%c", 
          palette[buf[xcnt + 8*ycnt] & 15]);
        fprintf(fout, "%c",
          palette[(buf[xcnt + 8*ycnt] & 240) >> 4]); 
        if (xcnt == 7) {
          if (ycnt < 15) {
            fprintf(fout, "\",\n\"");
          } else {
            fprintf(fout, "\"\n};\n");
          }
        }
      }
    }
  }
  fclose(fout);
  return (0);
}
