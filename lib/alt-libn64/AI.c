/*
 * Routines for using the AI to play sound
 */

#include "AI.h"
#include "N64sys.h"

volatile struct AI_regs_s * const AI_regs = (struct AI_regs_s *)0xa4500000;

int AI_busy() {
    return AI_regs->status & AI_status_busy;
}
int AI_full() {
    return AI_regs->status & AI_status_full;
}

void AI_set_frequency(unsigned long freq) {
    AI_regs->dacrate=48681812/freq;
    AI_regs->samplesize=15;
}

void AI_add_buffer(short * buf, unsigned long len) {
    while (AI_full()) {}
    AI_regs->address=UncachedAddr(buf);
    AI_regs->length=(len*2*2)&(~7);
    AI_regs->control=1; /* start DMA */
}
