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

/* XOR all bytes together */
int dexuxstrxor(char *instring, long stringlength) {
  int outbyte = 0;
  long i;  /* Loop counter */
  
  for (i = 0; i <= (stringlength - 1); i++) {
      outbyte =   (  (outbyte&  1)     ^  (*(instring+i)&  1)           ) +
                ( ( ((outbyte&  2)>>1) ^ ((*(instring+i)&  2)>>1) ) << 1) +
                ( ( ((outbyte&  4)>>2) ^ ((*(instring+i)&  4)>>2) ) << 2) +
                ( ( ((outbyte&  8)>>3) ^ ((*(instring+i)&  8)>>3) ) << 3) +
                ( ( ((outbyte& 16)>>4) ^ ((*(instring+i)& 16)>>4) ) << 4) +
                ( ( ((outbyte& 32)>>5) ^ ((*(instring+i)& 32)>>5) ) << 5) +
                ( ( ((outbyte& 64)>>6) ^ ((*(instring+i)& 64)>>6) ) << 6) +
                ( ( ((outbyte&128)>>7) ^ ((*(instring+i)&128)>>7) ) << 7);
  }

  return(outbyte);
}
