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

#ifndef __LINDEX_FORMAT_GME_H
#define __LINDEX_FORMAT_GME_H

typedef struct
{
  UINT8 magic[ 16 ];

  UINT16 pad0;
  UINT16 pad1;
  UINT8  pad2;

  UINT8  type[ 16 ];
  UINT8  pad3;
  UINT8  next[ 16 ];
  UINT8  pad4;

  UINT8  pad5[ 9 ];
}
gme_head;

typedef struct
{
  UINT8  name[ 256 ];
}
gme_desc;

typedef struct
{
  gme_head head;
  gme_desc desc[ 15 ];

  dex_card card;
}
gme_file;

void lindex_gmeLoadCard( UINT32 port, FILE* file );
void lindex_gmeSaveCard( UINT32 port, FILE* file );

#endif


