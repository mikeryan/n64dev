ROMBASE = libdemo
ADDRESS = 0x80000400
NAME = "N64 Demo Rom"
MINMBITS = 12

TARGET = mips-linux
CC = $(TARGET)-gcc
CXX = $(TARGET)-g++
LD = $(TARGET)-ld
OBJDUMP = $(TARGET)-objdump

LDFLAGS = -T ld.n64 --no-warn-mismatch -Map $(ROMBASE).map 
INCLUDE = /usr/mips-linux/include
LIBRARY = libn64.a

# dont use pic (gen. relocatable code) or abi calls (sgi api).
# dont use -mcpu=r4300 it generates some opcodes with a bit flipped (?!)
CFLAGS = -Wall -mips3 -Iinclude -O1 -mno-abicalls -EB -fno-pic # -v for compile info

# dont define for no debugging, otherwise CD64|etc.
DEBUG = -DDEBUG=CD64 # -g -GDB

# use -mrnames to generate mips register names in assembly files
# note gnu-as will not assemble mips reg names, they just look nice
MIPSREGNAMES=-mrnames

BOOTCODE = /usr/mips-linux/lib/boot.bin
OBJS = main.o

# I can't force alignment for these data files, so make sure they are a 
# multiple of 2 bytes if you want to use DMA
DATA = gotpost.pcm
GRAPHICS = mario.rgb

all: data.o $(OBJS) $(DATA)
	$(LD) $(OBJS) data.o $(LDFLAGS) -o $(ROMBASE).tmp
	dd if=$(BOOTCODE) of=$(ROMBASE).rom bs=4k count=1
	dd if=$(ROMBASE).tmp of=$(ROMBASE).rom bs=4k seek=1
	dd if=$(ROMBASE).rom of=$(ROMBASE).rom bs=128k count=$(MINMBITS) conv=notrunc,sync
	nifty $(ROMBASE).rom $(ADDRESS) $(NAME)
	rm $(ROMBASE).tmp
send:
	cd64comm -x -f $(ROMBASE).rom -sb4000000
	n64term

disassemble:
	$(TARGET)-objdump --disassemble-all -b binary -m mips --disassemble-zeroes -EB $(ROMBASE).rom

.cpp.o:
	$(CXX) -c $(CFLAGS) $(DEBUG) $< -o $@

.c.o:
	$(CC) -c $(CFLAGS)  $(DEBUG) $< -o $@

.S.s:
	$(CC) -E $(CFLAGS) $(DEBUG) $< -o $@

.S.o:
	$(CC) -c $(CFLAGS) $(DEBUG) $< -o $@

.s.o:
	$(CC) -c $(CFLAGS) $< -o $@

.c.s:
	$(CC) -S $(CFLAGS) $(MIPSREGNAMES) $(DEBUG) $< -o $@

data.o:	$(GRAPHICS)
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $(GRAPHICS) $(DATA) -o data.o

.rgb.o:
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $< -o $@

.pcm.o:
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $< -o $@

.tif.rgb:
	tiff2rgb $<

.SUFFIXES: .rgb .S .tif

clean:
	rm -f data.o $(OBJS) $(GRAPHICS) $(ROMBASE).rom *~ $(ROMBASE).map $(ROMBASE).dump *.s demo.tmp
 
