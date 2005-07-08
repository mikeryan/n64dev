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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lindex.h"

#include "dex_frame.h"
#include "dex_block.h"
#include "dex_card.h"
#include "dex_port.h"
#include "dex_string.h"
#include "dex_util.h"

#include "lindex_format_gme.h"
#include "lindex_format_raw.h"
#include "lindex_util.h"


BOOL dex_quiet   = FALSE;
BOOL dex_verbose = FALSE;
BOOL dex_debug   = FALSE;


// {{{ main
int main( int argumentCount, char* arguments[] )
{
  UINT32 port    = 0;
  UINT32 result  = 0;

  FILE* file;

  dex_quiet   = FALSE;
  dex_verbose = FALSE;
  dex_debug   = FALSE;

  if( !dex_quiet )
  {
    printf( "LinDex 0.0 - DexDrive Utilities For Linux         \n" );
    printf( "Copyright (C) 2001 Dan Page [http://www.phoo.org] \n" ); 
    printf( "\n" ); 
  }

  if( dex_utilDiscover( port = dex_portOpen( arguments[ 1 ] ) ) != DEX_DISCOVER_SUCCESS )
  {
    if( !dex_quiet )
    {
      printf( "error : couldn't find a DexDrive on port %s\n", arguments[ 1 ] );
      exit( 1 );
    }
  }

  dex_utilCheck( port );

  dex_utilPower( port, DEX_POWER_ON );

  printf( "Index Test\n" );
  printf( "----------\n" );
  printf( "\n" );
  lindex_utilDebugIndex( port );
  printf( "\n" );

  printf( "Block Test\n" );
  printf( "----------\n" );
  printf( "\n" );
  lindex_utilDebugBlock( port );
  printf( "\n" );

  printf( "Write Test\n" );
  printf( "----------\n" );
  printf( "\n" );
  lindex_utilDebugWrite( port );
  printf( "\n" );

  dex_utilPower( port, DEX_POWER_OFF );

  dex_portClose( port );

  return 0;
}
// }}}
