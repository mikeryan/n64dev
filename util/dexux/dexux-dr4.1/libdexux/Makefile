CC		= gcc
CPP		= gcc -E 
CFLAGS		= -O2 -g # -Wall 
#CFLAGS		= -O2 -s
LIBDIR		= -L.  
LIBS		= -ldexux
RANLIB		= ranlib
INCLUDE 	= -I.
AR		= ar rv
RM		= rm -f
MKDIR		= mkdir -p
INSTALL		= /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644
prefix		= /usr/local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
infodir		= ${prefix}/info
mandir		= ${prefix}/man
prog_manext	= 1
conf_manext	= 5
includedir	= ${prefix}/include
CLEAN_FILES	= *.o *.a *.so

LIBOBJS		= libdexux.a

OBJS		= libdexuxbinreverse.o libdexuxclear.o libdexuxclose.o \
		  libdexuxframebottom.o libdexuxframetop.o \
		  libdexuxgetblock.o libdexuxgetframe.o \
		  libdexuxjistoascii.o libdexuxopen.o libdexuxputblock.o \
		  libdexuxputframe.o \
		  libdexuxstatus.o libdexuxstrxor.o libdexuxwake.o

all: libdexux.a

libdexux.a: $(OBJS)
	$(AR) $*.a $(OBJS)

dexuxbinreverse.o: dexuxbinreverse.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxclear.o: dexuxclear.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxclose.o: dexuxclose.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxframebottom.o: dexuxframebottom.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxframetop.o: dexuxframetop.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxgetblock.o: dexuxgetblock.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxgetframe.o: dexuxgetframe.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxjistoascii.o: dexuxjistoascii.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxopen.o: dexuxopen.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxputblock.o: dexuxputblock.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxputframe.o: dexuxputframe.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

# dexuxsettimeout.o: dexuxsettimeout.c libdexux.h
#	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxstatus.o: dexuxstatus.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxstrxor.o: dexuxstrxor.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

dexuxwake.o: dexuxwake.c libdexux.h
	$(CC) -c $(CFLAGS) -o $*.o $<

clean:
	$(RM) $(CLEAN_FILES) 


