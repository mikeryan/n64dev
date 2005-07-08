TARGET = mips-linux
CC = $(TARGET)-gcc
CXX = $(TARGET)-g++
LD = $(TARGET)-ld

LIBRARY = libn64.a
HEADER = libn64.h

# dont use pic (gen. relocatable code) or abi calls (sgi api).
# dont use -mcpu=r4300 it generates some opcodes with a bit flipped (?!)
# dont use -O2 I dont know why yet but its taken hours to figure out thats
# why this hasnt been working, its pissed me off.
# werd. -O2 reorders blocks of asm code, so eg. PI_WAIT;IO_WRITE will become
# asm where pibusy is read, then the write done, then pi is checked to see
# if its busy!
CFLAGS = -Wall -mips3 -O1 -mno-abicalls -EB -fno-pic # -v for compile info

# use -mrnames to generate mips register names in assembly files
# note gnu-as will not assemble mips reg names, they just look nice
MIPSREGNAMES=-mrnames

OBJS = ai.o n64mon.o cd64.o cache.o contpad.o debug.o vi.o hardware_init_hook.o pi.o vr4300.o jpegd.o

DATA =

all: $(LIBRARY)

$(LIBRARY): $(HEADER) $(OBJS)
	$(TARGET)-ar rcs $(LIBRARY) $(OBJS)

$(HEADER):
	if [ -f $(HEADER) ]; then rm $(HEADER); fi
	cat *.h > $(HEADER)
.cpp.o:
	$(CXX) -c $(CFLAGS)  $< -o $@

.c.o:
	$(CC) -c $(CFLAGS)   $< -o $@

.S.s:
	$(CC) -E $(CFLAGS)  $< -o $@

.S.o:
	$(CC) -c $(CFLAGS)  $< -o $@

.s.o:
	$(CC) -c $(CFLAGS) $< -o $@
 
.c.s:
	$(CC) -S $(CFLAGS) $(MIPSREGNAMES)  $< -o $@

data.o:
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $(DATA) -o data.o

.rgb.o:
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $< -o $@

.pcm.o:
	$(LD) -b binary -oformat $(TARGET)-elf -e _fdata -no-warn-mismatch $< -o $@

.SUFFIXES: .rgb .S

clean:
	rm -f $(OBJS) *~ *.s $(LIBRARY) $(HEADER)

install:
	install -m 755 -o 0 -g 0 -d $(ROOT)/usr/mips-linux/lib
	install -m 755 -o 0 -g 0 -d $(ROOT)/usr/mips-linux/include
	install -m 755 -o 0 -g 0 libn64.a $(ROOT)/usr/mips-linux/lib
	install -m 755 -o 0 -g 0 libn64.h $(ROOT)/usr/mips-linux/include
	install -m 755 -o 0 -g 0 boot.bin $(ROOT)/usr/mips-linux/lib
