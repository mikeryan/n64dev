/* contpad.h  - Control Pad and Joystick */

#ifndef _CONTPADDECL_H
#define _CONTPADDECL_H

enum contPadButton {A_BUTTON=15,B_BUTTON=14,Z_TRIG=13,START_BUTTON=12,U_JPAD=11,D_JPAD=10,L_JPAD=9,R_JPAD=8,L_TRIG=5,R_TRIG=4,U_CBUTTONS=3,D_CBUTTONS=2,L_CBUTTONS=1,R_CBUTTONS=0};
typedef enum contPadButton contPadButton;

/* external functions */

void initContPads();
void readContPads();
int contPadToggle(int pad,contPadButton button);

#define PIF_ROM_START		0x1FC00000
#define PIF_ROM_END		0x1FC007BF
#define PIF_RAM_START		0x1FC007C0
#define PIF_RAM_END		0x1FC007FF

typedef struct {
  u32 cmd;
  unsigned A_BUTTON : 1;
  unsigned B_BUTTON : 1;
  unsigned Z_TRIG : 1;
  unsigned START_BUTTON : 1;
  unsigned U_JPAD : 1;
  unsigned D_JPAD : 1;
  unsigned L_JPAD : 1;
  unsigned R_JPAD : 1;
  unsigned empty : 2;
  unsigned L_TRIG : 1;
  unsigned R_TRIG : 1;
  unsigned U_CBUTTONS : 1;
  unsigned D_CBUTTONS : 1;
  unsigned L_CBUTTONS : 1;
  unsigned R_CBUTTONS : 1;
  s8      stick_x;		/* -80 <= stick_x <= 80 */
  s8      stick_y;		/* -80 <= stick_y <= 80 */
} PifContPad_t;

typedef union {
  PifContPad_t cont[8];
  u64 boundary_alignment;
} Pif;

/*
Each game controller channel has 4 error bits that are defined in bit
6-7 of cmd and res 
*/

#define CHNL_ERR_NORESP         0x80    /* Bit 7 (Rx): No response error */
#define CHNL_ERR_OVERRUN        0x40    /* Bit 6 (Rx): Overrun error */
#define CHNL_ERR_FRAME          0x80    /* Bit 7 (Tx): Frame error */
#define CHNL_ERR_COLLISION      0x40    /* Bit 6 (Tx): Collision error */
#define CHNL_ERR_MASK           0xC0    /* Bit 6-7: channel errors */


/* controller errors */
#define CONT_NO_RESPONSE_ERROR          0x8
#define CONT_OVERRUN_ERROR              0x4

/* Controller type */

#define CONT_ABSOLUTE           0x0001
#define CONT_RELATIVE           0x0002
#define CONT_JOYPORT            0x0004
#define CONT_EEPROM		0x8000

/* Controller status */

#define CONT_CARD_ON            0x01
#define CONT_CARD_PULL          0x02
#define CONT_ADDR_CRC_ER        0x04
#define CONT_EEPROM_BUSY	0x80

/* global pif, should get rid of this and have only functions in contpad.c */

extern Pif *pifr;

#define CONT(x) pifr->cont[x]
#define CONT_EXISTS(x) (pifr->cont[x].cmd&CONT_NO_RESPONSE_ERROR)

#endif /* _CONTPADDECL_H */
