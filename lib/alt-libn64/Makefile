CFLAGS = -std=gnu99 -O2
N64PREFIX = /usr/local/gccn64/bin/mips64-
CC = $(N64PREFIX)gcc
AS = $(N64PREFIX)as
LD = $(N64PREFIX)ld
AR = $(N64PREFIX)ar

libn64.a: entrypoint.o inthandler.o VI.o SI.o AI.o MI.o N64sys.o
	$(AR) -rcs -o libn64.a entrypoint.o inthandler.o VI.o SI.o AI.o MI.o N64sys.o

AI.o: AI.c AI.h
VI.o: VI.c VI_font.c VI.h
SI.o: SI.c SI.h N64sys.h
N64sys.o: N64sys.c N64sys.h regs.S

clean:
	rm -f *.o *.a
