/* contpad.c - Control Pad and Joystick functions */

#include "mips.h"
#include "si.h"
#include "contpad.h"

static u32 CONTPAD_PIF_READBUTTONS[16] = {
  0xFF010401,0xFFFFFFFF,
  0xFF010401,0xFFFFFFFF,
  0xFF010401,0xFFFFFFFF,
  0xFF010401,0xFFFFFFFF,
  0xFE000000,0x0,
  0x0,0x0,
  0x0,0x0,
  0x0,0x1 } ;

/* Global pif for accessing contpad data */
Pif *pifr;
static Pif pifr1,pifr2;

void initContPads()
{
  pifr=(Pif*)K0_TO_K1(&pifr1);
}

void readContPads()
{  
  if (!SI_BUSY) {
    if (pifr==(Pif*)K0_TO_K1(&pifr1))
      pifr=(Pif*)K0_TO_K1(&pifr2);
    else
      pifr=(Pif*)K0_TO_K1(&pifr1);
    IO_WRITE(SI_DRAM_ADDR_REG,K1_TO_PHYS(pifr));
    IO_WRITE(SI_PIF_ADDR_RD64B_REG,PIF_RAM_START);
    SI_WAIT;
    IO_WRITE(SI_DRAM_ADDR_REG,K0_TO_PHYS(&CONTPAD_PIF_READBUTTONS));
    IO_WRITE(SI_PIF_ADDR_WR64B_REG,PIF_RAM_START);
  }
}

// returns true if the button has changed
int contPadToggle(int pad,contPadButton button)
{
  u16 new;
  u16 *p1=(u16*)(K0_TO_K1(&pifr1)+pad*8+4);
  u16 *p2=(u16*)(K0_TO_K1(&pifr2)+pad*8+4);

  if (((*p1>>button)&1)!=((*p2>>button)&1)) {
    /* make sure that a button toggle is only recognised once, because
       we don't know when the SI will update it.
       Force previous bit of button being checked to whatever it is now
    */
    new=((*(u16*)((u32)pifr+pad*8+4))>>button);
    if (new==1) {
      *p1|=(1<<button);
      *p2|=(1<<button);
    }
    // do same but with & to force to 0 if new==0
    else {
      *p1&=(~((u16)1<<button));
      *p2&=(~((u16)1<<button));
    }
    return 1;
  }
    return 0;
}

// returns true if the button has gone from unpressed->pressed
int contPadHit(int pad,contPadButton button)
{
  if ((((*(u16*)((u32)pifr+pad*8+4))>>button)==1) 
      && (contPadToggle(pad,button)))
    return 1; else return 0;
}
