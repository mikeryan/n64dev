#ifndef __CD64LIB_H__
#define __CD64LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CD64_BUFFER_SIZE 32768

/* This is the only public header file for cd64lib. */

#if __STDC_VERSION >= 19990L
#include <stdint.h>
#else
#if !(defined __MSDOS__ || defined _MSC_VER)
#include <inttypes.h>
#else
#ifndef OWN_INTTYPES
#define OWN_INTTYPES /* signal that these are defined */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
#ifndef _MSC_VER /* _WIN32 */
typedef unsigned long long int uint64_t;
#else
typedef unsigned __int64 uint64_t;
#endif
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
#ifndef _MSC_VER /* _WIN32 */
typedef signed long long int int64_t;
#else
typedef signed __int64 int64_t;
#endif
#endif /* OWN_INTTYPES */
#endif /* __MSDOS__ || _MSC_VER */
#endif /* STDC_VERSION */

#include <stdio.h> /* FILE, FILENAME_MAX */
#include <ultra64/rom.h>

typedef enum {
	CD64BIOS = 0,
	GHEMOR = 1,
	ULTRALINK = 2
} protocol_t;

typedef enum {
	LIBIEEE1284 = 1,
	PPDEV = 2,
	PORTDEV = 3,
	RAWIO = 4
} method_t;

/* When using this structure, be sure to calloc it or otherwise set it to
 * zero before setting values or calling library functions. */

struct cd64_t {
	int using_ppa;
	protocol_t protocol;

	struct parport *ppdev;	/* libieee1284 */
	int ppdevfd;		/* ppdev */
	int portdevfd;		/* /dev/port */

	/* If using inb/outb or /dev/port, this is the I/O address
	 * Otherwise it is the parport* number */
	int port;

	/* Directory with io.dll or dlportio.dll. Used by the Windows ports. */
	char io_driver_dir[FILENAME_MAX];

	/* A flag that can be set/read to determine whether
	 * the current operation should be canceled. */
	int abort;

	int (*devopen)(struct cd64_t *cd64);
	int (*xfer)(struct cd64_t *cd64, uint8_t *write, uint8_t *read, int delayms);
	int (*devclose)(struct cd64_t *cd64);

	/* Progress callback is responsible for printing header info if the
	 * user wants it */

	void (*progress_callback)(uint32_t curbyte, uint32_t totalbytes);
	int (*notice_callback)(const char *format, ...);
	int (*notice_callback2)(const char *format, ...);

	/* Callbacks for read, write and seek operations. By default they point to
	 * callbacks in the library which just call fread(), fwrite(), ftell() and
	 * fseek(). You can change them so that the library doesn't read from or
	 * write to a FILE * (io_id). For example, a client can install its own
	 * callback to make it possible to read from .zip files. */
	int (*read_callback)(void *io_id, void *buffer, uint32_t size);
	int (*write_callback)(void *io_id, void *buffer, uint32_t size);
	int32_t (*tell_callback)(void *io_id);
	int (*seek_callback)(void *io_id, int32_t offset, int whence);
};

/* This function must be called and return successful before any of the
 * other functions may be used. */

int cd64_create(struct cd64_t *cd64, method_t method,
			uint16_t port, protocol_t protocol, int ppa);

/* The following five functions are wrappers above the I/O abstraction.
 * Use them to write code that works regardless of the underlying
 * transport. */

int cd64_send_byte(struct cd64_t *cd64, uint8_t what);
int cd64_send_dword(struct cd64_t *cd64, uint32_t what);
int cd64_grab_byte(struct cd64_t *cd64, uint8_t *val);
int cd64_grab_dword(struct cd64_t *cd64, uint32_t *val);
int cd64_trade_bytes(struct cd64_t *cd64, uint8_t give, uint8_t *recv);

/* Generic protocol handlers */

int cd64_bios_grab(struct cd64_t *cd64, void *io_id, uint32_t addr, uint32_t length,
		                int *elapsed);
int cd64_bios_send(struct cd64_t *cd64, void *io_id, uint32_t addr,
		                uint32_t length, int *elapsed, int cmd);

int cd64_ghemor_grab(struct cd64_t *cd64, void *io_id, uint8_t slow, int *elapsed);
int cd64_ghemor_send(struct cd64_t *cd64, void *io_id, uint32_t length,
		                int *elapsed);

/* Functions for sending files to CD64 */
int cd64_upload_dram(struct cd64_t *cd64, FILE *infile, uint32_t length,
		                        int *elapsed, int exec);
int cd64_upload_ram(struct cd64_t *cd64, FILE *infile, uint32_t length,
		                        int *elapsed, uint32_t address);

int cd64_upload_bootemu(struct cd64_t *cd64, FILE *infile, uint32_t length, int *elapsed);

int cd64_upload_sram(struct cd64_t *cd64, FILE *infile);
int cd64_upload_flashram(struct cd64_t *cd64, FILE *infile);
int cd64_upload_eeprom(struct cd64_t *cd64, FILE *infile);
int cd64_upload_mempak(struct cd64_t *cd64, FILE *infile, int8_t which);

/* Functions for receiving files from CD64 */
int cd64_download_cart(struct cd64_t *cd64, FILE *outfile, uint32_t length,
		                        int *elapsed);
int cd64_download_dram(struct cd64_t *cd64, FILE *outfile, uint32_t start,
		                        uint32_t end, int *elapsed);
int cd64_download_ram(struct cd64_t *cd64, FILE *outfile, uint32_t length,
		                        int *elapsed, uint32_t address);

int cd64_download_sram(struct cd64_t *cd64, FILE *outfile);
int cd64_download_flashram(struct cd64_t *cd64, FILE *outfile);
int cd64_download_eeprom(struct cd64_t *cd64, FILE *outfile);
int cd64_download_mempak(struct cd64_t *cd64, FILE *outfile, int8_t which);

/* Remote control functions */
int cd64_run_dram(struct cd64_t *cd64);
int cd64_run_cart(struct cd64_t *cd64);

/* This function simply gets the header from the cart and can be displayed
 * using ultra64_header_info() */

int cd64_download_header(struct cd64_t *cd64, n64header_t *head, uint32_t location);

#ifdef __cplusplus
}
#endif

#define BIOS_TEMP_RAM 0xa0300000

#define BIOS_DUMP_N64 'D'
#define BIOS_TRANSFER_N64 'B'

#define BIOS_DUMP_PI 'G'
#define BIOS_TRANSFER_PI 'T'
#define BIOS_EXECUTE_PI 'X'

#define GHEMOR_RESTORE_MEMPAK 1
#define GHEMOR_RESTORE_EEPROM 2
#define GHEMOR_RESTORE_SRAM 3
#define GHEMOR_RESTORE_FLASHRAM 4
#define GHEMOR_EXECUTE_BOOTEMU 5
#define GHEMOR_TRANSFER_PROGRAM 6
#define GHEMOR_DUMP_CART 7
#define GHEMOR_DUMP_MEMPAK 8
#define GHEMOR_DUMP_EEPROM 9
#define GHEMOR_DUMP_SRAM 10
#define GHEMOR_DUMP_FLASH 11
#define GHEMOR_RESET_DRAM 12
#define GHEMOR_RESET_CART 13

#endif
