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

void writegmeheader(FILE *fout) {
  int loopcnt;

  fprintf(fout, "123-456-STD");
  fprintf(fout, "%c%c%c%c%c", 0x00, 0x00, 0x00, 0x00, 0x00);
  fprintf(fout, "%c%c%c%c%c", 0x00, 0x00, 0x01, 0x00, 0x01);
  fprintf(fout, "MQQQQQQRSQQQQSQ");
  fprintf(fout, "%c%c%c%c", 0xA0, 0x00, 0xFF, 0xFF);
  fprintf(fout, "%c%c%c%c", 0xFF, 0xFF, 0xFF, 0x06);
  fprintf(fout, "%c%c%c%c", 0x07, 0xFF, 0xFF, 0xFF);
  fprintf(fout, "%c%c%c%c", 0xFF, 0x0C, 0xFF, 0xFF);
  fprintf(fout, "%c%c%c%c", 0xFF, 0x00, 0x00, 0x00);
  for (loopcnt=0; loopcnt < 962; loopcnt++) {
    fprintf(fout, "%c%c%c%c", 0x00, 0x00, 0x00, 0x00);
  }
}

void writevgsheader(FILE *fout) {
  int loopcnt;

  fprintf(fout, "VgsM");
  for (loopcnt = 0; loopcnt < 3; loopcnt++) {
    fprintf(fout, "%c%c%c%c", 0x01, 0x00, 0x00, 0x00);
  }
  fprintf(fout, "%c%c%c%c", 0x00, 0x02, 0x00, 0x00);
  for (loopcnt = 0; loopcnt < 11; loopcnt++) {
    fprintf(fout, "%c%c%c%c", 0x00, 0x00, 0x00, 0x00);
  }
}


int dexreadfull(int dexuxfd, int fullsaveformat, char *fname) {
  FILE *fout;
  char blockdata[8193];
  int blockcnt, progresscnt;

  if (strcmp(fname, "-") == 0) {
    fout=stdout;
  } else {
    fout=fopen(fname, "w");
    if (fout == NULL) {
      fprintf(stderr, "Error opening file, skipping: %s\n", fname);
      return(1);
    }
  }

  if (fullsaveformat == 1) {
    writevgsheader(fout);
  } else if (fullsaveformat == 2) {
    writegmeheader(fout);
  }

  fprintf(stderr, "Progress for: %s\n", fname);
  fprintf(stderr, "|");
  for (progresscnt = 0; progresscnt < 64; progresscnt++) {
    fprintf(stderr, "-");
  }
  fprintf(stderr, "|\n ");

  for (blockcnt = 0; blockcnt <= 15; blockcnt++) {
    if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxgetblock(%d)\n", blockcnt);
    dexuxgetblock(dexuxfd, blockcnt, blockdata);
    fwrite(blockdata, 8192, 1, fout);
    fprintf(stderr, "****");
  }
  fprintf(stderr, "\n");

  fclose(fout);

  return(0);
}
