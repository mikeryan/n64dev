/* cd64.c - This file was built upwards from reverse engineering the cd64 bios.
   Should tidy it up at some point */

#include <mips.h>
#include <unistd.h>
#include <stddef.h>
#include "libn64.h"

#define CD64REG_MODE0    0xB7800000
#define CD64REG_MODE1    0xB780000C
#define CD64REG_CDDATA16 0xB7800010
#define CD64REG_CDDATA32 0xB7800018
#define CD64REG_IO       0xB7800020
#define CD64REG_STATUS   0xB7800040
#define CD64REG_DATARD   0xB7800080
#define CD64REG_DATAWR   0xB7800084

#define CD64_WRITE(addr,data) *(vu32*)addr=(u32)data
#define CD64_READ(addr) *(vu32*)addr
//#define  CD64_WRITE(addr,data) IO_WRITE(addr,data)
//#define CD64_READ(addr) IO_READ(addr)

void cd64_init(cartemu *debug)
{

  cd64_pc_connect();

  debug->runCart=cd64_runcart;
  debug->sendByte=cd64_sendbyte;
  debug->recvByte=cd64_recvbyte;
  debug->read=cd64_read;
  debug->write=cd64_write;

  print("Communication established\n");
  print("=========================\n");

}

//#define PI_WAIT2 {u32 i=(int)0xffff;while(i--);PI_WAIT;}
#define PI_WAIT2 PI_WAIT
void cd64_sendbyte(char c)
{
  PI_WAIT;
  //cd64_mode(0);
  CD64_WRITE(CD64REG_DATAWR,c);
  //cd64_mode(5);
  PI_WAIT;
  //PI_WAIT2; // important! this may appear unnecessary because of the PI_WAIT
           // in cd64_mode, but for some reason it is 
}

char cd64_recvbyte()
{
  char c;

  PI_WAIT;
  //cd64_mode(0);
  while ((CD64_READ(CD64REG_STATUS)&0x40)==0);
  //PI_WAIT;
  c=(char)(CD64_READ(CD64REG_DATARD));
  //cd64_mode(5);
  PI_WAIT;
  //PI_WAIT2; // important! see above!

  return c;
}

void cd64_pc_connect()
{	
  char c,mode;
  
  cd64_mode(0);
  do {
    c=cd64_recvbyte();
    if (c=='W') cd64_sendbyte('R');
  }
  while (cd64_recvbyte()!='B');
  cd64_sendbyte('W');
  mode=cd64_recvbyte();

  if (mode=='p') {
    // pc connection okay
  }
  cd64_mode(5);
}

void cd64_mode(int mode)
{
	PI_WAIT; CD64_WRITE(CD64REG_MODE1,0x000a000a);
	PI_WAIT; CD64_WRITE(CD64REG_MODE0,mode);
	PI_WAIT; CD64_WRITE(CD64REG_MODE1,0);
	PI_WAIT;
}

void cd64_runcart()
{
  void (*f)();
  cd64_mode(7);
  piDmaRead((u32*)0x80000000,(u32*)0xB00004C0,692);
  PI_WAIT;
  f=(void((*)()))0x80000000;
  f();
}

int cd64_read(int fd,void *buf,size_t nbytes)
{
  int i = 0,rx_bytes=0;
  cd64_mode(0);
  if (fd==STDIN_FILENO)
    {
      debug.sendByte('r');
      debug.recvByte();
      debug.sendByte(nbytes);
      rx_bytes=debug.recvByte();
  
      for (i = 0; i < rx_bytes; i++)
	{
	debug.sendByte(i);
	*((char*)buf + i) = debug.recvByte();
	}
    }
  cd64_mode(5);
  return (rx_bytes);
}


int cd64_write(int fd,const void *buf,size_t nbytes)
{
  int i,commit,tx_bytes=0;
  cd64_mode(0);
  if (fd==STDOUT_FILENO || fd==STDERR_FILENO)
    {
      debug.sendByte('w');
      debug.recvByte();
      debug.sendByte(nbytes);
      commit=debug.recvByte();
      
      tx_bytes=nbytes>commit?commit:nbytes;
      
      for (i = 0; i < tx_bytes; i++) {
	debug.sendByte(*((char*)buf + i));
	debug.recvByte();
      }
    }
  cd64_mode(5);
  return (tx_bytes);
}

/*  CD64 BIOS FUNCTIONS 

The CD64 BIOS uses something like these; finally I think I know how it
works well enough to write the above functions but keep these here
just in case.

only reads and writes to 4 byte aligned addresses
#define CD64_WRITE(addr,data) PI_WAIT; *(u32*)(((u32)addr)&0xBFFFFFFC)=(u32)data
#define CD64_READ(addr) *(u32*)(((u32)addr)&0xBFFFFFFC)

Now I use the more standard IO_READ and IO_WRITE

u8 cd64rb(u32 *address) 
{
  PI_WAIT;
  return (u8)(CD64_READ(address));
}

u32 cd64rw(u32 *address) 
{
  PI_WAIT;
  return CD64_READ(address);
}

char read_echo_byte(int wait)
{
  char c;
  c=cd64_recvbyte();
  cd64_sendbyte(c);
  return c;
}

void send_echo_byte(char c)
{
  cd64_sendbyte(c);
  cd64_recvbyte();
}

*/





