// --------------------------------------------------------------------------
//
// LinDex - DexDrive Utilities For Linux
// Copyright (C) 2001 Dan Page [http://www.phoo.org]
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// --------------------------------------------------------------------------

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "lindex.h"

#include "dex_frame.h"
#include "dex_block.h"
#include "dex_card.h"
#include "dex_port.h"
#include "dex_string.h"
#include "dex_util.h"

// {{{ dex_stringCompare
BOOL dex_stringComp( UINT8 stringA[], UINT8 stringB[], UINT32 length )
{
  // Define local variables.
  UINT32 index = 0;

  // Loop through both strings comparing characters.
  for( index = 0; index < length; index++ )
  {
    // Check if the current characters match.
    if( stringA[ index ] != stringB[ index ] )
    {
      // The characters don't match so neither do the strings.
      return FALSE;
    }
  }

  // If all the strings match, return true.
  return TRUE;
}
// }}}
// {{{ dex_stringCopy
void dex_stringCopy( UINT8 stringA[], UINT8 stringB[], UINT32 length )
{
  // Define local variables.
  UINT32 index = 0;

  // Loop through both strings copying characters.
  for( index = 0; index < length; index++ )
  {
    stringA[ index ] = stringB[ index ];
  }
}
// }}}

// {{{ dex_string2ASCII
UINT8* dex_string2ASCII( UINT8 stringA[], UINT8 stringB[], UINT32 length )
{
  // Define local variables.
  UINT32 indexA = 0;
  UINT32 indexB = 0;

  for( indexA = 0; indexA < length; indexA++ )
  {
    if( isprint( stringA[ indexA ] ) )
    {
      stringB[ indexB++ ] = stringA[ indexA ];
    }
  }

  stringB[ indexB ] = '\0';

  return stringB;
}
// }}}
// {{{ dex_string2KANJI
UINT8* dex_string2KANJI( UINT8 stringA[], UINT8 stringB[], UINT32 length )
{

}
// }}}


