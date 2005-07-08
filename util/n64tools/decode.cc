// Copyright(C) 1999 Pooka (pooka@cygnus.ucdavis.edu)
// All code written by Pooka.
// You are free to use and distribute this code as long as you include
// the above copyright notice.

// This is the GBI command decoder. 
// I could see this having use in other programs, so here's the source
//  in case you're feeling ambitious.
// Currently, since I'm only interested in triangles and vertices, those
//  are the only commands which get encoded into something useful. The 
//  rest just generate text output, but it would be trivial (yet very time-
//  intensive) to go through and create classes for the rest of the commands. 
// If you add extra capabilities or decoding please send me a copy so
//  I can add it to mine too. Thanks.

extern "C"
{
 #include <stdio.h>
 #include <stdlib.h>
 #include <ultra64.h>
}

#include "global.h"
#include "classes/dlcmd.h"
#include "classes/vtxcmd.h"
#include "classes/tricmd.h"

#define EndOfList	DEF_DecodeError

#define FPFr	if(raw) fprintf

#define EndCase	FPFr(outfp, "),")

#define icase(x)					\
		case (x) :				\
		{					\
		 FPFr(outfp, "%s, ", #x);		\
		 break;					\
		}

#define jcase(x)					\
		case (x) :				\
		{					\
		 FPFr(outfp, "%s", #x);			\
		 break;					\
		}

extern unsigned long	Exclusions;
extern int		bigEndian;


//==-[ Decode() ]-==//

// This routine decodes a GBI command broken up into two 32-bit words.
// inword1 and inword2 are the same as w0 and w1 in the Gfx structure.
// raw tells it whether to output a decoded string to outfp or not.

DLcmd*
Decode(FILE* outfp, unsigned long inword1, unsigned long inword2, int raw)
{
 unsigned char	byte[8];			
 signed char	sb1, sb2, sb3, sb4, sb5, sb6, sb7, sb8;	// signed bytes
 unsigned char	ub1, ub2, ub3, ub4, ub5, ub6, ub7, ub8; // unsigned bytes
 unsigned short hw1, hw2, hw3, hw4;			// halfwords
 unsigned long	w1, w2;					// words

					// make various copies for easy coding

	memcpy(&byte[0], &inword1, sizeof(unsigned long));
	memcpy(&byte[4], &inword2, sizeof(unsigned long));

	memcpy(&w1, &byte[0], sizeof(unsigned long));
	memcpy(&w2, &byte[4], sizeof(unsigned long));

	if(bigEndian)			// flippity-flop
	{
	 memcpy(&hw1, &byte[0], sizeof(unsigned short));
	 memcpy(&hw2, &byte[2], sizeof(unsigned short));
	 memcpy(&hw3, &byte[4], sizeof(unsigned short));
	 memcpy(&hw4, &byte[6], sizeof(unsigned short));
	}
	else
	{
	 memcpy(&hw1, &byte[2], sizeof(unsigned short));
	 memcpy(&hw2, &byte[0], sizeof(unsigned short));
	 memcpy(&hw3, &byte[6], sizeof(unsigned short));
	 memcpy(&hw4, &byte[4], sizeof(unsigned short));
	 FLIPcharDWORD(byte);
	}

	ub1 = byte[0];
	ub2 = byte[1];
	ub3 = byte[2];
	ub4 = byte[3];
	ub5 = byte[4];
	ub6 = byte[5];
	ub7 = byte[6];
	ub8 = byte[7];

	sb1 = (signed char)byte[0];
	sb2 = (signed char)byte[1];
	sb3 = (signed char)byte[2];
	sb4 = (signed char)byte[3];
	sb5 = (signed char)byte[4];
	sb6 = (signed char)byte[5];
	sb7 = (signed char)byte[6];
	sb8 = (signed char)byte[7];

//----------------------------------------------=[ DMA commands ]=------//

	if(sb1 >= 0)		
	{
		FPFr(outfp, "DMA: ");

		switch(sb1)
		{
	  	 case G_SPNOOP :
			FPFr(outfp, "gsSPNoOp(");
			EndCase;
			if(!(Exclusions & ((unsigned long)1L << 1)))
			 return(EndOfList);
			break;

		 case G_MTX :
			FPFr(outfp, "gsSPMatrix(");
				FPFr(outfp, "0x%08lx, ", w2);
				if(ub2 & G_MTX_PROJECTION)
					{FPFr(outfp, "G_MTX_PROJECTION");}
				else
					{FPFr(outfp, "G_MTX_MODELVIEW");}
				if(ub2 & G_MTX_LOAD)
					{FPFr(outfp, " | G_MTX_LOAD");}
				else
					{FPFr(outfp, " | G_MTX_MUL");}
				if(ub2 & G_MTX_PUSH)
					{FPFr(outfp, " | G_MTX_PUSH");}
				else
					{FPFr(outfp, " | G_MTX_NOPUSH");}
				EndCase;
			break;

		 case G_RESERVED0 :	
			FPFr(outfp, "G_RESERVED0 - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 5)))
			 return(EndOfList);
			break;

		 case G_MOVEMEM :
			{
			 unsigned int p, l;
			 unsigned long s;
				p = ub2;
				l = hw2;
				s = w2;
			 if(p == G_MV_VIEWPORT)
			 {
			  FPFr(outfp, "gsSPViewport(");
			  FPFr(outfp, "0x%08lx", s);
			 }
			  
			 else if(p == G_MV_MATRIX_1)
			 {
			  FPFr(outfp, "gsSPForceMatrix(");
			  FPFr(outfp, "0x%08lx", s);
			 }
			  
			 else if(p == G_MV_MATRIX_2)
			 {
			  FPFr(outfp, "gsSPForceMatrix(");
			  FPFr(outfp, "0x%08lx", s-16);
			 }
			  
			 else if(p == G_MV_MATRIX_3)
			 {
			  FPFr(outfp, "gsSPForceMatrix(");
			  FPFr(outfp, "0x%08lx", s-32);
			 }
			  
			 else if(p == G_MV_MATRIX_4)
			 {
			  FPFr(outfp, "gsSPForceMatrix(");
			  FPFr(outfp, "0x%08lx", s-48);
			 }
			  
			 else if( (((p-G_MV_L0)/2)+1 >= 1) &&
				  (((p-G_MV_L0)/2)+1 <= 8) )
			 {
			  unsigned int n;
			  FPFr(outfp, "gsSPLight(");
			  n = ((p-G_MV_L0)/2)+1;
			  FPFr(outfp, "0x%08lx, ", s);
			  switch(n)
			  {
			   jcase(LIGHT_1);
			   jcase(LIGHT_2);
			   jcase(LIGHT_3);
			   jcase(LIGHT_4);
			   jcase(LIGHT_5);
			   jcase(LIGHT_6);
			   jcase(LIGHT_7);
			   jcase(LIGHT_8);
			  }
			 }

			 else if(p == G_MV_LOOKATX)
			 {
			  FPFr(outfp, "gsSPLookAtX(");
			  FPFr(outfp, "0x%08lx", s);
			 }
			  
			 else if(p == G_MV_LOOKATY)
			 {
			  FPFr(outfp, "gsSPLookAtY(");
			  FPFr(outfp, "0x%08lx", s);
			 }
			  
			 else
			 {
			  FPFr(outfp, "gsDma1p(G_MOVEMEM, ");
			  FPFr(outfp, "0x%08lx, ", s);
			  FPFr(outfp, "0x%04x, ", l);
			  FPFr(outfp, "0x%02x", p);
			 }
			 EndCase;
			}
			break;

		 case G_VTX :			
			{
			 unsigned n, v0;
			 unsigned long v;
			 FPFr(outfp, "gsSPVertex(");
				v = w2;
				n = hw2/sizeof(Vtx);
				v0 = ub2 & 0x0f;
				{FPFr(outfp, "0x%08lx, ", v);}
				{FPFr(outfp, "%u, ", n);}
				{FPFr(outfp, "%u", v0);}
			 EndCase;
			 return(new VTXcmd(v, n, v0));
			}
			break;

		 case G_RESERVED1 :
			FPFr(outfp, "G_RESERVED1 - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 6)))
			 return(EndOfList);
			break;

		 case G_DL :
			if(ub2 & G_DL_NOPUSH) 
				{FPFr(outfp, "gsSPBranchList(");}
			else
			{
				FPFr(outfp, "gsSPDisplayList(");
				FPFr(outfp, "0x%08lx", w2);
			}
			EndCase;
			break;

		 case G_RESERVED2 :
			FPFr(outfp, "G_RESERVED2 - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 7)))
			 return(EndOfList);
			break;

		 case G_RESERVED3 :
			FPFr(outfp, "G_RESERVED3 - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 8)))
			 return(EndOfList);
			break;

		 case G_SPRITE2D :
			FPFr(outfp, "gsSPSprite2D(");
				FPFr(outfp, "0x%08lx", w2);
			EndCase;
			break;

		 default:
			FPFr(outfp, "0x%08lx 0x%08lx - Unknown", w1, w2);
			return(EndOfList);
			break;
	 	}
	}
	
//----------------------------------------=[ Immediate commands ]=------//

	else if(sb1 <= (-65))
	{
		FPFr(outfp, "Imm: ");

		switch(sb1)
		{
		 case G_TRI1 :
			{
			 unsigned int v0, v1, v2, flag;
			 FPFr(outfp, "gsSP1Triangle(");
				v0 = ub6/10;
				v1 = ub7/10;
				v2 = ub8/10;
				flag = ub5;
				FPFr(outfp, "%u, ", v0);
				FPFr(outfp, "%u, ", v1);
				FPFr(outfp, "%u, ", v2);
				FPFr(outfp, "%u", flag);
			 EndCase;
			 return(new TRIcmd(v0, v1, v2, flag));
			}
			break;

		 case G_CULLDL :
			{
			 unsigned int vstart, vend;
			 FPFr(outfp, "gsSPCullDisplayList(");
				vstart = hw2/40;
				vend = (hw4/40)-1;
				FPFr(outfp, "%u, ", vstart);
				if(w2 == 0L)
				 {FPFr(outfp, "15");}
				else
				 {FPFr(outfp, "%u", vend);}
			 EndCase;
			}
			break;

		 case G_POPMTX :
			FPFr(outfp, "gsSPPopMatrix(G_MTX_MODELVIEW");
			EndCase;
			break;

		 case G_MOVEWORD :
			{
			 unsigned int index, offset;
			 unsigned long data;
			 index = (w1 & 0x00ffff00L) >> 8;
			 offset = ub4;
			 data = w2;

			 if(index == G_MW_SEGMENT)
			 {
			  FPFr(outfp, "gsSPSegment(");
				FPFr(outfp, "%u, ", offset >> 2);
				FPFr(outfp, "0x%08lx", data);
			 }

			 else if(index == G_MW_CLIP)
			 {
			  FPFr(outfp, "gsMoveWd(G_MW_CLIP, ");
			  switch(offset)
		   	  {
			   icase(G_MWO_CLIP_RNX);
			   icase(G_MWO_CLIP_RNY);
			   icase(G_MWO_CLIP_RPX);
			   icase(G_MWO_CLIP_RPY);
			   default:
				FPFr(outfp, "0x%x, ", offset);
			  }
			  switch(data)
			  {
			   jcase(FR_NEG_FRUSTRATIO_1);
			   jcase(FR_POS_FRUSTRATIO_1);
			   jcase(FR_NEG_FRUSTRATIO_2);
			   jcase(FR_POS_FRUSTRATIO_2);
			   jcase(FR_NEG_FRUSTRATIO_3);
			   jcase(FR_POS_FRUSTRATIO_3);
			   jcase(FR_NEG_FRUSTRATIO_4);
			   jcase(FR_POS_FRUSTRATIO_4);
			   jcase(FR_NEG_FRUSTRATIO_5);
			   jcase(FR_POS_FRUSTRATIO_5);
			   jcase(FR_NEG_FRUSTRATIO_6);
			   jcase(FR_POS_FRUSTRATIO_6);
			   default:
				FPFr(outfp, "0x%08lx", data);
			  }
			 }

			 else if(index == G_MW_MATRIX)
			 {
			  FPFr(outfp, "gsSPInsertMatrix(");
		    	  switch(offset)
			  {
			   icase(G_MWO_MATRIX_XX_XY_I);
			   icase(G_MWO_MATRIX_XZ_XW_I);
			   icase(G_MWO_MATRIX_YX_YY_I);
			   icase(G_MWO_MATRIX_YZ_YW_I);
			   icase(G_MWO_MATRIX_ZX_ZY_I);
			   icase(G_MWO_MATRIX_ZZ_ZW_I);
			   icase(G_MWO_MATRIX_WX_WY_I);
			   icase(G_MWO_MATRIX_WZ_WW_I);
			   icase(G_MWO_MATRIX_XX_XY_F);
			   icase(G_MWO_MATRIX_XZ_XW_F);
			   icase(G_MWO_MATRIX_YX_YY_F);
			   icase(G_MWO_MATRIX_YZ_YW_F);
			   icase(G_MWO_MATRIX_ZX_ZY_F);
			   icase(G_MWO_MATRIX_ZZ_ZW_F);
			   icase(G_MWO_MATRIX_WX_WY_F);
			   icase(G_MWO_MATRIX_WZ_WW_F);
			   default:
				FPFr(outfp, "0x%02x", offset);
			  }
			  FPFr(outfp, "0x%08lx", data);
			 }
	
			 else if(index == G_MW_POINTS)
			 {
			  unsigned int	vtx, where;
			  FPFr(outfp, "gsSPModifyVertex(");
			  where = offset % 40;
			  vtx = offset - where;
			  FPFr(outfp, "%u, ", vtx);
			  switch(where)
			  {
			   icase(G_MWO_POINT_RGBA);
			   icase(G_MWO_POINT_ST);
			   icase(G_MWO_POINT_XYSCREEN);
			   icase(G_MWO_POINT_ZSCREEN);
			   default:
				FPFr(outfp, "%u, ", where);
			  }
			  FPFr(outfp, "0x%08lx", data);
			 }
				
			 else if(index == G_MW_NUMLIGHT)
			 {
			  unsigned int	num;
			  FPFr(outfp, "gsSPNumLights(");
			  num = ((data - 0x80000000L) / 32) - 1;
			  switch(num)
			  {
			   icase(NUMLIGHTS_1);
			   icase(NUMLIGHTS_2);
			   icase(NUMLIGHTS_3);
			   icase(NUMLIGHTS_4);
			   icase(NUMLIGHTS_5);
			   icase(NUMLIGHTS_6);
			   icase(NUMLIGHTS_7);
			   default:
				FPFr(outfp, "%u", num);
			  }
			 }

			 else if(index == G_MW_LIGHTCOL)
			 {
			  FPFr(outfp, "gsMoveWd(G_MW_LIGHTCOL, ");
			  switch(offset)
			  {
			   icase(G_MWO_aLIGHT_1);
			   icase(G_MWO_bLIGHT_1);
			   icase(G_MWO_aLIGHT_2);
			   icase(G_MWO_bLIGHT_2);
			   icase(G_MWO_aLIGHT_3);
			   icase(G_MWO_bLIGHT_3);
			   icase(G_MWO_aLIGHT_4);
			   icase(G_MWO_bLIGHT_4);
			   icase(G_MWO_aLIGHT_5);
			   icase(G_MWO_bLIGHT_5);
			   icase(G_MWO_aLIGHT_6);
			   icase(G_MWO_bLIGHT_6);
			   icase(G_MWO_aLIGHT_7);
			   icase(G_MWO_bLIGHT_7);
			   icase(G_MWO_aLIGHT_8);
			   icase(G_MWO_bLIGHT_8);
			   default:
				FPFr(outfp, "%d, ", offset);
			  }
			  FPFr(outfp, "0x%08lx", data);
			 }

			 else if(index == G_MW_FOG)
			 {
			  FPFr(outfp, "gsMoveWd(G_MW_FOG, G_MWO_FOG, ");
			  FPFr(outfp, "0x%08lx", data);
			 }

			 else if(index == G_MW_PERSPNORM)
			 {
			  FPFr(outfp, "gsSPPerspNormalize(");
			  FPFr(outfp, "0x%08lx", data);
			 }
		
			 else
			 {		 
			  FPFr(outfp, "gsMoveWd(");
				FPFr(outfp, "%u, ", index);
				FPFr(outfp, "%u, ", offset);
				FPFr(outfp, "0x%08lx", data);
			 }
			 EndCase;
			}
			break;

		 case G_TEXTURE :
			{
			 unsigned int s, t, level, tile, on;
			 FPFr(outfp, "gsSPTexture(");
				s = hw3;
				t = hw4;
				level = (ub3 & 0x38)>>3;
				tile = ub3 & 0x07;
				on = ub4;
				FPFr(outfp, "%u, %u, ", s, t);
				FPFr(outfp, "%u, ", level);
				FPFr(outfp, "%u, ", tile);
				if(on & G_ON)
					{FPFr(outfp, "G_ON");}
				else
					{FPFr(outfp, "G_OFF");}
			 EndCase;
			}
			break;

		 case G_SETOTHERMODE_H :
			{
                         unsigned int sft, len;
                         unsigned long data;
                         sft = ub3;
                         len = ub4;
                         data = w2;
                         switch(sft)
                         {
                          case G_MDSFT_PIPELINE :
                             FPFr(outfp, "gsDPPipelineMode(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_CYCLETYPE :
                             FPFr(outfp, "gsDPSetCycleType(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTPERSP :
                             FPFr(outfp, "gsDPSetTexturePersp(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTDETAIL :
                             FPFr(outfp, "gsDPSetTextureDetail(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTLOD :
                             FPFr(outfp, "gsDPSetTextureLOD(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTLUT :
                             FPFr(outfp, "gsDPSetTextureLUT(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTFILT :
                             FPFr(outfp, "gsDPSetTextureFilter(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_TEXTCONV :
                             FPFr(outfp, "gsDPSetTextureConvert(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_COMBKEY :
                             FPFr(outfp, "gsDPSetCombineKey(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_RGBDITHER :
                             FPFr(outfp, "gsDPSetColorDither(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_COLORDITHER :
                             FPFr(outfp, "gsDPSetColorDither(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_ALPHADITHER :
                             FPFr(outfp, "gsDPSetAlphaDither(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          default:
                             FPFr(outfp, "gsSPSetOtherMode(G_SETOTHERMODE_H, ");
                             FPFr(outfp, "0x%02x, ", sft);
                             FPFr(outfp, "0x%02x, ", len);
                             FPFr(outfp, "0x%08lx", data);
                             break;
                         }
                         EndCase;
			}
			break;

		 case G_SETOTHERMODE_L :
			{
                         unsigned int sft, len;
                         unsigned long data;
                                sft = ub3;
                                len = ub4;
                                data = w2;

                         switch(sft)
                         {
                          case G_MDSFT_ALPHACOMPARE :
                             FPFr(outfp, "gsDPSetAlphaCompare(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_ZSRCSEL :
                             FPFr(outfp, "gsDPSetDepthSource(");
                             FPFr(outfp, "0x%08lx", data);
                             break;
                          case G_MDSFT_RENDERMODE :
                             FPFr(outfp, "gsDPSetRenderMode(");
                             FPFr(outfp, "0x%08lx, 0x%08lx", data, data);
                             break;
                          default:
                             FPFr(outfp, "gsSPSetOtherMode(G_SETOTHERMODE_L, ");
                             FPFr(outfp, "0x%02x, ", sft);
                             FPFr(outfp, "0x%02x, ", len);
                             FPFr(outfp, "0x%08lx", data);
                             break;
                         }
                         EndCase;
			}
			break;

		 case G_ENDDL :
			FPFr(outfp, "gsSPEndDisplayList(");
			EndCase;
			if(!(Exclusions & ((unsigned long)1L << 9)))
			 return(EndOfList);
			break;

		 case G_SETGEOMETRYMODE :
			FPFr(outfp, "gsSPSetGeometryMode( 0 ");
				if(w2 & G_ZBUFFER)
					{FPFr(outfp, "| G_ZBUFFER ");}
				if(w2 & G_SHADE)
					{FPFr(outfp, "| G_SHADE ");}
				if(w2 & G_SHADING_SMOOTH)
					{FPFr(outfp, "| G_SHADING_SMOOTH ");}
				if(w2 & G_CULL_BOTH)
					{FPFr(outfp, "| G_CULL_BOTH ");}
				else if(w2 & G_CULL_FRONT)
					{FPFr(outfp, "| G_CULL_FRONT ");}
				else if(w2 & G_CULL_BACK)
					{FPFr(outfp, "| G_CULL_BACK ");}
				if(w2 & G_FOG)
					{FPFr(outfp, "| G_FOG ");}
				if(w2 & G_LIGHTING)
					{FPFr(outfp, "| G_LIGHTING ");}
				if(w2 & G_TEXTURE_GEN)
					{FPFr(outfp, "| G_TEXTURE_GEN ");}
				if(w2 & G_TEXTURE_GEN_LINEAR)
					{FPFr(outfp,"| G_TEXTURE_GEN_LINEAR ");}
			EndCase;
			break;

		 case G_CLEARGEOMETRYMODE :
			FPFr(outfp, "gsSPClearGeometryMode( 0 ");
				if(w2 & G_ZBUFFER)
					{FPFr(outfp, "| G_ZBUFFER ");}
				if(w2 & G_SHADE)
					{FPFr(outfp, "| G_SHADE ");}
				if(w2 & G_SHADING_SMOOTH)
					{FPFr(outfp, "| G_SHADING_SMOOTH ");}
				if(w2 & G_CULL_BOTH)
					{FPFr(outfp, "| G_CULL_BOTH ");}
				else if(w2 & G_CULL_FRONT)
					{FPFr(outfp, "| G_CULL_FRONT ");}
				else if(w2 & G_CULL_BACK)
					{FPFr(outfp, "| G_CULL_BACK ");}
				if(w2 & G_FOG)
					{FPFr(outfp, "| G_FOG ");}
				if(w2 & G_LIGHTING)
					{FPFr(outfp, "| G_LIGHTING ");}
				if(w2 & G_TEXTURE_GEN)
					{FPFr(outfp, "| G_TEXTURE_GEN ");}
				if(w2 & G_TEXTURE_GEN_LINEAR)
					{FPFr(outfp,"| G_TEXTURE_GEN_LINEAR ");}
			EndCase;
			break;

		 case G_LINE3D :
			{
			 unsigned int v0, v1, flag;
			 FPFr(outfp, "gsSPLine3D(");
				v0 = ub6/10;
				v1 = ub7/10;
				flag = ub5;
				FPFr(outfp, "%u, ", v0);
				FPFr(outfp, "%u, ", v1);
				FPFr(outfp, "%u", flag);
			 EndCase;
			}
			break;

		 case G_RDPHALF_1 :
			FPFr(outfp, "gsImmp1(G_RDPHALF_1, ");
				FPFr(outfp, "0x%08lx", w2);
			EndCase;
			break;

		 case G_RDPHALF_2 :
			FPFr(outfp, "gsImmp1(G_RDPHALF_2, ");
				FPFr(outfp, "0x%08lx", w2);
			EndCase;
			break;

		 case G_RDPHALF_CONT :
			FPFr(outfp, "G_RDPHALF_CONT - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 4)))
			 return(EndOfList);
			break;

	 	 default :
			FPFr(outfp, "0x%08lx 0x%08lx - Unknown", w1, w2);
			return(EndOfList);
			break;
		}
	}

//-----------------------------------------------=[ RDP commands ]=------//

	else if( (sb1 < 0) && (sb1 > (-65)) )	
	{
		FPFr(outfp, "RDP: ");

		switch(sb1)
		{
		 case (signed char)G_NOOP :
			FPFr(outfp, "gsDPNoOp(");
			EndCase;
			if(!(Exclusions & ((unsigned long)1L << 2)))
			 return(EndOfList);
			break;

		 case (signed char)G_SETCIMG :
			{
			 unsigned int fmt, siz;
			 FPFr(outfp, "gsDPSetColorImage(");
				fmt = (ub2 & 0xe0) >> 5;
				siz = (ub2 & 0x18) >> 3;
				switch(fmt)
				{
				 icase(G_IM_FMT_RGBA);
				 icase(G_IM_FMT_YUV);
				 icase(G_IM_FMT_CI);
				 icase(G_IM_FMT_IA);
				 icase(G_IM_FMT_I);
				}
				switch(siz)
				{
				 icase(G_IM_SIZ_4b);
				 icase(G_IM_SIZ_8b);
				 icase(G_IM_SIZ_16b);
				 icase(G_IM_SIZ_32b);
				}
				FPFr(outfp, "%u, ", (hw2 & 0x0fff)+1);
				FPFr(outfp, "0x%08lx", w2);
			 EndCase;
			}
			break;

		 case (signed char)G_SETZIMG :
			FPFr(outfp, "gsDPSetDepthImage(");
				FPFr(outfp, "0x%08lx", w2);
			EndCase;
			break;

		 case (signed char)G_SETTIMG :
			{
			 unsigned int fmt, siz;
			 FPFr(outfp, "gsDPSetTextureImage(");
				fmt = (ub2 & 0xe0) >> 5;
				siz = (ub2 & 0x18) >> 3;
				switch(fmt)
				{
				 icase(G_IM_FMT_RGBA);
				 icase(G_IM_FMT_YUV);
				 icase(G_IM_FMT_CI);
				 icase(G_IM_FMT_IA);
				 icase(G_IM_FMT_I);
				}
				switch(siz)
				{
				 icase(G_IM_SIZ_4b);
				 icase(G_IM_SIZ_8b);
				 icase(G_IM_SIZ_16b);
				 icase(G_IM_SIZ_32b);
				}
				FPFr(outfp, "%u, ", (hw2 & 0x0fff)+1);
				FPFr(outfp, "0x%08lx", w2);
			 EndCase;
			}
			break;

		 case (signed char)G_SETCOMBINE :	// parsing this one
			{				// would be painful
			 unsigned long muxs0, muxs1;
			 FPFr(outfp, "gsDPSetCombineMode(");
				muxs0 = w1 & 0x00ffffffL;
				muxs1 = w2;
				FPFr(outfp, "0x%08lx, 0x%08lx", muxs0, muxs1);
			 EndCase;
			}
			break;

		 case (signed char)G_SETENVCOLOR :
			{
			 unsigned int r, g, b, a;
			 FPFr(outfp, "gsDPSetEnvColor(");
				r = ub5;
				g = ub6;
				b = ub7;
				a = ub8;
				FPFr(outfp, "%u, ", r);
				FPFr(outfp, "%u, ", g);
				FPFr(outfp, "%u, ", b);
				FPFr(outfp, "%u", a);
			 EndCase;
			}
			break;

		 case (signed char)G_SETPRIMCOLOR :
			{
			 unsigned int m, l, r, g, b, a;
			 FPFr(outfp, "gsDPSetPrimColor(");
				m = ub3;
				l = ub4;
				r = ub5;
				g = ub6;
				b = ub7;
				a = ub8;
				FPFr(outfp, "%u, ", m);
				FPFr(outfp, "%u, ", l);
				FPFr(outfp, "%u, ", r);
				FPFr(outfp, "%u, ", g);
				FPFr(outfp, "%u, ", b);
				FPFr(outfp, "%u", a);
			 EndCase;
			}
			break;

		 case (signed char)G_SETBLENDCOLOR :
			{
			 unsigned int r, g, b, a;
			 FPFr(outfp, "gsDPSetBlendColor(");
				r = ub5;
				g = ub6;
				b = ub7;
				a = ub8;
				FPFr(outfp, "%u, ", r);
				FPFr(outfp, "%u, ", g);
				FPFr(outfp, "%u, ", b);
				FPFr(outfp, "%u", a);
			 EndCase;
			}
			break;

		 case (signed char)G_SETFOGCOLOR :
			{
			 unsigned int r, g, b, a;
			 FPFr(outfp, "gsDPSetFogColor(");
				r = ub5;
				g = ub6;
				b = ub7;
				a = ub8;
				FPFr(outfp, "%u, ", r);
				FPFr(outfp, "%u, ", g);
				FPFr(outfp, "%u, ", b);
				FPFr(outfp, "%u", a);
			 EndCase;
			}
			break;

		 case (signed char)G_SETFILLCOLOR :
			FPFr(outfp, "gsDPSetFillColor(");
				FPFr(outfp, "0x%08lx", w2);
			EndCase;
			break;

		 case (signed char)G_FILLRECT :
			{
			 unsigned int lrx, lry, ulx, uly;
			 FPFr(outfp, "gsDPFillRectangle(");
				lrx = w1 & 0x00ffe000L >> 14;
				lry = w1 & 0x00000ffcL >> 2;
				ulx = w2 & 0x00ffe000L >> 14;
				uly = w2 & 0x00000ffcL >> 2;
				FPFr(outfp, "%u, ", ulx);
				FPFr(outfp, "%u, ", uly);
				FPFr(outfp, "%u, ", lrx);
				FPFr(outfp, "%u", lrx);
			 EndCase;
			}
			break;

		 case (signed char)G_SETTILE :
			{
			 unsigned int fmt, siz, line, tmem;
			 unsigned int tile, palette, cmt, maskt, shiftt;
			 unsigned int cms, masks, shifts;
			 FPFr(outfp, "gsDPSetTile(");
				fmt = (hw1 & 0x00e0) >> 5;
				siz = (hw1 & 0x0018) >> 3;
				line = (w1 & 0x0003fe00L) >> 9;
				tmem = w1 & 0x000001ffL;
				tile = ub5 & 0x07;
				palette = ub6 & 0xf0;
				cmt = ub6 & 0x0c;
				maskt = (w2 & 0x0003c000L) >> 14;
				shiftt = (hw4 & 0x3c00) >> 10;
				cms = (hw4 & 0x0300) >> 8;
				masks = (ub8 & 0xf0) >> 4;
				shifts = ub8 & 0x0f;
				switch(fmt)
				{
				 icase(G_IM_FMT_RGBA);
				 icase(G_IM_FMT_YUV);
				 icase(G_IM_FMT_CI);
				 icase(G_IM_FMT_IA);
				 icase(G_IM_FMT_I);
				}
				switch(siz)
				{
				 icase(G_IM_SIZ_4b);
				 icase(G_IM_SIZ_8b);
				 icase(G_IM_SIZ_16b);
				 icase(G_IM_SIZ_32b);
				}
				FPFr(outfp, "%u, ", line);
				FPFr(outfp, "%u, ", tmem);
				FPFr(outfp, "%u, ", tile);
				FPFr(outfp, "%u, ", palette);
				FPFr(outfp, " 0 ");
				if(cmt & G_TX_MIRROR)
					{FPFr(outfp, "| G_TX_MIRROR ");}
				else
					{FPFr(outfp, "| G_TX_NOMIRROR ");}
				if(cmt & G_TX_CLAMP)
					{FPFr(outfp, "| G_TX_CLAMP,");}
				else
					{FPFr(outfp, "| G_TX_WRAP,");}
				if(maskt == 0)
					{FPFr(outfp, "G_TX_NOMASK, ");}
				else
					{FPFr(outfp, "%u, ", maskt);}
				if(shiftt == 0)
					{FPFr(outfp, "G_TX_NOLOD, ");}
				else
					{FPFr(outfp, "%u, ", shiftt);}
				FPFr(outfp, " 0 ");
				if(cms & G_TX_MIRROR)
					{FPFr(outfp, "| G_TX_MIRROR ");}
				else
					{FPFr(outfp, "| G_TX_NOMIRROR ");}
				if(cms & G_TX_CLAMP)
					{FPFr(outfp, "| G_TX_CLAMP,");}
				else
					{FPFr(outfp, "| G_TX_WRAP,");}
				if(masks == 0)
					{FPFr(outfp, "G_TX_NOMASK, ");}
				else
					{FPFr(outfp, "%u, ", masks);}
				if(shifts == 0)
					{FPFr(outfp, "G_TX_NOLOD");}
				else
					{FPFr(outfp, "%u", shifts);}
			 EndCase;
			}
			break;

		 case (signed char)G_LOADTILE :
			{
			 unsigned int tile, uls, ult, lrs, lrt;
			 FPFr(outfp, "gsDPLoadTile(");
				tile = ub5 & 0x07;
				uls = (w1 & 0x00fff000L) >> 12;
				ult = w1 & 0x00000fffL;
				lrs = (w2 & 0x00fff000L) >> 12;
				lrt = w2 & 0x00000fffL;
				FPFr(outfp, "%u, ", tile);
				FPFr(outfp, "%u, ", uls);
				FPFr(outfp, "%u, ", ult);
				FPFr(outfp, "%u, ", lrs);
				FPFr(outfp, "%u", lrt);
			 EndCase;
			}
			break;

		 case (signed char)G_LOADBLOCK :
			{
			 unsigned int tile, uls, ult, lrs, dxt;
			 FPFr(outfp, "gsDPLoadBlock(");
				tile = ub5 & 0x07;
				uls = (w1 & 0x00fff000L) >> 12;
				ult = w1 & 0x00000fffL;
				lrs = (w2 & 0x00fff000L) >> 12;
				dxt = w2 & 0x00000fffL;
				FPFr(outfp, "%u, ", tile);
				FPFr(outfp, "%u, ", uls);
				FPFr(outfp, "%u, ", ult);
				FPFr(outfp, "%u, ", lrs);
				FPFr(outfp, "%u", dxt);
			 EndCase;
			}
			break;

		 case (signed char)G_SETTILESIZE :
			{
			 unsigned int tile, uls, ult, lrs, lrt;
			 FPFr(outfp, "gsDPSetTileSize(");
				tile = ub5 & 0x07;
				uls = (w1 & 0x00fff000L) >> 12;
				ult = w1 & 0x00000fffL;
				lrs = (w2 & 0x00fff000L) >> 12;
				lrt = w2 & 0x00000fffL;
				FPFr(outfp, "%u, ", tile);
				FPFr(outfp, "%u, ", uls);
				FPFr(outfp, "%u, ", ult);
				FPFr(outfp, "%u, ", lrs);
				FPFr(outfp, "%u", lrt);
			 EndCase;
			}
			break;

		 case (signed char)G_LOADTLUT :
			{
			 unsigned int tile, count;
			 FPFr(outfp, "gsDPLoadTLUTCmd(");
				tile = ub5 & 0x07;
				count = (w2 & 0x00fffc00L) >> 10;
				FPFr(outfp, "%u, ", tile);
				FPFr(outfp, "%u", count);
			 EndCase;
			}
			break;

		 case (signed char)G_RDPSETOTHERMODE :
			FPFr(outfp, "G_RDPSETOTHERMODE - Not Implemented");
			if(!(Exclusions & ((unsigned long)1L << 3)))
			 return(EndOfList);
			break;

		 case (signed char)G_SETPRIMDEPTH :
			{
			 unsigned int z, dz;
			 FPFr(outfp, "gsDPSetPrimDepth(");
				z = (w2 & 0xffff0000L) >> 16;
				dz = w2 & 0x0000ffffL;
				FPFr(outfp, "%u, ", z);
				FPFr(outfp, "%u", dz);
			 EndCase;
			}
			break;

		 case (signed char)G_SETSCISSOR :
			{
			 unsigned int mode, ulx, uly, lrx, lry;
			 FPFr(outfp, "gsDPSetScissor(");
				mode = ub5 & 0x03;
				ulx = ((w1 & 0x00fff000L) >> 12) >> 2;
				uly = (w1 & 0x00000fffL) >> 2;
				lrx = ((w2 & 0x00fff000L) >> 12) >> 2;
				lry = (w2 & 0x00000fffL) >> 2;
				if(mode == G_SC_NON_INTERLACE)
					{FPFr(outfp, "G_SC_NON_INTERLACE, ");}
				else if(mode == G_SC_ODD_INTERLACE)
					{FPFr(outfp, "G_SC_ODD_INTERLACE, ");}
				else if(mode == G_SC_EVEN_INTERLACE)
					{FPFr(outfp, "G_SC_EVEN_INTERLACE, ");}
				else
					{FPFr(outfp, "%u, ", mode);}
				FPFr(outfp, "%u, ", ulx);
				FPFr(outfp, "%u, ", uly);
				FPFr(outfp, "%u, ", lrx);
				FPFr(outfp, "%u", lry);
			 EndCase;
			}
			break;

		 case (signed char)G_SETCONVERT :
			{
			 int k0, k1, k2, k3, k4, k5;
			 FPFr(outfp, "gsDPSetConvert(");
				k0 = (w1 & 0x003fe000L) >> 13;
				k1 = (hw2 & 0x1ff0) >> 4;
				k2 = (hw2 & 0x01e0) | ((hw3 & 0xf800)>>11);
				k3 = (hw3 & 0x07fc) >> 2;
				k4 = (w2 & 0x0003fe00L) >> 9;
				k5 = w2 & 0x000001ffL;
				FPFr(outfp, "%d, ", k0);
				FPFr(outfp, "%d, ", k1);
				FPFr(outfp, "%d, ", k2);
				FPFr(outfp, "%d, ", k3);
				FPFr(outfp, "%d, ", k4);
				FPFr(outfp, "%d", k5);
			 EndCase;
			}
			break;

		 case (signed char)G_SETKEYR :
			{
			 unsigned int cR, sR, wR;
			 FPFr(outfp, "gsDPSetKeyR(");
				cR = (hw4 & 0xff00) >> 8;
				sR = ub8;
				wR = hw3 & 0x0fff;
				FPFr(outfp, "%u, ", cR);
				FPFr(outfp, "%u, ", sR);
				FPFr(outfp, "%u", wR);
			 EndCase;
			}
			break;

		 case (signed char)G_SETKEYGB :
			{
			 unsigned int cG, sG, wG, cB, sB, wB;
			 FPFr(outfp, "gsDPSetKeyGB(");
				cG = ub5;
				sG = ub6;
				wG = (w1 & 0x00fff000L) >> 12;
				cB = ub7;
				sB = ub8;
				wB = hw2 & 0x0fff;
				FPFr(outfp, "%u, ", cG);
				FPFr(outfp, "%u, ", sG);
				FPFr(outfp, "%u, ", wG);
				FPFr(outfp, "%u, ", cB);
				FPFr(outfp, "%u, ", sB);
				FPFr(outfp, "%u", wB);
			 EndCase;
			}
			break;

		 case (signed char)G_RDPFULLSYNC :
			FPFr(outfp, "gsDPFullSync(");
			EndCase;
			break;

		 case (signed char)G_RDPTILESYNC :
			FPFr(outfp, "gsDPTileSync(");
			EndCase;
			break;

		 case (signed char)G_RDPPIPESYNC :
			FPFr(outfp, "gsDPPipeSync(");
			EndCase;
			break;

		 case (signed char)G_RDPLOADSYNC :
			FPFr(outfp, "gsDPLoadSync(");
			EndCase;
			break;

		 case (signed char)G_TEXRECTFLIP :
			FPFr(outfp, "gsDPTextureRectangleFlip(");
			EndCase;
			break;

		 case (signed char)G_TEXRECT :
			FPFr(outfp, "gsDPTextureRectangle(");
			EndCase;
			break;

		 default:
			FPFr(outfp, "0x%08lx 0x%08lx - Unknown", w1, w2);
			return(EndOfList);
			break;
		}
	}
				
	return(NULL);
}


