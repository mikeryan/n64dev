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

// {{{ dex_cardPut
UINT8 dex_cardPut( UINT32 port, dex_card* card )
{
  // Define local variables.
  UINT16 index    = 0;
  UINT16 retry    = 0;

  BOOL   finished = FALSE;

  // Keep trying until we finish the card put or run out of retries.
  while( ( !finished ) && ( retry < DEX_FRAME_RETRY ) )
  {  
    // Assume we have finished.
    finished = TRUE;

    // Flush the input and output.
    dex_portIFlush( port );
    dex_portOFlush( port );

    // Write all the blocks to the card.
    for( index = 0; index < 16; index++ )
    {
      // Perform the write and check the result.
      if( dex_blockPut( port, ( index * 64 ), &( card->block[ index ] ) ) == DEX_BLOCK_PUT_ERROR )
      {
        finished = FALSE;
      }
    }

    // Increment the retry counter.
    retry++;
  }

  // We completed the put card procedure.
  return finished == TRUE ? DEX_CARD_PUT_SUCCESS : DEX_CARD_PUT_ERROR;
}
// }}}
// {{{ dex_cardGet
UINT8 dex_cardGet( UINT32 port, dex_card* card )
{
  // Define local variables.
  UINT16 index    = 0;
  UINT16 retry    = 0;

  BOOL   finished = FALSE;

  // Keep trying until we finish the card get or run out of retries.
  while( ( !finished ) && ( retry < DEX_FRAME_RETRY ) )
  {  
    // Assume we have finished.
    finished = TRUE;

    // Flush the input and output.
    dex_portIFlush( port );
    dex_portOFlush( port );

    // Read all the blocks from the card.
    for( index = 0; index < 16; index++ )
    {
      // Perform the read and check the result.
      if( dex_blockGet( port, ( index * 64 ), &( card->block[ index ] ) ) == DEX_BLOCK_GET_ERROR )
      {
        finished = FALSE;
      }
    }

    // Increment the retry counter.
    retry++;
  }

  // We completed the get card procedure.
  return finished == TRUE ? DEX_CARD_GET_SUCCESS : DEX_CARD_GET_ERROR;
}
// }}}
