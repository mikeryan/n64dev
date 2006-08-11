/*
 * system for managing buffered audio
 */

#include <string.h>
#include <libn64.h>
#include "display.h"

const int frequency=22050;
// # of samples
#define BUFFER_SIZE (22050/25/8*8) 
#define BUFFER_COUNT 4

// struct for a sound buffer to force it to be aligned to 8 bytes
union sb_align_u {
    unsigned long long longlongs[BUFFER_SIZE*2/8];
    short shorts[BUFFER_SIZE*2];
};

union sb_align_u sb[BUFFER_COUNT];
union sb_align_u temp_sb; // what the user writes to

int nowwriting;
int nowplaying;

void audio_AIcallback();

void initAudio() {
    AI_set_frequency(frequency);

    registerAIhandler(audio_AIcallback);
    set_AI_interrupt(1);

    nowplaying=0;
    nowwriting=0;

    memset(sb,0,sizeof(sb));
    //audio_AIcallback();
}

void writebuffer(const short * buffer) {
    while (nowplaying==nowwriting) {
	if(!AI_busy()) audio_AIcallback();
    }
    memcpy(UncachedShortAddr(sb[nowwriting].shorts),buffer,BUFFER_SIZE*4);
    nowwriting=(nowwriting+1)%BUFFER_COUNT;
}

void audio_AIcallback() {
    while (!AI_full()) {
	nowplaying=(nowplaying+1)%BUFFER_COUNT;
	AI_add_buffer(sb[nowplaying].shorts,BUFFER_SIZE);
    }
}

int getbufsize() {return BUFFER_SIZE;}
short * getbuf() {return temp_sb.shorts;}
