#include <stdio.h>
#include <asm/io.h>

// GSUpload
// An open source N64 Game Shark (tm) uploader
// based on disassembly of "Generic Uploader" and official utilities
// Linux version

unsigned int LPT1=0x378;

int InitGSComms(void);
int CloseGSComms(void);
int Upload(unsigned char * buffer, unsigned long size, unsigned long address);
int UploadFile(FILE * infile, unsigned long address);
int Read(unsigned char * buffer, unsigned long size, unsigned long address);
int GSFcn1(int);
int SendNibble(int);
int ReadWriteNibble(int);
int ReadWriteByte(int);
unsigned long ReadWrite32(unsigned long);
int CheckGSPresence(void);

// Super Mario 64 memory locations

unsigned long USpatchloc=0x80263844;
unsigned char USpatch[4]={8,0x0C,0,0};
unsigned char USpatchsig[4]={0x85,0xCF,0,0xAE};
unsigned long USuploadloc=0x80300000;

unsigned long EUpatchloc=0x80259228;
unsigned char EUpatch[4]={8,0xB,0x38,0xB0};
unsigned char EUpatchsig[4]={0x86,0x0A,0,0xAE};
unsigned long EUuploadloc=0x802ce2c0;

/*unsigned long JPShinpatchloc=0x80261528;
unsigned char JPShinpatch[4]={8,0xB,0x71,0x07}; //{8,0xc,0,0};
unsigned char JPShinpatchsig[4]={0x86,0x0A,00,0xAE};
unsigned long JPShinuploadloc=0x802DC41C; //0x80300000;*/

unsigned long JPShinpatchloc=0x80261528;
unsigned char JPShinpatch[4]={8,0xB,0xEB,0x0}; //{8,0xc,0,0};
unsigned char JPShinpatchsig[4]={0x86,0x0A,00,0xAE};
unsigned long JPShinuploadloc=0x802FAC00; //0x80300000;


int main(int argc, char ** argv) {
	unsigned char buf[4];
	FILE * infile, * romfile;

	printf("Neon64 GS uploader v0.2 (Linux version)\n");

	if (ioperm(LPT1,1,1) || ioperm(LPT1+1,1,1)) {
		printf("couldn't get LPT1, are you root?\n");
		return 1;
	}
	
	if (argc != 2) {
		printf("usage: %s rom.nes\n",argv[0]);
		outb(0,LPT1); // clear output pins, GS sometimes fails to boot with some pins high
		return 1;
	}
		
	infile=fopen("neon64gs.bin","rb");
	if (!infile) {printf("error opening neon64gs.bin\n"); return 1;}

	romfile = fopen(argv[1],"rb");
	if (!romfile) {printf("error opening %s\n",argv[1]); return 1;}

	// test memory locations to find version
	
	if (Read(buf,4,USpatchloc)) {printf("read failed\n"); return 1;}
	if (buf[0]==USpatchsig[0] && buf[1]==USpatchsig[1] &&
		buf[2]==USpatchsig[2] && buf[3]==USpatchsig[3]) {
		printf("Super Mario 64 US version detected\n");
		if (Upload(USpatch,4,USpatchloc)) {printf("\nupload failed\n"); return 1;}
		if (UploadFile(infile,USuploadloc)) {printf("\nupload failed\n"); return 1;}
	} else {	
		if (Read(buf,4,EUpatchloc)) {printf("read failed\n"); return 1;}
		if (buf[0]==EUpatchsig[0] && buf[1]==EUpatchsig[1] &&
			buf[2]==EUpatchsig[2] && buf[3]==EUpatchsig[3]) {
			printf("Super Mario 64 EU version detected\n");
			if (Upload(EUpatch,4,EUpatchloc)) {printf("\nupload failed\n"); return 1;}
			if (UploadFile(infile,EUuploadloc)) {printf("\nupload failed\n"); return 1;}
		} else {
			if (Read(buf,4,JPShinpatchloc)) {printf("read failed\n"); return 1;}
			if (buf[0]==JPShinpatchsig[0] && buf[1]==JPShinpatchsig[1] &&
				buf[2]==JPShinpatchsig[2] && buf[3]==JPShinpatchsig[3]) {
				printf("Super Mario 64 JP Shindou Edition detected\nUnfortunately there's no working support for it yet.\n");
				return 1;
			} else {
				if (Read(buf,2,0x80000400)) {printf("read failed\n"); return 1;}
				// Neon64 starts with a beq r0,r0
				if (buf[0]==0x10 && buf[1]==0x00) {
					printf("Neon64 already loaded.\n");
				} else {
					printf("unknown game in RAM, please report your situation to\nhalleyscometsoftware@hotmail.com\n");
					return 1;
				}
			}
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


int Upload(unsigned char * buffer, unsigned long size, unsigned long address) {
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

// my guess: used to get the GS and PC sync'd
int GSFcn1(int x) {
	int timeout=0x3e8;
	unsigned char result;
	
	while (timeout) {

		result<<=4;
		result|=SendNibble(x);

		// when we recieve a 6, then a 7, we're sync'd
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

// perhaps this is what actually freezes operation?
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
