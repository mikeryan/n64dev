/*

    Author:  BUNG Enterprises
    Compile: gcc -Wall -O2 -o drjr drjr.c
    Purpose: Communication with V64JR
    Usage:   [-w] [-v] [-t] [-a] <File>

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

/*#include <dos.h>*/
#include <stdio.h>
#include <stdlib.h>
/*#include <io.h>*/
/*#include <dir.h>*/
#include <unistd.h>


//#define ai 0x37b
//#define data 0x37c
#define trans_size 32768
#define set_ai_write outportb(port_a,5);		// ninit=1, nwrite=0
#define set_data_write outportb(port_a,1);	// ninit=0, nwrite=0
#define set_data_read outportb(port_a,0);	// ninit=0, nwrite=1
#define set_normal outportb(port_a,4);		// ninit=1, nwrite=1

char *file_name=NULL;
char write_en=0;
char test_en=0;
char verify_en=0;
char disp_on=1;
FILE *fptr;
union mix_buffer {
unsigned char buffer[32768];
unsigned short int bufferx[16384];
}mix;
unsigned short int port[2];
unsigned char port_no;
unsigned short int port_8,port_9,port_a,port_b,port_c;
unsigned char disp_buf[16];
short int i,j,page,sel,err,wv_mode;
char ch=' ';

unsigned char inportb(arg1)
{
  __asm__("
  movl    8(%ebp),%edx
  inb     %dx,%al
  movzbl  %al,%eax                  
  ");
}

unsigned char outportb(arg1,arg2)
{
  __asm__("
  movl    0x8(%ebp),%edx
  movl    0xc(%ebp),%eax
  outb    %al,%dx
  ");
}

unsigned short int inport(unsigned int arg1)
{
  __asm__("
  movl    8(%ebp),%edx
  inw     %dx,%ax
  ");
}

unsigned short int outport(unsigned int arg1,unsigned int arg2)
{
  __asm__("
  movl    0x8(%ebp),%edx
  movl    0xc(%ebp),%eax
  outw    %ax,%dx
  ");
}

/**************************************
*               Subroutine            *
**************************************/
void dump_buffer(void)
{
for (i=0; i<128 ; i++)
   {
   j=i & 0x0f;
   disp_buf[j]=mix.buffer[i];
   if (j==15)
      {
      printf("%04x : ",i&0xfff0);
      for (j=0; j<16 ; j++)
	 {
	 if (j==8) printf("- ");
	 printf("%02x ",disp_buf[j]);
	 }
      printf("-> ");
      for (j=0; j<16 ; j++)
	 {
	 if (disp_buf[j] < 0x20 || disp_buf[j] > 0x80)
	    printf (".");
	 else
	    printf ("%c",disp_buf[j]);
	 }
      printf("\n");
      }
   }
}

void set_ai(unsigned char _ai)
{
   set_ai_write			// ninit=1, nwrite=0
   outportb(port_b,_ai);
}

void set_ai_data(unsigned char _ai,unsigned char _data)
{
   set_ai(_ai);
   set_data_write		// ninit=0, nwrite=0
   outportb(port_c,_data);
}

void init_port(void)
{
   outportb(port_9,1);		// clear EPP time flag
   set_ai_data(6,0x0a);
   set_ai_data(7,0x05);		// 6==0x0a, 7==0x05 is pc_control mode
//   set_ai(5);
//   set_data_read
//   write_en=inportb(port_c);
   set_ai_data(5,write_en);		// d0=0 is write protect mode
}

void end_port(void)
{
   set_ai_data(5,write_en);		// d0=0 is write protect mode
   set_ai_data(7,0);		// release pc mode
   set_ai_data(6,0);		// 6==0x0a, 7==0x05 is pc_control mode
   set_normal			// ninit=1, nWrite=1
}

char write_32k(unsigned short int hi_word, unsigned short int lo_word)
{
   unsigned char unpass,pass1;
   unsigned short int i,j;
   unsigned short int fix,temp;
   init_port();
   set_ai_data(3,0x10|(hi_word>>8));
   set_ai_data(2,(hi_word & 0xff));
   for (i=0;i<0x40;i++){
      unpass=3;
      while(unpass){
	 set_ai_data(1,((i<<1)|lo_word));
	 set_ai_data(0,0);
	 set_ai(4);		// set address index=4
	 set_data_write		// ninit=0, nWrite=0
	 fix=i<<8;
	 for (j=0;j<256;j++){
	    outport(port_c,mix.bufferx[j+fix]);
	 }
	 set_data_read		// ninit=0, nWrite=1
	 if (wv_mode){
	    for (j=0;j<256;j++){
	       temp=inport(port_c);
	       if(mix.bufferx[j+fix]!=temp){
//		  printf("%2x%2x dram=%x, buffer=%x\n",i,j*2,temp,mix.bufferx[j+fix]);
		  break;
	       }
	    }
	 }
	 else{
	    pass1=1;
	    for (j=0;j<4;j++){
	       temp=inport(port_c);
	       if(mix.bufferx[j+fix]!=temp){
//		  printf("%2x%2x dram=%x, buffer=%x\n",i,j*2,temp,mix.bufferx[j+fix]);
		  pass1=0;
		  break;
	       }
	    }
	    if (pass1) {
//	       printf("@");
	       set_ai_data(1,((i<<1)|lo_word|1));
	       set_ai_data(0,0xf8);
	       set_ai(4);
	       set_data_read		// ninit=0, nWrite=1
	       for (j=252;j<256;j++){
		  temp=inport(port_c);
		  if(mix.bufferx[j+fix]!=temp){
//		     printf("%2x%2x dram=%x, buffer=%x\n",i,j*2,temp,mix.bufferx[j+fix]);
		     break;
		  }
	       }
	    }
	 }
	 set_ai(0);
	 set_data_read			// ninit=0, nwrite=1
	 if (inportb(port_c)!=0x00){
	    unpass--;
//	    printf("counter=%x ",inportb(data));
	    outportb(port_a,0x0b);	// set all pin=0 for debug
	    if (disp_on) printf("*");
	    init_port();
	    set_ai_data(3,0x10|(hi_word>>8));
	    set_ai_data(2,(hi_word & 0xff));
	    if (unpass==0)
	       return(1);
	 }
	 else
	    unpass=0;
      }
   }
//   outportb(ai,0);
//   printf("\na[7..0]=%02x\n",inportb(data));
//   outportb(ai,1);
//   printf("a[15..8]=%02x\n",inportb(data));
//   end_port();
   return(0);
}

char verify_32k(unsigned short int hi_word, unsigned short int lo_word)
{
   char unpass;
   unsigned short int i,j,temp;
   unsigned short int fix;
   init_port();
   set_ai_data(3,0x10|(hi_word>>8));
   set_ai_data(2,(hi_word & 0xff));
//   set_ai_data(3,0x10);
//   set_ai_data(2,hi_word);
   for (i=0;i<0x40;i++){
      unpass=3;
      while(unpass){
	 set_ai_data(1,((i<<1)|lo_word));
	 set_ai_data(0,0);
	 set_ai(4);
	 set_data_read			// ninit=0, nwrite=1
	 fix=i<<8;
	 for (j=0;j<256;j++){
	    temp=inport(port_c);
	    if (temp!=mix.bufferx[j+fix]){
//	       printf("verify error!!!\07\n");
//	       printf("%2x%2x dram=%x, buffer=%x\n",i,j*2,temp,mix.bufferx[j+fix]);
	       outportb(port_a,0x0b);	// all pin=0 for debug
	       if (disp_on) printf("#");
	       init_port();
	       set_ai_data(3,0x10|(hi_word>>8));
	       set_ai_data(2,(hi_word & 0xff));
	       unpass--;
	       if (unpass==0)
		   return(1);
	       else
		  break;
	    }
	 }
	 if (j==256) break;
      }
   }
//   outportb(ai,0);
//   printf("\na[7..0]=%02x\n",inportb(data));
//   outportb(ai,1);
//   printf("a[15..8]=%02x\n",inportb(data));
//   end_port();
   return(0);
}

void read_adr(void)
{
   set_ai_data(6,0x0a);		// enable pc mode
   set_ai_data(7,0x05);		// enable pc mode
   printf("\na[31..0]=");
   set_ai(3);
   set_data_read		// ninit=0, nwrite=1
   printf("%02x",inportb(port_c));
   set_ai(2);
   set_data_read		// ninit=0, nwrite=1
   printf("%02x",inportb(port_c));
   set_ai(1);
   set_data_read		// ninit=0, nwrite=1
   printf("%02x",inportb(port_c));
   set_ai(0);
   set_data_read		// ninit=0, nwrite=1
   printf("%02x\n",inportb(port_c));
   end_port();
}

void read_some(void)
{
   init_port();
   set_ai_data(0,0);
   set_ai_data(1,0);
   set_ai_data(2,0);
   set_ai_data(3,0x10);
   set_ai(4);
   set_data_read		// ninit=0, nWrite=1
   for (i=0;i<64;i++) {
      mix.bufferx[i]=inport(port_c);
   }
   dump_buffer();
   read_adr();
}

unsigned char check_card(void)
{
   init_port();
   set_ai_data(3,0x12);
   set_ai_data(2,0x34);
   set_ai_data(1,0x56);
   set_ai_data(0,0x78);
   set_ai(3);
   set_data_read		// ninit=0, nwrite=1
   if ((inportb(port_c)&0x1f)!=0x12) return(1);
   set_ai(2);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x34) return(1);
   set_ai(1);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x56) return(1);
   set_ai(0);
   set_data_read		// ninit=0, nwrite=1
   if (inportb(port_c)!=0x78) return(1);
   end_port();
   return(0);
}

short int read_file(void)
{
   if (fread((char *)mix.buffer,sizeof(char),trans_size,fptr)!=trans_size)
      {
      fclose(fptr);	/* read data error */
      return(-1);
      }
   printf(".");
   fflush(stdout);
   return(0);
}

short int download_n64()
{
   if((fptr=fopen(file_name,"rb"))==NULL)
      {/* open error */
      printf("open error !!!\07\n");
      return(-1);
      }
   if (sel==0)
      printf("Downloading");
   else
      printf("Verifying");
   for (page=0;page<0x200;page++) {
      if (read_file()!=0){
	 /*fclose(fptr);*/
	 printf("\n");
	 return (0);
      }
      if (sel==0){
	 if (write_32k(page,0)) {
	    read_adr();
	    /*fclose(fptr);*/
	    return(-1);
	 }
      }
      else{
	 if(verify_32k(page,0)) {
	    read_adr();
	    /*fclose(fptr);*/
	    return(-1);
	 }
      }
      if (read_file()!=0){
	 /*fclose(fptr);*/
	 return (-1);
      }
      if (sel==0){
	 if (write_32k(page,0x80)) {
	    read_adr();
	    /*fclose(fptr);*/
	    return(-1);
	 }
      }
      else{
	 if(verify_32k(page,0x80)) {
	    read_adr();
	    /*fclose(fptr);*/
	    return(-1);
	 }
      }

      }
   printf("\n");
   /*fclose(fptr);*/
   end_port();
   return(0);
}

void gen_pat_32k(unsigned short int offset)
{
   for (i=0; i<0x4000; i++){
      mix.bufferx[i]=i+offset;
   }
}


unsigned short int test_dram(void)
{
   unsigned short int pages;
   disp_on=0;		// display off
   sel=0;		//write 32k dram data
   gen_pat_32k(0x0000);
   write_32k(0,0);
   gen_pat_32k(0x8000);
   write_32k(0x100,0);
   gen_pat_32k(0x0000);
   pages=0;
   if (verify_32k(0,0)==0){	// find lower 128Mbits
      pages=0x100;
   }
   gen_pat_32k(0x8000);
   if (verify_32k(0x100,0)==0){	// find upper 128Mbits
      pages=0x200;
   }
   printf("DRAM testing...");
   for (page=0; page<pages; page++){
       gen_pat_32k(page*2);
       if (write_32k(page,0))
	  return(0);
       else
	  printf("w");
       fflush(stdout);
       gen_pat_32k(page*2 +1);
       if (write_32k(page,0x80))
	  return(0);
       else
	  printf("w");
       fflush(stdout);
   }
   for (page=0; page<pages; page++){
       gen_pat_32k(page*2);
       if (verify_32k(page,0))
	  return(0);
       else
	  printf("v");
       fflush(stdout);
       gen_pat_32k(page*2 +1);
       if (verify_32k(page,0x80))
	  return(0);
       else
	  printf("v");
       fflush(stdout);
   }
   return(pages);
}




void usage(char *progname)
{
   fprintf(stderr, "Usage: %s [-w] [-v] [-t] [-a] <File>\n", progname);
   fprintf(stderr, "-w : DRAM write protect disabled.\n");
   fprintf(stderr, "-v : verify File data vs DRAM data.\n");
   fprintf(stderr, "-t : test DRAM.\n");
   fprintf(stderr, "-a : enable cartridge and unprotect.\n");
   /*end_port();*/
   exit(2);
}


/*************************************************
*                  MAIN ENTRY                    *
*************************************************/
int main(int argc, char *argv[])
{
   char card_present;
   char *progname=argv[0];
   unsigned short int dram_size;
   printf("---------- DrJr pc-download EPP version 1.0 ----------\n");
/*
   port[0]=peek(0x40,8); 		// lpt1 base address
   port[1]=peek(0x40,10);		// lpt2 base address
   if (port[0]==0){
      printf("No Printer Port Avialable!\07\n");
      return(-1);
   }
*/
   port[0]=0x378;
   port[1]=0;
   ioperm(0x378, 6, 1);

    if (argc==1) usage(progname);
    for( i=1; i<argc; i++ ) {
       if( argv[i][0] == '-' ) {
	  char *c = argv[i]+1;
	  if( *(c+1) != '\0' ) usage(progname);
	  switch( *c ) {
	     case 'w':
		write_en = 1;
		break;
	     case 'v':
		verify_en = 1;
		break;
	     case 't':
		test_en = 1;
		break;
	     case 'W':
		write_en = 1;
		break;
	     case 'V':
		verify_en = 1;
		break;
	     case 'T':
		test_en = 1;
		break;
	     case 'a':
		write_en = 3;
		break;
	     default:
		usage(progname);
	  }
       }
       else {
	  if( file_name == NULL ) {
	     file_name = argv[i];
	  }
	  else {
	     usage(progname);
	  }
       }
    }
//   printf("program name=%s\n",progname);
//   printf("write_en=%x, verify=%x, test=%x\n",write_en,verify_en,test_en);
//   printf("filename=%s\n",file_name);

   wv_mode=0;
   if (verify_en){      // if sel=2 for write/verify
      sel=1;		// sel=1 for verify
   }
   else{
      sel=0;		// sel=0 for write
   }
   if (port[1]==0)
      port_no=1;		// only one printer port
   else
      port_no=2;		// two printer port
   card_present=0;
   for (i=0;i<port_no;i++){
      port_8=port[i];
      port_9=port_8+1;
      port_a=port_9+1;
      port_b=port_a+1;
      port_c=port_b+1;
      if (check_card()==0){
	 card_present=1;
	 break;
      }
   }

   if (card_present==0){
      printf("\nNo V64jr card present!!!\07\n\n");
      return(-1);
   }
   else
      printf("V64jr card found at port%d\n",port_no);
   init_port();
   set_ai(3);
   set_data_read
   printf("control(rst,wdf,rcf) = %x\n",inportb(port_c));


   if (test_en){
      dram_size=test_dram();
      if (dram_size){
	 printf("\nDRAM size=%dMbits\n",(dram_size/2));
      }
      else {
	 printf("Error!!!\07\n");
      }
   }
   disp_on=1;		// display #/*
   if (file_name!=NULL){
      if (download_n64()!=0)
	 printf("download error !!!\n");
   }
   if (write_en)
      printf("dram write protect disable\n");
   if (write_en & 2)
      printf("run cart enable\n");

//   set_ai_data(5,write_en);		// d0=0 is write protect mode
   end_port();
   return(0);
}

