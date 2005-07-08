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

// {{{ dex_framePut
UINT8 dex_framePut( UINT32 port, UINT16 address, dex_frame* frame )
{
  // Define local variables.
  UINT8  oBuffer[ 1024 ];
  UINT8  iBuffer[ 1024 ];

  UINT16 oLength     = 0;
  UINT16 iLength     = 0;

  UINT8  addressMSB  = 0;
  UINT8  addressLSB  = 0;
  UINT8  addressMSBR = 0;
  UINT8  addressLSBR = 0;

  UINT8  xor         = 0;

  UINT16 index       = 0;
  UINT16 retry       = 0;

  BOOL   finished    = FALSE;

  // Keep trying until we finish the frame put or run out of retries.
  while( ( !finished ) && ( retry < DEX_FRAME_RETRY ) )
  {
    // Print debugging information if we need to.
    if( dex_debug )
    {
      printf( "dex_framePut( ... ) [port=0x%02X, address=0x%02X, retry=0x%02X]\n", port, address, retry );
    }

    // Assume we have finished.
    finished = TRUE;

    // Flush the input and output.
    dex_portIFlush( port );
    dex_portOFlush( port );
  
    // Calculate the address parts and update the xor value.
    addressMSB  =  ( address >> 8 ) & 0x00FF;
    addressLSB  =  ( address >> 0 ) & 0x00FF;
    addressMSBR = dex_bitReverse( addressMSB );
    addressLSBR = dex_bitReverse( addressLSB );

    // Calculate the checksum.
    xor ^= addressMSB;
    xor ^= addressLSB;
    xor ^= addressMSBR;
    xor ^= addressLSBR;

    // Calculate the checksum.
    for( index = 0; index < 128; index++ )
    {
      xor ^= frame->raw[ index ];
    }

    // Write the get data command.
    oLength = dex_portPut( port, "\x49\x41\x49\x04", 4 );
    oLength = dex_portPut( port, &addressMSB, 1 );
    oLength = dex_portPut( port, &addressLSB, 1 );
    oLength = dex_portPut( port, &addressMSBR, 1 );
    oLength = dex_portPut( port, &addressLSBR, 1 );

    // Write the data.
    dex_portPut( port, frame->raw, 128 );

    // Write the checksum.
    dex_portPut( port, &xor, 1 );

    // Read the response.
    iLength = dex_portGet( port, iBuffer, 4 );

    // Check the response.
    if( ( iLength != 4 ) || ( !dex_stringComp( iBuffer, "\x49\x41\x49\x29", 4 ) ) )
    {
      finished = FALSE;
    }

    // Increment the retry counter.
    retry++;
  }

  // We completed the put frame procedure.
  return finished == TRUE ? DEX_FRAME_PUT_SUCCESS : DEX_FRAME_PUT_ERROR;
}
// }}}
// {{{ dex_frameGet
UINT8 dex_frameGet( UINT32 port, UINT16 address, dex_frame* frame )
{
  // Define local variables.
  UINT8  oBuffer[ 1024 ];
  UINT8  iBuffer[ 1024 ];

  UINT16 oLength    = 0;
  UINT16 iLength    = 0;

  UINT8  addressLSB = 0;
  UINT8  addressMSB = 0;

  UINT16 index      = 0;
  UINT16 retry      = 0;

  BOOL   finished   = FALSE;

  // Keep trying until we finish the frame get or run out of retries.
  while( ( !finished ) && ( retry < DEX_FRAME_RETRY ) )
  {
    // Print debugging information if we need to.
    if( dex_debug )
    {
      printf( "dex_frameGet( ... ) [port=0x%02X, address=0x%02X, retry=0x%02X]\n", port, address, retry );
    }

    // Assume we have finished.
    finished = TRUE;

    // Flush the input and output.
    dex_portIFlush( port );
    dex_portOFlush( port );

    // Calculate the address parts.
    addressLSB = ( address >> 0 ) & 0x00FF;
    addressMSB = ( address >> 8 ) & 0x00FF;

    // Write the get data command.
    oLength = dex_portPut( port, "\x49\x41\x49\x02", 4 );
    oLength = dex_portPut( port, &addressLSB, 1 );
    oLength = dex_portPut( port, &addressMSB, 1 );

    // Read the response.
    iLength = dex_portGet( port, iBuffer, 4 );

    // Check the response.
    if( ( iLength != 4 ) || ( !dex_stringComp( iBuffer, "\x49\x41\x49\x41", 4 ) ) )
    {
      finished = FALSE;
    }

    // Read the data.
    iLength = dex_portGet( port, frame->raw, 128 );

    // Check the response.
    if( iLength != 128 )
    {
      finished = FALSE;
    }

    // Read the checksum.
    iLength = dex_portGet( port, iBuffer, 1 );

    // Check the response.
    if( iLength != 1 )
    {
      finished = FALSE;
    }

    // Increment the retry counter.
    retry++;
  }

  // We completed the get frame procedure.
  return finished == TRUE ? DEX_FRAME_GET_SUCCESS : DEX_FRAME_GET_ERROR;
}
// }}}

// {{{ dex_bitReverse
UINT8 dex_bitReverse( UINT8 src )
{
  // Define local variables.
  UINT8 dst = 0;

  // Move bits to their new positions.
  dst |= ( src & 0x0001 ) << 7;
  dst |= ( src & 0x0002 ) << 5;
  dst |= ( src & 0x0004 ) << 3;
  dst |= ( src & 0x0008 ) << 1;
  dst |= ( src & 0x0010 ) >> 1;
  dst |= ( src & 0x0020 ) >> 3;
  dst |= ( src & 0x0040 ) >> 5;
  dst |= ( src & 0x0080 ) >> 7;

  // Return the new byte.
  return dst;
}
// }}}
