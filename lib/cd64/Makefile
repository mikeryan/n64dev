CC=gcc

ifdef DEBUG
# I think we only use gnu99 instead of c99 due to va_args extensions.
CFLAGS=-I../../include -Wall -W -pg -g -pedantic -ansi -DDEBUG
else
CFLAGS=-I../../include -Wall -W -O6 -funroll-loops -fexpensive-optimizations
endif

ifndef DJGPP
# uname is not available by default under DOS
OSTYPE=$(shell uname -s)
else
OSTYPE=DJGPP
endif

GCC_WIN=0
ifeq ($(findstring MINGW,$(OSTYPE)),MINGW)
GCC_WIN=1
endif
ifeq ($(findstring CYGWIN,$(OSTYPE)),CYGWIN)
GCC_WIN=1
endif

ifdef DJGPP
LDFLAGS=
else
ifeq ($(findstring BeOS,$(OSTYPE)),BeOS)
LDFLAGS=-nostart
else # Unix or Win GCC
LDFLAGS=-shared
endif
endif

ifeq ($(findstring DJGPP,$(OSTYPE)),)
ifneq ($(GCC_WIN),1)
CFLAGS+=-fPIC
else
# Cygwin and MinGW need an import library for a DLL
LDFLAGS+=-Wl,--out-implib,libcd64dll.a
endif
endif

# The next check is not really specific to FreeBSD or OpenBSD -- the version of
#  gcc I use is just old.
ifeq ($(findstring FreeBSD,$(OSTYPE)),)
ifeq ($(findstring OpenBSD,$(OSTYPE)),)
CFLAGS+=-std=gnu99
endif
endif


DEFAULT_BUILD=1

# If the user passed anything, we are not a default build.

ifdef LIBIEEE1284
DEFAULT_BUILD=0
else
ifdef PPDEV
DEFAULT_BUILD=0
else
ifdef PORTDEV
DEFAULT_BUILD=0
else
ifdef RAWIO
DEFAULT_BUILD=0
endif
endif
endif
endif

ifeq ($(DEFAULT_BUILD),1)
# Put default build options for each OS here

ifeq ($(findstring DJGPP,$(OSTYPE)),DJGPP)
RAWIO=1
endif

ifeq ($(findstring MINGW,$(OSTYPE)),MINGW)
RAWIO=1
endif

ifeq ($(findstring CYGWIN,$(OSTYPE)),CYGWIN)
RAWIO=1
endif

ifeq ($(findstring BeOS,$(OSTYPE)),BeOS)
RAWIO=1
endif

ifeq ($(findstring OpenBSD,$(OSTYPE)),OpenBSD)
# i386_iopl() is located in libi386.a (note the .a)
LIBS+=/usr/lib/libi386.a
LDFLAGS+=-L/usr/lib -li386
RAWIO=1
endif

ifeq ($(findstring FreeBSD,$(OSTYPE)),FreeBSD)
RAWIO=1
endif

ifeq ($(findstring Linux,$(OSTYPE)),Linux)
ifeq ($(shell if test -r /usr/include/ieee1284.h; then echo 1; else echo 0; fi),1)
LIBIEEE1284=1
endif
ifeq ($(shell if test -r /usr/include/linux/ppdev.h; then echo 1; else echo 0; fi),1)
PPDEV=1
endif
PORTDEV=1
RAWIO=1
endif

endif # DEFAULT_BUILD = 1

# Now for backend-specific defs

ifdef LIBIEEE1284
CFLAGS+=-DCD64_USE_LIBIEEE1284
LIBS+=/usr/lib/libieee1284.a
LDFLAGS+=-lieee1284
endif

ifdef PPDEV
CFLAGS+=-DCD64_USE_PPDEV
endif

ifdef PORTDEV
CFLAGS+=-DCD64_USE_PORTDEV
endif

ifdef RAWIO
CFLAGS+=-DCD64_USE_RAWIO
endif

default: all

ifeq ($(findstring DJGPP,$(OSTYPE)),DJGPP)
all:	libcd64.a
else
ifeq ($(GCC_WIN),1)
all:	libcd64.a cd64.dll
else
all:	libcd64.a libcd64.so
endif # GCC_WIN
endif # DJGPP

# libcd64 stuff

cd64io.o: cd64io.c
	$(CC) $(CFLAGS) $^ -c -o $@

cd64lib.o: cd64lib.c
	$(CC) $(CFLAGS) $^ -c -o $@

libcd64.a: cd64lib.o cd64io.o
	ld -r $^ $(LIBS) -o $*.o
#	rm -f $@
	ar crs $@ $*.o

ifeq ($(GCC_WIN),1)
cd64.dll: cd64lib.o cd64io.o
else
libcd64.so: cd64lib.o cd64io.o
endif
#	rm -f $@
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o *.so *.dll *.a a.out
