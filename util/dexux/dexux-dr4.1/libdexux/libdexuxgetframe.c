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

#include <sys/ioctl.h>
#include "libdexux.h"

#define USLEEPTIME 1000

/* Get a specified frame from the card */
int dexuxgetframe(int fd, unsigned char *address, char *data) {
  int available, readlength;
  char trashdata[4];

  dexuxclear(fd);
  write(fd, "IAI\x02", 4);
  write(fd, address+1, 1);
  write(fd, address+0, 1);
  usleep(USLEEPTIME);
  ioctl(fd, FIONREAD, &available);
  if (DEXUXDEBUGMODE) fprintf(stderr, "  available = %d\n", available);
  while (available < 132) {
    usleep(USLEEPTIME);
    ioctl(fd, FIONREAD, &available);
    if (DEXUXDEBUGMODE) fprintf(stderr, "  available = %d\n", available);
  }
  read(fd, trashdata, 4);
  readlength = read(fd, data, 128);

  return(readlength);
}
