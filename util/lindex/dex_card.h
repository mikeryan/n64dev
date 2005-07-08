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

#ifndef __DEX_CARD_H
#define __DEX_CARD_H

#define DEX_CARD_RETRY        0x0002
#define DEX_CARD_SLEEP        0x03E8

#define DEX_CARD_PUT_ERROR    0x0040
#define DEX_CARD_PUT_SUCCESS  0x0041

#define DEX_CARD_GET_ERROR    0x0040
#define DEX_CARD_GET_SUCCESS  0x0041


typedef struct
{
  dex_block block[ 16 ];
}
dex_card;


UINT8 dex_cardPut( UINT32 port, dex_card* card );
UINT8 dex_cardGet( UINT32 port, dex_card* card );

#endif


