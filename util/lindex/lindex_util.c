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

// {{{ lindex_utilDebugIndex
void lindex_utilDebugIndex( UINT32 port )
{
  // Define local variables.
  dex_frame gameFrame;
  dex_frame dataFrame;

  UINT8     ASCIIBuffer[ 1024 ];

  UINT16    index  = 0;

  // Loop through all the directory frames.
  for( index = 1; index < 16; index++ )
  {
    // Load the current directory frame.
    dex_frameGet( port, index, &dataFrame );

    // Dump the directory information for this frame.
    printf( " 0x%08X ", dataFrame.data.type );
    printf( " 0x%08X ", dataFrame.data.size );
    printf( " 0x%04X ", dataFrame.data.next );
    printf( " %s ", dex_string2ASCII( dataFrame.data.tCode, ASCIIBuffer, 2 )  );
    printf( " %s ", dex_string2ASCII( dataFrame.data.lCode, ASCIIBuffer, 10 ) );
    printf( " %s ", dex_string2ASCII( dataFrame.data.sCode, ASCIIBuffer, 8 )  );
    printf( "\n" );
  }
}
// }}}
// {{{ lindex_utilDebugBlock
void lindex_utilDebugBlock( UINT32 port )
{
  // Define local variables.
  dex_frame gameFrame;
  dex_frame dataFrame;

  UINT8     ASCIIBuffer[ 1024 ];
  UINT8     KANJIBuffer[ 1024 ];

  UINT16    chain[ 15 ];

  UINT16    index     = 0;

  UINT16    chainNext = 0;
  UINT16    chainSize = 0;

  // Loop through all the directory frames.
  for( index = 1; index < 16; index++ )
  {
    // Load the current directory frame.
    dex_frameGet( port, index, &dataFrame );

    // Clear the save chain variables.
    chainNext = 0;
    chainSize = 0;

    // Check if this block is the initial block in a save chain.
    if( dataFrame.data.type == DEX_INDEX_TYPE_INITIAL )
    {
      // Print the block information.
      printf( " %02X : Title        : %s\n"  , index, dex_string2KANJI( gameFrame.game.title, KANJIBuffer, 64 ) );

      printf( "      Blocks       : %02X\n", ( dataFrame.data.size / 8192 ) );

      printf( "      Region Code  : %s\n"  , dex_string2ASCII( dataFrame.data.tCode, ASCIIBuffer, 2 )  );
      printf( "      Produce Code : %s\n"  , dex_string2ASCII( dataFrame.data.lCode, ASCIIBuffer, 10 ) );
      printf( "      License Code : %s\n"  , dex_string2ASCII( dataFrame.data.sCode, ASCIIBuffer, 8 )  );

      printf( "      Save Chain   : " );

      // Add the block to the current save chain.
      chain[ chainSize++ ] = index;

      // Get the corresponding game frame from the card.
      dex_frameGet( port, index * 64, &gameFrame );

      // Loop through the entire save chain.
      while( ( chainNext = dataFrame.data.next ) != DEX_INDEX_NEXT_FINAL )
      {
        // Correct the next save chain pointer.
        chainNext = chainNext + 1;

        // Add the block to the current save chain.
        chain[ chainSize++ ] = chainNext;

        // Get the next directory frame.
        dex_frameGet( port, chainNext, &dataFrame );
      }     

      // Print the save chain.
      for( chainNext = 0; chainNext < chainSize; chainNext++ )
      {
        // Print a chain redirection.
        if( chainNext != 0 )
        {
          printf( "->" );
        }

        // Print the chain node.
        printf( "%02X", chain[ chainNext ] );
      }

      // Print a seperator. 
      printf( "\n" );
    }
  }
}    
// }}}
// {{{ lindex_utilDebugWrite
void lindex_utilDebugWrite( UINT32 port )
{
  // Define local variables.
  dex_frame testFrameA;
  dex_frame testFrameB;

  UINT16    indexA = 0;
  UINT16    indexB = 0;

  UINT16    index  = 0;

  // Fill the frames with initial data.
  for( indexA = 0; indexA < 16; indexA++ )
  {
    for( indexB = 0; indexB < 8; indexB++ )
    {
      // Calculate the frame index.
      index = ( indexA * 8 ) + indexB;

      // Fill the frames with known and zero data resepectivly.
      testFrameA.raw[ index ] = index;
      testFrameB.raw[ index ] = 0;
    }
  }

  // Write the first frame to the write test location on the card and
  // then read it back into the second frame.
  dex_framePut( port, 0x003F, &testFrameA );
  dex_frameGet( port, 0x003F, &testFrameB );

  // Print the contents of the two frames.
  for( indexA = 0; indexA < 16; indexA++ )
  {
    // Print the first frame.
    for( indexB = 0; indexB < 8; indexB++ )
    {
      // Calculate the frame index.
      index = ( indexA * 8 ) + indexB;

      // Print the current location with markers for locations which don't
      // match the other frame.
      printf( "%c", testFrameA.raw[ index ] == testFrameB.raw[ index ] ? ' ' : '[' );
      printf( "%02X", testFrameA.raw[ index ] );
      printf( "%c", testFrameA.raw[ index ] == testFrameB.raw[ index ] ? ' ' : ']' );
    }

    // Print a seperator.
    printf( "    " );

    // Print the second frame.
    for( indexB = 0; indexB < 8; indexB++ )
    {
      // Calculate the frame index.
      index = ( indexA * 8 ) + indexB;

      // Print the current location with markers for locations which don't
      // match the other frame.
      printf( "%c", testFrameA.raw[ index ] == testFrameB.raw[ index ] ? ' ' : '[' );
      printf( "%02X", testFrameB.raw[ index ] );
      printf( "%c", testFrameA.raw[ index ] == testFrameB.raw[ index ] ? ' ' : ']' );
    }

    // Print a seperator.
    printf( "\n" );
  }
}
// }}}
