/*
   General R4300 stuff
*/

#ifndef _N64SYS_H
#define _N64SYS_H

#define UncachedAddr(_addr) ((void *)(((unsigned long)_addr)|0x20000000))

#define CachedAddr(_addr) (((void *)(((unsigned long)_addr)&~0x20000000))

unsigned long read_count(void);

#endif // _N64SYS_H
