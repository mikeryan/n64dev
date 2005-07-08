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

#include <stdio.h>

#define DEXUXTTYDEVICE "/dev/ttyS1"
#define DEXUXTIMEOUT 10  /* in 1/10 seconds */

/* Global variable */
int DEXUXDEBUGMODE;

int dexuxopen(char *devname);
  /* Create a file descriptor and configure serial port */

int dexuxwake(int fd);
  /* Send initialization code to the DexDrive */

int dexuxclear(int fd);
  /* Read in and discard all waiting byte from serial port */

int dexuxstatus(int fd);
  /* Check for card presence */

int dexuxclose(int fd);
  /* Put DexDrive to sleep and close the file descriptor */

int dexuxgetframe(int fd, unsigned char *address, char *data);
  /* Get a specified frame from the card */

int dexuxgetblock(int fd, int blocknumber, char *blockdata);
  /* Call dexuxgetframe multiple times to get a whole block */

/* int dexuxsettimeout(int fd, int length); */
  /* Change timeout */

int dexuxframetop(long frameaddress);
  /* Get top half of frame address */

int dexuxframebottom(long frameaddress);
  /* Get bottom half of frame address */

int dexuxbinreverse(int inbyte);
  /* Reverse the 8 digits of inbyte */

int dexuxstrxor(char *instring, long stringlength);
  /* XOR all bytes together */

int dexuxputframe(int fd, char *address, char *data);
  /* Write a frame of data */

int dexuxputblock(int fd, int blocknumber, char *blockdata);
  /* Call dexuxputframe multiple times to write a whole block */

int dexuxjistoascii(unsigned char *shiftjis, unsigned char *ascii);
  /* Converts Shift JIS to ASCII */

