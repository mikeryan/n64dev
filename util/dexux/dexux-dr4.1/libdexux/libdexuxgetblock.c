/* Dexux
 *
 * Copyright (C) 2000 William Ono <wmono@users.sourceforge.net>
 * Copyright (C) 2000 Ben Galliart <bgallia@users.sourceforge.net>
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

#include "libdexux.h"

/* Call dexuxgetframe multiple times to get a whole block */
int dexuxgetblock(int fd, int blocknumber, char *blockdata) {
  char *pblockdata, *pframetoget, frametoget[2], framebuf[129];
  long frameaddress;
  int framenum; 

  pblockdata = blockdata;  pframetoget = frametoget;

  for (framenum = 0; framenum <= 63; framenum++) {
    frameaddress = (blocknumber * 0x40) + framenum;
    *(pframetoget + 0) = dexuxframetop(frameaddress);
    *(pframetoget + 1) = dexuxframebottom(frameaddress);
    dexuxgetframe(fd, frametoget, pblockdata);
    pblockdata += 128;
  }

  return(0);
}
