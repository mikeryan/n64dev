; Static display list which is modified every scanline to render graphics.
; This takes seven seconds to assemble, and including rdp.inc would paralyze
; compilation of the rest of the program, so this is in a file of its own.

thecolors equne $80009520

 org 0x430
#include macros.inc
#include rdp.inc
#include defs.inc
displaylist
fillstart
        dw  $FF100000|($118-1)   ; SetCimg
SetCimg dw  screenbuffer

scissor
        dw  $ED000000|(8<14)|(8<2)     ; SetScissor
        dw  ($118<14)|(232<2)
;        dw  $ED000000|(8<14)|(0<2)     ; SetScissor PAL
;        dw  ($118<14)|(240<2)


        dw  $EFB92CBF  ; RDPSetOtherMode (magic number from Destop)
        dw  $0F0A4000

        dw  $F7000000   ; set fill color
bgcolor dw  $00010001

fillrect dw  $F65003C0   ; fill rectangle (completely overwritten)
        dw  0

fillend
loadpalstart
        gsDPLoadTLUT_pal256(_bgpal)
loadpalend
bgsprstart
        ; set other mode
        dw $EF800CBF|G_TT_RGBA16|G_TF_POINT|G_CYC_1CYCLE
        dw $0F0A7008

        dw $fc11fe23    ; setcombine (magic number from Destop)
        dw $fffff3f9

        dw  $FA000000   ;SetPrimColor
primcolor dw  $ffffffff ; (used for color deemphasis)

        _gsDPLoadTextureBlock(bgsprbuffer,0,G_IM_FMT_CI,G_IM_SIZ_8b,256,1,0,2,2,0,0,0,0)
bg_spr_rect
        gsDPDrawTexturedRectangle(8,100,256,1,$400,$400)
        gsDPTileSync
        gsDPPipeSync
bgsprend
bgstart
        _gsDPLoadTextureBlock((bgbuffer+8),0,G_IM_FMT_CI,G_IM_SIZ_8b,256,1,0,2,2,0,0,0,0)
        ;_gsDPLoadTextureBlock((thecolors),0,G_IM_FMT_CI,G_IM_SIZ_8b,256,1,0,2,2,0,0,0,0)
bg_rect
        gsDPDrawTexturedRectangle(8,100,256,1,$400,$400)
        gsDPTileSync
        gsDPPipeSync
bgend
fgsprstart
        _gsDPLoadTextureBlock(fgsprbuffer,0,G_IM_FMT_CI,G_IM_SIZ_8b,256,1,0,2,2,0,0,0,0)
fg_spr_rect
        gsDPDrawTexturedRectangle(8,100,256,1,$400,$400)
        gsDPTileSync
        gsDPPipeSync
fgsprend
end_dl
