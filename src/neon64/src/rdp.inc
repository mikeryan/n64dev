; Developed from gbi.h by themind

#include GFXdefs.inc

#define TXL2WORDS(txls_, b_txl_) ((txls_)*(b_txl_)/8)
#define CALC_DXT(width_, b_txl_)  (((1<G_TX_DXT_FRAC)+TXL2WORDS(width_,b_txl_)-1)/TXL2WORDS(width_,b_txl_))
#define TXL2_4b_WORDS(txls_)  (txls_/16)
#define CALC_4b_DXT(width_) (((1<G_TX_DXT_FRAC)+TXL2_4b_WORDS(width_)-1)/TXL2_4b_WORDS(width_))
; #define TXL2WORDS(txls, b_txl)  MAX(1,((txls)*(b_txl)/8))
; #define TXL2WORDS_4b(txls)  MAX(1, ((txls)/16))
#define gsDPNoParam(cmd_)   dw  _SHIFTL(cmd_,24,8), 0
#define gsDPTileSync gsDPNoParam(G_RDPTILESYNC)
#define gsDPLoadSync gsDPNoParam(G_RDPLOADSYNC)
#define gsDPPipeSync gsDPNoParam(G_RDPPIPESYNC)
#define gsDPFullSync gsDPNoParam(G_RDPFULLSYNC)
#define gsDPSetTextureImage(f_, s_, w_, i_) gsSetImage(G_SETTIMG, f_, s_, w_, i_)
#define gsSetImage(cmd_, fmt_, siz_, width_, img_) dw _SHIFTL(cmd_, 24, 8) | _SHIFTL(fmt_, 21, 3) | _SHIFTL(siz_, 19, 2) | _SHIFTL(width_-1, 0, 12), img_
#define gsDPSetTile(fmt_,siz_,line_,tmem_,tile_,palette_,cmt_,maskt_,shiftt_,cms_,masks_,shifts_) dw _SHIFTL(G_SETTILE,24,8)|_SHIFTL(fmt_,21,3)|_SHIFTL(siz_,19,2)|_SHIFTL(line_,9,9)|_SHIFTL(tmem_,0,9)\
    dw  _SHIFTL(tile_,24,3)|_SHIFTL(palette_,20,4)|_SHIFTL(cmt_,18,2)|_SHIFTL(maskt_,14,4)|_SHIFTL(shiftt_,10,4)|_SHIFTL(cms_,8,2)|_SHIFTL(masks_,4,4)|_SHIFTL(shifts_,0,4)
#define gsDPGenericLoadTile(c_, tile_, uls_, ult_, lrs_, lrt_) dw _SHIFTL(c_, 24, 8) | _SHIFTL(uls_, 12, 12) | _SHIFTL(ult_, 0, 12),  _SHIFTL(tile_, 24, 3) | _SHIFTL(lrs_, 12, 12) | _SHIFTL(lrt_, 0, 12)
#define gsDPLoadTile(t_, uls_, ult_, lrs_, lrt_) gsDPGenericLoadTile(G_LOADTILE,t_,uls_,ult_,lrs_,lrt_)
#define gsDPSizeSetTile(t_, uls_, ult_, lrs_, lrt_) gsDPGenericLoadTile(G_SETTILESIZE,t_,uls_,ult_,lrs_,lrt_)
#define gsDPLoadBlock(tile_, uls_, ult_, lrs_, dxt_) dw (_SHIFTL(G_LOADBLOCK,24,8)|_SHIFTL(uls_,12,12)|_SHIFTL(ult_,0,12))\
    dw  (_SHIFTL(tile_,24,3)|_SHIFTL(lrs_, 12, 12)|_SHIFTL(dxt_, 0, 12))
#define gsDPLoadTLUTCmd(tile_, count_) dw _SHIFTL(G_LOADTLUT, 24, 8),_SHIFTL((tile_), 24, 3) | _SHIFTL((count_), 14, 10)

#define _gsDPLoadTextureBlock(timg_,tmem_,fmt_,siz_,width_,height_,pal_,cms_,cmt_,masks_,maskt_,shifts_,shiftt_) \
    gsDPSetTextureImage(fmt_,siz__LOAD_BLOCK,1,timg_)\
    gsDPSetTile(fmt_,siz__LOAD_BLOCK,0,tmem_,G_TX_LOADTILE,0,cmt_,maskt_,shiftt_,cms_,masks_,shifts_)\
    gsDPLoadSync\
    gsDPLoadBlock(G_TX_LOADTILE,0,0,width_*height_+siz__INCR>siz__SHIFT-1,CALC_DXT(width_,siz__BYTES))\
    gsDPPipeSync\
    gsDPSetTile(fmt_,siz_,((((width_)*siz__LINE_BYTES)+7)>3),tmem_,G_TX_RENDERTILE,pal_,cmt_,maskt_,shiftt_,cms_,masks_,shifts_)\
    gsDPSizeSetTile(G_TX_RENDERTILE,0,0,((width_)-1)<G_TEXTURE_IMAGE_FRAC,((height_)-1)<G_TEXTURE_IMAGE_FRAC)

#define gsDPLoadTLUT_pal16(pal_,dram_)\
    gsDPSetTextureImage(G_IM_FMT_RGBA,G_IM_SIZ_16b,1,dram_)\
    gsDPTileSync\
    gsDPSetTile(0,0,0,(256+(((pal_)&0xf)*16)),G_TX_LOADTILE,0,0,0,0,0,0,0)\
    gsDPLoadSync\
    gsDPLoadTLUTCmd(G_TX_LOADTILE,15)\
    gsDPPipeSync

#define gsDPLoadTLUT_pal256(dram_)\
    gsDPSetTextureImage(G_IM_FMT_RGBA,G_IM_SIZ_16b,1,dram_)\
    gsDPTileSync\
    gsDPSetTile(0,0,0,256,G_TX_LOADTILE,0,0,0,0,0,0,0)\
    gsDPLoadSync\
    gsDPLoadTLUTCmd(G_TX_LOADTILE, 255)\
    gsDPPipeSync

#define gsDPLoadTextureBlock_4b(timg_,fmt_,width_,height_,pal_,cms_,cmt_,masks_,maskt_,shifts_,shiftt_)\
    gsDPSetTextureImage(fmt_,G_IM_SIZ_16b,1,timg_)\
    gsDPSetTile(fmt_,G_IM_SIZ_16b,0,0,G_TX_LOADTILE,0,cmt_,maskt_,shiftt_,cms_,masks_,shifts_)\
    gsDPLoadSync\
    gsDPLoadBlock(G_TX_LOADTILE,0,0,(width_*height_+3>2)-1,CALC_4b_DXT(width_))\
    gsDPPipeSync\
    gsDPSetTile(fmt_,G_IM_SIZ_4b,(width_>1+7>3),0,G_TX_RENDERTILE,pal_,cmt_,maskt_,shiftt_,cms_,masks_,shifts_)\
    gsDPSizeSetTile(G_TX_RENDERTILE,0,0,(width_-1)<G_TEXTURE_IMAGE_FRAC,(height_-1)<G_TEXTURE_IMAGE_FRAC)

; my own, you can tell because of the low number of parameters
#define gsDPDrawTexturedRectangle(x_,y_,width_,height_,xscale_,yscale_)\
    dw _SHIFTL(G_TEXRECT,24,8)|_SHIFTL(width_+x_*4,12,12)|_SHIFTL(height_+y_*4,0,12)\
    dw _SHIFTL(x_*4,12,12)|_SHIFTL(y_*4,0,12)\
    dw 0\    ; shift texture
    dw _SHIFTL(xscale_,16,16)|_SHIFTL(yscale_,0,16)

#define gsDPDrawFlippedTexturedRectangle(x_,y_,width_,height_,xscale_,yscale_)\
    dw _SHIFTL(G_TEXRECTFLIP,24,8)|_SHIFTL(width_+x_*4,12,12)|_SHIFTL(height_+y_*4,0,12)\
    dw _SHIFTL(x_*4,12,12)|_SHIFTL(y_*4,0,12)\
    dw 0\    ; shift texture
    dw _SHIFTL(xscale_,16,16)|_SHIFTL(yscale_,0,16)
