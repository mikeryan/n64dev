# N64 Base Makefile - (C) Tom Whittock, 1999
# You can use this for whatever you want, but greet me if you use it.

CC = mips-mips-ecoff-gcc
LD = mips-mips-ecoff-ld
OBJDUMP = mips-mips-ecoff-objdump
LDFLAGS = -T ld.script -Map $(TARGET).map --oformat binary
CFLAGS = -mips3 -c
RAW2S = raw2s

BOOTCODE = ${HOME}/n64/lib/boot.code

TARGET = czn.bin
ADDRESS = 0x80000400
NAME = "Thank you CZN!"
MINMBITS = 2

OBJS = intro.o
DATA =  czn.o

$(TARGET): $(DATA) $(OBJS) ld.script Makefile
	$(LD) $(LDFLAGS) $(DATA) $(OBJS) -o $(TARGET).tmp
	dd if=$(BOOTCODE) of=$(TARGET) bs=4k count=1 conv=swab
	dd if=$(TARGET).tmp of=$(TARGET) bs=4k seek=1
	dd if=$(TARGET) of=$(TARGET) bs=128k count=$(MINMBITS) conv=notrunc,sync
	nifty $(TARGET) $(ADDRESS) $(NAME)
	rm $(TARGET).tmp

send: $(TARGET)
	jrsend $(TARGET)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.s.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(DATA) $(OBJS) $(TARGET) *~ $(TARGET).map $(TARGET).dump

.til.s:
	$(RAW2S) $< `basename $< .til` > $@

.raw.s:
	$(RAW2S) $< `basename $< .raw` > $@

.rgb.s:
	$(RAW2S) $< `basename $< .rgb` > $@

.SUFFIXES: .til .raw .rgb
