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

#ifndef __DEX_UTIL_H
#define __DEX_UTIL_H

#define DEX_DISCOVER_ERROR     0x0021
#define DEX_DISCOVER_SUCCESS   0x0024

#define DEX_POWER_OFF          0x0000
#define DEX_POWER_ON           0x0001

#define DEX_POWER_ERROR        0x0021
#define DEX_POWER_NONE         0x0022
#define DEX_POWER_VALID        0x0023

#define DEX_CHECK_ERROR        0x0021
#define DEX_CHECK_NONE         0x0022
#define DEX_CHECK_VALID        0x0023


UINT8 dex_utilDiscover( UINT32 port );

UINT8 dex_utilPower( UINT32 port, UINT8 power );
UINT8 dex_utilCheck( UINT32 port );

#endif



