/*
 *
 * cd64io.c
 *
 * I/O routines for CD64 device
 *
 * (c) 2004 Ryan Underwood
 * Portions (c) 2004 Daniel Horchner (OpenBSD, FreeBSD, BeOS, Win32, DOS)
 *
 * May be distributed under the terms of the GNU Lesser/Library General Public
 * License, or any later version of the same, as published by the Free Software
 * Foundation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined __unix__ || defined __BEOS__ /* ioctl() */
#include <unistd.h>
#endif

#include <ultra64/host/cd64lib.h>
#include "cd64io.h"

#define DEBUG_LOWLEVEL 0
#define BUSY_THRESHOLD 10000
#define MAX_TRIES 5

#ifdef CD64_USE_RAWIO

#if defined _WIN32 || defined __CYGWIN__
#ifdef __CYGWIN__
#define FILE_SEPARATOR_S "/"
#else
#define snprintf _snprintf
#define FILE_SEPARATOR_S "\\"
#endif

static void *io_driver = NULL;
static int io_driver_found = 0;
/* io.dll */
static char (WINAPI *PortIn)(short int) = NULL;
static void (WINAPI *PortOut)(short int, char) = NULL;
static short int (WINAPI *IsDriverInstalled)() = NULL;
/* DlPortIO.dll */
static unsigned char (__stdcall *DlPortReadPortUchar)(unsigned long) = NULL;
static void (__stdcall *DlPortWritePortUchar)(unsigned long, unsigned char) = NULL;

INLINE uint8_t inb(uint16_t);
INLINE void outb(uint8_t, uint16_t);
static uint8_t (*input_byte)(uint16_t) = inb;
static void (*output_byte)(uint8_t, uint16_t) = outb;
#endif

#ifdef  __BEOS__
static int io_portfd;

typedef struct st_ioport {
	unsigned int port;
	unsigned char data8;
	unsigned short data16;
} st_ioport_t;
#endif

#endif /* CD64_USE_RAWIO */

int cd64_send_byte(struct cd64_t *cd64, uint8_t what) {
	return cd64->xfer(cd64, &what, NULL, 0);
}

int cd64_send_dword(struct cd64_t *cd64, uint32_t what) {

	int ret = 1;
	ret &= cd64_send_byte(cd64, (uint8_t) (what>>24));
	ret &= cd64_send_byte(cd64, (uint8_t) (what>>16));
	ret &= cd64_send_byte(cd64, (uint8_t) (what>>8));
	ret &= cd64_send_byte(cd64, (uint8_t) what);
	return ret;
}

int cd64_grab_byte(struct cd64_t *cd64, uint8_t *val) {
	return cd64->xfer(cd64, NULL, val, 0);
}

int cd64_grab_dword(struct cd64_t *cd64, uint32_t *val) {

	int ret = 1;
	uint8_t grab;
	if (val == NULL) return 0;
	*val = 0;

	ret &= cd64_grab_byte(cd64, &grab);
	*val |= grab << 24;
	ret &= cd64_grab_byte(cd64, &grab);
	*val |= grab << 16;
	ret &= cd64_grab_byte(cd64, &grab);
	*val |= grab << 8;
	ret &= cd64_grab_byte(cd64, &grab);
	*val |= grab;
	return ret;
}

int cd64_trade_bytes(struct cd64_t *cd64, uint8_t give, uint8_t *recv) {
	return cd64->xfer(cd64, &give, recv, 0);
}

/* Backend-specific defs go down here. */

#ifdef CD64_USE_LIBIEEE1284

int cd64_open_ieee1284(struct cd64_t *cd64) {

	struct parport_list pplist;
	int ppflags = F1284_EXCL;
	int ppcaps = 0;
	int i;
	int opened = 0;

	if (cd64->ppdev || !cd64->using_ppa) return 0;

	if (ieee1284_find_ports(&pplist, 0) < 0) {
		cd64->notice_callback2("couldn't get port list\n");
		return 0;
	}

	if (cd64->port < pplist.portc) {
		/* Just use it as an index. */
		cd64->ppdev = pplist.portv[cd64->port];
	}
	else {
		/* Search for the ppdev matching its base address. */
		for (i = 0; i < pplist.portc; i++) {
			if (cd64->port == (int) pplist.portv[i]->base_addr) {
				cd64->ppdev = pplist.portv[i];
			}
		}
	}

	if (cd64->ppdev) {
		if (ieee1284_open(cd64->ppdev, ppflags, &ppcaps) < 0) {
			cd64->notice_callback2("failed opening ieee1284 port %d\n", cd64->port);
			cd64->ppdev = NULL;
		}
		else {
			opened = 1;
		}
	}

	ieee1284_free_ports(&pplist);

	if (opened && ieee1284_claim(cd64->ppdev) < 0) return 0;
	else return opened;
}

int cd64_close_ieee1284(struct cd64_t *cd64) {

	int ret;

	if (cd64->ppdev == NULL) return 1;

	ieee1284_release(cd64->ppdev);
	ret = ieee1284_close(cd64->ppdev);
	if (ret < 0) ret = 0;
	else {
		cd64->ppdev = NULL;
		ret = 1;
	}

	return ret;
}

static INLINE int cd64_wait_ieee(struct cd64_t *cd64) {

	/* With ppdev, could we use an interrupt instead?  The PPA
	 * could be modified... */

	int i = 0;
	int reset_tries = 0;
	while (i < 10000) i++; /* FIXME is this necessary? */
	i = 0;

	while((ieee1284_read_status(cd64->ppdev)^S1284_INVERTED) & S1284_BUSY) {
		i++;
		if (i >= BUSY_THRESHOLD) {
			/* The PPA is in a weird state.
			 * Try to knock some sense into it. */
			ieee1284_write_control(cd64->ppdev, (C1284_NINIT|C1284_NAUTOFD)^C1284_INVERTED);
			ieee1284_write_control(cd64->ppdev, C1284_NINIT^C1284_INVERTED);
			ieee1284_write_control(cd64->ppdev, (C1284_NINIT|C1284_NSTROBE)^C1284_INVERTED);
			ieee1284_write_control(cd64->ppdev, C1284_NINIT^C1284_INVERTED);
			reset_tries++;
			i = 0;
			USLEEP(1);
		}
		if (reset_tries > MAX_TRIES) break;
		if (cd64->abort) return 0;
	}

	return (reset_tries < MAX_TRIES);
}

int cd64_xfer_ieee1284(struct cd64_t *cd64, uint8_t *wr, uint8_t *rd, int delayms) {

	if (!cd64_wait_ieee(cd64)) { return 0; }

	if (delayms) USLEEP(delayms);
	ieee1284_data_dir(cd64->ppdev, 1);
	if (delayms) USLEEP(delayms);
	ieee1284_write_control(cd64->ppdev, (C1284_NINIT|C1284_NAUTOFD)^C1284_INVERTED);
	if (delayms) USLEEP(delayms);
	if (rd) {
		*rd = ieee1284_read_data(cd64->ppdev);
#if DEBUG_LOWLEVEL
		printf("got %xh", *rd);
		if (*rd > 0x20) printf(" (%c)", *rd);
		printf("\n");
#endif
	}

	if (delayms) USLEEP(delayms);
	ieee1284_data_dir(cd64->ppdev, 0);
	if (delayms) USLEEP(delayms);
	ieee1284_write_control(cd64->ppdev, C1284_NINIT^C1284_INVERTED);
	if (delayms) USLEEP(delayms);
	if (wr) {
		ieee1284_write_data(cd64->ppdev, *wr);
#if DEBUG_LOWLEVEL
		printf("put %xh", *wr);
		if (*wr > 0x20) printf(" (%c)", *wr);
		printf("\n");
#endif
	}
	if (delayms) USLEEP(delayms);
	ieee1284_write_control(cd64->ppdev, (C1284_NINIT|C1284_NSTROBE)^C1284_INVERTED);
	if (delayms) USLEEP(delayms);
	ieee1284_write_control(cd64->ppdev, C1284_NINIT^C1284_INVERTED);

	return 1;
}

#endif /* CD64_USE_LIBIEEE1284 */


#ifdef CD64_USE_PPDEV

int cd64_open_ppdev(struct cd64_t *cd64) {

	char *device = "/dev/parport%d";
	char realdev[128+1];

	if (cd64->ppdevfd || !cd64->using_ppa) return 0;
	/* This should be a port number only, not an address */
	if (cd64->port > PARPORT_MAX) return 0;

	snprintf(realdev, 128, device, cd64->port);

	if ((cd64->ppdevfd = open(realdev, O_RDWR)) == -1) {
		cd64->notice_callback2("open: %s", strerror(errno));
		cd64->ppdevfd = 0;
		return 0;
	}

	if (ioctl(cd64->ppdevfd, PPEXCL) != 0) {
		cd64->notice_callback2("PPEXCL: %s", strerror(errno));
		close(cd64->ppdevfd);
		cd64->ppdevfd = 0;
		return 0;
	}

	if (ioctl(cd64->ppdevfd, PPCLAIM) != 0) {
		cd64->notice_callback2("PPCLAIM: %s", strerror(errno));
		close(cd64->ppdevfd);
		cd64->ppdevfd = 0;
		return 0;
	}

	return 1;
}

int cd64_close_ppdev(struct cd64_t *cd64) {

	int ret = 1;

	if (cd64->ppdevfd == 0) return 1;

	if (ioctl(cd64->ppdevfd, PPRELEASE) != 0) {
		cd64->notice_callback2("PPRELEASE: %s", strerror(errno));
		ret = 0;
	}

	close(cd64->ppdevfd);
	cd64->ppdevfd = 0;
	return ret;
}

static INLINE int cd64_wait_ppdev(struct cd64_t *cd64) {

	/* With ppdev, could we use an interrupt instead?  The PPA
	 * could be modified... */

	int i = 0;
	int reset_tries = 0;
	uint8_t status;
	int dir;
	i = 0;

	if (ioctl(cd64->ppdevfd, PPRSTATUS, &status) != 0) cd64->notice_callback2("PPRSTATUS: %s", strerror(errno));

	while(status & 0x80) {
		i++;
		if (i >= BUSY_THRESHOLD) {
			/* The PPA is in a weird state.
			 * Try to knock some sense into it. */
			dir = 1;
			if (ioctl(cd64->ppdevfd, PPDATADIR, &dir) != 0) cd64->notice_callback2("PPDATADIR: %s", strerror(errno));
			status = PARPORT_CONTROL_INIT | PARPORT_CONTROL_AUTOFD; /* 0x26 */
			if (ioctl(cd64->ppdevfd, PPWCONTROL, &status) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));

			dir = 0;
			if (ioctl(cd64->ppdevfd, PPDATADIR, &dir) != 0) cd64->notice_callback2("PPDATADIR: %s", strerror(errno));
			status = PARPORT_CONTROL_INIT; /* 0x04 */
			if (ioctl(cd64->ppdevfd, PPWCONTROL, &status) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
			status = PARPORT_CONTROL_INIT | PARPORT_CONTROL_STROBE; /* 0x05 */
			if (ioctl(cd64->ppdevfd, PPWCONTROL, &status) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
			status = PARPORT_CONTROL_INIT; /* 0x04 */
			if (ioctl(cd64->ppdevfd, PPWCONTROL, &status) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
			reset_tries++;
			i = 0;
			USLEEP(1);
		}
		if (cd64->abort) return 0;
		if (reset_tries > MAX_TRIES) break;

		if (ioctl(cd64->ppdevfd, PPRSTATUS, &status) != 0) cd64->notice_callback2("PPRSTATUS: %s", strerror(errno));
	}

	return (reset_tries < MAX_TRIES);
}

int cd64_xfer_ppdev(struct cd64_t *cd64, uint8_t *wr, uint8_t *rd, int delayms) {

	uint8_t ctl;
	int dir;

	if (!cd64_wait_ppdev(cd64)) { return 0; }

	if (delayms) USLEEP(delayms);
	dir = 1;
	if (ioctl(cd64->ppdevfd, PPDATADIR, &dir) != 0) cd64->notice_callback2("PPDATADIR: %s", strerror(errno));
	if (delayms) USLEEP(delayms);
	ctl = PARPORT_CONTROL_INIT | PARPORT_CONTROL_AUTOFD;
	if (ioctl(cd64->ppdevfd, PPWCONTROL, &ctl) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
	if (delayms) USLEEP(delayms);
	if (rd) {
		if (ioctl(cd64->ppdevfd, PPRDATA, rd) != 0) cd64->notice_callback2("PPRDATA: %s", strerror(errno));
#if DEBUG_LOWLEVEL
		printf("got %xh", *rd);
		if (*rd > 0x20) printf(" (%c)", *rd);
		printf("\n");
#endif
	}

	if (delayms) USLEEP(delayms);
	dir = 0;
	if (ioctl(cd64->ppdevfd, PPDATADIR, &dir) != 0) cd64->notice_callback2("PPDATADIR: %s", strerror(errno));
	if (delayms) USLEEP(delayms);
	ctl = PARPORT_CONTROL_INIT;
	if (ioctl(cd64->ppdevfd, PPWCONTROL, &ctl) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
	if (delayms) USLEEP(delayms);
	if (wr) {
		if (ioctl(cd64->ppdevfd, PPWDATA, wr) != 0) cd64->notice_callback2("PPWDATA: %s", strerror(errno));
#if DEBUG_LOWLEVEL
		printf("put %xh", *wr);
		if (*wr > 0x20) printf(" (%c)", *wr);
		printf("\n");
#endif
	}
	if (delayms) USLEEP(delayms);
	ctl = PARPORT_CONTROL_INIT | PARPORT_CONTROL_STROBE;
	if (ioctl(cd64->ppdevfd, PPWCONTROL, &ctl) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));
	if (delayms) USLEEP(delayms);
	ctl = PARPORT_CONTROL_INIT;
	if (ioctl(cd64->ppdevfd, PPWCONTROL, &ctl) != 0) cd64->notice_callback2("PPWCONTROL: %s", strerror(errno));

	return 1;
}

#endif

#ifdef CD64_USE_PORTDEV

int cd64_open_portdev(struct cd64_t *cd64) {

	if (cd64->portdevfd || cd64->port == 0) return 0;

	if ((cd64->portdevfd = open("/dev/port", O_RDWR)) == -1) {
		cd64->notice_callback2("open: %s", strerror(errno));
		cd64->notice_callback2("portdev requires CAP_SYS_RAWIO capability");
		cd64->portdevfd = 0;
		return 0;
	}

	return 1;
}

int cd64_close_portdev(struct cd64_t *cd64) {

	if (cd64->portdevfd == 0) return 1;

	if (close(cd64->portdevfd) == -1) {
		cd64->notice_callback2("close: %s", strerror(errno));
		return 0;
	}
	cd64->portdevfd = 0;
	return 1;
}

static INLINE int cd64_wait_portdev(struct cd64_t *cd64) {

	int i = 0;
	int reset_tries = 0;
	uint8_t status;
	int dir;
	i = 0;

	if (cd64->using_ppa) {
		lseek(cd64->portdevfd, cd64->port+1, SEEK_SET);
		read(cd64->portdevfd, &status, 1);

		while(status & 0x80) {
			i++;
			if (i >= BUSY_THRESHOLD) {
				/* The PPA is in a weird state.
				 * Try to knock some sense into it. */
				dir = 1;
				status = 0x06 | (dir << 5);
				lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
				write(cd64->portdevfd, &status, 1);

				dir = 0;
				status = 0x04 | (dir << 5);
				lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
				write(cd64->portdevfd, &status, 1);
				status = 0x05 | (dir << 5);
				lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
				write(cd64->portdevfd, &status, 1);
				status = 0x04 | (dir << 5);
				lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
				write(cd64->portdevfd, &status, 1);

				reset_tries++;
				i = 0;
				USLEEP(1);
			}
			if (cd64->abort) return 0;
			if (reset_tries > MAX_TRIES) {
				break;
			}

			lseek(cd64->portdevfd, cd64->port+1, SEEK_SET);
			read(cd64->portdevfd, &status, 1);
		}
	}
	else { /* Comms link */
		lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
		read(cd64->portdevfd, &status, 1);
		while (status & 1) {
			/* Do we need to handle a stuck situation here? */
			lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
			read(cd64->portdevfd, &status, 1);
		}
	}

	return (reset_tries < MAX_TRIES);
}

int cd64_xfer_portdev(struct cd64_t *cd64, uint8_t *wr, uint8_t *rd, int delayms) {

	uint8_t ctl;
	int dir;

	if (cd64->using_ppa) {

		if (!cd64_wait_portdev(cd64)) { return 0; }

		if (delayms) USLEEP(delayms);
		dir = 1;
		ctl = 0x06 | (dir << 5);
		lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
		write(cd64->portdevfd, &ctl, 1);
		if (delayms) USLEEP(delayms);
		if (rd) {
			lseek(cd64->portdevfd, cd64->port, SEEK_SET);
			read(cd64->portdevfd, rd, 1);
#if DEBUG_LOWLEVEL
			printf("got %xh", *rd);
			if (*rd > 0x20) printf(" (%c)", *rd);
			printf("\n");
#endif
		}

		if (delayms) USLEEP(delayms);
		dir = 0;
		ctl = 0x04 | (dir << 5);
		lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
		write(cd64->portdevfd, &ctl, 1);
		if (delayms) USLEEP(delayms);
		if (wr) {
			lseek(cd64->portdevfd, cd64->port, SEEK_SET);
			write(cd64->portdevfd, wr, 1);
#if DEBUG_LOWLEVEL
			printf("put %xh", *wr);
			if (*wr > 0x20) printf(" (%c)", *wr);
			printf("\n");
#endif
		}
		if (delayms) USLEEP(delayms);
		ctl = 0x05 | (dir << 5);
		lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
		write(cd64->portdevfd, &ctl, 1);
		if (delayms) USLEEP(delayms);
		ctl = 0x04 | (dir << 5);
		lseek(cd64->portdevfd, cd64->port+2, SEEK_SET);
		write(cd64->portdevfd, &ctl, 1);
	}
	else { /* Comms link */
		lseek(cd64->portdevfd, cd64->port, SEEK_SET);
		write(cd64->portdevfd, wr, 1);
		if (!cd64_wait_portdev(cd64)) { return 0; }
		lseek(cd64->portdevfd, cd64->port, SEEK_SET);
		read(cd64->portdevfd, rd, 1);
	}

	return 1;
}

#endif


#ifdef CD64_USE_RAWIO

#if defined _WIN32 || defined __CYGWIN__

static void *open_module(char *module_name, struct cd64_t *cd64) {

	void *handle = LoadLibrary(module_name);
	if (handle == NULL) {
		LPTSTR strptr;

		cd64->notice_callback2("LoadLibrary: %s", strerror(errno));
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		              FORMAT_MESSAGE_FROM_SYSTEM |
		              FORMAT_MESSAGE_IGNORE_INSERTS,
		              NULL, GetLastError(),
		              MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR) &strptr, 0, NULL);
		cd64->notice_callback2(strptr);
		LocalFree(strptr);
		exit(1);
	}
	return handle;
}

static void close_module(void *handle, struct cd64_t *cd64) {

	if (!FreeLibrary((HINSTANCE) handle)) {
		LPTSTR strptr;

		cd64->notice_callback2("FreeLibrary: %s", strerror(errno));
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		              FORMAT_MESSAGE_FROM_SYSTEM |
		              FORMAT_MESSAGE_IGNORE_INSERTS,
		              NULL, GetLastError(),
		              MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR) &strptr, 0, NULL);
		cd64->notice_callback2(strptr);
		LocalFree(strptr);
		exit(1);
	}
}

static void *get_symbol(void *handle, char *symbol_name, struct cd64_t *cd64) {

	void *symptr = (void *) GetProcAddress((HINSTANCE) handle, symbol_name);
	if (symptr == NULL) {
		LPTSTR strptr;

		cd64->notice_callback2("GetProcAddress: %s", strerror(errno));
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		              FORMAT_MESSAGE_FROM_SYSTEM |
		              FORMAT_MESSAGE_IGNORE_INSERTS,
		              NULL, GetLastError(),
		              MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR) &strptr, 0, NULL);
		cd64->notice_callback2(strptr);
		LocalFree(strptr);
		exit(1);
	}
	return symptr;
}

/* io.dll */
static uint8_t io_input_byte(uint16_t port) {
	return PortIn(port);
}

static void io_output_byte(uint8_t byte, uint16_t port) {
	PortOut(port, byte);
}

/* DlPortIO.dll */
static uint8_t dlportio_input_byte(uint16_t port) {
	return DlPortReadPortUchar(port);
}

static void dlportio_output_byte(uint8_t byte, uint16_t port) {
	DlPortWritePortUchar(port, byte);
}

#define NODRIVER_MSG "ERROR: No (working) I/O port driver\n"

#ifdef __CYGWIN__
static int new_exception_handler(PEXCEPTION_RECORD exception_record,
                                 void *establisher_frame, PCONTEXT context_record,
                                 void *dispatcher_context) {

	(void) establisher_frame;
	(void) context_record;
	(void) dispatcher_context;
	if (exception_record->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION) {
		fputs(NODRIVER_MSG, stderr);
		exit(1);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
#elif defined _WIN32
static LONG new_exception_filter(LPEXCEPTION_POINTERS exception_pointers) {

	if (exception_pointers->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION) {
		fputs(NODRIVER_MSG, stderr);
		exit(1);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
#endif
#endif /* _WIN32 || __CYGWIN__ */

#if ((defined _WIN32 || defined __CYGWIN__ || defined __BEOS__ || \
     defined __MSDOS__) && \
    (defined __i386__ || defined __x86_64__)) || defined _MSC_VER
INLINE uint8_t inb(uint16_t port) {

#ifdef __MSDOS__
	return inportb(port);
#elif defined __BEOS__
	st_ioport_t temp;

	temp.port = port;
	ioctl(io_portfd, 'r', &temp, 0);

	return temp.data8;
#else /* Win32 */
	if (io_driver_found) {
		return input_byte(port);
	}
	else {
#ifdef _MSC_VER
		return (unsigned char) inp(port);
#else
		unsigned char byte;
		__asm__ __volatile__
		("inb %1, %0"
		  : "=a" (byte)
		  : "d" (port)
		);
		return byte;
#endif
	}
#endif
}

INLINE void outb(uint8_t byte, uint16_t port) {

#ifdef __MSDOS__
	outportb(port, byte);
#elif defined __BEOS__
	st_ioport_t temp;

	temp.port = port;
	temp.data8 = byte;
	ioctl(io_portfd, 'w', &temp, 0);
#else /* Win32 */
	if (io_driver_found) {
		output_byte(byte, port);
	}
	else {
#ifdef _MSC_VER
		outp(port, byte);
#else
		__asm__ __volatile__
		("outb %1, %0"
		  :
		  : "d" (port), "a" (byte)
		);
#endif
	}
#endif
}
#endif /* inb/outb defs */

int cd64_open_rawio(struct cd64_t *cd64) {

	int ret;
	(void) ret;

	/* NOTE: we will soon be able to use ioperm on the entire
	 * 16-bit port range.  Find out what Linux kernels support it. */

	if (cd64->port < 0x200) {
		cd64->notice_callback2("Erroneous port %xh", cd64->port);
		return 0;
	}

#ifdef __linux__
	if (cd64->port < 0x3fd) {
		if (cd64->using_ppa) {
			ret = ioperm(cd64->port, 3, 1);
		}
		else {
			ret = ioperm(cd64->port, 1, 1);
			ret |= ioperm(cd64->port+2, 1, 1);
		}

		if (ret == -1) {
			cd64->notice_callback2("ioperm: %s", strerror(errno));
			cd64->notice_callback2("rawio requires CAP_SYS_RAWIO capability");
			return 0;
		}
	}
	else {
		ret = iopl(3);
		if (ret == -1) {
			cd64->notice_callback2("iopl: %s", strerror(errno));
			cd64->notice_callback2("rawio requires CAP_SYS_RAWIO capability");
			return 0;
		}
	}
#elif defined __OpenBSD__
	/* I cannot test i386_set_ioperm(), so I only use i386_iopl() */
	ret = i386_iopl(3);
	if (ret == -1) {
		cd64->notice_callback2("i386_iopl: %s", strerror(errno));
		return 0;
	}
#elif defined __FreeBSD__
	cd64->portdevfd = open("/dev/io", O_RDWR);
	if (cd64->portdevfd == -1) {
		cd64->portdevfd = 0;
		cd64->notice_callback2("open: %s", strerror(errno));
		cd64->notice_callback2("Could not open I/O port device (/dev/io)");
		return 0;
	}
#elif defined __BEOS__
	io_portfd = open("/dev/misc/ioport", O_RDWR | O_NONBLOCK);
	if (io_portfd == -1) {
		io_portfd = 0;
		cd64->notice_callback2("open: %s", strerror(errno));
		cd64->notice_callback2("Could not open I/O port device (no driver)");
		exit(1);
	}
#elif defined _WIN32 || defined __CYGWIN__
	{
		char fname[FILENAME_MAX];
		io_driver_found = 0;

		if (!cd64->io_driver_dir[0]) strcpy(cd64->io_driver_dir, ".");
		snprintf (fname, FILENAME_MAX, "%s" FILE_SEPARATOR_S "%s",
		          cd64->io_driver_dir, "dlportio.dll");
		if (access(fname, F_OK) == 0) {
			io_driver = open_module(fname, cd64);

			io_driver_found = 1;
			DlPortReadPortUchar = (unsigned char (__stdcall *) (unsigned long))
			                      get_symbol(io_driver, "DlPortReadPortUchar", cd64);
			DlPortWritePortUchar = (void (__stdcall *) (unsigned long, unsigned char))
			                       get_symbol(io_driver, "DlPortWritePortUchar", cd64);
			input_byte = dlportio_input_byte;
			output_byte = dlportio_output_byte;
		}

		if (!io_driver_found) {
			snprintf (fname, FILENAME_MAX, "%s" FILE_SEPARATOR_S "%s",
			          cd64->io_driver_dir, "io.dll");
			if (access(fname, F_OK) == 0) {
				io_driver = open_module(fname, cd64);

				IsDriverInstalled = (short int (WINAPI *) ())
				                    get_symbol(io_driver, "IsDriverInstalled", cd64);
				if (IsDriverInstalled()) {
					io_driver_found = 1;
					PortIn = (char (WINAPI *) (short int))
					         get_symbol(io_driver, "PortIn", cd64);
					PortOut = (void (WINAPI *) (short int, char))
					          get_symbol(io_driver, "PortOut", cd64);
					input_byte = io_input_byte;
					output_byte = io_output_byte;
				}
			}
		}
	}

	{
		/* __try and __except are not supported by MinGW and Cygwin. MinGW has
		 * __try1 and __except1, but using them requires more code than we
		 * currently have. Cygwin does something stupid which breaks
		 * SetUnhandledExceptionFilter()... */
#ifdef __CYGWIN__                               /* Cygwin */
		exception_list list;
		exception_handler *org_handler;
		cygwin_internal(CW_INIT_EXCEPTIONS, &list);
		org_handler = list.handler;
		list.handler = new_exception_handler;
		input_byte(0x378);
		list.handler = org_handler;
#elif defined _WIN32                            /* MinGW & Visual C++ */
		LPTOP_LEVEL_EXCEPTION_FILTER org_exception_filter =
		  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER) new_exception_filter);
		input_byte(0x378);                          /* 0x378 is okay */

		/* if we get here accessing I/O port 0x378 did not cause an exception */
		SetUnhandledExceptionFilter(org_exception_filter);
#endif
	}
#endif /* _WIN32 || __CYGWIN__ */

	return 1;
}

int cd64_close_rawio(struct cd64_t *cd64) {

	int ret;
	(void) ret;
	(void) cd64;

#ifdef __linux__
	if (cd64->port < 0x3fd) {
		if (cd64->using_ppa) {
			ret = ioperm(cd64->port, 3, 0);
		}
		else {
			ret = ioperm(cd64->port, 1, 0);
			ret |= ioperm(cd64->port+2, 1, 0);
		}

		if (ret == -1) {
			cd64->notice_callback2("ioperm: %s", strerror(errno));
			return 0;
		}
	}
	else {
		ret = iopl(0);
		if (ret == -1) {
			cd64->notice_callback2("iopl: %s", strerror(errno));
			return 0;
		}
	}
#elif defined __OpenBSD__
	/* I cannot test i386_set_ioperm(), so I only use i386_iopl() */
	ret = i386_iopl(0);
	if (ret == -1) {
		cd64->notice_callback2("i386_iopl: %s", strerror(errno));
		return 0;
	}
#elif defined __FreeBSD__
	if (close(cd64->portdevfd) == -1) {
		cd64->notice_callback2("close: %s", strerror(errno));
		return 0;
	}
	cd64->portdevfd = 0;
#elif defined __BEOS__
	if (close(io_portfd) == -1) {
		cd64->notice_callback2("close: %s", strerror(errno));
		return 0;
	}
	io_portfd = 0;
#elif defined _WIN32 || defined __CYGWIN__
	close_module(io_driver, cd64);
	io_driver = NULL;
	io_driver_found = 0;
	input_byte = inb;
	output_byte = outb;
#endif

	return 1;
}

static INLINE int cd64_wait_rawio(struct cd64_t *cd64) {

	int i = 0;
	int reset_tries = 0;
	uint8_t status;
	int dir;
	i = 0;

	if (cd64->using_ppa) {
		status = inb((uint16_t) (cd64->port+1));

		while(status & 0x80) {
			i++;
			if (i >= BUSY_THRESHOLD) {
				/* The PPA is in a weird state.
				 * Try to knock some sense into it. */
				dir = 1;
				status = 0x06 | (dir << 5);
				outb(status, (uint16_t) (cd64->port+2));

				dir = 0;
				status = 0x04 | (dir << 5);
				outb(status, (uint16_t) (cd64->port+2));
				status = 0x05 | (dir << 5);
				outb(status, (uint16_t) (cd64->port+2));
				status = 0x04 | (dir << 5);
				outb(status, (uint16_t) (cd64->port+2));

				reset_tries++;
				i = 0;
				USLEEP(1);
			}
			if (cd64->abort) return 0;
			if (reset_tries > MAX_TRIES) {
				break;
			}

			status = inb((uint16_t) (cd64->port+1));
		}
	}
	else { /* Comms link */
		status = inb((uint16_t) (cd64->port+2));
		while (status & 1) {
			/* Do we need to handle a stuck situation here? */
			status = inb((uint16_t) (cd64->port+2));
		}
	}

	return (reset_tries < MAX_TRIES);
}

int cd64_xfer_rawio(struct cd64_t *cd64, uint8_t *wr, uint8_t *rd, int delayms) {

	uint8_t ctl;
	int dir;

	if (cd64->using_ppa) {

		if (!cd64_wait_rawio(cd64)) { return 0; }

		if (delayms) USLEEP(delayms);
		dir = 1;
		ctl = 0x06 | (dir << 5);
		outb(ctl, (uint16_t) (cd64->port+2));
		if (delayms) USLEEP(delayms);
		if (rd) {
			*rd = inb((uint16_t) cd64->port);
#if DEBUG_LOWLEVEL
			printf("got %xh", *rd);
			if (*rd > 0x20) printf(" (%c)", *rd);
			printf("\n");
#endif
		}

		if (delayms) USLEEP(delayms);
		dir = 0;
		ctl = 0x04 | (dir << 5);
		outb(ctl, (uint16_t) (cd64->port+2));
		if (delayms) USLEEP(delayms);
		if (wr) {
			outb(*wr, (uint16_t) cd64->port);
#if DEBUG_LOWLEVEL
			printf("put %xh", *wr);
			if (*wr > 0x20) printf(" (%c)", *wr);
			printf("\n");
#endif
		}
		if (delayms) USLEEP(delayms);
		ctl = 0x05 | (dir << 5);
		outb(ctl, (uint16_t) (cd64->port+2));
		if (delayms) USLEEP(delayms);
		ctl = 0x04 | (dir << 5);
		outb(ctl, (uint16_t) (cd64->port+2));
	}
	else { /* Comms link */
		outb(*wr, (uint16_t) cd64->port);
		if (!cd64_wait_rawio(cd64)) { return 0; }
		*rd = inb((uint16_t) cd64->port);
	}

	return 1;
}

#endif
