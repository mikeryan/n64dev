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

/* Write a frame of data */
int dexuxputframe(int fd, char *address, char *data) {
  char buffer[5];
  char sendstring[133], *psendstring;
  char checkbyte;
  int readlength;
  int i;  /* Loop counter */
  
  psendstring = sendstring;
  *(psendstring+132) = 0;

  *(psendstring+0) = *(address+0);
  *(psendstring+1) = *(address+1);
  *(psendstring+2) = dexuxbinreverse( *(address+0) );
  *(psendstring+3) = dexuxbinreverse( *(address+1) );
  
  for (i = 0; i <= 127; i++) {
    *(psendstring+4+i) = *(data+i);
  }
  
  dexuxclear(fd);

  checkbyte = dexuxstrxor(sendstring, 132);
  write(fd, "IAI\x04", 4);
  write(fd, sendstring, 132);
  write(fd, &checkbyte, 1);

  readlength = read(fd, buffer, 4);
  if (strncmp(buffer, "IAI\x29", 4)) {
          buffer[readlength] = 0;
          return(1);
  }

  return(readlength);
}

