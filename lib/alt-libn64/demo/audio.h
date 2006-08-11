#ifndef _AUDIO_H
#define _AUDIO_H

#include <libn64.h>

void initAudio();
void writebuffer(const short *);
int getbufsize();
short * getbuf();

#endif
