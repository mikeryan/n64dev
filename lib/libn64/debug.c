/* debug.c - Debugging functions */

#include <mips.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include "libn64.h"

/*
 * print -- do a raw print of a string
 */ 
void print(char *ptr)
{
  write(1,ptr,strlen(ptr));
}

/*
 * putnum -- print a 32 bit number in hex
 */
int putnum (unsigned int num)
{
  char  buffer[9];
  int   count;
  char  *bufptr = buffer;
  int   digit;
  
  for (count = 7 ; count >= 0 ; count--) {
    digit = (num >> (count * 4)) & 0xf;
    
    if (digit <= 9)
      *bufptr++ = (char) ('0' + digit);
    else
      *bufptr++ = (char) ('a' - 10 + digit);
  }

  *bufptr = (char) 0;
  print (buffer);
  return 0;
}

void send_hex_u32(u32 h)
{
  int i;
  u8 t;
  printf("0x");
  for (i=28;i>=0;i-=4) {
    t=(h>>i)&0x0F;
    if (t<=9) putchar(t+'0'); else putchar(t-10+'A');
  }
}

void dump_mem(u32 *ptr,int n)
{
  int i=0;
  for (i=1;(i+3)<=n;i+=4) {
    send_hex_u32((u32)&(ptr[i-1])); printf(": ");
    send_hex_u32(ptr[i-1]); putchar(' ');
    send_hex_u32(ptr[i]); putchar(' ');
    send_hex_u32(ptr[i+1]); putchar(' ');
    send_hex_u32(ptr[i+2]); putchar(' '); putchar('\n');  
  }
  if (i<=n) {
    send_hex_u32((u32)&(ptr[i-1])); printf(": ");
    while (i<=n) {
      send_hex_u32(ptr[i++]); putchar(' ');
    }
    putchar('\n');
  }
}


