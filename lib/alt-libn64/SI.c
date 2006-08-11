/*
   Routines for interfacing the PIF via the SI, used primarily for controller
   access

   Thanks to LaC for controller information
*/

#include "N64sys.h"
#include "SI.h"
#include <string.h>

volatile struct SI_regs_s * const SI_regs = (struct SI_regs_s *)0xa4800000;
void * const PIF_RAM = (void *)0x1fc007c0;

/*
   anatomy of SI_read_con_block
   0xff010401ffffffff
     \|\|\|\|\______|
      | | | |       |__ unused space, button data will be written here
      | | | |__ command 1: read controller button data
      | | |____ rx, receive 4 bytes (the button data)
      | |______ tx, transmit 1 byte (the command byte
      |________ tx, transmit 0xff bytes (thus ignored, just for padding)

   same for each of four channels (which correspond to the four controllers) 

   0xfe00000000000000
     \|__ tx=0xfe, end of commands

   0,0 empty space

   1: bit 0 of byte 63, when set, makes the PIF run the task. It is cleared
      when finished.

*/

unsigned long long SI_read_con_block[8] = {
    0xff010401ffffffff,
    0xff010401ffffffff,
    0xff010401ffffffff,
    0xff010401ffffffff,
    0xfe00000000000000,
    0,
    0,
    1
};

int SI_DMA_busy(void) {
    // clear interrupt
    while (SI_regs->status & (SI_status_DMA_busy|SI_status_IO_busy)) {
    }
}

void controller_exec_PIF(unsigned long long const inblock[8],
	unsigned long long outblock[8]) {
    volatile unsigned long long inblock_temp[8];
    volatile unsigned long long outblock_temp[8];

    data_cache_writeback_invalidate(inblock_temp,64);
    memcpy(UncachedAddr(inblock_temp),inblock,64);

    SI_DMA_busy();

    SI_regs->DRAM_addr = inblock_temp; // only cares about 23:0
    SI_regs->PIF_addr_write = PIF_RAM; // is it really ever anything else?

    SI_DMA_busy();

    data_cache_writeback_invalidate(outblock_temp,64);

    SI_regs->DRAM_addr = outblock_temp;
    SI_regs->PIF_addr_read = PIF_RAM;

    SI_DMA_busy();

    memcpy(outblock,UncachedAddr(outblock_temp),64);
}

void controller_Read(struct controller_data * output) {
    controller_exec_PIF(SI_read_con_block,(unsigned long long *)output);
}
