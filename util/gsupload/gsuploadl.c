#include <stdio.h>
#include <asm/io.h>

// GSUpload
// An open source N64 Game Shark (tm) uploader
// based on disassembly of "Generic Uploader" and official utilities
// Linux version

unsigned int LPT1=0x378;

int Upload(unsigned char * buffer, unsigned long size, unsigned long address);
int UploadFile(FILE * infile, unsigned long address);
unsigned long Read(unsigned long address);
int GSFcn1(int);
int SendNibble(int);
int ReadWriteNibble(int);
int ReadWriteByte(int);
unsigned long ReadWrite32(unsigned long);
int CheckGSPresence(void);

int main(int argc, char ** argv) {
	unsigned char buf[4]={8,0xc,0,0};
	FILE * infile, * romfile;

	printf("Neon64 GS uploader v0.1 (Linux version)\n");

	if (argc != 2) {
		printf("usage: %s rom.nes\n",argv[0]);
		return 1;
	}

	if (ioperm(LPT1,1,1) || ioperm(LPT1+1,1,1)) {
		printf("couldn't get LPT1, are you root?\n");
	}

	infile=fopen("neon64gs.bin","rb");
	if (!infile) {printf("error opening neon64gs.bin\n"); return 1;}

	romfile = fopen(argv[1],"rb");
	if (!romfile) {printf("error opening %s\n",argv[1]); return 1;}

	if (Upload(buf,4,0x80263844)) {printf("\nupload failed\n"); return 1;}
	if (UploadFile(infile,0x80300000)) {printf("\nupload failed\n"); return 1;}
	fclose(infile);

	printf("press a key when the Neon64 screen appears\n");
	getchar();
	
	if (UploadFile(romfile,0x80300000)) {printf("\nupload failed\n"); return 1;}
	fclose(romfile);

	return 0;
}

int Upload(unsigned char * buffer, unsigned long size, unsigned long address) {
	unsigned long c=0;
	int timeout=0x3e8;
	while (GSFcn1(3) && timeout) {
		timeout--; c++;
		//printf("init failed, try %d\n",c);
	}
	if (!timeout) {printf("init failed\n"); return 1;}
	CheckGSPresence();

	ReadWriteByte(2);
	ReadWrite32(address);
	ReadWrite32(size);
	
	for (c=0; c < size; c++) ReadWriteByte(buffer[c]);

	for (c=0; c < 8; c++) ReadWriteByte(0);

	if (GSFcn1(3)) return 1;
	CheckGSPresence();
	ReadWriteByte(0x64);
	
	return 0;
}

int UploadFile(FILE * infile, unsigned long address) {
	unsigned long c=0;
	int timeout=0x3e8;
	unsigned long size;
	char buf;
	
	if (fseek(infile,0,SEEK_END)) return 1;
	size=ftell(infile);
	rewind(infile);

	while (GSFcn1(3) && timeout) {
		timeout--; c++;
		//printf("init failed, try %d\n",c);
	}
	if (!timeout) {printf("init failed\n"); return 1;}
	CheckGSPresence();

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

	if (GSFcn1(3)) return 1;
	CheckGSPresence();
	ReadWriteByte(0x64);

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
		//printf("result==%02x\n",result);
		if (result==0x67) break;

		//usleep(100);

		SendNibble(x);
		SendNibble(x>>4);

		//usleep(100);

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


	if (!timeout) {printf("SendNibble timed out2\n"); return 0;}
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

