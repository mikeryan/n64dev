/*

    Author:  Garth Elgar
    Compile: gcc -Wall -O2 -o renamer renamer.c
    Purpose: Rename and split ROMs across CDs. Makes HTML list of each CD
    Usage:   renamer *.rom *.v64

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
#include <strings.h>
#include <errno.h>

#define CDSIZE 650*8 // Mbits

main(int argc,char *argv[])
{
  int i,j,c,b0,b1,flength,cdnumber=1;
  FILE *fin,*fout,*html;
  unsigned char hdr[0x40];
  char imagename[21],gamename[50],filename[50],tmpstr[10],country[4];
  long cdlength=0;

  html=fopen("n64roms.html","w+");
  fputs("<html>\n<head>\n<title>N64 ROM List</title>\n"\
	"<meta http-equiv=\"Content-Type\" content=\"text/html; "\
	"charset=iso-8859-1\">\n</head>\n\n<body bgcolor=\"#FFFFFF\">\n"\
	"<table border=\"1\" width=\"50%\">\n",html);
  
  for (i=1;i<argc;i++) 
    {
      printf("Processing %s\n",argv[i]);
      if ((fin=fopen(argv[i],"rb"))==NULL) {
	printf("Error opening file %s\n",argv[i]);
	exit(0);
      }
      if (fread(hdr,1,0x40,fin)==0) {
	printf("Error reading file %s\n",argv[i]);
	exit(0);
      }
      if (hdr[0]==0x37) { // v64 format file
	if (fclose(fin)==EOF) {
	  printf("Error closing input file\n");
	  exit(0);
	}
	if (rename(argv[i],"temp.v64")<0) { // don't use across devices!
	   printf("Error renaming file %s\n",argv[i]);
	   printf("errno is %d\n",errno);
	   exit(0);
	}
	if ((fin=fopen("temp.v64","rb"))==NULL)  {
	  printf("Error opening file temp.v64\n");
	  exit(0);
	}
	fout=fopen(argv[i],"wb+");
	while ((b0=fgetc(fin))!=EOF)
	  {
	    b1=fgetc(fin);
	    fputc(b1,fout);
	    fputc(b0,fout);
	  }
	fclose(fin);
	fclose(fout);
	unlink("temp.v64");
	fin=fopen(argv[i],"rb");
	fread(hdr,1,0x40,fin);
	  }
      memcpy(imagename,&(hdr[0x20]),20);
      imagename[20]=0x20;
      j=19;
      while (imagename[j]==0x20) j--;
      imagename[j+1]='\0';
      printf("Game name is %s. Change (y/n)? ",imagename);
      fgets(tmpstr,5,stdin);
      c=tmpstr[0];
      if (tolower(c)=='y') {
	printf("Enter new name : ");
	fgets(gamename,50,stdin);
	gamename[strlen(gamename)-1]='\0';
      }
      else
	strcpy(gamename,imagename);
      strcpy(filename,gamename);
      for (j=1;j<=strlen(filename);j++) {
	if (filename[j-1]==' ') filename[j-1]='_';
      }
      switch (hdr[0x3e]) {
      case 'D':
      case 'P':
      case 'U':
	strcpy(country,"PAL");
	strcat(filename,"_PAL.ROM");
	break;
      case 'A':
      case 'E':
	strcpy(country,"USA");
	strcat(filename,"_USA.ROM");
	break;
      case 'J':
	strcpy(country,"JAP");
	strcat(filename,"_JAP.ROM");
	break;
      }
      fseek(fin,0,SEEK_END);
      flength=ftell(fin)/(1024*128);
      if (cdlength+flength>CDSIZE) {
	cdnumber++;
	cdlength=flength;
      }
      else
	cdlength+=flength;
      fclose(fin);
      rename(argv[i],filename);
      fprintf(html,"<tr>\n"\
	      "<td>%s</td>\n"\
	      "<td>%s</td>\n"\
	      "<td>%d Mbit</td>\n"\
	      "<td>CD  %d</td>\n"\
	      "</tr>\n",
	      gamename,country,flength,cdnumber);
    }
  fputs("</table>\n</body>\n</html>\n",html);
  fclose(html);
}
