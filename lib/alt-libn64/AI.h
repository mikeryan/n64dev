#ifndef _AI_H
#define _AI_H

typedef struct AI_regs_s {
    short * address;
    unsigned long length;
    unsigned long control;
    unsigned long status;
    unsigned long dacrate;
    unsigned long samplesize;
} _AI_regs_s;

extern volatile struct AI_regs_s * const AI_regs;

#define AI_status_busy (1<<30)
#define AI_status_full (1<<31)

void AI_add_buffer(short * buf, unsigned long len);
void AI_set_frequency(unsigned long freq);
int AI_busy();
int AI_full();

#endif
