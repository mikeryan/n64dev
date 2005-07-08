/*

    Author:  cn@UFO
    Compile: gcc -Wall -O2 -o cd64comm cd64comm.c
    Purpose: Communication with CD64
    Usage:   cd64comm -P<port> -T -X -G -D -B -F<fname> -S<start> -L<length

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
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#include <unistd.h>
#include <asm/io.h>
#include <time.h>

int data_reg=0x310;
int status_reg=0x312;
int control_reg;
int ecp_reg;
int ecp_mode;
int ecp=0;
char buf[32768];

enum {X,P,F,S,G,L,T,D,B};
enum {x,p,f,s,g,l,t,d,b};
char _switches[] = {'X', 'P', 'F', 'S', 'G', 'L', 'T', 'D', 'B'};
char switches[sizeof (_switches)];
char *ptr_switches[sizeof (_switches)];


/*--------------------------------------------------------------------------*/
void parse_switches (int argc, char *argv[])
{
        char *cptr;
        char c;
        long i, j, k, l;

        for (i=1; argc>1; argc--)
        {
                cptr=argv[i++];                         // string

                j=0;                                            // first char
                c=cptr[j++];

                if (c=='-')                                     // switch following
                {
                        c=cptr[j];                              // switch
                        c=toupper (c);

                        for (k=0; k<sizeof (switches); k++)
                        {
                                if (c==_switches[k])
                                {
                                        switches[k]=1;          // switch on
                                        j++;                    // identified switch next char

if (isalnum(cptr[j])||(cptr[j]=='/')||(cptr[j]=='\\')) ptr_switches[k]=&cptr[j];

                                                else
                                                {
                                                        cptr=argv[i];                   
                                                        if (isalnum(cptr[0])||(cptr[0]=='/')||(cptr[0]=='\\')) ptr_switches[k]=cptr;
                                                }

                                        break;
                                }
                        }
                }
        }
}


/*--------------------------------------------------------------------------*
 * export to CD64
 * import same data from CD64
 *--------------------------------------------------------------------------*/
void send_byte (unsigned char c)
{
        unsigned char status;
        unsigned long i;

        outb (c,data_reg);
        i=0;
        while ((inb (status_reg)&1)&&(i<800000)) i++;
        if (i<800000)
        {
                if (c!=inb (data_reg))
                {
                        status=inb (data_reg);
                        printf ("\nBad!!");
                        printf ("\nvalue out was %2.2X\nvalue back was %2.2X\n", c, status);
                        exit (11);
                }
        }
                else
                {
                        printf ("\nBad!!");
                        printf ("\ncommunication time out error!!\n");
                        exit (11);
                }
}


/*--------------------------------------------------------------------------*
 * export to CD64  by ECP port
 *--------------------------------------------------------------------------*/
void send_pbyte (unsigned char c)
{
        unsigned char status;
        unsigned long i;

        i=0;
        while ((inb (status_reg)&0x80)&&(i<800000)) i++;
        if (i<800000)
        {
//           _   _
// set /PCRD  |_| to reset PCX
//              outb (control_reg, 0x24);
                outb (0x26,control_reg);
//              outb (control_reg, 0x24);

//           _   _
// set /PCWR  |_| to set PCX
// export to cd64
                outb (0x04,control_reg);
                outb (c,data_reg);
                outb (0x05,control_reg);
                outb (0x04,control_reg);
        }

                else
                {
                        printf ("\nBad!!");
                        printf ("\ncommunication time out error!!\n");
                        exit (11);
                }
}


/*--------------------------------------------------------------------------*
 * export to CD64
 * import new data from CD64
 *--------------------------------------------------------------------------*/
unsigned char exchange_byte (char c)
{
        char status;
        unsigned long i;

        outb (c,data_reg);
        i=0;
        while ((inb (status_reg)&1)&&(i<800000)) i++;
        if (i<800000)
        {
                status=inb (data_reg);
        }
                else
                {
                        printf ("\nBad!!");
                        printf ("\ncommunication time out error!!\n");
                        exit (11);
                }

        return status;
}


/*--------------------------------------------------------------------------*
 * export to CD64
 * import new data from CD64
 *--------------------------------------------------------------------------*/
unsigned char exchange_pbyte (char c)
{
        char status;
        unsigned long i;

        i=0;
        while ((inb (status_reg)&0x80)&&(i<800000)) i++;
        if (i<800000)
        {
//                       _   _
// set /PCRD  |_| to reset PCX
// import from cd64
//              outb (0x24,control_reg);
                outb (0x26,control_reg);
                status=inb (data_reg);
//              outb (control_reg, 0x24);

//                       _   _
// set /PCWR  |_| to set PCX
// export to cd64
                outb (0x04,control_reg);
                outb (c,data_reg);
                outb (0x05,control_reg);
                outb (0x04,control_reg);

        }
                else
                {
                        printf ("\nBad!!");
                        printf ("\ncommunication time out error!!\n");
                        exit (11);
                }

        return status;
}


/*--------------------------------------------------------------------------*
 * export to CD64
 * import new data from CD64
 *--------------------------------------------------------------------------*/
unsigned char read_pbyte (void)
{
        char status;
        unsigned long i;

        i=0;
        while ((inb (status_reg)&0x80)&&(i<800000)) i++;
        if (i<800000)
        {
//                       _   _
// set /PCRD  |_| to reset PCX
// import from cd64
//              outb (0x24,control_reg);
                outb (0x26,control_reg);
                status=inb (data_reg);
//              outb (0x24,control_reg);

//                       _   _
// set /PCWR  |_| to set PCX
// export to cd64
                outb (0x04,control_reg);
                outb (0x05,control_reg);
                outb (0x04,control_reg);

        }
                else
                {
                        printf ("\nBad!!");
                        printf ("\ncommunication time out error!!\n");
                        exit (11);
                }

        return status;
}


/*--------------------------------------------------------------------------*
 * export header to CD64
 * import new data from CD64
 *--------------------------------------------------------------------------*/
unsigned char header_byte (char c)
{
        char status, spinner;

        spinner ='|';
        outb (c,data_reg);
        do
        {

                status=inb (status_reg);

                switch (spinner)
                {
                        case '|':
                                spinner='/';
                                break;

                        case '/':
                                spinner='-';
                                break;

                        case '-':
                                spinner='\\';
                                break;

                        case '\\':
                                spinner='|';
                                break;
                }

                usleep (100000);
                printf ("\b%c", spinner);
	        fflush(0);
        }
        while (status&1);

        status=inb (data_reg);

        return status;
}


/*--------------------------------------------------------------------------*
 * export header to CD64
 * import new data from CD64
 *--------------------------------------------------------------------------*/
unsigned char header_pbyte (char c)
{
        char status, spinner;

        spinner ='|';
        do
        {
                status=inb (status_reg);

                switch (spinner)
                {
                        case '|':
                                spinner='/';
                                break;

                        case '/':
                                spinner='-';
                                break;

                        case '-':
                                spinner='\\';
                                break;

                        case '\\':
                                spinner='|';
                                break;
                }

                usleep (100000);
                printf ("\b%c", spinner);
	        fflush(0);
        }
        while (status&0x80);

//           _   _
// set /PCRD  |_| to reset PCX
// import from cd64
//      outb (0x24,control_reg);
        outb (0x26,control_reg);
        status=inb (data_reg);
//      outb (control_reg, 0x24);

//           _   _
// set /PCWR  |_| to set PCX
// export to cd64
        outb (0x04,control_reg);
        outb (c,data_reg);
        outb (0x05,control_reg);
        outb (0x04,control_reg);

        return status;
}


//----------------------------------------------------------------------------
void send_long (unsigned long value)
{
        send_byte (value>>24);
        send_byte (value>>16);
        send_byte (value>>8);
        send_byte (value);
}


//----------------------------------------------------------------------------
void send_plong (unsigned long value)
{
        send_pbyte (value>>24);
        send_pbyte (value>>16);
        send_pbyte (value>>8);
        send_pbyte (value);
}


//----------------------------------------------------------------------------
unsigned int verify_checksum (unsigned int checksum_out)
{
        unsigned int checksum_in;

        checksum_in=exchange_byte (checksum_out>>8);
        checksum_in=(checksum_in<<8)&0xff00;
        checksum_in=checksum_in+exchange_byte (checksum_out);
        checksum_in=checksum_in&0xfff;

        return checksum_in;
}


/*--------------------------------------------------------------------------*/
void usage_message (void)
{
        printf(
         "\n"
         "Usage:   CD64COMM -P<port> -T -X -G -D -B -F<fname> -S<start> -L<length>\n"
         "\n"
         "Example: CD64COMM -x -fTEST.BIN -sb4000000\n"
         "         CD64COMM -p300 -t -fTEST.BIN -sb4000000\n"
         "         CD64COMM -g -fTEST.BIN -sb2000000 -l800000\n"
         "         CD64COMM -p378 -x -fTEST.BIN -sb4000000\n"    
         "         CD64COMM -d -fTEST.BIN -s80000400 -l100000\n"
         "         CD64COMM -b -fTEST.BIN -s80300000\n"
         "\n"
         "Options:\n"
         "  -P     pc Port address 300, 310-default, 320, 330, 378-ECP PRINTER PORT\n"
         "  -T     Transfer a block of data to CD64 any dram address, but don't do\n"
         "         anything with it\n"
         "  -X     eXecute code after download to b4000000-default by CD64\n"
         "  -G     Grab game from CD64 to file\n"
         "  -D     Dump system memory from N64 to file\n"
         "  -B     download BOOT EMULATOR to N64 and play uncracked game in CD64\n"
         "  -F     Filename of binary image to download\\upload\n"
         "  -S     Start address b2000000=card start, b4000000=dram start,\n"
         "         80000000=N64 system memory start (total 32Mb)\n"
         "         80300000=boot emulator start (max 4Mb)\n"
         "  -L     Length of grab or dump 400000=32Mb, 800000=64Mb, 1000000=128Mb,\n"
         "         1800000=192Mb, 2000000=256Mb\n"
         "\n"
         "All value are hex\n"
         "\n");
}


/*--------------------------------------------------------------------------*/
unsigned long long_hex_atoi (char *str)
{
        unsigned long i=0, i2=0;
        int pos=0;
        char c;
        char *astr;

        while (str[pos]=='0') pos++;

        astr=(char *)str+pos;
        pos=0;

        while ((astr[pos]>='0'&&astr[pos]<='9')||(astr[pos]>='A'&&astr[pos]<='F')||(astr[pos]>='a'&&astr[pos]<='f'))
        {
                if (astr[pos]>='0'&&astr[pos]<='9')
                        i=astr[pos]-'0';

                        else if (astr[pos]>='A'&&astr[pos]<='F')
                                i=astr[pos]-'A'+10;

                        else if (astr[pos]>='a'&&astr[pos]<='f')
                                i=astr[pos]-'a'+10;

                pos++;
                i2<<=4;
                i2+=i;
        }

        return i2;
}


/*----------------------------------------------------------------------*/
// send('X',start address,file name)
int send (char comm, unsigned long saddr, char *str)
{
        unsigned char c;
        unsigned int checksum, checksum_in, temp, temp2;
        unsigned long flength, count;
        FILE *in;

        in=fopen (str, "rb");
        if (in==NULL)
        {
                printf ("\nCan't open file %s\n", str);
                exit (2);
        }

        if (setvbuf (in, buf, _IOFBF, 32767)!=0)
        {
                printf ("\nfailed to set up buffer for %s\n", str);
                exit (2);
        }
        
        fseek (in, 0, SEEK_END);
        flength=ftell (in);
        fseek (in, 0, SEEK_SET);

        if ((flength&0x00000003)!=0)
        {
                printf ("\nFile length not in longword! (ending with 0,4,8,c)\n");
                exit (20);
        }

        if (flength==0)
        {
                printf ("\nFile length equal zero!\n");
                exit (20);
        }

/* Initiate the download */
        printf ("\nWaiting for target to respond...");

        if (ecp==0)
        {
                inb (data_reg);
                do
                {
                        while (header_byte ('W')!='R');
                }
                while (header_byte ('B')!='W');

                printf ("\nSending file %s to CD64...\n", str);

                send_byte (comm);
                send_long (saddr);
                send_long (flength);

                checksum = 0;
                for (count=0; count<flength; count++)
                {
                        c=fgetc (in);
                        checksum+=c;
                        send_byte (c);
                }

                checksum&=0xfff;
                printf ("\nmy checksum %X", checksum);
                checksum_in=verify_checksum (checksum);
                printf ("\nconsoles %X", checksum_in);

                temp=exchange_byte (0);
                temp2=exchange_byte (0);

                if ((checksum_in==checksum)&&(temp=='O')&&(temp2=='K')) return 0;

                        else return 11;
        }

                        else
                        {

                                do
                                {
//           _   _
// set /PCRD  |_| to reset PCX
//                                      outb (control_reg, 0x24);
                                        outb (0x26,control_reg);
//                                      outb (control_reg, 0x24);

//           _   _
// set /PCWR  |_| to set PCX
// export to cd64
                                        outb (0x04,control_reg);
                                        outb ('W',data_reg);
                                        outb (0x05,control_reg);
                                        outb (0x04,control_reg);
                        
                                        header_pbyte ('B');
                                        temp=header_pbyte ('B');
                                        temp2=header_pbyte ('B');
                                }                               
                                while ((temp!='R')||(temp2!='W'));

                                printf ("\nSending file %s to CD64...\n", str);

                                send_pbyte (comm);
                                send_plong (saddr);
                                send_plong (flength);


                                checksum = 0;
                                for (count=0; count<flength; count++)
                                {
                                        c=fgetc (in);
                                        checksum+=c;
                                        send_pbyte (c);
                                }

                                checksum&=0xfff;
                                send_pbyte (checksum>>8);
                                send_pbyte (checksum);

// send dummy for import handshake
                                read_pbyte ();

                                temp=exchange_pbyte (0);
                                temp2=exchange_pbyte (0);

                                if ((temp=='O')&&(temp2=='K')) return 0;

                                        else return 11;

                        }

        return 0;
}


/*----------------------------------------------------------------------*/
int grab (char comm, unsigned long addr, unsigned long length, char *str)
{
        unsigned char c;
        unsigned int pc_checksum, n64_checksum, temp, temp2;
        unsigned long count, scount, chunksize, chunkcount, delay;
        FILE *out;

        out=fopen (str, "wb");
        if (out==NULL)
        {
                printf ("Can't open file %s\n", str);
                exit (21);
        }

        if (setvbuf (out, buf, _IOFBF, 32767)!=0)
        {
                printf ("failed to set up buffer for %s\n", str);
                exit (2);
        }
        
        if (length==0)
        {
                printf ("\nLength of zero!\n");
                exit (20);
        }

        if ((length&0x00000003)!=0)
        {
                printf ("\nLength not in longword! (last digit not 0,4,8,c)\n");
                exit (20);
        }

        if ((addr&0x00000003)!=0)
        {
                printf ("\naddress not in longword! (last digit not 0,4,8,c)\n");
                exit (20);
        }

/* Initiate the grab */
        printf ("\nWaiting for target to respond...");

        if (ecp==0)
        {
                inb (data_reg);
                do
                {
                        while (header_byte ('W')!='R');
                }
                while (header_byte ('B')!='W');

                printf ("\nGrabing file %s from CD64...\n", str);

                send_byte (comm);
                send_long (addr);
                send_long (length);

                pc_checksum=0;
                for (count=0; count<length; count++)
                {
                        c=exchange_byte (0);
                        fputc (c, out);
                        pc_checksum+=c;
                }

                pc_checksum&=0xfff;
                n64_checksum=exchange_byte (0)<<8;
                n64_checksum+=exchange_byte (0);
                n64_checksum&=0xfff;

                if (n64_checksum!=pc_checksum)
                {
                        printf("\nERROR : Checksum failed - Upload corrupt!\n");

                        return 1;
                }

                return 0;
        }
                        else
                        {

                                do
                                {
//           _   _
// set /PCRD  |_| to reset PCX
//                      outb (control_reg, 0x24);
                        outb (0x26,control_reg);
//                      outb (control_reg, 0x24);

//           _   _
// set /PCWR  |_| to set PCX
// export to cd64
                                        outb (0x04,control_reg);
                                        outb ('W',data_reg);
                                        outb (0x05,control_reg);
                                        outb (0x04,control_reg);
                        
                                        header_pbyte ('B');
                                        temp=header_pbyte ('B');
                                        temp2=header_pbyte ('B');
                                }                               
                                while ((temp!='R')||(temp2!='W'));

                printf ("\nGrabing file %s from CD64...\n", str);

                send_pbyte (comm);
                send_plong (addr);
                send_plong (length);

// send dummy for import handshake
                read_pbyte ();

                pc_checksum=0;
                for (count=0; count<length; count++)
                {
                        c=read_pbyte ();
                        fputc (c, out);
                        pc_checksum+=c;
                }

                pc_checksum&=0xfff;
                n64_checksum=exchange_pbyte (0)<<8;
                n64_checksum+=exchange_pbyte (0);
                n64_checksum&=0xfff;

                if (n64_checksum!=pc_checksum)
                {
                        printf("\nERROR : Checksum failed - Upload corrupt!\n");

                        return 1;
                }

                return 0;
                        }

        return 0;
}


/*-----------------------------------------------------------------------*/
main (int argc, char *argv[])
{
        FILE *in, *up;
        unsigned char c, *str;
        unsigned int checksum, checksum_in, temp, temp2;
        unsigned long flength, count, saddr;

	if (setuid(0)) {
	  printf("setuid failed\nThis program must run with root privs\n");
	  exit(1);
	}
	if (iopl(3)) { 
	  printf("Port IO Access denied - root?\n"); 
	  exit(1); 
	}

        parse_switches (argc, argv);

        printf("\nCD64 Up/Download utility Ver2.10\n");

        if (switches[f]==0)
        {
                usage_message();
                exit (18);
        }

        if (switches[p])
        {
                data_reg=atoi (ptr_switches[p]);

                switch (data_reg)
                {
                        case 378:
                                data_reg=0x378;
                                break;

                        case 300:
                                data_reg=0x300;
                                break;

                        case 310:
                                data_reg=0x310;
                                break;

                        case 320:
                                data_reg=0x320;
                                break;

                        case 330:
                                data_reg=0x330;
                                break;

                        default:
                                data_reg=0x310;
                }

                if (data_reg!=0x378)
                {
                        status_reg=data_reg+2;
                        ecp=0;
                }
                        else
                        {

// set ecp port to ps/2 mode
                                ecp=1;
                                ecp_reg=0x77a;
                                status_reg=0x379;
                                control_reg=0x37a;
                                ecp_mode=inb (ecp_reg);
                                temp=ecp_mode;
                                temp&=0x1f;
                                temp|=0x20;
                                outb (temp,ecp_reg);

// set power supply
                                outb (0x04,control_reg);
                                                                usleep (1);

//                      _   _
// set /PCRD |_| to reset PCX
//                              outb (control_reg, 0x24);
                                outb (0x26,control_reg);
//                              outb (control_reg, 0x24);

//                       _       _
// set /PCWR  |_| to set PCX
// set all outbut hi
                                outb (0x04,control_reg);
                                outb (0xff,data_reg);
                                outb (0x05,control_reg);
                                outb (0x04,control_reg);
                        }
        }

        if (!(switches[x]||switches[g]||switches[t]||switches[d]||switches[b]))
        {
                printf ("\nRequires either -X or -G or -T or -D or -B\n");
                exit (14);
        }

        if (!switches[s])
        {
                printf ("\nRequires -S\n");
                exit (14);
        }

        if (strlen (ptr_switches[S])!=8)
        {
                printf ("\n-S requires 8 digits\n");
                exit (14);
        }

        saddr=long_hex_atoi (ptr_switches[S]);

        if (switches[x])
        {
                in=fopen (ptr_switches[f], "rb");
                if (in==NULL )
                {
                        printf ("\nCan't open file %s\n", ptr_switches[f]);
                        exit (2);
                }

                if (send ('X', saddr, ptr_switches[f])==0)
                        printf ("\n%s file sent and executed OK!!!\n", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors transferring file\n");
                                exit (17);
                        }

                exit (0);
        }

        if (switches[t])
        {
                in=fopen (ptr_switches[f], "rb");
                if (in==NULL )
                {
                        printf ("\nCan't open file %s\n", ptr_switches[f]);
                        exit (2);
                }

                if (send ('T', saddr, ptr_switches[f])==0)
                        printf ("\n%s file sent and OK!!!\n", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors transferring file\n");
                                exit (17);
                        }

                exit (0);                

        }
    
        if (switches[b])
        {
                in=fopen (ptr_switches[f], "rb");
                if (in==NULL )
                {
                        printf ("\nCan't open file %s\n", ptr_switches[f]);
                        exit (2);
                }

                if (send ('B', saddr, ptr_switches[f])==0)
                        printf ("\n%s boot emulator file sent and executed OK!!!\n", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors transferring file\n");
                                exit (17);
                        }

                exit (0);
        }

        if (switches[g]&&switches[l])
        {
                if (grab ('G', saddr,long_hex_atoi (ptr_switches[l]), ptr_switches[F])==0)
                        printf ("\n%s file grabed and OK!!!\n", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors grabing file\n");
                                exit (17);
                        }

                exit (0);
        }

        if (switches[d]&&switches[l])
        {
                if (grab ('D', saddr,long_hex_atoi (ptr_switches[l]), ptr_switches[F])==0)
                        printf ("\n%s system memory dumped and OK!!!\n", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors dumping system memory\n");
                                exit (17);
                        }

                exit (0);
        }

                else usage_message();
}
