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

int dexreadfile(char *fname, char *inputbuffer, char *fileformat, int *imagesize, int *offset) {
  FILE *fin;
  int inputsize;
  char *pblock;
  char vgsmagic[6]="VgsM ";
  char gmemagic[12]="123-456-STD";
  char mcdmagic[3]="MC";

  vgsmagic[4]=0x01;

  if (DEXUXDEBUGMODE) fprintf(stderr, "dexreadfile(%s, %u)\n", fname, inputbuffer);

  if (strcmp("-", fname) == 0) {
    fin = stdin;
  } else {
    fin = fopen(fname, "r");
  }
  inputsize = fread(inputbuffer, 1, 262144, fin);
  fclose(fin);

  if (inputsize == 262144) {
    fprintf(stderr, "File is incorrect format (too long), skipping: %s\n",
            fname);
    return(1);
  }

  if (inputsize < 8246) {
    fprintf(stderr, "Error reading file or file is corrupted, skipping: %s\n",
            fname);
    return(1);
  }

  if (strncmp(vgsmagic, inputbuffer, 6) == 0) {
    *imagesize = 16;
    strcpy(fileformat, "VGS");
  } else if (strncmp(gmemagic, inputbuffer, 12) == 0) {
    *imagesize = 16;
    strcpy(fileformat, "GME");
  } else if (strncmp(mcdmagic, inputbuffer, 3) == 0) {
    *imagesize = 16;
    strcpy(fileformat, "MCD");
  }

  *offset = 0;
  pblock  = inputbuffer;

  if (*imagesize == 16) {

    if (inputsize < 131072) {
      fprintf(stderr, "Detected file format %s, but file appears", fileformat);
      fprintf(stderr, "to be corrupt\n");
      fprintf(stderr, "Skipping file: %s\n", fname);
      return(1);
    }

    *offset = (inputsize - 131072);
    pblock += *offset;

    if ((strncmp(mcdmagic, pblock, 3) != 0)) {
      fprintf(stderr, "Detected file format %s, but file appears", fileformat);
      fprintf(stderr, "to be corrupt\n");
      fprintf(stderr, "Skipping file: %s\n", fname);
      return(1);
    }

  } else {

    if ((inputbuffer[54] == 'S') && (inputbuffer[55] == 'C')) {
      if ((inputsize - 54) % 8192 != 0) {
        fprintf(stderr, "Detected file format PSX, but file appears");
        fprintf(stderr, " to be corrupt\n");
        fprintf(stderr, "Skipping file: %s\n", fname);
        return(1);
      }
      strcpy(fileformat, "PSX");
      *imagesize = (inputsize - 54) / 8192;
    } else if ((inputbuffer[128] == 'S') && (inputbuffer[129] == 'C')) {
      if ((inputsize - 128) % 8192 != 0) {
        fprintf(stderr, "Detected file format MCS, but file appears");
        fprintf(stderr, " to be corrupt\n");
        fprintf(stderr, "Skipping file: %s\n", fname);
        return(1);
      }
      *imagesize = (inputsize - 128) / 8192;
      strcpy(fileformat, "MCS");
    }

  }

  if (DEXUXDEBUGMODE) fprintf(stderr, "  dexreadfile -> %s, %s, %u, %u, %u\n", fname, fileformat, *imagesize, inputbuffer, *offset);

  return(0);

}
