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

// {{{ dex_utilDiscover
UINT8 dex_utilDiscover( UINT32 port )
{
  // Define local variables.
  UINT8  oBuffer[ 1024 ];
  UINT8  iBuffer[ 1024 ];

  UINT16 oLength = 0;
  UINT16 iLength = 0;

  UINT16 index   = 0;

  // Flush the input and output.
  dex_portIFlush( port );
  dex_portOFlush( port );

  // Write something random to the port.
  oLength = dex_portPut( port, "\x49\x41\x49\x21", 4 );
  iLength = dex_portGet( port, iBuffer, 4 );

  // Check the response.
  if( ( iLength != 4 ) || ( !dex_stringComp( iBuffer, "\x49\x41\x49\x21",4 ) ) )
  {
    // Something went wrong, we failed the detection.
    return DEX_DISCOVER_ERROR;
  }

  // Write the initialisation command.
  oLength = dex_portPut( port, "\x49\x41\x49\x00\x10\x29\x23\xBE\x84\xE1\x6C\xD6\xAE\x52\x90\x49\xF1\xF1\xBB\xE9\xEB", 21 );
  iLength = dex_portGet( port, iBuffer, 9 );

  // Check the response.
  if( ( iLength != 9 ) || ( !dex_stringComp( iBuffer, "\x49\x41\x49", 3 ) ) )
  {
    // Something went wrong, we failed the detection.
    return DEX_DISCOVER_ERROR;
  }

  // Write the flush command.
  for( index = 0; index < 10; index++ ) 
  {
    // Write the flush command.
    oLength = dex_portPut( port, "\x49\x41\x49\x27", 4 );

    // Flush the input and output.
    dex_portIFlush( port );
    dex_portOFlush( port );
  }

  // Read the response.
  iLength = dex_portGet( port, iBuffer, 4 );

  // Check the response.
  if( ( iLength != 4 ) || ( !dex_stringComp( iBuffer, "\x49\x41\x49\x21", 4 ) ) )
  {
    // Something went wrong, we failed the detection.
    return DEX_DISCOVER_ERROR;
  }

  // We completed the discovery procedure.
  return DEX_DISCOVER_SUCCESS;
}
// }}}

// {{{ dex_utilPower
UINT8 dex_utilPower( UINT32 port, UINT8 power )
{
  // Define local variables.
  UINT8  oBuffer[ 1024 ];
  UINT8  iBuffer[ 1024 ];

  UINT16 oLength = 0;
  UINT16 iLength = 0;

  // Flush the input and output.
  dex_portIFlush( port );
  dex_portOFlush( port );

  // Decide which type of operation we are dealing with.
  switch( power )
  {
    case DEX_POWER_OFF:
    {
      // Write the power command.
      oLength = dex_portPut( port, "\x49\x41\x49\x07\x00", 5 );
      iLength = dex_portGet( port, iBuffer, 4 );

      // Break from switch.
      break;
    }

    case DEX_POWER_ON:
    {
      // Write the power command.
      oLength = dex_portPut( port, "\x49\x41\x49\x07\x01", 5 );
      iLength = dex_portGet( port, iBuffer, 4 );

      // Break from switch.
      break;
    }

    default:
    {
      // Break from switch.
      break;
    }
  }

  // Check the response.
  if( iLength == 4 )
  {  
    // Return the success code.
    return iBuffer[ 3 ];
  }
  else
  {
    // Return the error code.
    return DEX_POWER_ERROR;
  }
}
// }}}
// {{{ dex_utilCheck
UINT8 dex_utilCheck( UINT32 port )
{
  // Define local variables.
  UINT8  oBuffer[ 1024 ];
  UINT8  iBuffer[ 1024 ];

  UINT16 oLength = 0;
  UINT16 iLength = 0;

  // Flush the input and output.
  dex_portIFlush( port );
  dex_portOFlush( port );

  // Write the check command.
  oLength = dex_portPut( port, "\x49\x41\x49\x01", 4 );
  iLength = dex_portGet( port, iBuffer, 5 );

  // Check the response.
  if( iLength == 5 )
  {  
    // Return the success code.
    return iBuffer[ 3 ];
  }
  else
  {
    // Return the error code.
    return DEX_CHECK_ERROR;
  }
}
// }}}


