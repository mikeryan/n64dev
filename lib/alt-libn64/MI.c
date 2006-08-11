/*
 * Tools for the "MIPS interface", all we do with it here is handle exceptions
 */

#include <malloc.h>
#include "MI.h"
#include "VI.h"
#include "AI.h"

volatile struct MI_regs_s * const MI_regs = (struct MI_regs_s *)0xa4300000;

struct callback_link * AI_callback = 0;
struct callback_link * VI_callback = 0;

void MI_handler(void) {
    unsigned long status = MI_regs->intr & MI_regs->mask;

    if (status & MI_INTR_SP) {} /* TODO: handle SP */
    if (status & MI_INTR_SI) {} /* TODO: handle SI */
    if (status & MI_INTR_AI) {
	callCallback(AI_callback);
	AI_regs->status=0; // clear interrupt
    }
    if (status & MI_INTR_VI) {
	callCallback(VI_callback);
	VI_regs->cur_line=VI_regs->cur_line; // clear interrupt
    }
    if (status & MI_INTR_PI) {} /* TODO: handle PI */
    if (status & MI_INTR_DP) {} /* TODO: handle DP */
}

void registerCallback(struct callback_link ** head, void (*callback)()) {
    struct callback_link * next=*head;
    (*head) = malloc(sizeof(struct callback_link));
    (*head)->next=next;
    (*head)->callback=callback;
}
void callCallback(struct callback_link * head) {
    while (head) {
	head->callback();
	head=head->next;
    }
}

void registerAIhandler(void (*callback)()) {
    registerCallback(&AI_callback,callback);
}

void registerVIhandler(void (*callback)()) {
    registerCallback(&VI_callback,callback);
}

void set_AI_interrupt(int active) {
    //AI_regs->control=0;
    //AI_regs->status=0;
    if (active) MI_regs->mask=MI_MASK_SET_AI;
    else MI_regs->mask=MI_MASK_CLR_AI;
}

void set_VI_interrupt(int active, unsigned long line) {
    if (active) {
	MI_regs->mask=MI_MASK_SET_VI;
	VI_regs->v_int=line;
    }
    else MI_regs->mask=MI_MASK_CLR_VI;
}

void set_MI_interrupt(int active) {
    if (active) {
	MI_regs->mask=MI_MASK_CLR_SP|MI_MASK_CLR_SI|MI_MASK_CLR_AI|MI_MASK_CLR_VI|MI_MASK_CLR_PI|MI_MASK_CLR_PI|MI_MASK_CLR_DP;
	asm("\tmfc0 $8,$12\n\tori $8,0x401\n\tmtc0 $8,$12\n\tnop":::"$8");
    }
    else asm("\tmfc0 $8,$12\n\tla $9,~0x400\n\tand $8,$9\n\tmtc0 $8,$12\n\tnop":::"$8","$9");
}
