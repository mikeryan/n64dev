#include <mips.h>
#include <stdio.h>
#include <stddef.h>
#include "libn64.h"

void initPi()
{
    PI_WAIT;
    IO_WRITE(PI_STATUS_REG,0x3);
}

void piDmaRead(void *dest,void *src,u32 length)
{
  //__cpu_flush();
  PI_WAIT
  IO_WRITE(PI_DRAM_ADDR_REG,K1_TO_PHYS(dest));
  IO_WRITE(PI_CART_ADDR_REG,K0_TO_PHYS(src));
  writeBackInvalDCache();
  IO_WRITE(PI_WR_LEN_REG,length-1);
}
