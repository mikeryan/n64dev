CHKSUM64 = $(N64_INST)/bin/chksum64

all: neon64bu.rom

INCLUDES  = 6502defs.inc 6502macs.inc GFXdefs.inc atables.inc control.inc
INCLUDES += defs.inc gcompile.inc inits.inc macros.inc menu.inc mmc1.inc
INCLUDES += mmc2.inc mmc3.inc nespages.inc pal.inc pattab.inc rdp.inc read.inc
INCLUDES += rom.inc save.inc trace.inc write.inc

neon64bu.rom: neon64.bin ../header
	cat ../header neon64.bin > neon64bu.rom
	ucon64 --padn 1052672 neon64bu.rom
	$(CHKSUM64) neon64bu.rom
	rm neon64bu.bak

# doesn't track all the dependencies
neon64.bin: codes.bin staticdl.bin logo.bin hardware.inc $(INCLUDES)
	n64 neon64

codes.bin: codes.asm
	n64 codes -h

staticdl.bin: staticdl.asm
	n64 -h staticdl

logo.bin: logo.asm
	n64 logo

hardware.inc: backup.inc
	cp backup.inc hardware.inc

clean:
	rm -f *.bin *.h hardware.inc neon64bu.rom
