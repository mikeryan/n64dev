CC=gcc
CFLAGS=-I../../include -Wall -W

ifndef DJGPP
# uname is not available by default under DOS
OSTYPE=$(shell uname -s)
endif

# The next check is not really specific to FreeBSD or OpenBSD -- the version of
#  gcc I use is just old.
ifeq ($(findstring FreeBSD,$(OSTYPE)),)
ifeq ($(findstring OpenBSD,$(OSTYPE)),)
CFLAGS+=-std=c99
endif
endif

default: libcart.a

u64cartinfo.o: u64cartinfo.c
	$(CC) $(CFLAGS) $^ -o $@ -c

libcart.a: u64cartinfo.o
	rm -f $@
	ar crv $@ $^
	ranlib $@

clean:
	rm -f *.o *.a a.out core
