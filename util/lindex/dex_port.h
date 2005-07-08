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

#ifndef __UINT32_H
#define __UINT32_H

#define DEX_PORT_RETRY        0x0008
#define DEX_PORT_SLEEP        0x03E8

#define DEX_PORT_OPEN_ERROR   0x0000
#define DEX_PORT_OPEN_SUCCESS 0x0001


UINT32 dex_portOpen( UINT8 device[]  );

UINT32 dex_portClose( UINT32 port );

UINT32 dex_portIFlush( UINT32 port );
UINT32 dex_portOFlush( UINT32 port );

UINT32 dex_portPut( UINT32 port, UINT8 message[], UINT32 length );
UINT32 dex_portGet( UINT32 port, UINT8 message[], UINT32 length );

#endif


