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

#ifndef __DEX_FRAME_H
#define __DEX_FRAME_H

#define DEX_FRAME_RETRY            0x0006
#define DEX_FRAME_SLEEP            0x03E8

#define DEX_FRAME_PUT_ERROR        0x0040
#define DEX_FRAME_PUT_SUCCESS      0x0041

#define DEX_FRAME_GET_ERROR        0x0040
#define DEX_FRAME_GET_SUCCESS      0x0041

#define DEX_INDEX_TYPE_INITIAL     0x00000051
#define DEX_INDEX_TYPE_MEDIAL      0x00000052
#define DEX_INDEX_TYPE_FINAL       0x00000053
#define DEX_INDEX_TYPE_FORMATTED   0x000000A0
#define DEX_INDEX_TYPE_DEL_INITIAL 0x000000A1
#define DEX_INDEX_TYPE_DEL_MEDIAL  0x000000A2
#define DEX_INDEX_TYPE_DEL_FINAL   0x000000A3
#define DEX_INDEX_TYPE_INDEX       0x0000434D
#define DEX_INDEX_TYPE_RESERVED    0xFFFFFFFF

#define DEX_INDEX_NEXT_FINAL       0xFFFF

#define DEX_INDEX_TERRITORY_US     0x0000
#define DEX_INDEX_TERRITORY_EU     0x0000
#define DEX_INDEX_TERRITORY_JP     0x0000


typedef struct
{
  UINT32 type;
  UINT32 size;
  UINT16 next;

  UINT8  tCode[ 2  ];
  UINT8  lCode[ 10 ];
  UINT8  sCode[ 8  ];
    
  UINT8  pad[ 97 ];

  UINT8  xor;
}
dex_dataFrame;

typedef struct
{
  unsigned short type;

  UINT8  icon;
  UINT8  used;
    
  UINT8  title[ 64 ];

  UINT8  pad[ 28 ];

  UINT16 palette[ 16 ];
}
dex_gameFrame;

typedef union
{
  UINT8         raw[ 128 ];

  dex_dataFrame data;
  dex_gameFrame game;
}
dex_frame;


UINT8 dex_framePut( UINT32 port, UINT16 address, dex_frame* frame );
UINT8 dex_frameGet( UINT32 port, UINT16 address, dex_frame* frame );

UINT8 dex_bitReverse( UINT8 src );

#endif

