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

/* Check for card presence */
/* returns: 0 = card, 1 = no card, 2 = uh oh! */
int dexuxstatus(int fd) {
  char buffer[255];
  int readlength;

  dexuxclear(fd);
  write(fd, "IAI\x01",4);

  readlength = read(fd, &buffer, 254);
  buffer[readlength]='\0';

  if (!strncmp(buffer, "IAI\x22",readlength)) {
    return(1); /* No card */
  } else if (!strncmp(buffer, "IAI\x23\x10",readlength)) {
    return(0); /* Card detected */
  } else if (!strncmp(buffer, "IAI\x23\x00",readlength)) {
    return(0); /* Card detected */
  } else {
    return(2); /* Unexpect result */
  }
}

