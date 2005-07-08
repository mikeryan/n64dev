
#ifndef __ROM_H__
#define __ROM_H__

/* Based on Daedalus header */

#define N64HEADER_SIZE 0x40
#define BYTES_IN_MBIT 0x20000

#define SwapEndian(x) \
	  ((x >> 24)&0x000000FF) \
	| ((x >> 8 )&0x0000FF00) \
	| ((x << 8 )&0x00FF0000) \
	| ((x << 24)&0xFF000000)

typedef enum {
	UNKNOWN = 0,
	EEP4K = 1,
	EEP16K = 2,
	SRAM = 3,
	FLASHRAM = 4
} savetype_t;

typedef struct {	/* From Daedalus */
	unsigned char x1; /* initial PI_BSB_DOM1_LAT_REG value */
	unsigned char x2; /* initial PI_BSB_DOM1_PGS_REG value */
	unsigned char x3; /* initial PI_BSB_DOM1_PWD_REG value */
	unsigned char x4; /* initial PI_BSB_DOM1_RLS_REG value */

	unsigned long int ClockRate;
	unsigned long int BootAddress;
	unsigned long int Release;
	unsigned long int CRC1;
	unsigned long int CRC2;
	unsigned long int Unknown0;
	unsigned long int Unknown1;
	char Name[20];
	unsigned long int Unknown2;
	unsigned short int Unknown3;
	unsigned char Unknown4;
	unsigned char Manufacturer;
	unsigned short int CartID;
	char CountryID;
	unsigned char Unknown5;
} n64header_t;

typedef enum {
	OS_TV_NTSC = 0,
	OS_TV_PAL,
	OS_TV_MPAL
} tv_type_t;

typedef struct
{
	char nCountryID;
	char szName[15];
	unsigned long int nTvType;
} CountryIDInfo_t;

#endif
