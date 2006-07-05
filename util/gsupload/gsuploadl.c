#include <stdio.h>
#include <sys/io.h>

/*
   GSUpload 0.3 (linux)
   An open source N64 Game Shark (tm) uploader
   based on disassembly of "Generic Uploader" and official utilities
*/

unsigned int LPT1=0x378;

int InitGSComms(void);
int CloseGSComms(void);
int Upload(const unsigned char * buffer, unsigned long size, unsigned long address);
int UploadFile(FILE * infile, unsigned long address);
int Read(unsigned char * buffer, unsigned long size, unsigned long address);
int GSFcn1(int);
int SendNibble(int);
int ReadWriteNibble(int);
int ReadWriteByte(int);
unsigned long ReadWrite32(unsigned long);
int CheckGSPresence(void);

/* Patches - all patches and signatures are 4 bytes */

typedef struct patch {
    unsigned long loc; /* where to put the patch code */
    unsigned char code[4]; /* the patch code */
    unsigned char sig[4]; /* the patch signature (what should be where the patch is to be written) */
    unsigned long uploadloc; /* where to upload the executable */
    char * name; /* name of the game being patched */
} patch;

#define PATCHCOUNT 6

const patch patches[PATCHCOUNT] = {

/* Super Mario 64 (U) - hcs */
    {0x80263844, {8,0x0C,0,0}, {0x85,0xCF,0,0xAE}, 0x80300000, "Super Mario 64 US"},
/* Super Mario 64 (E) - hcs */
    {0x80259228, {8,0xB,0x38,0xB0}, {0x86,0x0A,0,0xAE}, 0x802ce2c0, "Super Mario 64 PAL"},
/* Super Mario 64 (J) - nekokabu */
    {0x80263438, {8,0x0B,0xFB,0xD4}, {0x85,0xCF,0,0xAE}, 0x802FEF50, "Super Mario 64 JP"},
/* Super Mario 64 - Shindou Edition (J) - nekokabu */
    {0x8025583C, {8,0x0B,0x70,0xE8}, {0x31,0xF8,0xFF,0xEF}, 0x802DC3A0, "Super Mario 64 JP Shindou Edition"},
/* Mario Party (U) - nekokabu */
    {0x8002cd6c, {8,0x0B,0x20,0x00}, {0x00,0x02,0x14,0x03}, 0x802C8000, "Mario Party US"},
/* Mario Party (J) - nekokabu */
    {0x8002cc9c, {8,0x0B,0xF3,0xC0}, {0x00,0x02,0x14,0x03}, 0x802fcf00, "Mario Party JP"}

};

int main(int argc, char ** argv) {
	unsigned char buf[4];
	FILE * infile, * romfile;
	int i;

	printf("Neon64 GS uploader v0.3 (Linux version)\n");

	if (ioperm(LPT1,1,1) || ioperm(LPT1+1,1,1)) {
		printf("couldn't get LPT1, are you root?\n");
		return 1;
	}
	
	if (argc != 2) {
		printf("usage: %s rom.nes\n",argv[0]);
		outb(0,LPT1); /* clear output pins, GS sometimes fails to boot with some pins high */
		return 1;
	}
		
	infile=fopen("neon64gs.bin","rb");
	if (!infile) {printf("error opening neon64gs.bin\n"); return 1;}

	romfile = fopen(argv[1],"rb");
	if (!romfile) {printf("error opening %s\n",argv[1]); return 1;}

	/* test memory locations to find version */
	for (i=0;i<PATCHCOUNT;i++) {
	    if (Read(buf,4,patches[i].loc)) {printf("read failed\n"); return 1;}
	    if (!memcmp(buf,patches[i].sig,4)) {
		    printf("%s detected\n",patches[i].name);
		    if (Upload(patches[i].code,4,patches[i].loc)) {printf("\nupload failed\n"); return 1;}
		    if (UploadFile(infile,patches[i].uploadloc)) {printf("\nupload failed\n"); return 1;}
		    break;
	    }
	}

	if (i==PATCHCOUNT) {
	    if (Read(buf,2,0x80000400)) {printf("read failed\n"); return 1;}
	    /* Neon64 starts with a beq r0,r0 */
	    if (buf[0]==0x10 && buf[1]==0x00) {
		    printf("Neon64 already loaded.\n");
	    } else {
		    printf("unknown game in RAM, please report your situation to\nhalleyscometsoftware@hotmail.com\n");
		    return 1;
	    }
	}
	
	fclose(infile);

	printf("press a key when Neon64 screen appears\n");
	getchar();

	if (UploadFile(romfile,0x80300000)) {printf("\nupload failed\n"); return 1;}
	fclose(romfile);

	return 0;
}

int InitGSComms(void) {
	int timeout=0x3e8;
	while (GSFcn1(3) && timeout) {
		timeout--;
	}
	if (!timeout) {printf("init failed\n"); return 1;}
	if (!CheckGSPresence()) {printf("init failed2\n"); return 1;}
	return 0;
}

int CloseGSComms(void) {
	if (GSFcn1(3)) return 1;
	if (!CheckGSPresence()) return 1;
	ReadWriteByte(0x64);

	return 0;
}

int Read(unsigned char * buffer, unsigned long size, unsigned long address) {
	unsigned long c=0;
	
	if (InitGSComms()) return 1;
	
	ReadWriteByte(1);
	ReadWrite32(address);
	ReadWrite32(size);
	
	for (c=0; c < size; c++) buffer[c]=ReadWriteByte(0);

	for (c=0; c < 8; c++) ReadWriteByte(0);

	if (CloseGSComms()) return 1;
	
	return 0;
}


int Upload(const unsigned char * buffer, unsigned long size, unsigned long address) {
	unsigned long c=0;
	
	if (InitGSComms()) return 1;
	
	ReadWriteByte(2);
	ReadWrite32(address);
	ReadWrite32(size);
	
	for (c=0; c < size; c++) ReadWriteByte(buffer[c]);

	for (c=0; c < 8; c++) ReadWriteByte(0);

	if (CloseGSComms()) return 1;
	
	return 0;
}

int UploadFile(FILE * infile, unsigned long address) {
	unsigned long c=0;
	unsigned long size;
	char buf;
	
	if (fseek(infile,0,SEEK_END)) return 1;
	size=ftell(infile);
	rewind(infile);

	if (InitGSComms()) return 1;

	printf("Uploading...00%%");

	ReadWriteByte(2);
	ReadWrite32(address);
	ReadWrite32(size);
	
	for (c=0; c < size; c++) {
		fread(&buf,1,1,infile);
		if (c & 0x400) printf("\b\b\b%02d%%",c*100/size);
		ReadWriteByte(buf);
	}

	for (c=0; c < 8; c++) ReadWriteByte(0);

	if (CloseGSComms()) return 1;

	printf("\b\b\bDone.\n");
	
	return 0;
}

/* my guess: used to get the GS and PC sync'd */
int GSFcn1(int x) {
	int timeout=0x3e8;
	unsigned char result;
	
	while (timeout) {

		result<<=4;
		result|=SendNibble(x);

		/* when we recieve a 6, then a 7, we're sync'd */
		if (result==0x67) break;

		SendNibble(x);
		SendNibble(x>>4);

		timeout--;
	}

	if (!timeout) return 1;
	return 0;
}

int SendNibble(int x) {
	int timeout=0x3e8,retval;
	
	outb(x&0xf|0x10,LPT1);

	while (timeout && inb(LPT1+1)&8) timeout--;
	
	retval=(inb(LPT1+1)&0xf0^0x80)>>4;

	outb(0,LPT1);

	timeout=0x64;
	while (timeout && inb(LPT1+1)&8) timeout--;

	if (!timeout) {/*printf("SendNibble timed out2\n");*/ return 0;}
	else return retval;
}

int ReadWriteNibble(int x) {
	int retval;
	outb(x&0xf|0x10,LPT1);
	
	while ((~inb(LPT1+1))&8);
	while ((~inb(LPT1+1))&8);

	retval=(inb(LPT1+1)&0xf0^0x80)>>4;

	outb(0,LPT1);

	while (inb(LPT1+1)&8);
	while (inb(LPT1+1)&8);

	return retval;
}

int ReadWriteByte(int x) {
	return (ReadWriteNibble(x>>4)<<4)|ReadWriteNibble(x);
}

unsigned long ReadWrite32(unsigned long x) {
	return ((unsigned long)ReadWriteNibble(x>>28)<<28)|
		((unsigned long)ReadWriteNibble(x>>24)<<24)|
		((unsigned long)ReadWriteNibble(x>>20)<<20)|
		((unsigned long)ReadWriteNibble(x>>16)<<16)|
		((unsigned long)ReadWriteNibble(x>>12)<<12)|
		((unsigned long)ReadWriteNibble(x>>8)<<8)|
		((unsigned long)ReadWriteNibble(x>>4)<<4)|
		(unsigned long)ReadWriteNibble(x);
}

/* perhaps this is what actually freezes operation? */
int CheckGSPresence(void) {
	int timeout=0x3e8,result;
	
	while (timeout) {
		result=(ReadWriteNibble(0x47>>4)<<4)|ReadWriteNibble(0x47);
		timeout--;
		if (result != 0x67) continue;
		result=(ReadWriteNibble(0x54>>4)<<4)|ReadWriteNibble(0x54);
		if (result == 0x74) break;
	}

	if (timeout) return 1;
	return 0;
}
