; From Destop's Heavy 64 demo and gbi.h

G_SPNOOP               equne  0               ;/* handle 0 gracefully */
G_MTX                  equne  1
G_RESERVED0            equne  2               ;/* not implemeted */
G_MOVEMEM              equne  3               ;/* move a block of memory (up to 4 words) to dmem */
G_VTX                  equne  4
G_RESERVED1            equne  5               ;/* not implemeted */
G_DL                   equne  6
G_RESERVED2            equne  7               ;/* not implemeted */
G_RESERVED3            equne  8               ;/* not implemeted */
G_SPRITE2D             equne  9               ;/* sprite command */
G_IMMFIRST             equne  $BF
G_TRI1                 equne  $BF
G_CULLDL               equne  $BE
G_POPMTX               equne  $BD
G_MOVEWORD             equne  $BC
G_TEXTURE              equne  $BB
G_SETOTHERMODE_H       equne  $BA
G_SETOTHERMODE_L       equne  $B9
G_ENDDL                equne  $B8
G_SETGEOMETRYMODE      equne  $B7
G_CLEARGEOMETRYMODE    equne  $B6
G_LINE3D               equne  $B5
G_RDPHALF_1            equne  $B4
G_RDPHALF_2            equne  $B3
G_RDPHALF_CONT         equne  $B2
G_NOOP                 equne  0xc0            ;/*   0 */
G_SETCIMG              equne  0xff            ;/*  -1 */
G_SETZIMG              equne  0xfe            ;/*  -2 */
G_SETTIMG              equne  0xfd            ;/*  -3 */
G_SETCOMBINE           equne  0xfc            ;/*  -4 */
G_SETENVCOLOR          equne  0xfb            ;/*  -5 */
G_SETPRIMCOLOR         equne  0xfa            ;/*  -6 */
G_SETBLENDCOLOR        equne  0xf9            ;/*  -7 */
G_SETFOGCOLOR          equne  0xf8            ;/*  -8 */
G_SETFILLCOLOR         equne  0xf7            ;/*  -9 */
G_FILLRECT             equne  0xf6            ;/* -10 */
G_SETTILE              equne  0xf5            ;/* -11 */
G_LOADTILE             equne  0xf4            ;/* -12 */
G_LOADBLOCK            equne  0xf3            ;/* -13 */
G_SETTILESIZE          equne  0xf2            ;/* -14 */
G_LOADTLUT             equne  0xf0            ;/* -16 */
G_RDPSETOTHERMODE      equne  0xef            ;/* -17 */
G_SETPRIMDEPTH         equne  0xee            ;/* -18 */
G_SETSCISSOR           equne  0xed            ;/* -19 */
G_SETCONVERT           equne  0xec            ;/* -20 */
G_SETKEYR              equne  0xeb            ;/* -21 */
G_SETKEYGB             equne  0xea            ;/* -22 */
G_RDPFULLSYNC          equne  0xe9            ;/* -23 */
G_RDPTILESYNC          equne  0xe8            ;/* -24 */
G_RDPPIPESYNC          equne  0xe7            ;/* -25 */
G_RDPLOADSYNC          equne  0xe6            ;/* -26 */
G_TEXRECTFLIP          equne  0xe5            ;/* -27 */
G_TEXRECT              equne  0xe4            ;/* -28 */
G_TRI_FILL             equne  0xc8            ;/* fill triangle:            11001000 :32  bytes!!*/
G_TRI_FILL_ZBUFF       equne  0xc9            ;/* fill, zbuff triangle:     11001001 :40 bytes!!*/
G_TRI_TXTR             equne  0xca            ;/* texture triangle:         11001010 :136 bytes!!*/
G_TRI_TXTR_ZBUFF       equne  0xcb            ;/* texture, zbuff triangle:  11001011 :136 bytes!!*/
G_TRI_SHADE            equne  0xcc            ;/* shade triangle:           11001100 :136 bytes!!*/
G_TRI_SHADE_ZBUFF      equne  0xcd            ;/* shade, zbuff triangle:    11001101 :112 bytes!!*/
G_TRI_SHADE_TXTR       equne  0xce            ;/* shade, texture triangle:  11001110 :176 bytes!!*/
G_TRI_SHADE_TXTR_ZBUFF equne  0xcf            ;/* shade, txtr, zbuff trngl: 11001111 :176 bytes!!*/
G_RDP_TRI_FILL_MASK    equne  0x08
G_RDP_TRI_SHADE_MASK   equne  0x04
G_RDP_TRI_TXTR_MASK    equne  0x02
G_RDP_TRI_ZBUFF_MASK   equne  0x01
G_TEXTURE_IMAGE_FRAC   equne  2
G_TEXTURE_SCALE_FRAC   equne  16
G_ON                   equne  1
G_OFF                  equne  0
G_DL_PUSH              equne 0x00
G_DL_NOPUSH            equne 0x01

G_SCALE_FRAC           equne  8
G_ROTATE_FRAC          equne  16
G_MTX_MODELVIEW        equne  0x00            ;/* matrix types */
G_MTX_PROJECTION       equne  0x01
G_MTX_MUL              equne  0x00            ;/* concat or load */
G_MTX_LOAD             equne  0x02
G_MTX_NOPUSH           equne  0x00            ;/* push or not */
G_MTX_PUSH             equne  0x04
G_MAXFBZ               equne  0x3fff          ;/* 3b exp, 11b mantissa */
G_ZBUFFER              equne  0x00000001
G_TEXTURE_ENABLE       equne  0x00000002      ;/* Microcode use only */
G_SHADE                equne  0x00000004      ;/* enable Gouraud interp */
G_SHADING_SMOOTH       equne  0x00000200      ;/* flat or smooth shaded */
G_CULL_FRONT           equne  0x00001000
G_CULL_BACK            equne  0x00002000
G_CULL_BOTH            equne  0x00003000      ;/* To make code cleaner */
G_FOG                  equne  0x00010000
G_LIGHTING             equne  0x00020000
G_TEXTURE_GEN          equne  0x00040000
G_TEXTURE_GEN_LINEAR   equne  0x00080000
G_LOD                  equne  0x00100000      ;/* NOT IMPLEMENTED */
G_FOG_H                equne  (G_FOG/0x10000)
G_LIGHTING_H           equne  (G_LIGHTING/0x10000)
G_TEXTURE_GEN_H        equne  (G_TEXTURE_GEN/0x10000)
G_TEXTURE_GEN_LINEAR_H equne  (G_TEXTURE_GEN_LINEAR/0x10000)
G_LOD_H                equne  (G_LOD/0x10000) ;/* NOT IMPLEMENTED */
G_TX_LOADTILE          equne  7
G_TX_RENDERTILE        equne  0
G_TX_NOMIRROR          equne  0
G_TX_WRAP              equne  0
G_TX_MIRROR            equne  0x1
G_TX_CLAMP             equne  0x2
G_TX_NOMASK            equne  0
G_TX_NOLOD             equne  0
G_TX_DXT_FRAC          equne  11
G_IM_FMT_RGBA          equne  0
G_IM_FMT_YUV           equne 1
G_IM_FMT_CI            equne 2    ; color indexed
G_IM_FMT_IA            equne 3    ; intensity alpha
G_IM_FMT_I             equne 4    ; intensity
G_IM_SIZ_4b            equne 0
G_IM_SIZ_8b            equne 1
G_IM_SIZ_16b           equne 2
G_IM_SIZ_32b           equne 3

G_IM_SIZ_4b_BYTES       equne 1 ;0
G_IM_SIZ_4b_TILE_BYTES  equne G_IM_SIZ_4b_BYTES
G_IM_SIZ_4b_LINE_BYTES  equne G_IM_SIZ_4b_BYTES

G_IM_SIZ_8b_BYTES       equne 1
G_IM_SIZ_8b_TILE_BYTES  equne G_IM_SIZ_8b_BYTES
G_IM_SIZ_8b_LINE_BYTES  equne G_IM_SIZ_8b_BYTES

G_IM_SIZ_16b_BYTES       equne  2
G_IM_SIZ_16b_TILE_BYTES  equne G_IM_SIZ_16b_BYTES
G_IM_SIZ_16b_LINE_BYTES  equne G_IM_SIZ_16b_BYTES

G_IM_SIZ_32b_BYTES       equne 4
G_IM_SIZ_32b_TILE_BYTES  equne 2
G_IM_SIZ_32b_LINE_BYTES  equne 2

G_IM_SIZ_4b_LOAD_BLOCK  equne G_IM_SIZ_16b
G_IM_SIZ_8b_LOAD_BLOCK  equne G_IM_SIZ_16b
G_IM_SIZ_16b_LOAD_BLOCK equne G_IM_SIZ_16b
G_IM_SIZ_32b_LOAD_BLOCK equne G_IM_SIZ_32b

G_IM_SIZ_4b_SHIFT   equne 2
G_IM_SIZ_8b_SHIFT   equne 1
G_IM_SIZ_16b_SHIFT  equne 0
G_IM_SIZ_32b_SHIFT  equne 0

G_IM_SIZ_4b_INCR    equne 3
G_IM_SIZ_8b_INCR    equne 1
G_IM_SIZ_16b_INCR   equne 0
G_IM_SIZ_32b_INCR   equne 0

G_CCMUX_COMBINED       equne  0
G_CCMUX_TEXEL0         equne  1
G_CCMUX_TEXEL1         equne  2
G_CCMUX_PRIMITIVE      equne  3
G_CCMUX_SHADE          equne  4
G_CCMUX_ENVIRONMENT    equne  5
G_CCMUX_CENTER         equne  6
G_CCMUX_SCALE          equne  6
G_CCMUX_COMBINED_ALPHA equne  7
G_CCMUX_TEXEL0_ALPHA   equne  8
G_CCMUX_TEXEL1_ALPHA   equne  9
G_CCMUX_PRIMITIVE_ALPHA equne  10
G_CCMUX_SHADE_ALPHA    equne  11
G_CCMUX_ENV_ALPHA      equne  12
G_CCMUX_LOD_FRACTION   equne  13
G_CCMUX_PRIM_LOD_FRAC  equne  14
G_CCMUX_NOISE          equne  7
G_CCMUX_K4             equne  7
G_CCMUX_K5             equne  15
G_CCMUX_1              equne  6
G_CCMUX_0              equne  31
G_ACMUX_COMBINED       equne  0
G_ACMUX_TEXEL0         equne  1
G_ACMUX_TEXEL1         equne  2
G_ACMUX_PRIMITIVE      equne  3
G_ACMUX_SHADE          equne  4
G_ACMUX_ENVIRONMENT    equne  5
G_ACMUX_LOD_FRACTION   equne  0
G_ACMUX_PRIM_LOD_FRAC  equne  6
G_ACMUX_1              equne  6
G_ACMUX_0              equne  7
G_MDSFT_ALPHACOMPARE   equne  0
G_MDSFT_ZSRCSEL        equne  2
G_MDSFT_RENDERMODE     equne  3
G_MDSFT_BLENDER        equne  16
G_MDSFT_BLENDMASK      equne  0               ;/* unsupported */
G_MDSFT_ALPHADITHER    equne  4
G_MDSFT_RGBDITHER      equne  6
G_MDSFT_COMBKEY        equne  8
G_MDSFT_TEXTCONV       equne  9
G_MDSFT_TEXTFILT       equne  12
G_MDSFT_TEXTLUT        equne  14
G_MDSFT_TEXTLOD        equne  16
G_MDSFT_TEXTDETAIL     equne  17
G_MDSFT_TEXTPERSP      equne  19
G_MDSFT_CYCLETYPE      equne  20
G_MDSFT_COLORDITHER    equne  22              ;/* unsupported in HW 2.0 */
G_MDSFT_PIPELINE       equne  23
G_PM_1PRIMITIVE        equne  (1<G_MDSFT_PIPELINE)
G_PM_NPRIMITIVE        equne  (0<G_MDSFT_PIPELINE)
G_CYC_1CYCLE           equne  (0<G_MDSFT_CYCLETYPE)
G_CYC_2CYCLE           equne  (1<G_MDSFT_CYCLETYPE)
G_CYC_COPY             equne  (2<G_MDSFT_CYCLETYPE)
G_CYC_FILL             equne  (3<G_MDSFT_CYCLETYPE)
G_TP_NONE              equne  (0<G_MDSFT_TEXTPERSP)
G_TP_PERSP             equne  (1<G_MDSFT_TEXTPERSP)
G_TD_CLAMP             equne  (0<G_MDSFT_TEXTDETAIL)
G_TD_SHARPEN           equne  (1<G_MDSFT_TEXTDETAIL)
G_TD_DETAIL            equne  (2<G_MDSFT_TEXTDETAIL)
G_TL_TILE              equne  (0<G_MDSFT_TEXTLOD)
G_TL_LOD               equne  (1<G_MDSFT_TEXTLOD)
G_TT_NONE              equne  (0<G_MDSFT_TEXTLUT)
G_TT_RGBA16            equne  (2<G_MDSFT_TEXTLUT)
G_TT_IA16              equne  (3<G_MDSFT_TEXTLUT)
G_TF_POINT             equne  (0<G_MDSFT_TEXTFILT)
G_TF_AVERAGE           equne  (3<G_MDSFT_TEXTFILT)
G_TF_BILERP            equne  (2<G_MDSFT_TEXTFILT)
G_TC_CONV              equne  (0<G_MDSFT_TEXTCONV)
G_TC_FILTCONV          equne  (5<G_MDSFT_TEXTCONV)
G_TC_FILT              equne  (6<G_MDSFT_TEXTCONV)
G_CK_NONE              equne  (0<G_MDSFT_COMBKEY)
G_CK_KEY               equne  (1<G_MDSFT_COMBKEY)
G_CD_MAGICSQ           equne  (0<G_MDSFT_RGBDITHER)
G_CD_BAYER             equne  (1<G_MDSFT_RGBDITHER)
G_CD_NOISE             equne  (2<G_MDSFT_RGBDITHER)
G_CD_DISABLE           equne  (3<G_MDSFT_RGBDITHER)
G_CD_ENABLE            equne  G_CD_NOISE      ;/* HW 1.0 compatibility mode */
G_AD_PATTERN           equne  (0<G_MDSFT_ALPHADITHER)
G_AD_NOTPATTERN        equne  (1<G_MDSFT_ALPHADITHER)
G_AD_NOISE             equne  (2<G_MDSFT_ALPHADITHER)
G_AD_DISABLE           equne  (3<G_MDSFT_ALPHADITHER)
G_AC_NONE              equne  (0<G_MDSFT_ALPHACOMPARE)
G_AC_THRESHOLD         equne  (1<G_MDSFT_ALPHACOMPARE)
G_AC_DITHER            equne  (3<G_MDSFT_ALPHACOMPARE)
G_ZS_PIXEL             equne  (0<G_MDSFT_ZSRCSEL)
G_ZS_PRIM              equne  (1<G_MDSFT_ZSRCSEL)
AA_EN                  equne  0x8
Z_CMP                  equne  0x10
Z_UPD                  equne  0x20
IM_RD                  equne  0x40
CLR_ON_CVG             equne  0x80
CVG_DST_CLAMP          equne  0
CVG_DST_WRAP           equne  0x100
CVG_DST_FULL           equne  0x200
CVG_DST_SAVE           equne  0x300
ZMODE_OPA              equne  0
ZMODE_INTER            equne  0x400
ZMODE_XLU              equne  0x800
ZMODE_DEC              equne  0xc00
CVG_X_ALPHA            equne  0x1000
ALPHA_CVG_SEL          equne  0x2000
FORCE_BL               equne  0x4000
TEX_EDGE               equne  0x0000          ;/* used to be 0x8000 */
G_BL_CLR_IN            equne  0
G_BL_CLR_MEM           equne  1
G_BL_CLR_BL            equne  2
G_BL_CLR_FOG           equne  3
G_BL_1MA               equne  0
G_BL_A_MEM             equne  1
G_BL_A_IN              equne  0
G_BL_A_FOG             equne  1
G_BL_A_SHADE           equne  2
G_BL_1                 equne  2
G_BL_0                 equne  3
G_MV_VIEWPORT          equne  0x80
G_MV_LOOKATY           equne  0x82
G_MV_LOOKATX           equne  0x84
G_MV_L0                equne  0x86
G_MV_L1                equne  0x88
G_MV_L2                equne  0x8a
G_MV_L3                equne  0x8c
G_MV_L4                equne  0x8e
G_MV_L5                equne  0x90
G_MV_L6                equne  0x92
G_MV_L7                equne  0x94
G_MV_TXTATT            equne  0x96
G_MV_MATRIX_1          equne  0x9e            ;/* NOTE: this is in moveword table */
G_MV_MATRIX_2          equne  0x98
G_MV_MATRIX_3          equne  0x9a
G_MV_MATRIX_4          equne  0x9c
G_MW_MATRIX            equne  0x00            ;/* NOTE: also used by movemem */
G_MW_NUMLIGHT          equne  0x02
G_MW_CLIP              equne  0x04
G_MW_SEGMENT           equne  0x06
G_MW_FOG               equne  0x08
G_MW_LIGHTCOL          equne  0x0a
G_MW_POINTS            equne  0x0c
G_MW_PERSPNORM         equne  0x0e
G_MWO_NUMLIGHT         equne  0x00
G_MWO_CLIP_RNX         equne  0x04
G_MWO_CLIP_RNY         equne  0x0c
G_MWO_CLIP_RPX         equne  0x14
G_MWO_CLIP_RPY         equne  0x1c
G_MWO_SEGMENT_0        equne  0x00
G_MWO_SEGMENT_1        equne  0x01
G_MWO_SEGMENT_2        equne  0x02
G_MWO_SEGMENT_3        equne  0x03
G_MWO_SEGMENT_4        equne  0x04
G_MWO_SEGMENT_5        equne  0x05
G_MWO_SEGMENT_6        equne  0x06
G_MWO_SEGMENT_7        equne  0x07
G_MWO_SEGMENT_8        equne  0x08
G_MWO_SEGMENT_9        equne  0x09
G_MWO_SEGMENT_A        equne  0x0a
G_MWO_SEGMENT_B        equne  0x0b
G_MWO_SEGMENT_C        equne  0x0c
G_MWO_SEGMENT_D        equne  0x0d
G_MWO_SEGMENT_E        equne  0x0e
G_MWO_SEGMENT_F        equne  0x0f
G_MWO_FOG              equne  0x00
G_MWO_aLIGHT_1         equne  0x00
G_MWO_bLIGHT_1         equne  0x04
G_MWO_aLIGHT_2         equne  0x20
G_MWO_bLIGHT_2         equne  0x24
G_MWO_aLIGHT_3         equne  0x40
G_MWO_bLIGHT_3         equne  0x44
G_MWO_aLIGHT_4         equne  0x60
G_MWO_bLIGHT_4         equne  0x64
G_MWO_aLIGHT_5         equne  0x80
G_MWO_bLIGHT_5         equne  0x84
G_MWO_aLIGHT_6         equne  0xa0
G_MWO_bLIGHT_6         equne  0xa4
G_MWO_aLIGHT_7         equne  0xc0
G_MWO_bLIGHT_7         equne  0xc4
G_MWO_aLIGHT_8         equne  0xe0
G_MWO_bLIGHT_8         equne  0xe4
G_MWO_MATRIX_XX_XY_I   equne  0x00
G_MWO_MATRIX_XZ_XW_I   equne  0x04
G_MWO_MATRIX_YX_YY_I   equne  0x08
G_MWO_MATRIX_YZ_YW_I   equne  0x0c
G_MWO_MATRIX_ZX_ZY_I   equne  0x10
G_MWO_MATRIX_ZZ_ZW_I   equne  0x14
G_MWO_MATRIX_WX_WY_I   equne  0x18
G_MWO_MATRIX_WZ_WW_I   equne  0x1c
G_MWO_MATRIX_XX_XY_F   equne  0x20
G_MWO_MATRIX_XZ_XW_F   equne  0x24
G_MWO_MATRIX_YX_YY_F   equne  0x28
G_MWO_MATRIX_YZ_YW_F   equne  0x2c
G_MWO_MATRIX_ZX_ZY_F   equne  0x30
G_MWO_MATRIX_ZZ_ZW_F   equne  0x34
G_MWO_MATRIX_WX_WY_F   equne  0x38
G_MWO_MATRIX_WZ_WW_F   equne  0x3c
G_MWO_POINT_RGBA       equne  0x10
G_MWO_POINT_ST         equne  0x14
G_MWO_POINT_XYSCREEN   equne  0x18
G_MWO_POINT_ZSCREEN    equne  0x1c
