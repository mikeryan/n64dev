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

int dexwritefull(int dexuxfd, char *fname, char *fileformat, unsigned char *pblock) {
  int progresscnt, blockcnt;
  unsigned char headerdata[129];
  unsigned char framerequest[2], *pframerequest;

    pframerequest = framerequest;

    for (blockcnt = 1; blockcnt <= 15; blockcnt++) {
      *(pframerequest+0) = 0;
      *(pframerequest+1) = blockcnt;
      dexuxgetframe(dexuxfd, framerequest, headerdata);

      if ((headerdata[0] & 240) != 0xA0) {
	fprintf(stderr, "Detected file format %s which contains", fileformat);
        fprintf(stderr, " a full memorycard image\n");
        fprintf(stderr, "But the card in the drive still has data which");
        fprintf(stderr, " has not been deleted\n");
        fprintf(stderr, "Skipping file: %s\n", fname);
        return(1);
      }
    }

    fprintf(stderr, "Progress for: %s\n", fname);
    fprintf(stderr, "|");
    for (progresscnt = 0; progresscnt < 64; progresscnt++) { 
      fprintf(stderr, "-");
    }
    fprintf(stderr, "|\n ");

    for (blockcnt = 0; blockcnt <= 15; blockcnt++) {
      dexuxputblock(dexuxfd, blockcnt, pblock);
      pblock += 8192;
      fprintf(stderr, "****");
    }         

    fprintf(stderr, "\n");

  return(0);
}

