/*

    Author:  Tom Whittock
    Compile: gcc -Wall -O2 -o raw2s raw2s.c
    Purpose: Convert a binary file into a GCC .S asm file
    Usage:   raw2s  <binary file> <name of object>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

*/

#include <stdio.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char **argv)
{
  FILE *bin;
  unsigned char buf[1024];
  unsigned int count, i;
  
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <binary file> <name of object>\n", argv[0]);
    return 1;
  }

  bin = fopen(argv[1], "rb");
  if (bin == NULL) {
    fprintf(stderr, "Couldn't open %s: %s\n", argv[1],
	    strerror(errno));
    return 1;
  }

  printf(".globl\t%s\n", argv[2]);
  printf(".data\n");
  printf("\t.align\t4\n");
  printf("%s:\n", argv[2]);
  do {
    count = fread(buf, 1, 1024, bin);

    for (i = 0 ; i < count ; i++) {
      if (i % 8 == 0) {
	printf("\n\t.byte 0x%02x", buf[i]);
      } else {
	printf(", 0x%02x", buf[i]);
      }
    }
  } while (count == 1024);
  printf("\n");
  fclose(bin);
  return 0;
}





