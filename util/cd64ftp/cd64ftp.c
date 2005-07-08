/*
 * cd64ftp
 *
 * A simple libcd64 client.
 *
 * (c) 2004 Ryan Underwood
 * Portions (c) 2004 Daniel Horchner (OpenBSD, FreeBSD, BeOS, Win32, DOS)
 *
 * May be distributed under the terms of the GNU General Public
 * License version 2 or any later version as published by the Free
 * Software Foundation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined __unix__ || defined __BEOS__
#include <unistd.h>
#ifndef __MSDOS__
#include <signal.h>
#endif
#if !(defined __MSDOS__ || defined __BEOS__)
#include <sched.h>
#endif
#endif /* __unix__ || __BEOS__ */
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#if !(defined __MSDOS__ || defined _MSC_VER || defined __FreeBSD__)
#include <getopt.h>
#endif

#include <ultra64/host/cd64lib.h>
#include <ultra64/host/cartinfo.h>
#include <ultra64/rom.h>
#include <ultra64/cartmem.h>

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

void shell(void);
struct cd64_t *cd64;

#if 0
#if defined __unix__ && defined _POSIX_PRIORITY_SCHEDULING
static void set_realtime_sched(int priority) {

	struct sched_param sp;
	pid_t myPid;

	memset(&sp, 0, sizeof(struct sched_param));
	sp.sched_priority = sched_get_priority_max(SCHED_RR) - priority;
	myPid = getpid();

	if (sched_setscheduler(myPid, SCHED_RR, &sp) == 0) {
		printf("activated realtime scheduling\n");
	} else {
		printf("cannot set realtime scheduling! please run as root!\n");
	}

	/* Were we setuid root? */
	if ((!getuid()) != (!geteuid())) {
		/* If so, set the euid back to the user who ran it. */
		seteuid(getuid());
		setegid(getgid());
	}
}
#endif
#endif

int check_ext(char *fname, char *ext) {

	char *p;
	char *buf = NULL;
	int val = 0;

	if (ext[0] != '.') {
		buf = malloc(strlen(ext)+2);
		if (buf == NULL) return 0;
		sprintf(buf, ".%s", ext);
		ext = buf;
	}
	p = strstr(fname, ext);
	if (p != NULL) {
		/* Verify its at the end of the filename. */
		if (!strcmp(p, ext)) val = 1;
	}
	if (buf) free(buf);
	return val;
}

void break_func(int signum) {
	(void) signum;
	cd64->abort = 1;
	fprintf(stderr, "\nuser abort\n");
}

void progress(uint32_t cur, uint32_t length) {

	/* FIXME : print cart info here. */
	if (cur > 1) { fprintf(stderr, "\r"); }
	fprintf(stderr, "%d/%d Mbits (%.2f%% complete)",
			(int) cur/BYTES_IN_MBIT, (int) length/BYTES_IN_MBIT, 100*((cur*1.0)/length));
}

void cd64header(uint32_t location) {

	n64header_t carthead;

	if (!cd64->devopen(cd64)) {
		printf("Error opening CD64 -- make sure nothing else uses its port\n");
		return;
	}
	if (!cd64_download_header(cd64,&carthead,location)) {
		printf("Error getting header\n");
		return;
	}
	switch (location) {
		case 0xb0000000:
			printf("BIOS header:\n");
			break;
		case 0xb2000000:
			printf("Cartridge header:\n");
			break;
		case 0xb4000000:
			printf("DRAM header:\n");
			break;
		default:
			break;
	}

	ultra64_header_info(&carthead);
	if (!cd64->devclose(cd64)) {
		printf("Error closing CD64\n");
	}

}

int cmd_uploadcart(char *fname, int length, int exec) {

	int ret, elapsed;
	FILE *file;

	if (!fname || strlen(fname) < 1) {
		printf("Need a filename to upload from.\n");
		return 0;
	}

	if ((file = fopen(fname, "rb")) == NULL) {
		printf("Failed to open file %s for reading.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		printf("Error opening CD64 -- make sure nothing else uses its port\n");
		fclose(file);
		return 0;
	}

	if (!length) {
#if 1
		struct stat fstate;
		stat(fname, &fstate);
		length = (int) fstate.st_size;
#else
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
#endif
	}

	cd64->progress_callback = progress;

	printf("Going to upload %d (0x%x) bytes from file %s to CD64 DRAM\n",
	       length, length, fname);

	ret = cd64_upload_dram(cd64, file, length, &elapsed, exec);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		printf("Checksum good\n");
		printf("Elapsed time: %.2f seconds (%.3f MB/sec)\n",
		       elapsed/1000000.0, (length/BYTES_IN_MBIT)/(8*(elapsed/1000000.0)));
	}

	fclose(file);
	return ret;

}

int cmd_bootemu(char *fname) {

	int ret, elapsed;
	FILE *file;
	int length;

	if (!fname || strlen(fname) < 1) {
		printf("Need a filename to upload from.\n");
		return 0;
	}

	if ((file = fopen(fname, "rb")) == NULL) {
		printf("Failed to open file %s for reading.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		printf("Error opening CD64 -- make sure nothing else uses its port\n");
		fclose(file);
		return 0;
	}

#if 1
	{
		struct stat fstate;
		stat(fname, &fstate);
		length = (int) fstate.st_size;
	}
#else
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);
#endif

	cd64->progress_callback = progress;

	printf("Going to upload %d (0x%x) bytes from bootemu file %s to N64 RAM\n",
				length, length, fname);

	ret = cd64_upload_bootemu(cd64, file, length, &elapsed);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		printf("Checksum good\n");
		printf("Elapsed time: %.2f seconds (%.3f MB/sec)\n",
				elapsed/1000000.0, (length/BYTES_IN_MBIT)/(8*(elapsed/1000000.0)));
	}

	fclose(file);
	return ret;
}

int cmd_downloadcart(char *fname, int megabits) {

	int ret, elapsed;
	FILE *file;

	if (megabits == 0 && cd64->protocol == CD64BIOS) {
		printf("Need a megabit size to download when using CD64 BIOS\n");
		return 0;
	}

	if ((file = fopen(fname, "w+b")) == NULL) {
		printf("Failed to open file %s for writing.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		fclose(file);
		return 0;
	}

	cd64->progress_callback = progress;

	printf("Going to download cart in CD64 to %s\n", fname);

	ret = cd64_download_cart(cd64, file, megabits*BYTES_IN_MBIT, &elapsed);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		/* int mins = elapsed / 60000000;
		elapsed %= 60; */
		printf("Checksum verified good\n");
		printf("Elapsed time: %.2f seconds (%.3f KB/sec)\n",
		       elapsed/1000000.0, (1024*megabits)/(8*(elapsed/1000000.0)));
	}

	fclose(file);
	return ret;
}

int cmd_downloadmem(char *fname, uint32_t start, uint32_t end) {

	int ret, elapsed;
	FILE *file;

	if (end == 0) {
		/* Just dump 1mbit */
		end = start + 0x100000;
	}

	if (end - start <= 0) {
		printf("Bad memory range: %lxh-%lxh\n", (long unsigned int) start,
		       (long unsigned int) end);
		return 0;
	}

	if ((file = fopen(fname, "w+b")) == NULL) {
		printf("Failed to open file %s for writing.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		fclose(file);
		return 0;
	}

	cd64->progress_callback = progress;

	printf("Going to download CD64 DRAM %lxh-%lxh to %s\n",
	       (long unsigned int) start, (long unsigned int) end, fname);

	ret = cd64_download_dram(cd64, file, start, end, &elapsed);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		/* int mins = elapsed / 60000000;
		elapsed %= 60; */
		printf("Checksum verified good\n");
		printf("Elapsed time: %.2f seconds (%.3f KB/sec)\n",
		       elapsed/1000000.0, (end-start)/(elapsed/1000000.0));
	}

	fclose(file);
	return ret;
}

int cmd_uploaddram(char *fname, uint32_t start, uint32_t end) {

	int ret, elapsed;
	FILE *file;

	if (start == 0 && end == 0) {
		if ((file = fopen(fname, "rb")) == NULL) {
			printf("Failed to open file %s for reading.\n", fname);
			return 0;
		}
		fseek(file, 0, SEEK_END);
		end = ftell(file);
		fseek(file, 0, SEEK_SET);
		fclose(file);
	}

	if (start == 0) {
		start = 0xb4000000;
		end += 0xb4000000; /* use as offset */
	}
	if (start < 0xb4000000) {
		/* Treat it as an offset into dram */
		start += 0xb4000000;
		end += 0xb4000000;
	}

	if (end - start <= 0) {
		printf("Bad memory range: %lxh-%lxh\n", (long unsigned int) start,
		       (long unsigned int) end);
		return 0;
	}

	if ((file = fopen(fname, "rb")) == NULL) {
		printf("Failed to open file %s for reading.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		fclose(file);
		return 0;
	}

	cd64->progress_callback = progress;

	printf("Going to upload %s to CD64 DRAM %lxh-%lxh\n", fname,
	       (long unsigned int) start, (long unsigned int) end);

	ret = cd64_bios_send(cd64, file, start, end-start, &elapsed, BIOS_TRANSFER_PI);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		/* int mins = elapsed / 60000000;
		elapsed %= 60; */
		printf("Checksum verified good\n");
		printf("Elapsed time: %.2f seconds (%.3f KB/sec)\n",
		       elapsed/1000000.0, (end-start)/(elapsed/1000000.0));
	}

	fclose(file);
	return ret;
}

int cmd_downloadsram(char *fname) {

	int ret;
	FILE *file;

	if ((file = fopen(fname, "w+b")) == NULL) {
		printf("Failed to open file %s for writing.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		fclose(file);
		return 0;
	}

	cd64->progress_callback = progress;

	printf("Going to download cart SRAM to %s\n", fname);

	ret = cd64_download_sram(cd64, file);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		printf("Checksum verified good\n");
	}

	fclose(file);
	return ret;
}

int cmd_downloadflashram(char *fname) {

	int ret;
	FILE *file;

	if ((file = fopen(fname, "w+b")) == NULL) {
		printf("Failed to open file %s for writing.\n", fname);
		return 0;
	}

	if (!cd64->devopen(cd64)) {
		fclose(file);
		return 0;
	}

	cd64->progress_callback = progress;

	printf("Going to download cart FlashRAM to %s\n", fname);

	ret = cd64_download_flashram(cd64, file);
	cd64->devclose(cd64);

	if (ret < 1) {
		if (ret == -1) {
			printf("(Bad checksum)\n");
		}
	}
	else {
		/* int mins = elapsed / 60000000;
		elapsed %= 60; */
		printf("Checksum verified good\n");
	}

	fclose(file);
	return ret;
}


void print_help(void) {

	printf("cd64ftp\n"
	"-m (rawio,ppdev,libieee1284,portdev)\n"
	"-p port\n"
	"-d directory with I/O driver\n" /* display only under Windows? */
	"-r [realtime sched]\n" /* display only under Unix (with _POSIX_PRIORITY_SCHEDULING)? */
	"-h [help]\n"
	"-b [comms link]\n"
	"-g [ghemor]\n"
	"-u [ultralink]\n"
	"\n"
	);

}

void shell(void) {

	char buf[100];

	cd64->abort = 0;

	while (1) {
		char *p;
		int quit = 0;

		if (cd64->abort) exit(EXIT_SUCCESS);

		printf("cd64> ");
		fgets(buf, sizeof(buf)-1, stdin);
		p = strtok(buf, " \n");
/*		printf("buf: %s\n",buf); */

		if (!p) continue;

		if (!strcmp(p, "getrom")) {
			char fname[PATH_MAX+10];
			int megabits = 0;
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");

				if (p) {
					megabits = atoi(p);
				}
			}
			else {
				strcpy(fname, "lastdump.rom");
			}

			if (!check_ext(fname, "rom")) {
				strcat(fname, ".rom");
			}

			/* check for user's given size here */

			ret = cmd_downloadcart(fname, megabits);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "getsram")) {
			char fname[PATH_MAX+10];
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
			}
			else {
				strcpy(fname, "sramdump.sram");
			}

			if (!check_ext(fname, "sram")) {
				strcat(fname, ".sram");
			}

			ret = cmd_downloadsram(fname);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "bootemu")) {
			char fname[PATH_MAX+1];
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
			}
			else {
				strcpy(fname, "dx-be.rom");
			}

			if (!check_ext(fname, "rom")) {
				strcat(fname, ".rom");
			}

			ret = cmd_bootemu(fname);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "getflash")) {
			char fname[PATH_MAX+1];
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
			}
			else {
				strcpy(fname, "flashdump.flash");
			}

			if (!check_ext(fname, "flash")) {
				strcat(fname, ".flash");
			}

			ret = cmd_downloadflashram(fname);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "getmempak")) {
			char fname[PATH_MAX+1];
			FILE *file;
			int ret;
			int8_t which = -1;

			strcpy(fname, "mempakdump.mem");

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");
				if (p) {
					which = (int8_t)(atoi(p) & 0xff);
				}
			}

			if (!check_ext(fname, "mem")) {
				strcat(fname, ".mem");
			}

			if ((file = fopen(fname, "w+b")) == NULL) {
				printf("Failed to open file %s for writing.\n", fname);
				continue;
			}

			if (!cd64->devopen(cd64)) {
				fclose(file);
				continue;
			}

			cd64->progress_callback = NULL;

			printf("Going to download controller pack from slot %d to %s\n",
			       which, fname);

			ret = cd64_download_mempak(cd64, file, which);
			cd64->devclose(cd64);

			if (ret < 1) {
				printf("Operation failed.\n");
				if (ret == -1) {
					printf("(Bad checksum)\n");
				}
			}
			else {
				printf("Checksum verified good\n");
				printf("Operation succeeded.\n");
			}

			fclose(file);
			continue;
		}

		else if (!strcmp(p, "putmempak")) {
			char fname[PATH_MAX+1];
			FILE *file;
			int ret;
			int8_t which = -1;

			strcpy(fname, "mempakdump.mem");

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");
				if (p) {
					which = (int8_t)(atoi(p) & 0xff);
				}
			}

			if (!check_ext(fname, "mem")) {
				strcat(fname, ".mem");
			}

			if ((file = fopen(fname, "rb")) == NULL) {
				printf("Failed to open file %s for reading.\n", fname);
				continue;
			}

			if (!cd64->devopen(cd64)) {
				fclose(file);
				continue;
			}

			cd64->progress_callback = NULL;

			printf("Going to upload controller pack %d from %s\n", which, fname);

			ret = cd64_upload_mempak(cd64, file, which);
			cd64->devclose(cd64);

			if (ret < 1) {
				printf("Operation failed.\n");
				if (ret == -1) {
					printf("(Bad checksum)\n");
				}
			}
			else {
				printf("Checksum verified good\n");
				printf("Operation succeeded.\n");
			}

			fclose(file);
			continue;
		}

		else if (!strcmp(p, "getram")) {
			char fname[PATH_MAX+1];
			uint32_t start = 0;
			uint32_t end = 0;
			int mbits = 0;
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");

				if (p) {
					start = strtoul(p, NULL, 16);
					p = strtok(NULL, " \n");

					if (p) {
						if (*p == 'x') {
							p = strtok(NULL, " \n");
							if (p) end = strtoul(p, NULL, 16);
							else {
								printf("error\n");
								continue;
							}
						}
						else {
							mbits = atoi(p);
						}
					}
				}
			}
			else {
				strcpy(fname, "memdump.ram");
			}

			if (!check_ext(fname, "ram")) {
				strcat(fname, ".ram");
			}

			if (end == 0 && mbits != 0) {
				end = start + (0x100000/8)*mbits;
			}

			ret = cmd_downloadmem(fname, start, end);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "putdram")) {
			char fname[PATH_MAX+1];
			uint32_t start = 0;
			uint32_t end = 0;
			int mbits = 0;
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");

				if (p) {
					start = strtoul(p, NULL, 16);
					p = strtok(NULL, " \n");

					if (p) {
						if (*p == 'x') {
							p = strtok(NULL, " \n");
							if (p) end = strtoul(p, NULL, 16);
							else {
								printf("error\n");
								continue;
							}
						}
						else {
							mbits = atoi(p);
						}
					}
				}
			}
			else {
				strcpy(fname, "memdump.ram");
			}

			if (end == 0 && mbits != 0) {
				end = start + (0x100000/8)*mbits;
			}

			ret = cmd_uploaddram(fname, start, end);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}


		else if (!strcmp(p, "putrom")) {
			char fname[PATH_MAX+1];
			int length = 0;
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");

				if (p) {
					length = atoi(p);
				}
			}
			else {
				strcpy(fname, "");
			}

			if (!check_ext(fname, "rom")) {
				strcat(fname, ".rom");
			}

			/* check for user's given size here */

			ret = cmd_uploadcart(fname, length, 0);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "exec")) {
			char fname[PATH_MAX+1];
			int length = 0;
			int ret;

			p = strtok(NULL, " \n");

			if (p) {
				strncpy(fname, p, PATH_MAX);
				p = strtok(NULL, " \n");

				if (p) {
					length = atoi(p);
				}
			}
			else {
				strcpy(fname, "");
			}

			if (!check_ext(fname, "rom")) {
				strcat(fname, ".rom");
			}

			/* check for user's given size here */

			ret = cmd_uploadcart(fname, length, 1);

			if (ret < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}

		}

		else if (!strcmp(p, "rundram")) {
			cd64->devopen(cd64);
			if (cd64_run_dram(cd64) < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}
			cd64->devclose(cd64);
		}

		else if (!strcmp(p, "runcart")) {
			cd64->devopen(cd64);
			if (cd64_run_cart(cd64) < 1) {
				printf("Operation failed.\n");
			}
			else {
				printf("Operation succeeded.\n");
			}
			cd64->devclose(cd64);
		}

		else if (!strcmp(p, "ghemor")) {
			cd64->protocol = GHEMOR;
			printf("Set protocol to Ghemor\n");
		}

		else if (!strcmp(p, "cd64")) {
			cd64->protocol = CD64BIOS;
			printf("Set protocol to CD64 BIOS\n");
		}

		else if (!strcmp(p, "port")) {
			p = strtok(NULL, " \n");
			if (p) {
				cd64->port = strtoul(p, NULL, 16);
			}
			else {
				printf("Need a hex port number.\n");
			}
			continue;
		}

		else if (!strcmp(p, "ls")) {
			cd64header(0xb2000000);
			cd64header(0xb4000000);
			cd64header(0xb0000000);
		}

		else if (!strcmp(p, "quit")) {
			quit = 1;
		}

		else if (!strcmp(p, "help")) {
			puts("getrom\t\t"
			     "getsram\t\t"
			     "bootemu\t\t"
			     "getflash\t"
			     "getmempak\n"
			     "putmempak\t"
			     "getram\t\t"
			     "putdram\t\t"
			     "putrom\t\t"
			     "exec\n"
			     "rundram\t\t"
			     "runcart\t\t"
			     "ghemor\t\t"
			     "cd64\t\t"
			     "port\n"
			     "ls\t\t"
			     "quit\t\t"
			     "help");
		}

		else {
			printf("Unrecognized command.\n");
		}

		if (quit) exit(EXIT_SUCCESS);
	}

}

#ifdef _MSC_VER
/* Quick and dirty getopt() implementation */
static char *optarg = NULL;
static int optind = 0, opterr = 1, optopt = '?';

static char getopt(int argc, char **argv, char *options) {

	char *ptr;

	optind++;
	if (optarg) optind++;
	optarg = NULL;

	if (optind >= argc || optind < 1) return -1;
	if (/*argv[optind][0] == '-' && */ strlen(argv[optind]) != 2) {
		if (opterr) fprintf (stderr, "%s: unrecognized option `%s'\n", argv[0], argv[optind]);
		optopt = '?';
		return optopt;
	}

	if ((ptr = strpbrk(argv[optind], options)) != NULL) {
		optopt = argv[optind][1];
		ptr = strchr(options, optopt);              /* always successful */
		if (ptr[1] == ':') {
			if (optind + 1 < argc) {
				optarg = argv[optind + 1];
				return optopt;
			}
			else {
				if (opterr) fprintf (stderr, "%s: option `%s' requires an argument\n", argv[0], argv[optind]);
				optopt = '?';
				return optopt;
			}
		}
		else {
			return optopt;
		}
	}
	else {
		if (opterr) fprintf (stderr, "%s: unrecognized option `%s'\n", argv[0], argv[optind]);
		optopt = '?';
		return optopt;
	}
}
#endif

int main(int argc, char **argv) {

	char curopt;
	char *io_driver_dir = NULL;
	int n;
	int protocol = -1;
	int method = -1;
	int port = -1;
	int is_parallel = 1;

	while ((curopt = getopt(argc, argv, "gum:p:rchd:")) != -1) {
		switch (curopt) {
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
				break;
			case 'g':
				protocol = GHEMOR;
				break;
			case 'u':
				protocol = ULTRALINK;
				break;
			case 'b':
				is_parallel = 0;
				break;
			case 'm':
				if (!strcmp(optarg, "ppdev")) {
					method = PPDEV;
				} else if (!strcmp(optarg, "libieee1284")) {
					method = LIBIEEE1284;
				} else if (!strcmp(optarg, "portdev")) {
					method = PORTDEV;
				} else if (!strcmp(optarg, "rawio")) {
					method = RAWIO;
				}
				break;
			case 'p':
				port = strtol(optarg, NULL, 16);
				break;
			case 'r':
#if 0
#if defined __unix__ && defined _POSIX_PRIORITY_SCHEDULING
				if (!geteuid()) {
					set_realtime_sched(0);
				}
				else {
					printf("Not root, realtime sched disabled\n");
				}
#endif
#else
				abort();
#endif
				break;
			case 'd':
				io_driver_dir = optarg;
				break;
			case '?':
			case ':':
				exit(EXIT_FAILURE);
				break;
			default:
				abort();
				break;
		}
	}

	if (method == -1 && is_parallel) {
#ifdef __linux__
		printf("No method was given, defaulting to ppdev\n");
		method = PPDEV;
#else
		printf("No method was given, defaulting to rawio\n");
		method = RAWIO;
#endif
	}

	if (!is_parallel) {
#ifdef __linux__
		if (method != RAWIO && method != PORTDEV) {
			printf("Invalid method for comms link, selecting portdev\n");
			method = PORTDEV;
		}
#else
		printf("Selecting rawio for comms link\n");
		method = RAWIO;
#endif
	}

	if (port == -1) {
		switch(method) {
			case LIBIEEE1284:
			case PPDEV:
				port = 0;
				break;
			case RAWIO:
			case PORTDEV:
				port = 0x3bc;
				break;
			default:
				abort();
				break;
		}
		printf("No port given, defaulting to %x\n", port);
	}

	if (protocol == -1) {
		printf("No protocol was given, defaulting to CD64 BIOS protocol\n");
		protocol = CD64BIOS;
	}

	printf("Method: ");
	switch (method) {
		case LIBIEEE1284:
			printf("libieee1284 port %d\n", port);
			break;
		case PPDEV:
			printf("ppdev on /dev/parport%d\n", port);
			break;
		case PORTDEV:
			printf("/dev/port with port %xh\n", port);
			break;
		case RAWIO:
			printf("raw I/O with port %xh\n", port);
			break;
		default:
			abort();
			break;
	}
	printf("Protocol: ");
	switch(protocol) {
		case CD64BIOS:
			printf("CD64 BIOS\n");
			break;
		case GHEMOR:
			printf("Ghemor\n");
			break;
		case ULTRALINK:
			printf("UltraLink\n");
			break;
		default:
			abort();
			break;
	}
	if (is_parallel) {
		printf("Using Parallel port adapter\n");
	}
	else {
		printf("Using Comms link interface\n");
	}


#if defined __unix__ && !defined __MSDOS__ || defined __BEOS__
	signal(SIGINT, break_func);
#endif

	cd64 = (struct cd64_t *) calloc(1, sizeof(struct cd64_t));
	if (!cd64) {
		perror("Unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	if (io_driver_dir) {
		n = strlen(io_driver_dir);
		memcpy(cd64->io_driver_dir, io_driver_dir, n > FILENAME_MAX? FILENAME_MAX : n);
		cd64->io_driver_dir[FILENAME_MAX-1] = 0;
	}

	if (!cd64_create(cd64, method, (uint16_t) port, protocol, is_parallel)) {
		printf("Some problem when initializing libcd64\n");
		exit(EXIT_FAILURE);
	}

	shell();
	exit(EXIT_FAILURE);
}
