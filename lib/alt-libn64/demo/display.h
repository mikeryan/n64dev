#ifndef _VIDEO_H
#define _VIDEO_H

#include <libn64.h>

typedef struct display_context {
    struct VI_config conf;
    int idx;
} _display_context;

void initDisplay();
struct display_context * lockDisplay();
void showDisplay(struct display_context *);

#endif
