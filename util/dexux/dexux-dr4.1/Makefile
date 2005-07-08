CC		= gcc
CPP		= gcc -E 
CFLAGS		= -O2 -g # -Wall 
#CFLAGS		= -O2 -s
LIBDIR		= -L.  
LIBS		= -ldexux
RANLIB		= ranlib
INCLUDE 	= -I. -I./libdexux
AR		= ar rc
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
CLEAN_FILES	= *.o *.a *.so dex

LIBOBJS		= libdexux/libdexux.a

OBJS		= dex.o dexdelete.o dexlist.o dexreadfull.o \
		  dexreadsingle.o dexwrite.o dexwritesingle.o \
		  dexreadfile.o dexwritefull.o 

all: dex

dex: $(LIBOBJS) $(OBJS)
	$(CC) $(INCLUDE) -o dex $(OBJS) $(LIBOBJS)

libdexux/libdexux.a: libdexux/*.c
	cd libdexux ; make

dexwritesingle.o: dexwritesingle.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexwritefull.o: dexwritefull.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexreadfile.o: dexreadfile.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexdelete.o: dexdelete.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexlist.o: dexlist.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexreadfull.o: dexreadfull.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexreadsingle.o: dexreadsingle.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dexwrite.o: dexwrite.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

dex.o: dex.c libdexux/libdexux.h
	$(CC) $(INCLUDE) -c $(CFLAGS) -o $*.o $<

clean:
	$(RM) $(CLEAN_FILES) ; cd libdexux ; make clean

