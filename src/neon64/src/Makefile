CHKSUM64 = $(N64INST)/bin/chksum64

all: neon64bu.rom

neon64bu.rom: neon64.bin ../header
	cat ../header neon64.bin > neon64bu.rom
	ucon64 --padn 1052672 neon64bu.rom
	$(CHKSUM64) neon64bu.rom
	rm neon64bu.bak

# doesn't track all the dependencies
neon64.bin: codes.bin staticdl.bin logo.bin hardware.inc
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
