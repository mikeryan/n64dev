LINK_FLAGS = -T n64ld.x
PROG_NAME = test
CFLAGS = -std=gnu99 -O2
CC = mips64-gcc
AS = mips64-as

$(PROG_NAME).n64: header $(PROG_NAME).elf
	mips64-objcopy $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	cat -B header $(PROG_NAME).bin > $(PROG_NAME).n64
	chksum64 $(PROG_NAME).n64

test.o: test.c VI.h N64sys.h
VI.o: VI.c VI_font.c VI.h
SI.o: SI.c SI.h N64sys.h
N64sys.o: N64sys.c N64sys.h regs.S

$(PROG_NAME).elf : entrypoint.o $(PROG_NAME).o VI.o SI.o N64sys.o
	mips64-ld entrypoint.o $(PROG_NAME).o VI.o SI.o N64sys.o -lm -lc -lnosys -o $(PROG_NAME).elf $(LINK_FLAGS)

clean:
	rm *.n64 *.elf *.o *.bin
