/*
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

/*
        April - May 1998 (chuckjr@sundial.net)
        compile: g++ cd64comm.cc -o cd64comm -DLINUX
        - Must be setuid root or ran as root to have direct access to hardware
        interface
*/

#include <iostream.h>
#include <stdio.h>
#include <malloc.h>

#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#ifndef LINUX
#       include <conio.h>
#endif
#ifdef LINUX
#       include <sys/io.h>
#       include <unistd.h>
#endif

int     status_reg = 0x322;
int data_reg   = 0x320;

#ifndef LINUX
#       define OUTPORT(port, val)       outp(port, val)
#       define INPORT(port)            inp(port)
#else
#       define OUTPORT(port, val)       outb(val, port); usleep(1)
#       define INPORT(port)            inb(port); usleep(1)
#endif
#ifdef LINUX
#       define getch getchar
#endif


/*--------------------------------------------------------------------------*/
void c_break (int signum)
{
        OUTPORT(data_reg, 0);
        printf ("Control-Break pressed.  Program aborting ...\n");
        exit (13);
}
                                                                                                  

/*--------------------------------------------------------------------------*/
void test_key (void)
{
        int temp;

        if(0)
        {
                temp=getch ();
                if (temp==27||temp==3)
                {
                        printf ("\nQuitting program!!");
                        exit (13);
                }
        }
}


enum {X,P,F,S,G,L,T};
enum {x,p,f,s,g,l,t};
char _switches[] = {'X', 'P', 'F', 'S', 'G', 'L', 'T'};
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
                cptr=argv[i++];                 // string

                j=0;                            // first char
                c=cptr[j++];

                if (c=='-')                     // switch following
                {
                        c=cptr[j];              // switch
                        c=toupper (c);

                        for (k=0; k<sizeof (switches); k++)
                        {
                                if (c==_switches[k])
                                {
                                        switches[k]=1;          // switch on
                                        j++;                    // identified switch next char

                                        if (isalnum (cptr[j])) ptr_switches[k]=&cptr[j];

                                                else
                                                {
                                                        cptr=argv[i];                   
                                                        if (isalnum (cptr[0])) ptr_switches[k]=cptr;
                                                }

                                        break;
                                }
                        }
                }
        }
}


/*--------------------------------------------------------------------------*/
void send_byte (unsigned char c)
{
        unsigned char status;

        OUTPORT (data_reg, c);
        do
        {
                status=INPORT (status_reg);
        }
        while (status&1);

        status=INPORT (data_reg);

        if (c!=status)
        {
                printf ("\nBad");
                printf ("\nvalue out was %2.2X\nvalue back was %2.2X\n", c, status);
                exit (11);
        }
}


//-------------------------------------------------------------------------
unsigned char exchange_byte (char c)
{
        char status;

        OUTPORT (data_reg, c);
        do
        {
                status=INPORT (status_reg);
        }
        while (status&1);

        status=INPORT (data_reg);

        return status;
}


//--------------------------------------------------------------------------
unsigned char header_byte (char c)
{
        char status, spinner;

        spinner ='|';
        OUTPORT (data_reg, c);
	fprintf(stderr, "put %x %c\n", c, c);
        do
        {
                status=INPORT (status_reg);
                switch (spinner)
                {
                        case '|': spinner='/'; break;

                        case '/': spinner='-'; break;

                        case '-': spinner='\\'; break;

                        case '\\': spinner='|'; break;
                }

//		usleep(50);
                test_key();
                fprintf (stderr, "status %x\n", status);
        }
        while (status&1);

        status=INPORT (data_reg);

	fprintf(stderr, "got %x %c\n", status, status);
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
        cout
                << "\n"
                << "Usage :       cd64comm -P<port> -T -X -G -F<fname> -S<start> -L<length>\n"
                << "\n"
                << "Example :     cd64comm -x -fTEST.BIN -sb4000000\n"
                << "              cd64comm -p300 -t -fTEST.BIN -sb4000000\n"
                << "              cd64comm -g -fTEST.BIN -sb2000000 -l800000\n"
                << "\n"
                << "Options :\n"
                << "  -P          pc Port address 300, 310-default, 320, 330 \n"
                << "  -T          Transfer a block of data to CD64 but don't do anything with it\n"
                << "  -X          eXecute code after download\n"
                << "  -G          Grab game from CD64 to file\n"
                << "  -F          Filename of binary image to download\\upload\n"
                << "  -S          Start address b2000000=card start, b4000000=dram start\n"
                << "  -L          Length of grab 800000=64Mb, 1000000=128Mb\n"
                << "\nAll lengths are hex\n"
                << "\n";
}


/*--------------------------------------------------------------------------*/
unsigned long long_hex_atoi (char *str)
{
        unsigned long i=0, i2=0;
        int pos=0;
        char c;
        char *astr;

        while (str[pos]=='0')

        pos++;
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
int send (char comm, unsigned long saddr, char *str)
{
        unsigned char c;
        unsigned int checksum, checksum_in, temp, temp2;
        unsigned long flength, count;
        FILE *in;

        in=fopen (str, "rb");
        if (in==NULL)
        {
                printf ("Can't open file %s\n", str);
                exit (2);
        }

        fseek (in, 0, SEEK_END);
        flength=ftell (in);
        fseek (in, 0, SEEK_SET);

        if ((flength&0x00000003)!=0)
        {
                printf ("\nFile length not in longword!");
                exit (20);
        }

        temp=1;
        do
        {
                if (header_byte ('W')=='R')
                        if (header_byte ('B')=='W')
                                temp=0;
        }
        while (temp);

        printf ("\nSending file %s  \n", str);

        send_byte (comm);
        send_long (saddr);
        send_long (flength);

        checksum = 0;
        for (count=0; count<flength; count++)
        {
                c=fgetc (in);
                checksum+=c;
                checksum&=0xfff;
                send_byte (c);
        }

        printf ("my checksum %X\n", checksum);
        checksum_in=verify_checksum (checksum);
        printf ("consoles %X\n", checksum_in);

        temp=exchange_byte (0);
        temp2=exchange_byte (0);

        if ((checksum_in==checksum)&&(temp=='O')&&(temp2=='K')) return 0;

                else return 11;
}


/*----------------------------------------------------------------------*/
int grab (char comm, unsigned long addr, unsigned long length, char *str)
{
        unsigned char c;
        unsigned int pc_checksum, n64_checksum, temp;
        unsigned long count, scount, chunksize, chunkcount, delay;
        FILE *out;

        out=fopen (str, "wb");
        if (out==NULL)
        {
                printf ("Can't open file %s\n", str);
                exit (21);
        }

        if (length==0)
        {
                printf ("\nLength of zero!");
                exit (20);
        }

        if ((length&0x00000003)!=0)
        {
                printf ("\nLength not in longword!");
                exit (20);
        }

        if ((addr&0x00000003)!=0)
        {
                printf ("\naddress not in longword!");
                exit (20);
        }

        /* Initiate the download */
        printf ("\nWaiting for target to respond...");
	fflush(stdout);
        do
        {
                if (header_byte ('W')=='R')
                       if (header_byte ('B')=='W')
                                temp=0;
        }
        while (temp);

        send_byte ('G');
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
                cout<<"\nERROR : Checksum failed - Upload corrupt!\n";

                return 1;
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

        signal (SIGINT, c_break);
        parse_switches (argc, argv);

        cout<<"\nCD64 Up/Download utility Ver1.02\n";

#ifdef LINUX
        if((iopl(3)) != 0) {
                perror("cd64");
                exit(-1);
        }
#endif

        if (switches[f]==0)
        {
                usage_message();
                exit (18);
        }

        if (switches[p])
        {
                data_reg=atoi (ptr_switches[2]);
                switch (data_reg)
                {
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

                status_reg=data_reg+2;
        }

        if (!(switches[x]||switches[g]||switches[t]))
        {
                printf ("\nRequires either -X or -G or -T");
                exit (14);
        }

        if (switches[s]) saddr=long_hex_atoi (ptr_switches[S]);

        else saddr=0xb4000000;

        if (switches[x])
        {
            in=fopen (ptr_switches[f], "rb");
                if (in==NULL )
                {
                        printf ("Can't open file %s\n", ptr_switches[f]);
                        exit (2);
                }

                if (send ('X', saddr, ptr_switches[f])==0)
                        printf ("\n%s file sent and executed OK", ptr_switches[f]);

                else
                {
                        printf ("\nWARNING!!!!! errors transferring file\n");
                        exit (17);
                }

                exit (0);
        }

        if (switches[t])
        {
                if (send ('T', saddr, ptr_switches[f])==0)
                        printf ("\n%s file sent and OK", ptr_switches[f]);

                        else
                        {
                                printf ("\nWARNING!!!!! errors transferring file\n");
                                exit (17);
                        }

                exit (0);                

        }
    
        if (switches[g]&&switches[l])
        {
                grab ('G', saddr,long_hex_atoi (ptr_switches[l]), ptr_switches[F]);
                exit (0);
        }

        else usage_message();

}
