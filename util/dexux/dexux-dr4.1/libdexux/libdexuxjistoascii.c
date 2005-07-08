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

/* Converts Shift JIS to ASCII */
int dexuxjistoascii(unsigned char *shiftjis, unsigned char *ascii) {
  unsigned char *pascii;
  int i, asciipos = 0; /* counters */

  pascii = ascii;
  memset(ascii, 0, 65);

  for (i = 0; i <= 63; i++) {
    if (shiftjis[i] > 0x00 && shiftjis[i] <= 0x7F) {
      *(pascii+asciipos++) = shiftjis[i];  /* ASCII */
    } else if (shiftjis[i] >= 0xA0 && shiftjis[i] <= 0xDF) {
      *(pascii+asciipos++) = '.';  /* Single-width katakana */
    } else if (shiftjis[i] == 0x81) {   /* Special symbols, two-byte */
      switch(shiftjis[++i]) {
          case 0x40: *(pascii+asciipos++) = ' '; break;
/*        case 0x41: *(pascii+asciipos++) = ','; break; */ /* Japanese , */
/*        case 0x42: *(pascii+asciipos++) = '.'; break; */ /* Japanese . */
          case 0x43: *(pascii+asciipos++) = ','; break;
          case 0x44: *(pascii+asciipos++) = '.'; break;

          case 0x46: *(pascii+asciipos++) = ':'; break;
          case 0x47: *(pascii+asciipos++) = ';'; break;
          case 0x48: *(pascii+asciipos++) = '?'; break;
          case 0x49: *(pascii+asciipos++) = '!'; break;

          case 0x5E: *(pascii+asciipos++) = '/'; break;
          case 0x6D: *(pascii+asciipos++) = '['; break;
          case 0x6E: *(pascii+asciipos++) = ']'; break;
          default:   *(pascii+asciipos++) = '.'; break;
      }
    } else if (shiftjis[i] == 0x82) {   /* Double-width digits, latin, hiragana */
      i++;
      
      if (shiftjis[i] < 0x40) {
          *(pascii+asciipos++) = '!';   /* Illegal in specification */
      } else if (shiftjis[i] >= 0x40 && shiftjis[i] <= 0x4E) {
          *(pascii+asciipos++) = '.';   /* Invalid? */
      } else if (shiftjis[i] >= 0x4F && shiftjis[i] <= 0x58) {
          *(pascii+asciipos++) = shiftjis[i] - 0x1F;   /* Digits */
      } else if (shiftjis[i] >= 0x60 && shiftjis[i] <= 0x79) {
          *(pascii+asciipos++) = shiftjis[i] - 0x1F;   /* Upper case */
      } else if (shiftjis[i] >= 0x7A && shiftjis[i] <= 0x7E) {
          *(pascii+asciipos++) = '.';   /* Invalid? */
      } else if (shiftjis[i] == 0x7F) {
          *(pascii+asciipos++) = '!';   /* Illegal in specification */
      }
      else if (shiftjis[i] >= 0x81 && shiftjis[i] <= 0xA6)
          *(pascii+asciipos++) = shiftjis[i] - 0x20;   /* Lower case */

      else switch(shiftjis[i]) {
          /* It's something else */
          default:   *(pascii+asciipos++) = '.'; break;
      }
    } else if (shiftjis[i] >= 0x83 && shiftjis[i] <= 0x9F) {
      i++;  *(pascii+asciipos++) = '?';  /* Some other two-byte sequence */
    } else if (shiftjis[i] >= 0xE0 && shiftjis[i] <= 0xFC) {
      i++;  *(pascii+asciipos++) = '?';  /* Some other two-byte sequence */
    } else if (shiftjis[i] != 0) {
      *(pascii+asciipos++) = '?';  /* Something else */
    }
  }
  return(0);
}
