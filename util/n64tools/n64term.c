/*

    Author:  Garth Elgar
    Compile: gcc -Wall -O2 -o n64term n64term.c
    Purpose: Terminal to communiate with N64
    Usage:   n64term

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
#include <unistd.h>
#include <asm/io.h>

int data_reg=0x310;
int status_reg=0x312;
int control_reg;

void send_byte(unsigned char c)
{
outb(c,data_reg);
}

char read_byte()
{
  while (inb(status_reg)&1) ;
  //printf("received %c\n",inb(data_reg));
  return (inb(data_reg));
}

unsigned char pc_swap_byte(char c,int wait)
     /* Send a byte, then recv a byte. Return the recv'd byte */
{
  char status;
  outb (c,data_reg);
  do
    {
      status=inb (status_reg);
      if (wait) usleep (100000);
    }
  while (status&1);
  status=inb(data_reg);
  return status;
}

main() 
{
  unsigned char c;
  int n,nbytes,i;
  char *buf;
  
  if (setuid(0)) {
    printf("setuid failed\nThis program must run with root privs\n");
    exit(1);
  }
  
  if (iopl(3)) { 
    printf("Port IO Access denied - no root? no procomm card?\n"); 
    exit(1); 
  }
  
  inb(data_reg);
  printf("PC:\tHandshaking...\n");
  do
    {
      while (pc_swap_byte('W',1)!='R');
    }
  while (pc_swap_byte('B',1)!='W');
  printf("PC:\tHandshaking done\n");
  
  send_byte('p'); // send mode
  while (1) {
    switch (c=read_byte()) {
    case 'r':
      send_byte('r');
      n=read_byte();
      buf=(char*)malloc(n);
      nbytes=read(STDIN_FILENO,buf,n);
      send_byte(nbytes);
      for (i=0;i<nbytes;i++)
	{
	  read_byte();
	  send_byte(buf[i]);
	}
      free(buf);
      break;
    case 'w':
      send_byte('w');
      n=read_byte();
      buf=(char*)malloc(n);
      send_byte(n);
      for (i=0;i<n;i++)
	{
	  buf[i]=read_byte();
	  send_byte(buf[i]);
	}
      nbytes=0;
      do {
	nbytes+=write(STDOUT_FILENO,&buf[nbytes],n-nbytes);
      }
      while (nbytes<n);
      free(buf);
      break;
    default:
      write(STDOUT_FILENO,&c,1);
      send_byte(c);
    }
    fflush(stdin);
    fflush(stdout);
  }
}
