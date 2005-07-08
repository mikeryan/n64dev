/*

    Author:  Garth Elgar
    Compile: gcc -Wall -O2 -o insertfs insertfs.c
			 or with ms visual c++ using -lwsock32.lib
    Purpose: Insert a filing system into a Nintendo 64 ROM
    Usage:   insertfs <n64romfile> *.jpg

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#define O_BINARY 0
#endif

#ifndef _MIPS_DECL_H_
/* MIPS datatypes */
typedef unsigned char                   u8;     /* unsigned  8-bit */
typedef unsigned short                  u16;    /* unsigned 16-bit */
typedef unsigned long                   u32;    /* unsigned 32-bit */
#ifdef WIN32
typedef unsigned __int64                u64;    /* unsigned 64-bit */
#else
typedef unsigned long long              u64;    /* unsigned 64-bit */
#endif
#endif

#define FILENAME_LEN 24
typedef struct fs { // sizeof=2*16
  u8 filename[FILENAME_LEN];
  u32 length;
  u32 next_file;
} fs_struct;

int mult(int x,int n)
{
  int num=x;
  while (num%n!=0) num++;
  return num;
}

// network to host u64
u64 ntohu64(u64 n)
{
  return (((n&0xff)<<56)|((n&0xff00)<<40)|((n&0xff0000)<<24)|((n&0xff000000)<<8)
	  |((n&0xff00000000)>>8)|((n&0xff0000000000)>>24)|((n&0xff000000000000)>>40)|
	  ((n&0xff00000000000000)>>56));
}

void addfile(int fin,int fout,char* inname,char* outname)
{
  unsigned int length;
  fs_struct file;
  int j,len;
  u32 buf[100];
  
  length=lseek(fin,0,SEEK_END);
  file.length=htonl(length);
  printf("Adding file %s (length %d) to %s\n",
	 inname,ntohl(file.length),outname);
  
  strncpy(file.filename,inname,FILENAME_LEN);
  for (j=strlen(file.filename)+1;j<FILENAME_LEN;j++)
    file.filename[j]='\0';
  if (strlen(inname)>strlen(file.filename))
    printf("Warning: filename truncated to %s\n",file.filename);
  
  file.next_file=htonl(mult(sizeof(fs_struct)+length,2));
  // printf("next file %d\n",ntohl(file.next_file));
  write(fout,&file,sizeof(fs_struct));
  lseek(fin,0,SEEK_SET);
  
  while ((len=read(fin,buf,sizeof(buf)))>0)
    write(fout,buf,len);
  
  for (;((sizeof(fs_struct)+length)%2);length++)
    write(fout,"\0",1);
	close(fin);
}

int main(int argc,char *argv[])
{
  int i,j;
  unsigned int lastheader,currentpos;

  u64 bits,magic=(0x524f4d4653000001);
  int fin,fout,len;
  fs_struct header;

  void *ptr;

  if (argc<3) {
    printf("insertfs <rom file> file1 file2 ...\n");
    exit(0);
  }

  if ((fout=open(argv[1],O_RDWR|O_BINARY,0))<0)
    {
      printf("error opening rom\n");
      exit(1);
    }
  
  do
    {
      if (read(fout,&bits,8)==0)
	{
	  printf("Error reading rom file\n");
	  exit(1);
	}
    bits=ntohu64(bits);
    }
  while (bits!=magic);

  for (i=2;i<argc;i++)
    {
#ifdef WIN32
      // okay..how long do you think it took me to figure out
      // how to do this in windows? argh
      struct _finddata_t ffblk;
      int done=0,ref;
      
      ref=_findfirst(argv[i],&ffblk);
      
      while ((ref>=0) && (!done))
	{
	  if ((fin=open(ffblk.name,O_RDONLY|O_BINARY,0))<0) 
	    {
	      printf("Error opening file %s\n",argv[i]);
	      exit(0);
	    }
	  lastheader=lseek(fout,0,SEEK_CUR);
	  addfile(fin,fout,ffblk.name,argv[1]);
	  done=_findnext(ref,&ffblk);
	}
#else
      if ((fin=open(argv[i],O_RDONLY|O_BINARY,0))<0) 
	{
	  printf("Error opening file %s\n",argv[i]);
	  exit(0);
	}
      lastheader=lseek(fout,0,SEEK_CUR);
      addfile(fin,fout,argv[i],argv[1]);
#endif
    }
  currentpos=lseek(fout,0,SEEK_CUR);
  lseek(fout,lastheader,SEEK_SET);
  read(fout,&header,sizeof(fs_struct));
  header.next_file=0;
  lseek(fout,lastheader,SEEK_SET);
  write(fout,&header,sizeof(fs_struct));
  lseek(fout,currentpos,SEEK_SET);     
  
  // dont just write 1 byte at a time
  // cos win32 does it *very* slowly
  // set minimum rom size
  i=j=lseek(fout,0,SEEK_CUR);
  while(i<(1024*512*3)) i++;
  ptr=calloc(1,i-j);
  write(fout,ptr,i-j);
  free(ptr);

  // pad to 4mbit boundary
  i=j=lseek(fout,0,SEEK_CUR);
  while (i%(1024*512)) i++;
  ptr=calloc(1,i-j);
  write(fout,ptr,i-j);
  free(ptr);

#ifdef WIN32
  chsize(fout,len=lseek(fout,0,SEEK_CUR));
#else
  ftruncate(fout,len=lseek(fout,0,SEEK_CUR));
#endif

  // checksum routine ripped from nifty
  printf("Fixing checksum\n");

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

  lseek(fout, CKSUM_START, SEEK_SET);
  {
    int swapped=0;
    unsigned char buf[BUFSIZE];
    unsigned long sum1, sum2;
    unsigned long i;
    unsigned long c1, k1, k2;
    unsigned long t1, t2, t3, t4, t5, t6;
    unsigned int n;
    long clen = CKSUM_LENGTH;
    long rlen = len - CKSUM_START;

    t1 = t2 = t3 = t4 = t5 = t6 = CKSUM_STARTVAL;

    for (;;) {
      if (rlen > 0) {
        n = read(fout,buf, min2(BUFSIZE, clen));
        if ( (n & 0x03) != 0) {
          n += read(fout, buf+n, 4-(n&3));
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

    L2B(sum1, &buf[0], swapped);
    L2B(sum2, &buf[4], swapped);
    lseek(fout, CKSUM_HDRPOS, SEEK_SET);
    write(fout,buf,8);
  }

  close(fout);
  printf("All done!\n");
  return 0;
}
