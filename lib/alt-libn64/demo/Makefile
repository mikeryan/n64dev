LIBN64PATH = /home/agashlin/alt-libn64/alt-libn64/
GCCN64PREFIX = /usr/local/gccn64/bin/mips64-
DRJRPATH = /home/agashlin/drjr
CHKSUM64PATH = /home/agashlin/chksum64
LINK_FLAGS = -L$(LIBN64PATH) -ln64 -lm -lc -lnosys -T n64ld.x
PROG_NAME = demo
CFLAGS = -std=gnu99 -O2 -I$(LIBN64PATH)
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

$(PROG_NAME).v64: $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).v64
	cat $(LIBN64PATH)/header $(PROG_NAME).bin | dd of=$(PROG_NAME).v64 bs=2M conv=swab conv=sync count=1
	$(CHKSUM64PATH) $(PROG_NAME).v64

$(PROG_NAME).elf : $(PROG_NAME).o display.o audio.o
	$(LD) -o $(PROG_NAME).elf $(PROG_NAME).o display.o audio.o $(LINK_FLAGS)

send: $(PROG_NAME).v64
	sudo $(DRJRPATH) $(PROG_NAME).v64
clean:
	rm -f *.v64 *.elf *.o *.bin
