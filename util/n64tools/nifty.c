#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 32768
#define min2(a, b) ( (a)<(b) ? (a) : (b) )

#define CKSUM_START 0x1000
#define CKSUM_LENGTH 0x100000L
#define CKSUM_HDRPOS 0x10
#define CKSUM_END (CKSUM_START + CKSUM_LENGTH)

#define CKSUM_STARTVAL 0xf8ca4ddc

#define ROL(i, b) (((i)<<(b)) | ((i)>>(32-(b))))

#define B2L(b, s) ( (((b)[0^(s)] & 0xffL) << 24) | \
                           (((b)[1^(s)] & 0xffL) << 16) | \
                           (((b)[2^(s)] & 0xffL) <<  8) | \
                           (((b)[3^(s)] & 0xffL)) )

#define L2B(l, b, s)  (b)[0^(s)] = ((l)>>24)&0xff; \
                             (b)[1^(s)] = ((l)>>16)&0xff; \
                             (b)[2^(s)] = ((l)>> 8)&0xff; \
                             (b)[3^(s)] = ((l)    )&0xff;

#define HEADER_MAGIC 0x80371240

int swapped;

void
change_address(FILE *f, unsigned int addr)
{
  unsigned char buf[4];
  
  L2B(addr, buf, swapped);
  
  fseek(f, 8, SEEK_SET);
  fwrite(buf, 1, 4, f);
}

void
change_name(FILE *f, char *name)
{
  char n[21];
  char buf;
  int i;
  
  strncpy(n, name, 20);
  n[20] = '\0';
  
  if (strlen(n) < 20) {
    i = strlen(n);
    for (;i < 20 ; i++) {
      n[i] = '\0';
    }
  }

  if (swapped)
    for(i = 0 ; i < 20 ; i+=2) {
      buf = n[i];
      n[i] = n[i+1];
      n[i+1] = buf;
    }
  
  fseek(f, 0x20, SEEK_SET);
  fwrite(n, 20, 1, f);
}


void
calculate_chksum(FILE *f)
{
  unsigned char buf[BUFSIZE];
  unsigned long sum1, sum2;
  unsigned long flen;

  fseek(f, 0, SEEK_SET);
  
  if( fread(buf, 1, 12, f) != 12 ) {
    fprintf(stderr, "Oh dear, I just wiped out the first 4K of something that isn't a N64 image. Sorry\n");
    exit(1);
  }

  if ( B2L(buf, 0) == HEADER_MAGIC ) {
    swapped = 0;
  } else if ( B2L(buf, 1) == HEADER_MAGIC ) {
    swapped = 1;
  } else {
    fprintf(stderr, "I don't know the format of this file\n");
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  flen = ftell(f);

  if (flen < CKSUM_END) {
    if (flen < CKSUM_START) {
      fprintf(stderr, "File is too short to be an N64 image\n");
      exit(1);
    } else if ( (flen & 3) != 0 ) {
      fprintf(stderr, "File isn't a multiple of 4 bytes long\n");
      exit(1);
    }
  }

  fseek(f, CKSUM_START, SEEK_SET);
  {
    unsigned long i;
    unsigned long c1, k1, k2;
    unsigned long t1, t2, t3, t4, t5, t6;
    unsigned int n;
    long clen = CKSUM_LENGTH;
    long rlen = flen - CKSUM_START;

    t1 = t2 = t3 = t4 = t5 = t6 = CKSUM_STARTVAL;

    for (;;) {
      if (rlen > 0) {
	n = fread(buf, 1, min2(BUFSIZE, clen), f);
	if ( (n & 0x03) != 0) {
	  n += fread(buf+n, 1, 4-(n&3), f);
	}
      } else {
	n = min2(BUFSIZE, clen);
      }

      if ( (n == 0) || ((n&3) != 0) ) {
	if ( (clen != 0) || (n != 0) ){
	  fprintf(stderr, "A short has been read, cksum may be wrong\n");
	}
	break;
      }

      for ( i = 0 ; i < n ; i += 4 ) {
	c1 = B2L(&buf[i], swapped);
	k1 = t6 + c1;
	if (k1 < t6) t4++;
	t6 = k1;
	t3 ^= c1;
	k2 = c1 & 0x1f;
	k1 =ROL(c1, k2);
	t5 += k1;
	if (c1 < t2) {
	  t2 ^= k1;
	} else {
	  t2 ^= t6 ^ c1;
	}
	t1 += c1 ^ t5;
      }
      if (rlen > 0) {
	rlen -= n;
	if (rlen <= 0) memset(buf, 0, BUFSIZE);
      }
      clen -= n;
    }
    sum1 = t6 ^ t4 ^ t3;
    sum2 = t5 ^ t2 ^ t1;
  }

  L2B(sum1, &buf[0], swapped);
  L2B(sum2, &buf[4], swapped);

  fseek(f, CKSUM_HDRPOS, SEEK_SET);
  fwrite(buf, 1, 8, f);
}

int
main(int argc, char **argv)
{
  FILE *f;
  unsigned int a;
  char n[256];
  
  if (argc != 4) {
    fprintf(stderr, "Usage: nifty <rom name> <address> <image name>\n");
    return 1;
  }

  f = fopen(argv[1], "r+b");
  if (f == NULL) {
    char err[21 + strlen(argv[1])];
    sprintf(err, "Couldn't open file %s", argv[1]);
    perror(err);
    return 1;
  }

  a = strtoul(argv[2], NULL, 0);
  strncpy(n, argv[3], 255);

  calculate_chksum(f);
  change_address(f, a);
  change_name(f, n);
  fclose(f);
  return 0;
}
