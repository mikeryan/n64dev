CC=gcc
CFLAGS=-I../../include -Wall -W
LIBS=-L../../lib/cd64 -lcd64 -L../../lib/cart -lcart

ifndef DJGPP
# uname is not available by default under DOS
OSTYPE=$(shell uname -s)
endif

# The next check is not really specific to FreeBSD or OpenBSD -- the version of
#  gcc I use is just old.
ifeq ($(findstring FreeBSD,$(OSTYPE)),)
ifeq ($(findstring OpenBSD,$(OSTYPE)),)
CFLAGS+=-std=gnu99
endif
endif

default: all

all:	cd64ftp

# cd64ftp stuff

cd64ftp.o:	cd64ftp.c
	$(CC) $(CFLAGS) cd64ftp.c -c -o cd64ftp.o

cd64ftp:	cd64ftp.o ../../lib/cd64/libcd64.a ../../lib/cart/libcart.a
	$(CC) cd64ftp.o $(LIBS) -o cd64ftp

clean:
	rm -f *.o cd64ftp a.out *.flash *.sram *.eeprom *.mem *.rom *.ram

test:	cd64ftp
# Setting {LD_}LIBRARY_PATH is only necessary when cd64ftp is linked to the
#  shared library version of libcd64 (and libcd64 isn't located in a standard
#  library directory).
ifeq ($(findstring BeOS,$(OSTYPE)),BeOS)
	LIBRARY_PATH=../libcd64/:$(LIBRARY_PATH) ./cd64ftp
else
	LD_LIBRARY_PATH=../libcd64/:$(LD_LIBRARY_PATH) ./cd64ftp
endif
