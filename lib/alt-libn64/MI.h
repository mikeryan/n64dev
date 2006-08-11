#ifndef _MI_H
#define _MI_H

typedef struct MI_regs_s {
    unsigned long mode;
    unsigned long version;
    unsigned long intr;
    unsigned long mask;
} _MI_regs_s;

typedef struct callback_link {
    void (*callback)();
    struct callback_link * next;
} _callback_link;

extern volatile struct MI_regs_s * const MI_regs;

#define MI_INTR_SP 0x01
#define MI_INTR_SI 0x02
#define MI_INTR_AI 0x04
#define MI_INTR_VI 0x08
#define MI_INTR_PI 0x10
#define MI_INTR_DP 0x20

#define MI_MASK_CLR_SP 0x0001
#define MI_MASK_SET_SP 0x0002
#define MI_MASK_CLR_SI 0x0004
#define MI_MASK_SET_SI 0x0008
#define MI_MASK_CLR_AI 0x0010
#define MI_MASK_SET_AI 0x0020
#define MI_MASK_CLR_VI 0x0040
#define MI_MASK_SET_VI 0x0080
#define MI_MASK_CLR_PI 0x0100
#define MI_MASK_SET_PI 0x0200
#define MI_MASK_CLR_DP 0x0400
#define MI_MASK_SET_DP 0x0800

void MI_handler();
void registerCallback(struct callback_link ** head, void (*callback)());
void callCallback(struct callback_link * head);
void registerAIhandler(void (*callback)());
void registerVIhandler(void (*callback)());
void set_AI_interrupt(int);
void set_VI_interrupt(int,unsigned long);
void set_MI_interrupt(int);

#endif
