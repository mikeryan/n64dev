/*
 *
 * cd64lib.c
 *
 * Library routines for CD64 handling
 *
 * (c) 2004 Ryan Underwood
 * Portions (c) 2004 Daniel Horchner (Win32, read/write/seek callbacks)
 *
 * May be distributed under the terms of the GNU Lesser/Library General
 * Public License, or any later version of the same, as published by the Free
 * Software Foundation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined __unix__ || defined __BEOS__
#include <unistd.h>
#endif
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#include <ultra64/host/cd64lib.h>
#include <ultra64/rom.h>
#include <ultra64/cartmem.h>

#include "cd64io.h"

static uint8_t *cd64_tmp_buf;
static uint32_t cd64_tmp_buf_offset;

static int cd64_notice_helper(FILE *file, const char *format, va_list argptr) {

	int n_chars;

	fputs("libcd64: ", file);
	n_chars = vfprintf(file, format, argptr);
	fputc('\n', file);
	fflush(file);

	return n_chars;
}

int cd64_notice(const char *format, ...) {

	va_list argptr;
	int n_chars;

	va_start(argptr, format);
	n_chars = cd64_notice_helper(stdout, format, argptr);
	va_end(argptr);

	return n_chars;
}

int cd64_notice2(const char *format, ...) {

	va_list argptr;
	int n_chars;

	va_start(argptr, format);
	n_chars = cd64_notice_helper(stderr, format, argptr);
	va_end(argptr);

	return n_chars;
}

int cd64_read(void *io_id, void *buffer, uint32_t size) {
	return fread(buffer, 1, size, (FILE *) io_id);
}

int cd64_write(void *io_id, void *buffer, uint32_t size) {
	return fwrite(buffer, 1, size, (FILE *) io_id);
}

int32_t cd64_tell(void *io_id) {
	return (int32_t) ftell((FILE *) io_id);
}

int cd64_seek(void *io_id, int32_t offset, int whence) {
	return fseek((FILE *) io_id, offset, whence);
}

int cd64_create(struct cd64_t *cd64, method_t method,
			uint16_t port, protocol_t protocol, int ppa) {

	cd64->using_ppa = (ppa)? 1 : 0;
	cd64->protocol = protocol;
	cd64->port = port;

	if (!cd64->notice_callback) cd64->notice_callback = cd64_notice;
	if (!cd64->notice_callback2) cd64->notice_callback2 = cd64_notice2;

	cd64->read_callback = cd64_read;
	cd64->write_callback = cd64_write;
	cd64->tell_callback = cd64_tell;
	cd64->seek_callback = cd64_seek;

#ifdef CD64_USE_LIBIEEE1284
	if (method == LIBIEEE1284) {
		cd64->devopen = cd64_open_ieee1284;
		cd64->xfer = cd64_xfer_ieee1284;
		cd64->devclose = cd64_close_ieee1284;
		return 1;
	}
	else
#endif

#ifdef CD64_USE_PPDEV
	if (method == PPDEV) {
		cd64->devopen = cd64_open_ppdev;
		cd64->xfer = cd64_xfer_ppdev;
		cd64->devclose = cd64_close_ppdev;
		return 1;
	}
	else
#endif

#ifdef CD64_USE_PORTDEV
	if (method == PORTDEV) {
		cd64->devopen = cd64_open_portdev;
		cd64->xfer = cd64_xfer_portdev;
		cd64->devclose = cd64_close_portdev;
		return 1;
	}
	else
#endif

#ifdef CD64_USE_RAWIO
	if (method == RAWIO) {
		cd64->devopen = cd64_open_rawio;
		cd64->xfer = cd64_xfer_rawio;
		cd64->devclose = cd64_close_rawio;
		return 1;
	}
	else
#endif

	{
		switch (method) {
			case LIBIEEE1284:
				cd64->notice_callback2("libieee1284 not supported.");
				break;
			case PPDEV:
				cd64->notice_callback2("ppdev not supported.");
				break;
			case PORTDEV:
				cd64->notice_callback2("portdev not supported.");
				break;
			case RAWIO:
				cd64->notice_callback2("rawio not supported.");
				break;
			default:
				cd64->notice_callback2("unknown hw access method.");
				break;
		}
		return 0;
	}

}

/* CD64 BIOS routines */

static int cd64_bios_sync(struct cd64_t *cd64) {

	unsigned char ret1 = 0, ret2 = 0;

	cd64->notice_callback("Waiting for CD64 comms to come online...");

	while (!(ret1 == 'R' && ret2 == 'W')) {
		if (cd64->abort) return 0;
                /* approximation here */
		cd64_send_byte(cd64, 'W');
		cd64_send_byte(cd64, 'B');
		cd64_trade_bytes(cd64, 'B', &ret1);
		cd64_trade_bytes(cd64, 'B', &ret2);
	}
	return 1;
}

int cd64_bios_grab(struct cd64_t *cd64, void *io_id, uint32_t addr,
		uint32_t length, int *elapsed) {

	unsigned int i;
	unsigned short pc_csum = 0, n64_csum = 0;
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
	struct timeb tb;
#else
	struct timeval tv;
#endif
	unsigned long int sec = 0, usec = 0;
	uint8_t send, recv;
	uint8_t cmd = 0xff;
	uint8_t tmp;

	if (!length || length&0x00000003 || addr&0x00000003) return 0;
	if (addr <= 0xa03fffff && addr+length <= 0xa03fffff) {
		cmd = BIOS_DUMP_N64;
	}
	else if (addr >= 0xa8000000 && addr <= 0xbfffffff
			&& addr+length <= 0xbfffffff) {
		cmd = BIOS_DUMP_PI;
	}

	if (cmd == 0xff) {
		cd64->notice_callback2("Invalid memory range %lxh-%lxh for operation.",
		                       (long unsigned int) addr, (long unsigned int) addr+length);
		return 0;
	}

	/* Try to get in sync with the CD64
	 * We use a delay here to give the PPA a chance to power up. */
	send = 0xff;
	cd64->xfer(cd64, &send, NULL, 1000);
	cd64_bios_sync(cd64);

	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		sec = tb.time;
		usec = tb.millitm*1000;
#else
		gettimeofday(&tv, 0);
		sec = tv.tv_sec;
		usec = tv.tv_usec;
#endif
	}

	cd64_send_byte(cd64, cmd);
	cd64_send_dword(cd64, addr);
	cd64_send_dword(cd64, length);

	/* dummy exchange, needed for some reason */
	cd64_grab_byte(cd64, &recv);

	for (i = 1; i <= length; i++) {

		if (cd64->abort) return 0;
		if (!cd64_grab_byte(cd64, &tmp)) return 0;
		if (!cd64->write_callback(io_id, &tmp, 1)) {
			cd64->notice_callback2("Error writing to output.");
			return 0;
		}
		pc_csum += tmp;
		if ((i % CD64_BUFFER_SIZE == 0) && cd64->progress_callback) {
			cd64->progress_callback(i, length);
		}
	}

	if (cd64->progress_callback) {
		cd64->progress_callback(i, length);
	}

	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		*elapsed = ((tb.time - sec)*1000000) + ((tb.millitm*1000) - usec);
#else
		gettimeofday(&tv, 0);
		*elapsed = ((tv.tv_sec - sec)*1000000) + (tv.tv_usec - usec);
#endif
	}

	pc_csum &= 0xfff;
	cd64_trade_bytes(cd64, 0, &recv);
	n64_csum = recv << 8;
	cd64_trade_bytes(cd64, 0, &recv);
	n64_csum += recv;
	n64_csum &= 0xfff;

/*	debug("\nVerifying checksum: pcsum = %d, n64sum = %d\n", pc_csum, n64_csum); */
	if (pc_csum^n64_csum) return -1;
	else return 1;
}

int cd64_bios_send(struct cd64_t *cd64, void *io_id, uint32_t addr,
		uint32_t length, int *elapsed, int cmd) {

	unsigned int i;
	uint16_t pc_csum = 0;
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
	struct timeb tb;
#else
	struct timeval tv;
#endif
	unsigned long int sec = 0, usec = 0;
	uint8_t send;
	uint8_t buf[4];
	int valid = 1;
	uint8_t send_bogus_csum = 0;
	uint8_t tmp;
	int32_t pos;

	if (!io_id || !length || length&0x00000003 || addr&0x00000003) return 0;
	if (cmd != BIOS_TRANSFER_PI && cmd != BIOS_EXECUTE_PI && cmd != BIOS_TRANSFER_N64
			&& !(cd64->protocol == GHEMOR && cmd == GHEMOR_TRANSFER_PROGRAM)) return 0;

	if (cmd == BIOS_TRANSFER_PI || cmd == BIOS_EXECUTE_PI) {
		if (addr < 0xa8000000 || addr > 0xbfffffff ||
				addr+length > 0xbfffffff) valid = 0;
	}
	else if (cmd == BIOS_TRANSFER_N64) {
		if (addr < 0xa0000000 || addr > 0xa03fffff
				|| addr+length > 0xa03fffff) valid = 0;

		if (addr != BIOS_TEMP_RAM || length > 0x80000) {
			/* FIXME: is 0x80000 (512Kbit) really all the CD64
			 * BIOS will copy from a mempak?  See if it copies
			 * 1Mbit from a 4x linear card.  */

			/* We are sending somewhere in RAM besides the CD64's
			 * scratch area.  We will send a bogus checksum so the
			 * CD64 doesn't try to run it or copy it to a mempak.
			 */
			send_bogus_csum = 1;
		}
	}

	if (!valid) {
		cd64->notice_callback2("Invalid address %lxh for operation.",
		                       (long unsigned int) addr);
		return 0;
	}

	if (cd64->protocol == GHEMOR && addr != 0xb4000000 &&
			(cmd == BIOS_TRANSFER_PI || cmd == BIOS_EXECUTE_PI)) {
		/* They might try to send to Ghemor in BIOS mode, but
		 * oh well.  Warn them if we know it's Ghemor. */
		cd64->notice_callback("Ignoring address %lxh != 0xb4000000 for Ghemor.",
		                      (long unsigned int) addr);
	}
	if (cmd == GHEMOR_TRANSFER_PROGRAM) {
		cd64->notice_callback("Ghemor ignores this command currently...");
	}


	/* Try to get in sync with the CD64
	 * We use a delay here to give the PPA a chance to power up. */
	send = 0xff;
	cd64->xfer(cd64, &send, NULL, 1000);
	cd64_bios_sync(cd64);

	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		sec = tb.time;
		usec = tb.millitm*1000;
#else
		gettimeofday(&tv, 0);
		sec = tv.tv_sec;
		usec = tv.tv_usec;
#endif
	}

	cd64_send_byte(cd64, (uint8_t) cmd);
	cd64_send_dword(cd64, addr); /* Ghemor ignores this */
	cd64_send_dword(cd64, length);

	pos = cd64->tell_callback(io_id);

	i = 0;
	while (i < length) {

		if (cd64->abort) return 0;
		cd64->read_callback(io_id, &tmp, 1);
		pc_csum += tmp;
		pc_csum &= 0xfff;
		if (!cd64_send_byte(cd64, tmp)) {
			if (cd64->protocol == CD64BIOS) {
				/* Probably the BIOS was stupid and dropped a
				 * send as it likes to.  Try to recover from
				 * a convenient point. */
				while (i % 4 != 0) i--;
				cd64->seek_callback(io_id, pos+i, SEEK_SET);
				cd64->read_callback(io_id, &tmp, 1);
				cd64->notice_callback("Trying to recover dropped send, waiting for CD64...");
				cd64_bios_sync(cd64);
				cd64_send_byte(cd64, (uint8_t) cmd);
				cd64_send_dword(cd64, addr+i);
				cd64_send_dword(cd64, length-i);

				if (!cd64_send_byte(cd64, tmp)) {
					/* Oh well, at least we tried. */
					return -1;
				}

				/* Unfortunately we can only calculate a checksum
				 * from _this_ point onward. */
				pc_csum = tmp;

				/* Now continue as if nothing ever happened. */
			}
			else {
				cd64->notice_callback2("Send dropped, unable to recover.");
				return 0;
			}
		}

		if ((i % CD64_BUFFER_SIZE == 0) && cd64->progress_callback) {
			cd64->progress_callback(i, length);
		}
		i++;
	}
	if (cd64->progress_callback) {
		cd64->progress_callback(i, length);
	}

	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		*elapsed = ((tb.time - sec)*1000000) + ((tb.millitm*1000) - usec);
#else
		gettimeofday(&tv, 0);
		*elapsed = ((tv.tv_sec - sec)*1000000) + (tv.tv_usec - usec);
#endif
	}

/*	debug("checksum: 0x%x\n",pc_csum) */
	cd64_send_byte(cd64, (unsigned char)((pc_csum & 0xff00) >> 8));
/*	debug("Sent checksum high byte: 0x%x\n",(unsigned char)pc_csum>>8); */
	cd64_send_byte(cd64, (uint8_t) ((pc_csum & 0xff) + send_bogus_csum));
/*	debug("Sent checksum low byte: 0x%x\n",pc_csum); */
	cd64_grab_byte(cd64, &buf[2]);
	cd64_trade_bytes(cd64, 0, &buf[0]);
	cd64_trade_bytes(cd64, 0, &buf[1]);

/*	debug("Got: (dummy) 0x%x,  0x%x (%c), 0x%x (%c)",buf[2], buf[0], buf[0], buf[1],buf[1]); */
	if (buf[0]=='O' && buf[1]=='K') return 1;
	else if (send_bogus_csum) {
		cd64->notice_callback("No way to verify integrity of data in N64 RAM.");
		return 1;
	}
	return -1;
}

int cd64_ghemor_grab(struct cd64_t *cd64, void *io_id, uint8_t slow, int *elapsed) {

	int ret;
	uint8_t tmp;
	int sec = 0, usec = 0;
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
	struct timeb tb;
#else
	struct timeval tv;
#endif
	uint32_t len;
	uint16_t mycsum = 0;
	uint16_t cd64csum;
	unsigned int i;

	if (slow) {
		cd64->notice_callback2("Ghemor slow receive feature not supported.");
		return 0;
	}
	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		sec = tb.time;
		usec = tb.millitm*1000;
#else
		gettimeofday(&tv, 0);
		sec = tv.tv_sec;
		usec = tv.tv_usec;
#endif
	}

	cd64_send_byte(cd64, slow);
	i = 0;
	while (cd64_grab_byte(cd64, &tmp) && tmp != 1) {
		i++;
		if (i > 25) {
			cd64->notice_callback2("Ghemor was not ready.");
			return 0;
		}
	}

	cd64_grab_dword(cd64, &len);
	cd64->notice_callback("Downloading %lu megabits via Ghemor.",
	                      (long unsigned int) len/BYTES_IN_MBIT);
	for (i = 0; i < len; i++) {
		if (!cd64_grab_byte(cd64, &tmp)) return 0;
		if (!cd64->write_callback(io_id, &tmp, 1)) {
			cd64->notice_callback2("Error writing to output.");
			return 0;
		}
		mycsum += tmp;
		mycsum &= 0xfff;
		if ((i % CD64_BUFFER_SIZE == 0) && cd64->progress_callback) {
			cd64->progress_callback(i, len);
		}
	}
	if (cd64->progress_callback) {
		cd64->progress_callback(i, len);
	}

	cd64_grab_byte(cd64, &tmp);
	cd64csum = tmp << 8;
	cd64_grab_byte(cd64, &tmp);
	cd64csum |= tmp;
	if (mycsum^cd64csum) ret = -1;
	else ret = 1;
	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		*elapsed = ((tb.time - sec)*1000000) + ((tb.millitm*1000) - usec);
#else
		gettimeofday(&tv, 0);
		*elapsed = ((tv.tv_sec - sec)*1000000) + (tv.tv_usec - usec);
#endif
	}

	return ret;
}

int cd64_ghemor_send(struct cd64_t *cd64, void *io_id, uint32_t length,
		int *elapsed) {

	int sec = 0, usec = 0;
	uint16_t mycsum = 0;
	unsigned int i;
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
	struct timeb tb;
#else
	struct timeval tv;
#endif
	uint8_t tmp;

	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		sec = tb.time;
		usec = tb.millitm*1000;
#else
		gettimeofday(&tv, 0);
		sec = tv.tv_sec;
		usec = tv.tv_usec;
#endif
	}

	cd64_send_byte(cd64, 0); /* No slow mode for sends */
	i = 0;
	while (cd64_grab_byte(cd64, &tmp) && tmp != 1) {
		i++;
		if (i > 25) {
			cd64->notice_callback2("Ghemor was not ready.");
			return 0;
		}
	}

	cd64_send_dword(cd64, length);
	for (i = 0; i < length; i++) {
		if (!cd64->read_callback(io_id, &tmp, 1)) {
			cd64->notice_callback2("Error reading from input.");
			return 0;
		}
		if (!cd64_send_byte(cd64, tmp)) return 0;
		mycsum += tmp;
		mycsum &= 0xfff;
		if ((i % CD64_BUFFER_SIZE == 0) && cd64->progress_callback) {
			cd64->progress_callback(i, length);
		}
	}
	if (cd64->progress_callback) {
		cd64->progress_callback(i, length);
	}
	cd64_send_byte(cd64, (uint8_t)((mycsum << 8) & 0xff));
	cd64_send_byte(cd64, (uint8_t)(mycsum & 0xff));
	if (elapsed != NULL) {
#if (defined _WIN32 && !defined __CYGWIN__) || defined __MSDOS__
		ftime(&tb);
		*elapsed = ((tb.time - sec)*1000000) + ((tb.millitm*1000) - usec);
#else
		gettimeofday(&tv, 0);
		*elapsed = ((tv.tv_sec - sec)*1000000) + (tv.tv_usec - usec);
#endif
	}

	return 1;
}


/* Generic API functions */

int cd64_upload_dram(struct cd64_t *cd64, FILE *source, uint32_t length,
			int *elapsed, int exec) {

	if (cd64->protocol == CD64BIOS || cd64->protocol == GHEMOR) {
		int cmd;
		if (exec == 1) cmd = BIOS_EXECUTE_PI;
		else cmd = BIOS_TRANSFER_PI;

		if (cd64->protocol == CD64BIOS && length == 0) {
			cd64->notice_callback2("CD64 BIOS needs a file length.");
			return 0;
		}
		if (cd64->protocol == CD64BIOS) {
			cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		}

		return cd64_bios_send(cd64, source, 0xb4000000, length, elapsed, cmd);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_bootemu(struct cd64_t *cd64, FILE *infile, uint32_t length, int *elapsed) {

	if (cd64->protocol == CD64BIOS) {

		if (cd64->protocol == CD64BIOS && length == 0) {
			cd64->notice_callback2("CD64 BIOS needs a file length.\n");
			return 0;
		}

		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_send(cd64, infile, BIOS_TEMP_RAM, length, elapsed,
		                      BIOS_TRANSFER_N64);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_EXECUTE_BOOTEMU);
		return cd64_ghemor_send(cd64, infile, length, elapsed);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_ram(struct cd64_t *cd64, FILE *infile, uint32_t length,
		int *elapsed, uint32_t address) {

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_send(cd64, infile, address, length,
		                      elapsed, BIOS_TRANSFER_N64);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_mempak(struct cd64_t *cd64, FILE *infile, int8_t which) {

	int32_t len;
	int32_t pos = cd64->tell_callback(infile);
	cd64->seek_callback(infile, 0, SEEK_END);
	len = cd64->tell_callback(infile);
	cd64->seek_callback(infile, pos, SEEK_SET);
	if (len != CONTROLLER_MEMPAK_LENGTH) {
		cd64->notice_callback("Mempack file must be %d bytes, not %d.",
		                      CONTROLLER_MEMPAK_LENGTH, len);
	}

	if (cd64->protocol == CD64BIOS) {
		if (which != -1) {
			cd64->notice_callback2("CD64 BIOS doesn't let mempak index be chosen.");
			return 0;
		}
		cd64->notice_callback("Choose Memory Manager->Up/Download Pak.");
		return cd64_bios_send(cd64, infile, BIOS_TEMP_RAM, CONTROLLER_MEMPAK_LENGTH,
		                      NULL, BIOS_TRANSFER_N64);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESTORE_MEMPAK);
		cd64_send_byte(cd64, which);
		return cd64_ghemor_send(cd64, infile, CONTROLLER_MEMPAK_LENGTH, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_sram(struct cd64_t *cd64, FILE *infile) {

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_send(cd64, infile, 0xa8000000, CART_SRAM_LENGTH,
		                      NULL, BIOS_TRANSFER_PI);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESTORE_SRAM);
		return cd64_ghemor_send(cd64, infile, CART_SRAM_LENGTH, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_flashram(struct cd64_t *cd64, FILE *infile) {

	/* Urm, we need to figure out if this really works.  Probably, CTR
	 * needs to release a new Ghemor version.  Maybe it works with
	 * CD64 BIOS but probably not. */

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_send(cd64, infile, 0xa8000000, CART_FLASHRAM_LENGTH,
		                      NULL, BIOS_TRANSFER_PI);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESTORE_FLASHRAM);
		return cd64_ghemor_send(cd64, infile, CART_FLASHRAM_LENGTH, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_upload_eeprom(struct cd64_t *cd64, FILE *infile) {

	/* Check the size of the EEPROM data first */

	int32_t origpos = cd64->tell_callback(infile);
	int32_t length;

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback2("CD64 BIOS can only transfer EEPROM through BRAM Manager.");
		return 0;
	}

	cd64->seek_callback(infile, 0, SEEK_END);
	length = cd64->tell_callback(infile);
	cd64->seek_callback(infile, origpos, SEEK_SET);

	if (length != CART_EEPROM_LENGTH && length != CART_2XEEPROM_LENGTH) {
		cd64->notice_callback2("Wrong length of EEPROM data: %d bytes", (int) length);
		return 0;
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESTORE_EEPROM);
		return cd64_ghemor_send(cd64, infile, length, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

static int cd64_write_mem(void *dummy, void *buffer, uint32_t size) {

	(void) dummy;
	memcpy (cd64_tmp_buf + cd64_tmp_buf_offset, buffer, size);
	cd64_tmp_buf_offset += size;
	return size;
}

int cd64_download_header(struct cd64_t *cd64, n64header_t *head, uint32_t location) {

	if (cd64->protocol == CD64BIOS) {
		int size = sizeof(n64header_t);
		int ret;
		int (*org_write_cb)(void *, void *, uint32_t) = cd64->write_callback;

		while (size % 4 != 0) size++;
		if (!head) return 0;
		cd64_tmp_buf = (uint8_t *) head;
		cd64_tmp_buf_offset = 0;
		cd64->write_callback = cd64_write_mem;
		ret = cd64_bios_grab(cd64, (void *) -1, location, size, NULL); /* -1 is just a random (non-zero) value */
		cd64->write_callback = org_write_cb; /* restore original callback */
		return 1;
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_cart(struct cd64_t *cd64, FILE *outfile, uint32_t length,
			int *elapsed) {

	if (cd64->protocol == CD64BIOS) {
		int ret;
		unsigned int i;
		int32_t curpos = 0;
		int32_t origpos = cd64->tell_callback(outfile);
		if (length == 0) {
			cd64->notice_callback2("CD64 BIOS needs a file length.");
			return 0;
		}
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");

		ret = cd64_bios_grab(cd64, outfile, 0xb2000000, length, elapsed);
		/* Scan through the file at 8MBit intervals to
		 * see if we overdumped.  If we did, truncate the
		 * file.  */
		i = 0;
		cd64->seek_callback(outfile, origpos, SEEK_SET);
		cd64->notice_callback("Checking for overdump...");
		while (i < length) {
			int j = 0;
			int overdump = 1;
			uint8_t buf[4];

			curpos = cd64->tell_callback(outfile);

			while(i+j < length) {
				cd64->read_callback(outfile, &buf, 4);

				/* To elaborate on what we are checking here:
				 * When the CD64 accesses an address which is not
				 * decoded, in each 32-bit word is the lower 16 bits
				 * of the address of that 32-bit word, repeated twice.
				 * The pattern therefore looks like:
				 * 00 00 00 00 00 04 00 04 00 08 00 08 00 0c 00 0c
				 * and continues on like that.  This pattern is what
				 * we are looking for here.  It is possible, but
				 * extremely unlikely, that this pattern appears in a
				 * actual game and begins on a 8Mbit boundary too. */

				if (
					   ((uint8_t*)buf)[0] != ((j >> 8) & 0xff)
					|| ((uint8_t*)buf)[1] != (j & 0xff)
					|| ((uint8_t*)buf)[2] != ((j >> 8) & 0xff)
					|| ((uint8_t*)buf)[3] != (j & 0xff)
				) {
					overdump = 0;
					break;
				}
				else {
					j+=4;
				}
			}

			if (overdump) {
				break;
			}
			i += 0x100000;
			cd64->seek_callback(outfile, curpos+0x100000, SEEK_SET);
		}

		if (i < length) {
			cd64->notice_callback("File apparently overdumped.");
#if (!defined _WIN32 || defined __CYGWIN__)
			/* Don't call ftruncate() if the user installed a callback, because
			 * outfile may not be a real FILE *. */
			if (cd64->read_callback == cd64_read) {
				cd64->notice_callback("Truncating to %dMbits.", i/BYTES_IN_MBIT);
				ftruncate(fileno(outfile), curpos+i);
			}
#endif
		}
		return ret;
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_DUMP_CART);
		return cd64_ghemor_grab(cd64, outfile, 0, elapsed);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_dram(struct cd64_t *cd64, FILE *outfile, uint32_t start,
			uint32_t end, int *elapsed) {

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_grab(cd64, outfile, 0xb4000000, end-start, elapsed);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_sram(struct cd64_t *cd64, FILE *outfile) {

	if (cd64->protocol == CD64BIOS) {
		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		return cd64_bios_grab(cd64, outfile, CART_SRAM_ADDR, CART_SRAM_LENGTH, NULL);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_DUMP_SRAM);
		return cd64_ghemor_grab(cd64, outfile, 0, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_flashram(struct cd64_t *cd64, FILE *outfile) {

	/* We might be able to support CD64 BIOS here.  Needs testing. */
	if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_DUMP_FLASH);
		return cd64_ghemor_grab(cd64, outfile, 0, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_eeprom(struct cd64_t *cd64, FILE *outfile) {

	if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_DUMP_EEPROM);
		return cd64_ghemor_grab(cd64, outfile, 0, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_download_mempak(struct cd64_t *cd64, FILE *outfile, int8_t which) {

	if (cd64->protocol == CD64BIOS) {
		if (which != -1) {
			cd64->notice_callback2("CD64 BIOS doesn't let mempak index be chosen.");
			return 0;
		}
		cd64->notice_callback("Choose Memory Manager->Up/Download Pak.");
		return cd64_bios_grab(cd64, outfile, BIOS_TEMP_RAM, CONTROLLER_MEMPAK_LENGTH, NULL);
	}
	else if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_DUMP_MEMPAK);
		cd64_send_byte(cd64, which);
		return cd64_ghemor_grab(cd64, outfile, 0, NULL);
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

static int cd64_read_mem(void *dummy, void *buffer, uint32_t size) {

	(void) dummy;
	memcpy (buffer, cd64_tmp_buf + cd64_tmp_buf_offset, size);
	cd64_tmp_buf_offset += size;
	return size;
}

static int32_t cd64_tell_mem(void *dummy) {

	(void) dummy;
	return cd64_tmp_buf_offset;
}

static int cd64_seek_mem(void *dummy, int32_t offset, int whence) {

	(void) dummy;
	(void) whence; /* only called with SEEK_SET */
	cd64_tmp_buf_offset = offset;
	return 0;
}

int cd64_run_dram(struct cd64_t *cd64) {

	if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESET_DRAM);
		return 1;
	}
	else if (cd64->protocol == CD64BIOS) {
		/* Heh. Write some dummy bytes to the cart area. We
		 * can't just send a zero length because the CD64
		 * BIOS gives "File length error". */
		uint8_t dummy[4] = { 0, 0, 0, 0 };
		int ret;
		int (*org_read_cb)(void *, void *, uint32_t) = cd64->read_callback;
		int32_t (*org_tell_cb)(void *) = cd64->tell_callback;
		int (*org_seek_cb)(void *, int32_t, int) = cd64->seek_callback;

		cd64->notice_callback("Choose CD64 Tools->Pro Comms Link.");
		cd64_tmp_buf = dummy;
		cd64_tmp_buf_offset = 0;
		cd64->read_callback = cd64_read_mem;
		cd64->tell_callback = cd64_tell_mem;
		cd64->seek_callback = cd64_seek_mem;
		ret = cd64_bios_send(cd64, (void *) -1, 0xb2000000, 4, NULL, BIOS_EXECUTE_PI); /* -1 is just a random (non-zero) value */
		cd64->read_callback = org_read_cb; /* restore original callbacks */
		cd64->tell_callback = org_tell_cb;
		cd64->seek_callback = org_seek_cb;
		return ret;
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}

int cd64_run_cart(struct cd64_t *cd64) {

	if (cd64->protocol == GHEMOR) {
		cd64_bios_sync(cd64);
		cd64_send_byte(cd64, GHEMOR_RESET_CART);
		return 1;
	}
	cd64->notice_callback2("Operation not supported by protocol.");
	return 0;
}
