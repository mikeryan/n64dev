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

/* Send initialization code to the DexDrive */
int dexuxwake(int fd) {
  char buffer[255];
  int readlength;
  int i; /* Loop counter */
  
  write(fd, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 32);
  
  readlength = read(fd, &buffer, 4);
  buffer[readlength]='\0';
  if (strncmp(buffer, "IAI!", 4)) {
          return(1);
  }
   
  write(fd, "IAI\x00\x10\x29\x23\xBE\x84\xE1\x6C\xD6\xAE\x52\x90\x49\xF1\xF1\xBB\xE9\xEB", 21);
  readlength = read(fd, &buffer, 9);
  buffer[readlength]='\0';
  if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxwake(): version responce: %.2x %.2x %.2x %.2x %.2x %.2x (firmware version 1.%d)\n", buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[8] - 58);
  if (strncmp(buffer, "IAI\x40\x1B\x50\x53\x58\x46",8)) {
          return(1);
  }
   
  for (i=0; i<10; i++) {
    write(fd, "IAI\x27",4);
    tcdrain(fd);
    usleep(1000);
    dexuxclear(fd);
  }

  write(fd, "IAI\x07\x01",5);
  readlength = read(fd, &buffer, 254);
  buffer[readlength]='\0';
  if (strncmp(buffer, "IAI",3)) {
          return(1);
  }

  dexuxclear(fd);
  return(0);
}

