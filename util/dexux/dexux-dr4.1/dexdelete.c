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

int dexdelete(int dexuxfd, int blocknum, int undeleteflag) {
  unsigned char headerdata[129];
  char chunkrequest[2], *pchunkrequest;

  pchunkrequest = chunkrequest;

  *(pchunkrequest+0) = 0;
  *(pchunkrequest+1) = blocknum;
  dexuxgetframe(dexuxfd, chunkrequest, headerdata);

/*
  fprintf(stderr, "dexdelete: blocknum %d, undeleteflag %d\n", blocknum,
    undeleteflag);
  fprintf(stderr, "dexdelete: status %.2x, statushi %.2x, statuslow %.2x\n",
    headerdata[0], headerdata[0] & 240, headerdata[0] & 15);
*/

  if ((((headerdata[0] & 240) != 0xA0) && (undeleteflag == 0)) ||
      (((headerdata[0] & 240) != 0x50) && (undeleteflag == 1))) {
    if (undeleteflag == 0) {
      headerdata[0] = (headerdata[0] & 15) + 0xA0;
    } else {
      headerdata[0] = (headerdata[0] & 15) + 0x50;
    }
    headerdata[127] = dexuxstrxor(headerdata, 127);
    dexuxputframe(dexuxfd, chunkrequest, headerdata);
  }

  if (headerdata[8] != 0xFF) {
    dexdelete(dexuxfd, headerdata[8] + 1, undeleteflag);
  }

  return(0);
}
