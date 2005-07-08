/* PIF Boot ROM emulation, for use in simulation environments

   This needs to be loaded at the general exception/reset vector, for R4300
   that's 0xBFC00000 (?).  Its size is 1984 bytes. (It is followed by the 64
   bytes of SI RAM.)

   Probably the real PIF code copies itself from there into RSP memory,
   according to the return address that it leaves behind when it jumps to the
   boot code.  Access to the PIF region is disabled once it has executed.

   Correct operation of the boot code is entirely dependent on the values that
   the PIF program leaves behind.  It is theorized that correct operation of
   program code is dependent on the values the boot code leaves behind.

   USA and Japan consoles have the same embedded PIF code, and they can run
   each other's programs interchangeably.

   Euro consoles have a different PIF and can only run Euro programs.  Those
   use PAL instead of NTSC.  (PAL-M is a 60Hz PAL variant.)

   All consoles can generate all TV encodings (NTSC, PAL, PAL-M) in hardware
   through the VDC-NUS chip.  There is additionally a possibility of using the
   RGB output directly with a SCART cable, with a modification to the console.
   The display refresh can be set by software to either 60Hz or 50Hz.  The
   osViModeTable[] structure from libultra contains predefined video parameters
   that a program might want to set, in the form of register values to be
   written to the VI when osViSetMode() is called.  There are predefined modes
   for any permutation of the following values: NTSC/PAL/PAL-M, point sampled
   vs antialiased, interlaced vs non-interlaced, low-res vs hi-res.  If we want
   to run a PAL program on a NTSC TV or vice versa, the only change that needs
   to be made is to exchange the NTSC/PAL parameters with ones appropriate for
   the TV that will be used.  Unfortunately, there is no way to detect the TV
   type from software, so this is hard-coded in the program.  An application
   called PALadin can be used to modify these tables in a given N64 program.
   It will only work on a N64 program which uses libultra or libultra's video
   tables.

TODO:
   PALadin can "territory fix" programs to run on a different PIF.  We should
   explore this.  Do Euro carts use the same boot code as US?  If so, where is
   the territory lockout performed?

   We should have two versions of this code, one for USA/Jap and one for Euro.
   Or conditional compilation.
*/

#include "mips.h"
#include "n64hw.h"
#include "cd64.h"
#include "pi.h"
#define OS_TV_PAL		0
#define OS_TV_NTSC		1
#define OS_TV_MPAL		2


/* CRC values stolen from Daedalus */
#define CIC_6101_BOOT_CRC 0xb4086651            // CIC-6101
#define CIC_6102_BOOT_CRC 0xb9c47dc8            // CIC-6102 or CIC-7101
#define CIC_6103_BOOT_CRC 0xedce5ad9            // CIC-6103 or CIC-7103
#define CIC_6104_BOOT_CRC 0xb3d6a525            // CIC-6104?   CIC-7102
#define CIC_6105_BOOT_CRC 0xb53c588a            // CIC-6105 or CIC-7105
#define CIC_6106_BOOT_CRC 0x06d8ed9c            // CIC-6106 or CIC-7106

#define CIC_6101_BOOT_CIC 0x00023f3f
#define CIC_6102_BOOT_CIC 0x00063f3f
#define CIC_6103_BOOT_CIC 0x0002783f
#define CIC_6104_BOOT_CIC 0x00023f3f            // Incorrect?
#define CIC_6105_BOOT_CIC 0x0002913f
#define CIC_6106_BOOT_CIC 0x0002853f            // Same as FZero???


start:

/*
Determine what CIC chip is being used by the cart

Perform CRC32 over 0x40-0x1000, and look up the result.

The original PIFROM code would have instead first talked to the CIC to find out
for which CIC the bootcode needs to be, then used its own hash algorithm
(unknown) on the bootcode, and checked if the result matches its stored value
for the bootcode for that CIC, before jumping to the bootcode.  This is all theory.
If we can dump the PIFROM somehow, we will know this.
*/

jal BootcodeCRC32

/* See what CRC was returned */
/* Start with 6101 */
li	t0,	CIC_6101_BOOT_CRC
bne	v0,	t0,	try6103
nop
li	v0,	CIC_6101_BOOT_CIC
b	done

try6103:
li	t0,	CIC_6103_BOOT_CRC
bne	v0,	t0,	try6104
nop
li	v0,	CIC_6103_BOOT_CIC
b done

try6104:
li	t0,	CIC_6104_BOOT_CRC
bne	v0,	t0,	try6105
nop
li	v0,	CIC_6104_BOOT_CIC
b done

try6105:
li	t0,	CIC_6105_BOOT_CRC
bne	v0,	t0,	try6106
nop
li	v0,	CIC_6105_BOOT_CIC
b done

try6106:
li	t0,	CIC_6106_BOOT_CRC
bne	v0,	t0,	try6102
nop
li	v0,	CIC_6106_BOOT_CIC
b done

/* Default to CIC-NUS-6102 */
try6102:
li	t0,	CIC_6102_BOOT_CRC
bne	v0,	t0,	default
nop
default:
li	v0,	CIC_6102_BOOT_CIC
b done

done:
la	a0,	osCicId
sw	v0,	0 (a0)

/* Next, copy boot code and supplemental structures into RSP memory */

/* Copy boot code to SP_DMEM */

li	a0,	0xB4000000	/* Cart */
li	a1,	0xA4000000	/* RSP DMEM */
li	t0,	0x1000		/* End of cart header */
li	t1,	0x40		/* Start of boot code */
add	a0,	a0,	t1	/* Start at offset 0x40 */
add	a1,	a1,	t1
sub	t0,	t0,	t1	/* We will copy 0x1000-0x40 bytes */

to_dmem_copy:
jal	WaitPI
nop
lw	t1,	0 (a0)	/* Load from cart */
sw	t1,	0 (a1)  /* Store to RSP DMEM */
sub	t0,	t0,	4	/* Bytes left to copy */
beq	t0,	zero,	done_dmem_copy
add	a0,	a0,	4
add	a1,	a1,	4
b	to_dmem_copy
nop

done_dmem_copy:

/* This stuff is expected to be in RSP IMEM for CIC-6105 bootcode. */

li	a0,	0xA4001000	/* RSP IMEM */
li	t0,	0x3c0dbfc0
sw	t0,	0 (a0)
li	t0,	0x8da807fc	/* 0xbda807fc for PAL? */
sw	t0,	4 (a0)
li	t0,	0x25ad07c0
sw	t0,	8 (a0)
li	t0,	0x31080080
sw	t0,	12 (a0)
li	t0,	0x5500fffc
sw	t0,	16 (a0)
li	t0,	0x3c0dbfc0
sw	t0,	20 (a0)
li	t0,	0x8da80024
sw	t0,	24 (a0)
li	t0,	0x3c0bb000
sw	t0,	28 (a0)

/* Set up the registers according to bootcode's expectation */
/* Register values stolen from Daedalus */

#warning We are forced to modify 'at' here.
dli	at,	0x0000000000000000
dli	v0,	0xFFFFFFFFD1731BE9 /* for CIC-6105 */
dli	v1,	0xFFFFFFFFD1731BE9
dli	a0,	0x0000000000001BE9
dli	a1,	0xFFFFFFFFF45231E5
dli	a2,	0xFFFFFFFFA4001F0C /* in SP_IMEM */
dli	a3,	0xFFFFFFFFA4001F08 /* in SP_IMEM */
dli	o32_t0,	0x00000000000000C0 /* 0x70? */
dli	o32_t1,	0x0000000000000000
dli	o32_t2,	0x0000000000000040
dli	o32_t3,	0xFFFFFFFFA4000040 /* for CIC-6105 , bootcode address? */
dli	o32_t4,	0xFFFFFFFFD1330BC3
dli	o32_t5,	0xFFFFFFFFD1330BC3
dli	o32_t6,	0x0000000025613A26
dli	o32_t7,	0x000000002EA04317
dli	s0,	0x0000000000000000
dli	s1,	0x0000000000000000
dli	s2,	0x0000000000000000
dli	s3,	0x0000000000000000

/* This TV type value is set for only the boot code's benefit we think */
#ifdef EURO
li	s4,	OS_TV_PAL
#else
li	s4,	OS_TV_NTSC
#endif

dli	s5,	0x0000000000000000

/* Set up the CIC byte: (cic & 0x0000ff00) >> 8 */
la	s6,	osCicId
lw	s6,	0 (s6)
srl	s6,	s6,	8
and	s6,	s6,	0xff

dli	s7,	0x0000000000000006
dli	t8,	0x0000000000000000
dli	t9,	0xFFFFFFFFD73F2993
dli	k0,	0x0000000000000000
dli	k1,	0x0000000000000000
dli	gp,	0x0000000000000000
dli	sp,	0xFFFFFFFFA4001FF0 /* in SP_IMEM */
dli	s8,	0x0000000000000000
dli	ra,	0xFFFFFFFFA4001554 /* 0xFFFFFFFFA4000040 ? */

/* Extra register setups; do we need to do these? */
#ifdef EURO
dli	a1,	0xFFFFFFFFDECAAAD1
dli	o32_t6,	0x000000000CF85C13
dli	t8,	0x0000000000000002
dli	s4,	0x0000000000000000
dli	s7,	0x0000000000000006
dli	ra,	0xFFFFFFFFA4001554
#else
dli	a1,	0x000000005493FB9A;
dli	o32_t6,	0xFFFFFFFFC2C20384;
dli	t8,	0x0000000000000003;
dli	s4,	0x0000000000000001;
dli	s7,	0x0000000000000000;
dli	ra,	0xFFFFFFFFA4001550;
#endif

/* Finally, jump to bootcode which has been copied to RSP data memory */
j	0xA4000040
nop


WaitPI:
la	t1,	PI_STATUS
readPI_busy:
lw	t1,	0 (t1)
and	t1,	t1,	PI_BUSY
bne	t1,	zero,	readPI_busy 
nop
jr	ra


BootcodeCRC:

move	s0,	ra
jal	MakeCRCTable
move	ra,	s0

move	s0,	a0
move	s1,	a1

li	v0,	0
xor	v0,	v0,	0xffffffff  /* CRC */

li	a0,	0xB4000000	/* Cart */
la	a1,	CRCTable
li	t0,	0x1000		/* End of cart header */
li	t1,	0x40		/* Start of boot code */
add	a0,	a0,	t1	/* Start at offset 0x40 */
sub	t0,	t0,	t1	/* We will copy 0x1000-0x40 bytes */

copy_CRCTable:
jal	WaitPI
nop
lw	t1,	0 (a0)	/* Load 4 bytes from cart */
move	t2,	v0
xor	t2,	t2,	t1
and	t2,	t2,	0xff
mul	t2,	t2,	4 /* offset into CRCTable */
add	t2,	a1,	t2
lw	t2,	0 (t2)

srl	t3,	v0,	8
xor	v0,	t2,	t3

sub	t0,	t0,	4	/* Bytes left to CRC -= 4 */
beq	t0,	zero,	done_CRCTable
add	a0,	a0,	4
b	copy_CRCTable
nop

done_CRCTable:
xor	v0,	v0,	0xffffffff
move	a0,	s0
move	a1,	s1
jr	ra


MakeCRCTable:

move	s0,	a0

move	t0,	zero
li	t1,	31
li	t2,	1
sub	t3,	t1,	0
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	1
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	2
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	4
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	5
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	7
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	8
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	10
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	11
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	12
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	16
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	22
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	23
sll	s4,	t2,	t3
or	t0,	t0,	s4
sub	t3,	t1,	26
sll	s4,	t2,	t3
or	t0,	t0,	s4

/* Assertion.  Just infinite loop if we messed up. */
li	t1,	0xedb88320
bne	t0,	t1,	MakeCRCTable

la	t2,	poly
sw	t1,	0 (t2)

li	t1,	0
li	t2,	256
la	a0,	CRCTable

loop:
beq	t1,	t2,	done_crctable_loop
move	t3,	t1

li	s4,	0
li	s5,	8

loop2:
beq	s4,	s5,	done2
and	s6,	t3,	1
bnez	s6,	polyloop
srl	t3,	t3,	1 /* Executed in delay slot */
b	afterpoly
nop

polyloop:
/* t3 has been shifted right one */
li	s6,	1   /* starting with base ^ 1 */
move	s7,	t3  /* exponent */
la	a0,	poly
lw	s8,	0 (a0) /* base */
move	t3,	s8

powloop:
beq	s6,	s7,	donepow
nop
mul	t3,	t3,	s8
add	s6,	s6,	1
b	powloop
nop

donepow:
afterpoly:
add	s4,	s4,	1
b	loop2
nop

done2:
mul	s4,	t1,	4
add	s4,	a0,	s4
sw	t3,	0 (s4)

add	t1,	t1,	1
b	loop
nop

done_crctable_loop:

move	a0,	s0
jr	ra

.data
osCicId: .word 0
poly:	.word 0

;CRCTable: .skip 1024, 0

