GCC = gcc -Wall -O2
PREFIX = usr/bin

all:
	$(GCC) -o cd64comm cd64comm.c
	$(GCC) -o drjr drjr.c
	$(GCC) -o insertfs insertfs.c
	$(GCC) -o n64term n64term.c
	$(GCC) -o nifty nifty.c
	$(GCC) -o raw2s raw2s.c
	$(GCC) -o renamer renamer.c
	$(GCC) -ltiff -o tiff2rgb tiff2rgb.c

install:
	install -m 755 -o 0 -g 0 -d       $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 cd64comm $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 drjr 	  $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 insertfs $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 n64term  $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 nifty    $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 raw2s    $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 renamer  $(ROOT)/$(PREFIX)
	install -m 755 -o 0 -g 0 tiff2rgb $(ROOT)/$(PREFIX)

clean:
	rm -f cd64comm drjr insertfs n64term nifty raw2s renamer tiff2rgb *~
