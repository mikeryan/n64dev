/* ai.c - Audio Interface functions */

#include "mips.h"
#include "ai.h"

void initAudio()
{
  // VI_PAL_CLOCK=49656530

  IO_WRITE(AI_DACRATE_REG,VI_PAL_CLOCK/22050-1);
  IO_WRITE(AI_BITRATE_REG,16-1);
}

void playAudio(u32* sample,u32 len)
{
  while (IO_READ(AI_STATUS_REG)&AI_STATUS_FIFO_FULL);
  IO_WRITE(AI_DRAM_ADDR_REG,(u32*)K0_TO_PHYS(sample));
  IO_WRITE(AI_LEN_REG,len&(u32)0xFFFFFFF8);
  IO_WRITE(AI_CONTROL_REG,AI_CONTROL_DMA_ON);
}
