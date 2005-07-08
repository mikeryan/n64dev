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

#ifndef __DEX_STRING_H
#define __DEX_STRING_H

BOOL dex_stringComp( UINT8 stringA[], UINT8 stringB[], UINT32 length );
void dex_stringCopy( UINT8 stringA[], UINT8 stringB[], UINT32 length );

UINT8* dex_string2ASCII( UINT8 stringA[], UINT8 stringB[], UINT32 length );
UINT8* dex_string2KANJI( UINT8 stringA[], UINT8 stringB[], UINT32 length );

#endif

