; Constants and variables (quite a mix, eh?)

cyclesperscanline       equne   256/3
cyclesperhretrace       equne   85/3+1 ; so extra third evens out
interruptlatency        equne   7
sprdmalatency           equne   512
a2dthreshold            equne   $20
fpsrate                 equne   2   ; 0.5 Hz
cyclesbeforenmi         equne   8   ; 4 slows Big Foot, 12 breaks DW3
cyclesafterfixit        equne   20

freq            equne       46800
abufsize        equne       freq/60*3*4
samplesperline  equne       freq/60/260
soundrate       equne       1789773*32/freq
soundperiod     equne       48681812/freq

cachepages      equne     32
cachepagesize   equne     $1000

sp_base_reg     equne     0xa4040000
sp_mem_addr_reg equne     0x00
sp_dram_addr_reg equne    0x04
sp_rd_len_reg   equne     0x08
sp_wr_len_reg   equne     0x0c
sp_status_reg   equne     0x10
sp_dma_full_reg equne     0x14
sp_dma_busy_reg equne     0x18
; Status and control signals for the RSP
; 0:  CPU->RSP: 1 Begin scanline        ; RSP->CPU: 0 Agknowledge
; 1:  RSP->CPU: 1 Done with scanline    ; CPU->RSP: 0 Agknowledge
; 2:  CPU->RSP: 1 Sprite change         ; RSP->CPU: 0 Agknowledge
; 3:  CPU->RSP: 1 Pal change            ; RSP->CPU: 0 Agknowledge
; 4:  CPU->RSP: 1 no gfx mode           ; RSP->CPU: 0 on new frame
; 5:  RSP->CPU: 8 sprite limit reached (PPU)
; 6:  RSP->CPU: Sprite 0 hit (PPU)
; 7:  RSP->CPU: In vblank (PPU)
sp_clr_ppugo    equne       0x200
sp_set_ppugo    equne       0x400
sp_clr_ppudone  equne       0x800
sp_set_ppudone  equne      0x1000
sp_clr_sprchg   equne      0x2000
sp_set_sprchg   equne      0x4000
sp_clr_palchg   equne      0x8000
sp_set_palchg   equne     0x10000
sp_clr_nogfx    equne     0x20000
sp_set_nogfx    equne     0x40000
sp_clr_8sprite  equne     0x80000
sp_set_8sprite  equne    0x100000
sp_clr_spr0hit  equne    0x200000
sp_set_spr0hit  equne    0x400000
sp_clr_vblank   equne    0x800000
sp_set_vblank   equne   0x1000000

; Variables aplenty. All should be initialized to zero.
; I really should have data segment support in U64ASM...
firstvar       equne      0x80100000|0xa0000000
spacedout:     equne      firstvar
screenbuffer:  equne      spacedout+(280*2*2)
screenbuffer2  equne      screenbuffer+(280*256*2)
screenbuffer3  equne      screenbuffer2+(280*256*2)
bgbuffer       equne      screenbuffer3+(280*2*256*2)
bgsprbuffer    equne      bgbuffer+256+16
fgsprbuffer    equne      bgsprbuffer+256+8
zerobuffers    equne      fgsprbuffer+256+8
vram:          equne      zerobuffers+(256*3+32)
sramheader     equne      vram+0x4000
sramm:         equne      sramheader+128
realpal:       equne      sramm+0x2000                ; truecolor palette
abuf1          equne      realpal+0x20          ; audio buffer 1
abuf2          equne      abuf1+(abufsize/8+1*8) ; audio buffer 2
bgline         equne      abuf2+(abufsize/8+1*8) ; line with background color
pattblc        equne      bgline+640            ; compiled patterns
_SPRRAM        equne      pattblc+(cachepages*cachepagesize)
_bgpal         equne      _SPRRAM+256
nops           equne      _bgpal+256
mem            equne      nops+32&0x8fffffff
sram           equne      mem+0x800
nespagetable   equne      sram+0x2000
pattab         equne      nespagetable+0x800
_VRAM_T        equne      pattab+1024
_PPUControl1   equne      _VRAM_T+8
_PPUControl2   equne      _PPUControl1+8
metacache      equne      _PPUControl2+8     ; info on cache entries
_VRAM_V2       equne      metacache+(8*cachepages)
_VRAM_X        equne      _VRAM_V2+8
_VRAMPages     equne      _VRAM_X+8
_gcpages       equne      _VRAMPages+(16*8)
pchist         equne      _gcpages+(8*8)
pchcnt         equne      pchist+32
s8save         equne      pchcnt+8
savehi         equne      s8save+8
savelo         equne      savehi+4
pputoggle      equne      savelo+4
scrbuf         equne      pputoggle+4
framecount     equne      scrbuf+4
framerate      equne      framecount+4
runra          equne      framerate+4
agenra         equne      runra+4
dmcra          equne      agenra+4
mmc3ra         equne      dmcra+4
lastppu1       equne      mmc3ra+4
controller1    equne      lastppu1+4
controller1cnt equne      controller1+4
controller2    equne      controller1cnt+4
controller2cnt equne      controller2+4
controlstrobe  equne      controller2cnt+4
scanline       equne      controlstrobe+4
scanleft       equne      scanline+4
sprramaddr     equne      scanleft+4
lastvram       equne      sprramaddr+4
cyclesaftersp0 equne      lastvram+4
debugflag      equne      cyclesaftersp0+4
statussave     equne      debugflag+4
rasave         equne      statussave+4
lentab         equne      rasave+4 ; 32
frqtab         equne      lentab+32   ; 32
dmcfrqtab      equne      frqtab+32   ; 32
dmcdectab      equne      dmcfrqtab+32   ; 64
dmcinctab      equne      dmcdectab+64   ; 64
square_table   equne      dmcinctab+64   ; 32
dtn_table      equne      square_table+32   ; 256
sqtab          equne      dtn_table+256  ; 64
tritab         equne      sqtab+64       ; 32
mmc3irqcnt     equne      tritab+32
mmc3irqlatch   equne      mmc3irqcnt+2
mmc3irqen      equne      mmc3irqlatch+2
awhichplay     equne      mmc3irqen+2
awhichdraw     equne      awhichplay+2
abufdrawpos    equne      awhichdraw+2 ; halfword
_VRAM_V        equne      abufdrawpos+2
sq1cnt         equne      _VRAM_V+2
sq2cnt         equne      sq1cnt+2
tricnt         equne      sq2cnt+2
rndcnt         equne      tricnt+2
dmccnt         equne      rndcnt+2
rndcval        equne      dmccnt+2
rndfrq         equne      rndcval+2
frame4         equne      rndfrq+2
frame2         equne      frame4+1
sq1len         equne      frame2+1
sq2len         equne      sq1len+1
trilen         equne      sq2len+1
rndlen         equne      trilen+1
sq1duty        equne      rndlen+1
sq2duty        equne      sq1duty+1
triduty        equne      sq2duty+1
sq1dec         equne      triduty+1
sq2dec         equne      sq1dec+1
rnddec         equne      sq2dec+1
sq1vol         equne      rnddec+1
sq2vol         equne      sq1vol+1
rndvol         equne      sq2vol+2 ; leave space for tri vol
sq1decvol      equne      rndvol+1
sq2decvol      equne      sq1decvol+1
rnddecvol      equne      sq2decvol+2   ; leave space for tri
sq1swp         equne      rnddecvol+1
sq2swp         equne      sq1swp+1
asq1           equne      sq2swp+1 ; all 4 square wave 1 regs
asq2           equne      asq1+4        ;  " square wave 2 regs
atri           equne      asq2+4        ;  " triangle regs
arnd           equne      atri+4        ;  " noise regs
lincnt         equne      arnd+4
astatus        equne      lincnt+1      ; length counter status (read)
aenable        equne      astatus+1     ; channel enable (write)
sq1en          equne      aenable+1
sq2en          equne      sq1en+1
trien          equne      sq2en+1
rnden          equne      trien+1
killframe      equne      rnden+1
dmcfreq        equne      killframe+1   ; & mode
dmcdelta       equne      dmcfreq+1
dmcadrload     equne      dmcdelta+1
dmclenload     equne      dmcadrload+1
dmccurbit      equne      dmclenload+1
dmcbitcnt      equne      dmccurbit+1
dmcadr         equne      dmcbitcnt+1/2+1*2
dmclen         equne      dmcadr+2
mmc3watch      equne      dmclen+2
tribuf         equne      mmc3watch+4
mmc1reg        equne      tribuf+1
mmc1regcnt     equne      mmc1reg+4
mmc1regtmp     equne      mmc1regcnt+4
mmc1prgrom     equne      mmc1regtmp+4
ppustatus      equne      mmc1prgrom+4
mmc3scanflag   equne      ppustatus+1
mmc3after      equne      mmc3scanflag+1
mmc14koverride equne      mmc3after+1
mmc2_0         equne      mmc14koverride+1
mmc2_1fe       equne      mmc2_0+1
mmc2_1fd       equne      mmc2_1fe+1
mmc2_toggle    equne      mmc2_1fd+1    ; 0=fe, 1=fd
soundirq       equne      mmc2_toggle+1 ; $4017
zapper         equne      soundirq+1
arkpad         equne      zapper+1
arkcnt         equne      arkpad+1
invblank       equne      arkcnt+1
mapper         equne      invblank+1
savespace      equne      mapper+1
cbnmi          equne      savespace+1
mmc3irqon      equne      cbnmi+1
mapperusessram equne      mmc3irqon+1
lincnthalt     equne      mapperusessram+1
traperrors2    equne      lincnthalt+1
lastvar        equne      traperrors2+1|0xa0000000 ; just a marker

proga          equne      0xa0300000
prog           equne      proga+16
prgrom         equne      prog&0x8fffffff

; I'm sorry, is this the five minute argument or the full half hour?
