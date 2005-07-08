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

#ifndef __DEX_BLOCK_H
#define __DEX_BLOCK_H

#define DEX_BLOCK_RETRY       0x0004
#define DEX_BLOCK_SLEEP       0x03E8

#define DEX_BLOCK_PUT_ERROR   0x0040
#define DEX_BLOCK_PUT_SUCCESS 0x0041

#define DEX_BLOCK_GET_ERROR   0x0040
#define DEX_BLOCK_GET_SUCCESS 0x0041


typedef struct
{
  dex_frame frame[ 64 ];
}
dex_block;


UINT8 dex_blockPut( UINT32 port, UINT16 address, dex_block* block );
UINT8 dex_blockGet( UINT32 port, UINT16 address, dex_block* block );

#endif

