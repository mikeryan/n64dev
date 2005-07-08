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

int dexlist(int dexuxfd) {
  int blockcnt, titlecnt, productcnt, idcnt, printable;
  int foundentry=0, blocksavailable=15, blocksused=0;
  unsigned char headerdata[129], blockdata[129];
  unsigned char title_shiftjis[65], *ptitle_shiftjis, title_ascii[65];
  char chunkrequest[2], *pchunkrequest;

  pchunkrequest = chunkrequest;
  ptitle_shiftjis = title_shiftjis;

  for (blockcnt=1; blockcnt<=15; blockcnt++) {
    *(pchunkrequest+0) = 0;
    *(pchunkrequest+1) = blockcnt;
    if (DEXUXDEBUGMODE) fprintf(stderr, "Getting address %d %d\n", *(pchunkrequest+0), *(pchunkrequest+1));
    dexuxgetframe(dexuxfd, chunkrequest, headerdata);
    if ((headerdata[0] & 240) != 0xA0) blocksavailable--;
    if ((headerdata[0] & 240) == 0x50) blocksused++;
    if ((headerdata[0] & 15) == 0x01) {
      if (foundentry == 0) {
        foundentry = 1;
        fprintf(stderr, "#  Title                            ");
        fprintf(stderr, "Size Del Product Code     ID    Frames\n");
      }

      /* Block # */
      if (blockcnt < 10) {
        fprintf(stderr, "%d  ", blockcnt);
      } else {
        fprintf(stderr, "%d ", blockcnt);
      }

      *(pchunkrequest+0) = dexuxframetop(blockcnt * 0x40);
      *(pchunkrequest+1) = dexuxframebottom(blockcnt * 0x40);
      dexuxgetframe(dexuxfd, chunkrequest, blockdata);

      /* Title */
      for (titlecnt=0x04; titlecnt <= 0x43;
           *(ptitle_shiftjis+titlecnt-4) = blockdata[titlecnt++]) ;
      dexuxjistoascii(title_shiftjis, title_ascii);
      printable=1;
      for (titlecnt=0; titlecnt < 32; titlecnt++)
        if (isprint(title_ascii[titlecnt]) && printable) {
          fprintf(stderr, "%c", title_ascii[titlecnt]);
        } else {
          printable=0;
          fprintf(stderr, " ");
        }

      /* Size */
      if (headerdata[6] != 0x01) {
        fprintf(stderr, "   %d  ", headerdata[5] >> 5);
      } else if (headerdata[5] < 0x40) {
        fprintf(stderr, "   %d  ", (headerdata[5] >> 5) + 8);
      } else {
        fprintf(stderr, "  %d  ", (headerdata[5] >> 5) + 8);
      }

      /* Delete status */
      if ((headerdata[0] & 240) == 0x50) {
        fprintf(stderr, " N  ");
      } else {
        fprintf(stderr, " Y  ");
      }

      /* Product Code */
      printable=1;
      if (isprint(headerdata[0x0A]) && isprint(headerdata[0x0B])) {
        fprintf(stderr, "%c%c-",
                headerdata[0x0A], headerdata[0x0B]);
      } else {
        printable=0;
        fprintf(stderr, "   ");
      }
      for (productcnt = 0x0C; productcnt <= 0x15; productcnt++) {
        if (isprint(headerdata[productcnt]) && printable) {
          fprintf(stderr, "%c", headerdata[productcnt]);
        } else {
          printable=0;
          fprintf(stderr, " ");
        }
      }

      fprintf(stderr, " ");

      /* Identifier */
      printable=1;
      for (idcnt = 0x16; idcnt <= 0x1D; idcnt++) {
        if (isprint(headerdata[idcnt]) && printable) {
          fprintf(stderr, "%c", headerdata[idcnt]);
        } else {
          printable=0;
          fprintf(stderr, " ");
        }
      }

      /* Icon Frames */
      fprintf(stderr, "    %d\n", blockdata[0x02] & 3);
    }
  }
  if (foundentry) {
    fprintf(stderr, "      Blocks in use:  %d     Blocks available:  %d\n",
            blocksused, blocksavailable);
  } else {
    fprintf(stderr, "      Blocks in use: None    Blocks available:  %d\n",
            blocksavailable);
  }

  return(0);
}
