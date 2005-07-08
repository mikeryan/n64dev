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

/* Call dexuxputframe multiple times to write a whole block */
int dexuxputblock(int fd, int blocknumber, char *blockdata) {
  char frametoput[2], *pframetoput, framedata[129], *pframedata;
  long frameaddress;
  int framenum, charnum;

  pframetoput = frametoput;
  pframedata = framedata;

  for (framenum = 0; framenum <= 63; framenum++) {
    frameaddress = (blocknumber * 0x40) + framenum;
    *(pframetoput + 0) = dexuxframetop(frameaddress);
    *(pframetoput + 1) = dexuxframebottom(frameaddress);

    /* Copy frame from correct position in blockdata */
    for (charnum = 0; charnum <= 127; charnum++) {
      *(pframedata + charnum) = blockdata[ (framenum * 128) + charnum ];
    }

    dexuxputframe(fd, frametoput, framedata);
  }

  return(0);
}
