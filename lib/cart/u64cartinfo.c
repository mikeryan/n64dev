/* 
 * libultra64
 *
 * Copyright (c) 2004
 * Ryan Underwood
 *
 * May be distributed under GNU LGPL license as published by
 * Free Software Foundation.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ultra64/rom.h>
#include <ultra64/host/cartinfo.h>

char savedesc[][40] = {
	"Unknown Save Type",
	"4K EEPROM (512 Bytes)",
	"16K EEPROM (2048 Bytes)",
	"SRAM (32 KBytes)",
	"FlashRAM (128 KBytes)",
};

static const CountryIDInfo_t CountryCodeInfo[] =
{
	{  0,  "0", OS_TV_NTSC },
	{ '7', "Beta", OS_TV_NTSC },
	{ 'A', "NTSC", OS_TV_NTSC },
	{ 'D', "Germany", OS_TV_PAL },
	{ 'E', "USA", OS_TV_NTSC },
	{ 'F', "France", OS_TV_PAL },
	{ 'I', "Italy", OS_TV_PAL },
	{ 'J', "Japan", OS_TV_NTSC },
	{ 'P', "Europe", OS_TV_PAL },
	{ 'S', "Spain", OS_TV_PAL },
	{ 'U', "Australia", OS_TV_PAL },
	{ 'X', "PAL", OS_TV_PAL },
	{ 'Y', "PAL", OS_TV_PAL },
	{ 0, "", OS_TV_NTSC }
};

void ultra64_header_info(n64header_t *carthead) {

	unsigned int i;

	if (carthead == NULL) return;

	if (memcmp(carthead, "\x80\x37\x12\x40", 4) && memcmp(carthead, "\x37\x80\x40\x12", 4)) {
		printf("Not a recognizable N64 cartridge header.\n");
		/* See if it's a CD64 "empty cart" range. */
		for (i = 0; i < sizeof(n64header_t); i+=4) {
			if (!(
				   ((unsigned char*)carthead)[i] == 0
				&& ((unsigned char*)carthead)[i+1] == i
				&& ((unsigned char*)carthead)[i+2] == 0
				&& ((unsigned char*)carthead)[i+3] == i
			))
				break;
		}
		if (i == sizeof(n64header_t)) {
			printf("If this is a CD64, this appears to be "
				"cart mapped memory with no cart.\n");
		}
		return;
	}
	else {
		printf("* Name: %s\n",carthead->Name);
	}

	printf("* Country: ");
	for (i = 0; ; i++) {
		if (!strcmp(CountryCodeInfo[i].szName,"")) {
			printf(" invalid\n");
			break;
		}
		if (CountryCodeInfo[i].nCountryID == carthead->CountryID) {
			printf("%s (%c)", CountryCodeInfo[i].szName, carthead->CountryID);
			switch (CountryCodeInfo[i].nTvType) {
				case OS_TV_NTSC:
					printf(" [NTSC]\n");
					break;
				case OS_TV_PAL:
					printf(" [PAL]\n");
					break;
				default:
					printf("\n");
					break;
			}
			break;
		}
	}
}

