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

/* Information found on http://www.easysw.com/~mike/serial/ was
 * invaluable in the production of this code.  Thanks to Michael Sweet.
 */

/* Portmon by Mark Russinovich, found on http://www.sysinternals.com,
 * was used to watch the serial port operations between the DexDrive and
 * the DexPlorer software.
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
#include "dexuxclient.h"

#define DEXUXVERSION "0.5beta"
#define DEXUXAUTHORS "William Ono and Ben Galliart"
#define DEXUXCOPYRIGHTYEARS "2000"

int main (int argc, char **argv) {
  char devport[4096];
  int argcnt, dexuxfd;
  int dexclosed = 1, fullsaveformat = 0, singlesaveformat = 0;

  DEXUXDEBUGMODE = 0;

  devport[0] = 0;

  for (argcnt = 1; argcnt < argc; argcnt++) {

    if ((argv[argcnt][0] == '-') && (argv[argcnt][1] != '\0')) {      
      if (argv[argcnt][1] == 'D') {
        DEXUXDEBUGMODE = 1;
      } else if (argv[argcnt][1] == 'd') {
        if (argv[argcnt][2] == '\0') {
          argcnt++;
          if (argcnt == argc) continue;
          strncpy(devport, argv[argcnt], 4096);
          continue;
        } else {
          char *pargv;
          pargv = argv[argcnt];
          pargv++;  pargv++; 
          strncpy(devport, pargv, 4096);
          continue;
        }
      } else if (argv[argcnt][1] == 'f') {
        argcnt++;
        if (argcnt == argc) continue;
        if (strcmp("mcd", argv[argcnt]) == 0) {
          fullsaveformat=0;
        } else if (strcmp("mc", argv[argcnt]) == 0) {
          fullsaveformat=0;
        } else if (strcmp("ddf", argv[argcnt]) == 0) {
          fullsaveformat=0;
        } else if (strcmp("vgs", argv[argcnt]) == 0) {
          fullsaveformat=1;
        } else if (strcmp("gme", argv[argcnt]) == 0) {
          fullsaveformat=2;
        } else if (strcmp("psx", argv[argcnt]) == 0) {
          singlesaveformat=0;
        } else if (strcmp("mcs", argv[argcnt]) == 0) {
          singlesaveformat=1;
        } else if (strcmp("xpm", argv[argcnt]) == 0) {
          singlesaveformat=2;
        }
      } else if (argv[argcnt][1] == 'h') {
        fprintf(stderr, "DEXUX client version %s\n", DEXUXVERSION);
        fprintf(stderr, "Written by %s\n", DEXUXAUTHORS);
        fprintf(stderr, "Copyright %s\n", DEXUXCOPYRIGHTYEARS);
        fprintf(stderr, "This program is covered by the General Public License\n\n");
	fprintf(stderr, "Command line options:\n");
        fprintf(stderr, "  -D            -- debug messages (very messy)\n");
        fprintf(stderr, "  -d [device]   -- port that the card reader is connected\n");
	fprintf(stderr, "  -f [format]   -- format to save files blocks/cards which have been read\n");
        fprintf(stderr, "    Formats available:\n");
        fprintf(stderr, "    Single game formats:  psx, mcs, xpm\n");
        fprintf(stderr, "    NOTE: XPM is an image format\n");
        fprintf(stderr, "    Entire game card image formats:  mcs, mc, ddf, vgs, gme\n\n");
        fprintf(stderr, "  l[ist]\n");
        fprintf(stderr, "  r[ead] [block#] filename\n");
        fprintf(stderr, "  w[rite] filename\n");
        fprintf(stderr, "  d[elete]   block# [additional block#s]\n");
        fprintf(stderr, "  u[ndelete] block# [addtional block#s]\n"); 
        continue;
      } else {
        fprintf(stderr, "Unknown option skipped: %s, -h to get help\n", argv[argcnt]);
        continue;
      } 
    }

    if (dexclosed) {
	if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxopen(%s)\n", devport);
        dexuxfd = dexuxopen(devport);
        if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxwake()\n");
        if (dexuxwake(dexuxfd)) {
          fprintf(stderr, "Fatal error: Could not find the DexDrive\n");
          return(1);
        }
        if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxstatus()\n");
        if (dexuxstatus(dexuxfd)) {
          fprintf(stderr, "Please insert memory card\n");
          while (dexuxstatus(dexuxfd)) { usleep(50000); }
        }
	dexclosed = 0;
    }

    if (argv[argcnt][0] == 'l') {
      if (DEXUXDEBUGMODE) fprintf(stderr, "dexlist()\n");
      dexlist(dexuxfd);
      continue;
    } else if (argv[argcnt][0] == 'w') {
      argcnt++;
      if (argcnt == argc) {
        fprintf(stderr, "Please specify save image filename to");
        fprintf(stderr, " be written to card\n");
        continue;
      }
      if (DEXUXDEBUGMODE) fprintf(stderr, "dexwrite(%s)\n", argv[argcnt]);
      dexwrite(dexuxfd, argv[argcnt]);
      continue;
    } else if (argv[argcnt][0] == 'r') {
      int blocknum = 0;
      argcnt++;
      if (argcnt == argc) {
        fprintf(stderr, "Please specify filename to save the");
        fprintf(stderr, " image too\n");
        continue;
      }
      if ((argv[argcnt][0] >= '1') && (argv[argcnt][0] <= '9')) {
        if (argv[argcnt][1] == '\0') {
          blocknum = argv[argcnt][0] - '0';
        } else if ((argv[argcnt][0] == '1') &&
                   (argv[argcnt][1] >= '0') &&
                   (argv[argcnt][1] <= '5') &&
                   (argv[argcnt][2] == '\0')) {
          blocknum = 10 + argv[argcnt][1] - '0';
        }
      }
      if (blocknum == 0) {
	if (DEXUXDEBUGMODE) fprintf(stderr, "dexreadfull(%d, %s)\n",
	  fullsaveformat, argv[argcnt]);
        dexreadfull(dexuxfd, fullsaveformat, argv[argcnt]);
      } else {
        argcnt++;
        if (argcnt == argc) {
          fprintf(stderr, "Please specify filename to save the");
          fprintf(stderr, " image too\n");
          continue;
        }
	if (DEXUXDEBUGMODE) fprintf(stderr, "dexreadsingle(%d, %d, %s)\n",
          singlesaveformat, blocknum, argv[argcnt]);
        dexreadsingle(dexuxfd, singlesaveformat, blocknum, argv[argcnt]);
      }
      continue;
    } else if (argv[argcnt][0] == 'd') {
      int blocknum = 16;

      while ((blocknum > 0) && (argv[argcnt + 1] != NULL)) {
        if ((argv[argcnt + 1][0] >= '1') &&
            (argv[argcnt + 1][0] <= '9')) {
          if (argv[argcnt + 1][1] == '\0') {
            argcnt++;
            blocknum = argv[argcnt][0] - '0';
	    if (DEXUXDEBUGMODE) fprintf(stderr, "dexdelete(%d, 0)\n", blocknum);
            dexdelete(dexuxfd, blocknum, 0);
          } else if ((argv[argcnt + 1][0] == '1') &&
                     (argv[argcnt + 1][1] >= '0') &&
                     (argv[argcnt + 1][1] <= '5') &&
                     (argv[argcnt + 1][2] == '\0')) {
            argcnt++;
            blocknum = 10 + argv[argcnt][1] - '0';
	    if (DEXUXDEBUGMODE) fprintf(stderr, "dexdelete(%d, 0)\n", blocknum);
            dexdelete(dexuxfd, blocknum, 0);
          } else {
            blocknum = 0;
          }
        } else blocknum = 0;
      }
      continue;
    } else if (argv[argcnt][0] == 'u') {
      int blocknum = 16;

      while ((blocknum > 0) && (argcnt + 1 != argc)) {
        if ((argv[argcnt + 1][0] >= '1') &&
            (argv[argcnt + 1][0] <= '9')) {
          if (argv[argcnt + 1][1] == '\0') {
            argcnt++;
            blocknum = argv[argcnt][0] - '0';
	    if (DEXUXDEBUGMODE) fprintf(stderr, "dexdelete(%d, 1)\n", blocknum);
            dexdelete(dexuxfd, blocknum, 1);
          } else if ((argv[argcnt + 1][0] == '1') &&
                     (argv[argcnt + 1][1] >= '0') &&
                     (argv[argcnt + 1][1] <= '5') &&
                     (argv[argcnt + 1][2] == '\0')) {
            argcnt++;
            blocknum = 10 + argv[argcnt][1] - '0';
	    if (DEXUXDEBUGMODE) fprintf(stderr, "dexdelete(%d, 1)\n", blocknum);
            dexdelete(dexuxfd, blocknum, 1);
          } else {
            blocknum = 0;
          }
        } else blocknum = 0;
      }
      continue;
    }
  } /* for argcnt */

  if (DEXUXDEBUGMODE) fprintf(stderr, "dexuxclose()\n");
  dexuxclose(dexuxfd);

  return(0);

}
