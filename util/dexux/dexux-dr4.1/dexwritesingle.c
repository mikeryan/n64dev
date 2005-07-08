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

int dexwritesingle(int dexuxfd, char *fname, char *fileformat, unsigned char *pblock, int imagesize) {
  int copycnt, progresscnt;
  int blockcnt, offset=0, blocksavailable=0, blocksavaillist[15];
  unsigned char headerdata[129], headerdatain[129], framerequest[2], *pframerequest;

    pframerequest = framerequest;

    if (DEXUXDEBUGMODE) fprintf(stderr, "dexwritesingle()\n  Creating TOC entry...\n");
    if (strcmp(fileformat, "PSX") == 0) {
      if (DEXUXDEBUGMODE) fprintf(stderr, "  File is in PSX format with imagesize of %d\n", imagesize);
      headerdata[0] = 0x51;
      headerdata[1] = 0;
      headerdata[2] = 0;
      headerdata[3] = 0;
      headerdata[4] = 0;
      if (imagesize < 8) {
        headerdata[5] = imagesize << 5;
        headerdata[6] = 0;
      } else {
        headerdata[5] = (imagesize - 8) << 5;
        headerdata[6] = 1;
      }
      headerdata[7] = 0;
      headerdata[9] = 0;
      for (copycnt = 0; copycnt <= 20; copycnt++) {
        headerdata[copycnt + 10] = pblock[copycnt]; 
      }
      for (copycnt = 0x1E; copycnt < 0x7F; copycnt++) {
        headerdata[copycnt] = 0;
      }
      headerdata[127] = dexuxstrxor(headerdata, 127);
      pblock += 74; 
    } else /* MCS format */ {
      if (DEXUXDEBUGMODE) fprintf(stderr, "  File is in MCS format with imagesize of %d\n", imagesize);
      for (copycnt = 0; copycnt < 128; copycnt++) {
        headerdata[copycnt] = pblock[copycnt];
      }
      pblock += 128;
    }

    for (blockcnt = 1; blockcnt <= 15; blockcnt++) {
      *(pframerequest+0) = 0;
      *(pframerequest+1) = blockcnt;
      dexuxgetframe(dexuxfd, framerequest, headerdatain);

      if ((headerdatain[0] & 240) == 0xA0) {
        blocksavaillist[blocksavailable] = blockcnt;
        blocksavailable++;
        if (DEXUXDEBUGMODE) fprintf(stderr, "  Available block found: #%d\n", blockcnt);
      }
      if (imagesize == blocksavailable) break;
    }

    if (imagesize > blocksavailable) {
      fprintf(stderr, "File requires %d block(s) but only", imagesize);
      fprintf(stderr, " %d block(s) available on the card\n", blocksavailable);
      fprintf(stderr, "Skipping file: %s\n", fname);
      return(1);
    }

    fprintf(stderr, "Progress for %s (%d blocks in size):\n", fname, imagesize);

    for (blockcnt = 0; blockcnt < imagesize; blockcnt++) {
      fprintf(stderr, "  Writting block #%d\n", blockcnt + 1); 
      if (blockcnt != (imagesize - 1)) {
	if (DEXUXDEBUGMODE) fprintf(stderr, "  blockcnt != (imagesize -1)\n");
        if (blockcnt > 0) {
          if (DEXUXDEBUGMODE) fprintf(stderr, "  blockcnt > 0\n");
          headerdata[0] = 0x52;
        }
        headerdata[8] = blocksavaillist[blockcnt + 1] - 1;
        headerdata[9] = 0;
      } else {
        if (DEXUXDEBUGMODE) fprintf(stderr, "  blockcnt == (imagesize - 1)\n");
        if (blockcnt > 0) {
          if (DEXUXDEBUGMODE) fprintf(stderr, "  blockcnt > 0\n");
          headerdata[0] = 0x53;
        }
        headerdata[8] = 0xFF;
        headerdata[9] = 0xFF;
      }
      if (DEXUXDEBUGMODE) fprintf(stderr, "  headerdata\n");
      if (DEXUXDEBUGMODE) fprintf(stderr, "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15\n");
      if (DEXUXDEBUGMODE) fprintf(stderr, "    %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c\n", 
        headerdata[ 0], headerdata[ 1], headerdata[ 2], headerdata[ 3], 
        headerdata[ 4], headerdata[ 5], headerdata[ 6], headerdata[ 7],
        headerdata[ 8], headerdata[ 9], headerdata[10], headerdata[11],
        headerdata[12], headerdata[13], headerdata[14], headerdata[15],
        headerdata[16], headerdata[17], headerdata[18], headerdata[19],
        headerdata[20], headerdata[21]);
      headerdata[127] = dexuxstrxor(headerdata, 127);
      *(pframerequest+0) = 0;
      *(pframerequest+1) = blocksavaillist[blockcnt];
      if (DEXUXDEBUGMODE) fprintf(stderr, "  Writting to block #%d\n", blocksavaillist[blockcnt]);
      if (DEXUXDEBUGMODE) fprintf(stderr, "  Address %d %d\n", *(pframerequest+0), *(pframerequest+1));
      dexuxputframe(dexuxfd, framerequest, headerdata);
      dexuxputblock(dexuxfd, blocksavaillist[blockcnt], pblock);

      pblock += 8192;
    } 

  return(0);
}

