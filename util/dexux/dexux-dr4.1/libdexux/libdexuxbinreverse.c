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

/* Reverse the 8 digits of inbyte */
int dexuxbinreverse(int inbyte) {
  int outbyte;

  outbyte =            (inbyte &   1) << 7;
  outbyte = outbyte + ((inbyte &   2) << 5);
  outbyte = outbyte + ((inbyte &   4) << 3);
  outbyte = outbyte + ((inbyte &   8) << 1);
  outbyte = outbyte + ((inbyte &  16) >> 1);
  outbyte = outbyte + ((inbyte &  32) >> 3);
  outbyte = outbyte + ((inbyte &  64) >> 5);
  outbyte = outbyte + ((inbyte & 128) >> 7);

  return(outbyte);
}
