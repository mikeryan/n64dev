// Copyright(C) 1999 by Pooka (pooka@cygnus.ucdavis.edu)
// All code written by Pooka except for portions of the header check code
// which are from nifty.c written by Tom Whittock.
// You are free to use and distrubite this code as long as you include the
// above copyright notice.

extern "C"
{
 #include <stdio.h>
 #include <stdlib.h>
 #include <X11/Intrinsic.h>
#ifndef DOS
 #include <Xm/FileSB.h>
 #include <Xm/RowColumn.h>
#endif
 #include <math.h>
}

#include <pr/gbi.h>

#include "global.h"
#include "classes/dlcmd.h"
#include "classes/vtxcmd.h"
#include "classes/tricmd.h"
#include "classes/wordlist.h"
#include "classes/cmdlist.h"
#include "classes/vtx_tn.h"
#include "classes/texture.h"
#include "classes/vtxlist.h"
#include "classes/vllist.h"
#include "classes/addrlist.h"
#ifndef DOS
#include "classes/viewer.h"
#include "vw_info.h"
#include "vw_help.h"
#include "vw_cback.h"
#include "xpm/question.xpm"
#endif
#include "version.h"

#define DEF_OutFormat_Vtx	0
#define DEF_OutFormat_Inventor	1
#define DEF_OutFormat_OpenGL	2
#define DEF_OutFormat_None	3

char*	_ProgramName="dlrip   ";

int		bigEndian=1;			// endian flag
unsigned long	saddr=0L;			// rom start address
unsigned long	scanStart=0L;			// scan start address
unsigned long	rom_length=0L;			// ROM length, in 32-bit words

char		previewFlags[4096];		// previewer cmd line args
int		CalculateNormals=1;		// calc norms flag
int		numFound=0;			// number of valid DL's found
int		DoPreview=0;			// do preview?
int		DoScan=0;			// do scan?
int		DoAutoLUT=1;			// use auto LUT
int		DoLUT=1;			// use LUT
int		OutputFormat=DEF_OutFormat_None;// output format
int		TriangleMin=0;			// minimum number of triangles
int		saveType=0;			// output type for Save As...
unsigned long	Exclusions=0L;			// BOL marker exclusions
unsigned long	PossibleOptions=0L;		// options for possible shapes
unsigned long	ScanIncrement=1L;		// scanning increment


					// opens output file
FILE*
OpenNextFile(char* base, int num)
{
 char	fn[4096];
 FILE*	outfp;

	if(base[0] == (char)NULL)
	 return(stdout);

	if(num >= 0)
	 sprintf(fn, "%s%04d", base, num);
	else
	 sprintf(fn, "%s", base);

	outfp = fopen(fn, "w");
        if(outfp == NULL)
        {
         perror("fopen");
         FPF(stderr, "Error: Error opening output file %s.\n", fn);
         exit(1);
        }
	return(outfp);
}

					// determines endian-ness of machine
int
IsMachineBigEndian()
{
 char		w[4];
 unsigned long	tw;

	w[0] = 0x01;
	w[1] = 0x02;
	w[2] = 0x03;
	w[3] = 0x04;

	memcpy(&tw, w, 4);

	if(tw == 0x01020304L)
	 return(1);
	else
	 return(0);
}

					// flips a dword
void
FLIPcharDWORD(unsigned char* buf)
{                      
 unsigned char temp; 

         temp = buf[0]; buf[0] = buf[3]; buf[3] = temp;
         temp = buf[1]; buf[1] = buf[2]; buf[2] = temp;
         temp = buf[4]; buf[4] = buf[7]; buf[7] = temp;
         temp = buf[5]; buf[5] = buf[6]; buf[6] = temp;
}

					// flips an hword
void
FLIPcharWORD(unsigned char* buf)
{                      
 unsigned char temp; 

         temp = buf[0]; buf[0] = buf[3]; buf[3] = temp;
         temp = buf[1]; buf[1] = buf[2]; buf[2] = temp;
}



					// main program
int
main(int argc, char* argv[])
{
 FILE*		infp;				// input stream pointer
 char		InFile[4096], OutFile[4096];	// filenames
 int		format = DEF_FormatUnknown;	// file format
 int		swapped=0;			// image is byteflipped or not
 int		err, i;				// misc 
 char		buf[80];
 unsigned int	k;

 unsigned char	cbuf[W_BUFSIZE*4];		// read buffer
 unsigned long	wbuf;				// after-read buffer
 unsigned char	bbuf[8];			// 64-bit/8-byte buf
 unsigned long	w1, w2;				// Gfx.w0, Gfx.w1
 unsigned long 	offset=0;			// misc
 unsigned long	count=0;
 long		filelen=0L;		

 WordList	memfile;			// memory copy of rom image
 WordList	WLspEndDisplayList;		// list of gsSPEndDispList's
 CommandList	cmdlist;			// internal DL representation
 DLcmd*		newcmd;				// new DL command
 AddrList	addrlist;			// address look up table

						// CLI vars
 int            c;
 int            iflg=0, oflg=0, bflg=0, fflg=0, rflg=0, cflg=0, qflg=0;
 int		xflg=0, Qflg=0, Sflg=0, sflg=0, mflg=0, tflg=0, pflg=0;
 int		Bflg=0, aflg=0, lflg=0, Lflg=0;
 int            errflg=0;
 extern char*   optarg;

							// parse arguments

	while((c=getopt(argc,argv,"hQfrLlbBi:o:c:q:s:S:m:t:p:a:x:")) != -1)
        {
         switch(c)
         {
          case 'i':					// infile
                if(iflg)
                 errflg++;
                else
                {
                 iflg++;
                 strcpy(InFile, optarg);
                }
                break;
          case 'o':					// outfile
                if(oflg)
                 errflg++;
                else
                {
                 oflg++;
                 strcpy(OutFile, optarg);
                }
                break;
          case 'm':					// output format
                if(mflg)
                 errflg++;
                else
                {
                 mflg++;
                 OutputFormat = atoi(optarg);
                }
                break;
          case 'a':					// add LUT entry
                {
		 unsigned long ad, of;
		 char *adptr, *ofptr;
		 adptr = strtok(optarg, ":"); 
	  	 if(adptr == NULL)
		 {
		  errflg++;
		  break;
		 }
		 ofptr = strtok(NULL, ":");
	  	 if(ofptr == NULL)
		 {
		  errflg++;
		  break;
		 }
		 ad = strtoul(adptr, NULL, 0);
		 of = strtoul(ofptr, NULL, 0);
		 addrlist.Add(AddrLUTentry(ad, of));
                 aflg++;
                }
                break;
          case 'p':					// possible options
                if(pflg)
                 errflg++;
                else
                {
                 pflg++;
                 PossibleOptions = strtoul(optarg, NULL, 0);
                }
                break;
          case 'c':					// normal calc
                if(cflg)
                 errflg++;
                else
                {
                 cflg++;
                 CalculateNormals = atoi(optarg);
                }
                break;
          case 't':					// minimum num of tri
                if(tflg)
                 errflg++;
                else
                {
                 tflg++;
                 TriangleMin = atoi(optarg);
                }
                break;
          case 's':					// scan start
                if(sflg)
                 errflg++;
                else
                {
                 sflg++;
                 scanStart = strtoul(optarg, NULL, 0);
                }
                break;
	  case 'S':					// do scan
		if(Sflg)
		 errflg++;
	 	else
		{
		 Sflg++;
		 DoScan = 1;
		 ScanIncrement = strtoul(optarg, NULL, 0);
		}
		break;
          case 'q':					// viewer args
                if(qflg)
                 errflg++;
                else
                {
                 qflg++;
                 strcpy(previewFlags, optarg);
                }
                break;
	  case 'l':					// auto LUT disable
		if(lflg)
		 errflg++;
	 	else
		{
		 lflg++;
		 DoAutoLUT = 0;
		}
		break;
	  case 'L':					// LUT disable
		if(Lflg)
		 errflg++;
	 	else
		{
		 Lflg++;
		 DoLUT = 0;
		}
		break;
	  case 'b':					// byteflip
		if(bflg)
		 errflg++;
	 	else
		{
		 bflg++;
		}
		break;
	  case 'B':					// no byteflip
		if(Bflg)
		 errflg++;
	 	else
		{
		 Bflg++;
		}
		break;
	  case 'Q':					// preview
		if(Qflg)
		 errflg++;
	 	else
		{
		 Qflg++;
		 DoPreview = 1;
		}
		break;
	  case 'x':					// exclude cmds
		{
		 int pos;
		 xflg++;
		 pos = atoi(optarg);
		 Exclusions |= (unsigned long)1L << pos;
		}
		break;
	  case 'r':					// raw output
		if(rflg)
		 errflg++;
	 	else
		{
		 rflg++;
		}
		break;
	  case 'f':					// force use
		if(fflg)
		 errflg++;
	 	else
		{
		 fflg++;
		}
		break;
          case 'h':					// help
                errflg++;
		break;
          case ':':
                errflg++;
		break;
          case '?':
                errflg++;
		break;
	 }
	}

        if(errflg)						// help
        {
 FPF(stdout,
   "dlrip "DLRIP_VERSION" (bugs/questions/comments: pooka@cygnus.ucdavis.edu)\n"
   "usage: dlrip -i <infile> [-o <outfile>] [options]\n"
   "            -i <infile>  : input file\n"
   "            -o <outfile> : output filename base (stdout if not given)\n"
   "   Options:\n"
   "            -b           : force byteflip (default = autodetect)\n"
   "            -B           : force no byteflip (default = autodetect)\n"
   "            -c <dir>     : calculate normals (default = 1)\n"
   "                         : <dir> = 0 - don't calculate\n"
   "                         :         1 - counterclockwise\n" 
   "                         :         2 - clockwise\n"
#ifndef DOS
   "            -Q           : preview output in graphical viewer\n"
   "            -q <args>    : X Toolkit options for previewer\n"
#endif
   "            -f           : ignore unknown file type errors\n"
   "            -x <id>      : exclude cmd from BOL list (use mult. times)\n"
   "                         : <id> = 1 - G_SPNOOP\n"
   "                         :        2 - G_NOOP\n"
   "                         :        3 - G_RDPSETOTHERMODE\n"
   "                         :        4 - G_RDPHALF_CONT\n"
   "                         :        5 - G_RESERVED0\n"
   "                         :        6 - G_RESERVED1\n"
   "                         :        7 - G_RESERVED2\n"
   "                         :        8 - G_RESERVED3\n"
   "                         :        9 - G_ENDDL\n"
   "            -S <inc>     : scan for vertex data every <inc> words\n"
   "            -s <offset>  : set starting scan offset (in ROM image)\n"
   "            -a <loc>     : add a segment location to the LUT\n"
   "                         : <loc> = address:offset\n"
   "            -l           : only use manually-entered LUT entries\n"
   "            -L           : disable the LUT completely\n"
   "            -t <num>     : ignore models with < num tris (def=0=off)\n"
   "            -p <opt>     : ignore models with property (use mult. times)\n"
   "                         : <opt> = 1 - Zero normals\n"
   "                         :         2 - More than 5 (0,0,0) points\n"
   "            -m <opt>     : output format (default=3)\n"
   "                         : <opt> = 0 = Vtx_tn\n"
   "                         :         1 = Open Inventor 2.0\n"
   "                         :         2 = OpenGL 1.1\n"
   "                         :         3 = No output\n"
   "            -r           : output raw display lists to stdout\n"
   "            -h           : this help\n");
	  exit(1);
	}

	if(!iflg)
	{
	 FPF(stderr, "Error: You must supply an input file with -i "
		"<filename>.\n");
	 exit(1);
	}

	if(Lflg && aflg)
	{
	 FPF(stderr, "Error: Don't use -a with -L.\n");
	 exit(1);
	}

//==-[ Misc initialization stuff ]-==//

	addrlist.Print(stderr);

						// input file
	if(!strcmp(InFile, "-"))
	 infp = stdin;
	else
	{
	 infp = fopen(InFile, "rb");
         if(infp == NULL)
         {
          perror("fopen");
          FPF(stderr, "Error: Error opening input file %s.\n", InFile);
          exit(1);
         }
	}
                                                // seek to end
						// output file
	if(!oflg)
	 OutFile[0] = (char)NULL;
	else if(!strcmp(OutFile, "-"))
	 OutFile[0] = (char)NULL;

        if(fseek(infp, 0L, SEEK_END) != 0)
        {
         perror("fseek");
         FPF(stderr, "Error: Error seeking file.\n");
         exit(1);
        }
						// get file length
	filelen = ftell(infp);
                                                // seek to start
        if(fseek(infp, 0L, SEEK_SET) != 0)
        {
         perror("fseek");
         FPF(stderr, "Error: Error seeking file.\n");
         exit(1);
        }
						// detect endian-ness
	bigEndian = IsMachineBigEndian();
	FPF(stderr, "Machine : %s\n", bigEndian?"Big endian":"Little endian");
	fflush(stderr);

						// get byte order and format
	if( fread(buf, 1, 12, infp) != 12 )
	{
	 perror("fread");
	 FPF(stderr, "Error: Error reading file.\n");
	 exit(1);
	}
	if ( B2L(buf, 0) == (signed long)DEF_HeaderMagic )
	{
	 swapped = 0;
	 format = DEF_FormatZ64;
	}
	else if ( B2L(buf, 1) == (signed long)DEF_HeaderMagic )
	{
	 swapped = 1;
	 format = DEF_FormatV64;
	}
	else if(!memcmp(&buf[1], "ELF", 3))
	{
	 swapped = 0;
	 format = DEF_FormatELF;
	}
	else
	{
	 if(!fflg)
	 {
    	  FPF(stderr, "Error: Unknown file type. Use -f to force use.\n");
	  exit(1);
	 }
	 format = DEF_FormatUnknown;
	}

						// print format info
	FPF(stderr, "File format: %s\n",
		 format==DEF_FormatZ64 ? "Z64/CD64":
		(format==DEF_FormatV64 ? "Doctor V64" :
		(format==DEF_FormatELF ? "ELF" : "Unknown")));
	fflush(stderr);

						// seek to beginning
	if(fseek(infp, 0L, SEEK_SET) != 0)
	{ 
	 perror("fseek");
	 FPF(stderr, "Error: Error seeking file.\n");
	 exit(1);
	}
						// bypass chunking
	memfile.AllocateChunk(filelen >> 2);

						// load file in 32-bit units
	if(Bflg)
	 FPF(stderr, "Loading file to memory (with forced no-byteflip)...");
	else if(bflg)
	 FPF(stderr, "Loading file to memory (with forced byteflip)...");
	else
	 FPF(stderr, "Loading file to memory (with %s)...",
		swapped ? "byteflip":"no byteflip");
	fflush(stderr);
	count = 0;
	for(;;)			
	{
	 err = fread(cbuf, sizeof(unsigned long), W_BUFSIZE, infp); 
	 if(err <= 0)
	 {
	  if(feof(infp))
	   break;
	  if(ferror(infp))
	  {
	   perror("fread");
	   FPF(stderr, "Error: Error reading file.\n");
	   exit(1);
	  }
	  else
	  {
	   perror("fread");
	   FPF(stderr, "Error: Unknown file error.\n");
	   exit(1);
	  }
	 }
	 else
	 {
	  unsigned char* cbptr;
	  for(i=0; i<err; i++)			// add read words to list
	  {
	   cbptr = &cbuf[i<<2];
	   if(format == DEF_FormatELF)
	    FLIPcharWORD(cbptr);
	   if(Bflg)					// force no flip
	    wbuf = B2L(cbptr, False);
	   else if(bflg)				// force flip
	    wbuf = B2L(cbptr, True);
	   else						// auto
	    wbuf = B2L(cbptr, swapped);
	   memfile.Add(wbuf);
	  }
	  rom_length += err;				// adjust length
	 }
	 count++;
	 if(!(count % 8))				// print dots
	  fputc('.', stderr); fflush(stderr);
	}
	fputc('\n', stderr); fflush(stderr);

							// output length

	FPF(stderr, "Loaded %lu bytes (%lu Mbits)\n", rom_length<<2,
		(rom_length<<5)/1048576L);
	fflush(stderr);

		// remember, memfile offsets are in 32-bit words, not bytes

	saddr = memfile[0x02];				// get start address 
	if(format != DEF_FormatELF)
	 FPF(stderr, "Starting address : 0x%08lx\n", saddr);
	fflush(stderr);
							// print ROM name
	FPF(stderr, "ROM name : \"");	
	if(format == DEF_FormatELF)
	 FPF(stderr, "ELF");
	else
	{
	 for(i=0x08; i<0x0d; i++)
	 {
	  memcpy(bbuf, &memfile[i], 4);
	  if(bigEndian)
	  {
	   fputc(bbuf[0], stderr); fputc(bbuf[1], stderr);
	   fputc(bbuf[2], stderr); fputc(bbuf[3], stderr);
	  }
 	  else
	  {
	   fputc(bbuf[3], stderr); fputc(bbuf[2], stderr);
	   fputc(bbuf[1], stderr); fputc(bbuf[0], stderr);
	  }
	 }
	}
	FPF(stderr, "\"\n");
	fflush(stderr);

//==-[ Scan for gsSPEndDisplayList()s ]-==//

	FPF(stderr, "Scanning for gsSPEndDisplayList()'s...");
	fflush(stderr);

	WLspEndDisplayList = memfile.Find(DEF_gsSPEndDisplayList);

	FPF(stderr, "Found %ld\n", WLspEndDisplayList.Length());
	fflush(stderr);

	if(!WLspEndDisplayList.Length())
	 exit(0);

//==-[ Scan backwards from each DL end ]-==//

	FPF(stderr, "Processing...");
	fflush(stderr);

	count = 0;

	if(WLspEndDisplayList.Length() > 0)
	{
	 for(i=0; i<WLspEndDisplayList.Length(); i++)
	 {
	  if(rflg)
	  {
	   FPF(stdout, "0x%08lx : [ End Display List ]\n",
		WLspEndDisplayList[i] * 4L);
	   fflush(stdout);
	  }
	  for(k=0;;k+=2)
	  {
	   offset = WLspEndDisplayList[i];		// get file offset
	   if(rflg)
	   {
	    FPF(stdout, "0x%08lx : ", (offset - k)*4L);
	    fflush(stdout);
	   }
 	   w1 = memfile[offset - k];			// snarf w0 and w1
	   w2 = memfile[offset - (k-1)];
	   newcmd = Decode(stdout, w1, w2, rflg);	// send to decoder
	   if(rflg)
	   {
	    fputc('\n', stdout);	
	    fflush(stdout);
	   }
	   if(!(count%1024))				// print dots
	   {
	    fputc('.', stderr);
	    fflush(stderr);
	   }
	   count++;
	   if((newcmd == DEF_DecodeError) && (!k))	// ignore first endDL
	    continue;
	   if((newcmd == DEF_DecodeError) && k)		// possible list start
	    break;
	   else if((offset-k-1) <= 0)			// beginning of memfile
	    break;
	   else if(newcmd != DEF_CmdID_Undefined)	// valid command
	    cmdlist.Add(newcmd);
	  }
	  if(rflg)
	  {
	   FPF(stdout, "--------------------------------------------------------------------------\n");
	   fflush(stdout);
	  }
	  ParseList(OutFile, cmdlist, memfile, addrlist);// parse extracted list
	  cmdlist.Erase();				// get ready for next
	 }
	}
}


					// prints a Vtx_tn for testing
void
PrintVtx(FILE* outfp, Vtx* invtx)
{
 short*		ob;
 unsigned short	flag;
 short*		tc;
 signed char*	n;
 unsigned char	a;

	ob = invtx->n.ob;
	flag = invtx->n.flag;
	tc = invtx->n.tc;
	n = invtx->n.n;
	a = invtx->n.a;

	FPF(outfp, "ob   = %d, %d, %d\n", ob[0], ob[1], ob[2]);
	FPF(outfp, "flag = %d\n", flag);
	FPF(outfp, "tc   = %d, %d\n", tc[0], tc[1]);
	FPF(outfp, "n    = %d, %d, %d\n", n[0], n[1], n[2]);
	FPF(outfp, "a    = %d\n", a);
}


#ifndef DOS
							// exit callback
void
CB_DLRIP_Exit(Widget parent, XtPointer client, XtPointer call)
{
 int    ans;
 FrameMsg*      fm = (FrameMsg*)client;

        ans = MultiButtonBox(parent,
                "Exit?", "Are you sure you\nwish to exit?",
                "black", "grey", (Pixmap)question_xpm,
                "No", NULL, NULL, NULL, CB_Help, (XtPointer)HELP_NoExit,
                        CB_DisplayInfo, (XtPointer)CreateFM(fm, INFO_No),
                "Yes", NULL, NULL, NULL, CB_Help, (XtPointer)HELP_YesExit,
                        CB_DisplayInfo, (XtPointer)CreateFM(fm, INFO_Yes),
                (char*)NULL);
        if(ans != 2)
         return;
        exit(0);
}


							// next model callback

void
CB_DLRIP_NextModel(Widget parent, XtPointer client, XtPointer call)
{
	((Viewer*)((FrameMsg*)client)->viewer)->SetExit(TRUE);
}


							// set output format	
void
CB_DLRIP_SetSaveType(Widget parent, XtPointer client, XtPointer call)
{
	saveType = (int)(((FrameMsg*)client)->infonum);
}

							// OK 
void
CB_DLRIP_SaveOk(Widget parent, XtPointer client, XtPointer call)
{
 XmString	str;
 char*		path; 
 FILE*		outfp;
 VtxListList*	vll;
 Widget		fsb;

	vll = (VtxListList*)(((FrameMsg*)client)->vtxlist);
	fsb = *((FrameMsg*)client)->w;

	XtVaGetValues(fsb, XmNdirSpec, &str, NULL);	// snarf path

	XmStringGetLtoR(str, XmFONTLIST_DEFAULT_TAG, &path);

        outfp = OpenNextFile(path, -1);			// open file
        switch(saveType)				// do output
        {
         case DEF_OutFormat_Vtx:
                vll->Print(OUTPUT_VTX, "dlrip" , outfp);
                break;
         case DEF_OutFormat_Inventor:
                vll->Print(OUTPUT_INV, "dlrip" , outfp);
                break;
         case DEF_OutFormat_OpenGL:
                vll->Print(OUTPUT_OGL, "dlrip" , outfp);
                break;
        }
	fclose(outfp);
	XtUnmanageChild(fsb);
}

							// Cancel
void
CB_DLRIP_SaveCancel(Widget parent, XtPointer client, XtPointer call)
{
	XtUnmanageChild(*((FrameMsg*)client)->w);
}



							// File->Save as...
void
CB_DLRIP_SaveAs(Widget parent, XtPointer client, XtPointer call)
{
 static Widget	fsb, saveAsTypePD, saveAsTypeMenu;
 Widget		okbut, cancelbut;
 Arg		args[20];
 Cardinal	ac;
 static int	first=1;

	if(!first)
	{
	 XtManageChild(fsb);
	 return;
	}

	first = 0;

	((FrameMsg*)client)->w = &fsb;
	
	ac=0;
        XtSetArg(args[ac],XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL);ac++;
        XtSetArg(args[ac], XmNresizePolicy, XmRESIZE_ANY); ac++;
	fsb = XmCreateFileSelectionDialog(parent, "DLRIP_saveAsBox", args, ac);
	
	okbut = (Widget)XmFileSelectionBoxGetChild(fsb, XmDIALOG_OK_BUTTON);
	cancelbut = (Widget)XmFileSelectionBoxGetChild(fsb,
		XmDIALOG_CANCEL_BUTTON);

	XtVaSetValues(okbut, XmNhighlightOnEnter, True, NULL);
	XtVaSetValues(cancelbut, XmNhighlightOnEnter, True, NULL);

	XtAddCallback(okbut, XmNactivateCallback, CB_DLRIP_SaveOk,
		CreateFM((FrameMsg*)client, 0));
	XtAddCallback(cancelbut,XmNactivateCallback,CB_DLRIP_SaveCancel,
		CreateFM((FrameMsg*)client, 0));

        ac = 0;
        saveAsTypePD=(Widget)XmCreatePulldownMenu(fsb, "DLRIP_saveAsTypePD",
		args, ac);

        ac = 0;
        XtSetArg(args[ac], XmNsubMenuId, saveAsTypePD); ac++;
        saveAsTypeMenu=(Widget)XmCreateOptionMenu(fsb, "DLRIP_saveAsTypeMenu",
                args, ac);
        XtAddCallback(saveAsTypeMenu, XmNhelpCallback, CB_Help,
                (XtPointer)HELP_DLRIP_SaveAsTypeMenu);
        XtAddEventHandler(saveAsTypeMenu, EnterWindowMask,False,CB_DisplayInfo,
                        (XtPointer)CreateFM((FrameMsg*)client,
			INFO_DLRIP_SaveAsTypeMenu));
        XtManageChild(saveAsTypeMenu);

        CreatePushButton(saveAsTypePD, "DLRIP_saveAsTypeVtx",
		CB_DLRIP_SetSaveType,
                CreateFM((FrameMsg*)client, DEF_OutFormat_Vtx),
                CreateFM((FrameMsg*)client, INFO_DLRIP_SaveAsTypeVtx),
			HELP_DLRIP_SaveAsTypeVtx);

        CreatePushButton(saveAsTypePD,"DLRIP_saveAsTypeInventor",
		CB_DLRIP_SetSaveType,
                CreateFM((FrameMsg*)client, DEF_OutFormat_Inventor),
                CreateFM((FrameMsg*)client, INFO_DLRIP_SaveAsTypeInventor),
			HELP_DLRIP_SaveAsTypeInventor);

        CreatePushButton(saveAsTypePD, "DLRIP_saveAsTypeOpenGL",
		CB_DLRIP_SetSaveType,
                CreateFM((FrameMsg*)client, DEF_OutFormat_OpenGL),
                CreateFM((FrameMsg*)client, INFO_DLRIP_SaveAsTypeOpenGL),
			HELP_DLRIP_SaveAsTypeOpenGL);

	XtManageChild(fsb);
}
	
#endif	// DOS

							// Parse command list
void
ParseList(char* outfile, CommandList& cmdlist, WordList& memfile,
	  AddrList& addrlist)
{
 register long		i, j, k;			// misc
 int			num;
 int			v0;
 long			temp_l;
 unsigned long		min;
 unsigned long		vaddr;	
 unsigned long		offset;
 unsigned long 		w1, w2, w3, w4;
 Vtx			vtxCache[16];			// pseudo vtx cache
 VtxList		newlist;			// the list of vertices
 char			objName[2048];			// output obj name
 FILE*			outfp;				// output stream
 Texture		dummytex;			// dummy texture
 unsigned long		scanOffset=0L;			// scanning offset
 WordList		dl_addresses;			// addresses used in DL
 int			numNotInLUT=1;			// DL items not in LUT
 static int		first=1;			// first flag
#ifndef DOS
 static Viewer		theViewer;			// opengl viewer
 static MenuItem	MI_nextModel, MI_exit;		// menu item stuff
 static MenuItem	MI_saveAs;
 static char*		nextModel_name = "DLRIP_nextModel";	
 static char*		exit_name = "exit";
 static char*		saveAs_name = "DLRIP_saveAs";
#endif


	if(first)
	{
#ifndef DOS
	 MI_nextModel.name = nextModel_name;		// add menu items to
	 MI_nextModel.cback = CB_DLRIP_NextModel;	// opengl viewer
	 MI_nextModel.client = &theViewer;
	 MI_nextModel.info = INFO_DLRIP_NextModel;
	 MI_nextModel.help = HELP_DLRIP_NextModel;

	 MI_exit.name = exit_name;
	 MI_exit.cback = CB_DLRIP_Exit;
	 MI_exit.client = &theViewer;
	 MI_exit.info = INFO_Exit;
	 MI_exit.help = HELP_Exit;

	 MI_saveAs.name = saveAs_name;
	 MI_saveAs.cback = CB_DLRIP_SaveAs;
	 MI_saveAs.client = &theViewer;
	 MI_saveAs.info = INFO_DLRIP_SaveAs;
	 MI_saveAs.help = HELP_DLRIP_SaveAs;

 	 theViewer.FileItems.add(&MI_nextModel);
 	 theViewer.FileItems.add(&MI_saveAs);
 	 theViewer.FileItems.add(&MI_exit);
#endif
	 first = 0;
	}

	if(!cmdlist.Length())				// if no items, return
	 return;
					// commands come out in reversed order
	cmdlist.Reverse();		//  so we need to flip them around

	if(DoScan)
	 scanOffset = scanStart;	// set beginning scan offset

_scanStart:

	newlist.WipeList();				// wipe lists
	dl_addresses.Erase();		

	if(scanOffset > (rom_length<<2))		// check bound
	 return;

	if(DoLUT && (numNotInLUT == 0))			// no more left to scan
	 return;

	numNotInLUT = 0;				// clear counter

	for(i=0L; i<cmdlist.Length(); i++)
	{
	 if( (!(i & 0x000003ffL)) && (!DoScan))		// (i&0x3ff)==(i%512)
	 {						// mas pronto
	  fputc('.', stderr);
	 }
							// gsSPVertex
	 if(cmdlist[i]->Id() == DEF_CmdID_gsSPVertex)
	 {
	  num =  ((VTXcmd*)cmdlist[i])->GetNum();	// get num to load
	  v0 =	((VTXcmd*)cmdlist[i])->GetV0();		// get starting point

	  if( (num > 16) || (num < 1) )			// skip bad cases
	   continue; 
	  if( (v0 > 15) || (v0 < 0) )
	   continue;

	  vaddr = ((VTXcmd*)cmdlist[i])->GetVAddr();	// get address

	  dl_addresses.Add(vaddr);			// add addr to list

	  if((!DoScan) && (vaddr < saddr))		// check for code seg
	  {						// not in code seg
	   temp_l = addrlist.Find(vaddr, 0xff000000);	// check LUT
	   if(temp_l >= 0)				// if in LUT, use LUT
	   {
	    offset = addrlist[temp_l].offset + (vaddr & 0x00ffffffL);
	    scanOffset = offset;
	   }
	   else						// not in LUT, skip
	   {
	    numNotInLUT++;
	    scanOffset = 0x1000L;
	    continue;
	   }
	  }
	  else if(DoScan)				// scanning
	  {
	   if(DoLUT)					// if using LUT
	   {
	    temp_l = addrlist.Find(vaddr, 0xff000000);	// check LUT
	    if(temp_l >= 0)				// if in LUT, use LUT
	    {
	     offset = addrlist[temp_l].offset + (vaddr & 0x00ffffffL);
	     FPF(stderr, "\rLUT : 0x%08lx->0x%08lx:0x%08lx", vaddr,
		addrlist[temp_l].address, addrlist[temp_l].offset);
	    }
	    else					// not in LUT
	    {
	     numNotInLUT++;
	     offset = scanOffset + (vaddr & 0x00ffffffL);
	     FPF(stderr, "\rScanning : 0x%08lx:0x%08lx", vaddr, scanOffset);
	    }
	   }
	   else						// not using LUT
	   {
	    offset = scanOffset + (vaddr & 0x00ffffffL);
	    FPF(stderr, "\rScanning : 0x%08lx:0x%08lx", vaddr, scanOffset);
	   }
	  }
	  else						// code segment
	  {
	   scanOffset = 0x1000L;
	   offset = vaddr - saddr + 0x1000L;
	  }

	  if(offset  >= (rom_length<<2))		// check bounds
	   continue;

	  offset >>= 2;		// need offset in words, not bytes, so div by 4

	  for(j=0; j<num; j++)				// load each vertex
	  {
	   k = j << 2;			// k = j * 4
	   w1 = memfile[offset+k  ]; 	// each vtx_tn is 4 words long,
	   w2 = memfile[offset+k+1];	//  so we move by 4's
	   w3 = memfile[offset+k+2];
	   w4 = memfile[offset+k+3];
	   vtxCache[j + v0].n.ob[0] = (w1 & 0xffff0000L) >> 16;
	   vtxCache[j + v0].n.ob[1] = w1 & 0x0000ffffL;
	   vtxCache[j + v0].n.ob[2] = (w2 & 0xffff0000L) >> 16;
	   vtxCache[j + v0].n.flag = w2 & 0x0000ffffL;
	   vtxCache[j + v0].n.tc[0] = (w3 & 0xffff0000L) >> 16;
	   vtxCache[j + v0].n.tc[1] = w3 & 0x0000ffffL;
	   vtxCache[j + v0].n.n[0] = (w4 & 0xff000000L) >> 24;
	   vtxCache[j + v0].n.n[1] = (w4 & 0x00ff0000L) >> 16;
	   vtxCache[j + v0].n.n[2] = (w4 & 0x0000ff00L) >> 8;
	   vtxCache[j + v0].n.a = w4 & 0x000000ffL;
	  }
	 }	// end gsSPVertex

 							// gsSP1Triangle

	 else if(cmdlist[i]->Id() == DEF_CmdID_gsSP1Triangle)
	 {
	  CVtx_tn		vtx;
	  short			ob[3];
	  double		obf[3];
	  short			tc[2];
	  double		tcf[2];
	  signed char		n[3];
	  double		fn[3];
	  unsigned char		alpha;
	  unsigned short	flag;
	  int			vnum;
	  int			v[3];

	  v[0] = ((TRIcmd*)cmdlist[i])->GetV0();	// get vtx_tns
	  v[1] = ((TRIcmd*)cmdlist[i])->GetV1();
	  v[2] = ((TRIcmd*)cmdlist[i])->GetV2();
	  flag = ((TRIcmd*)cmdlist[i])->GetFlag();

	  if( (v[0] > 15) || (v[0] < 0))		// skip bad cases
	   continue; 
	  if( (v[1] > 15) || (v[1] < 0))
	   continue; 
	  if( (v[2] > 15) || (v[2] < 0))
	   continue; 
	  if(flag > 2)
	   continue; 

	  for(vnum=0; vnum<3; vnum++)			// build triangle
	  {
	   ob[0] = vtxCache[v[vnum]].n.ob[0];
	   ob[1] = vtxCache[v[vnum]].n.ob[1];
	   ob[2] = vtxCache[v[vnum]].n.ob[2];

	   obf[0] = (double)vtxCache[v[vnum]].n.ob[0];
	   obf[1] = (double)vtxCache[v[vnum]].n.ob[1];
	   obf[2] = (double)vtxCache[v[vnum]].n.ob[2];

	   tc[0] = vtxCache[v[vnum]].n.tc[0];
	   tc[1] = vtxCache[v[vnum]].n.tc[1];

	   tcf[0] = (double)vtxCache[v[vnum]].n.tc[0];
	   tcf[1] = (double)vtxCache[v[vnum]].n.tc[1];

	   n[0] = vtxCache[v[flag]].n.n[0];
	   n[1] = vtxCache[v[flag]].n.n[1];
	   n[2] = vtxCache[v[flag]].n.n[2];
	   fn[0] = (double)(vtxCache[v[flag]].n.n[0])/128.0;
	   fn[1] = (double)(vtxCache[v[flag]].n.n[1])/128.0;
	   fn[2] = (double)(vtxCache[v[flag]].n.n[2])/128.0;

	   alpha = vtxCache[v[vnum]].n.a;

	   vtx = CVtx_tn(ob, obf, flag, tc, tcf, n, fn, alpha);

	   newlist.Add(vtx);				// add vertex to list
	  }
	 }	// end gsSP1Triangle
	}

	if(newlist.Length() > (TriangleMin * 3))	// if we got something
	{						//  that we want
	 if((!DoScan) || (DoScan && newlist.PossibleShape(PossibleOptions)))
	 {
	  if(DoLUT)
	  {
	   min = dl_addresses.FindMin();		// get min address
	   if((min != 0xffffffffL) && 			// if not in LUT
		(addrlist.Find(min, 0xff000000) < 0))
	   {						// add mapping to LUT
	    FPF(stderr, "\nNotice: Possible address mapping 0x%08lx:0x%08lx.\n"
		      	 "        Adding mapping to LUT.\n",
			(min & 0xff000000), scanOffset);
	    if(DoAutoLUT)
	     addrlist.Add(AddrLUTentry(min, scanOffset));
	   }
	  }
	  newlist.DoCalcNorms = CalculateNormals;	
	  newlist.CalcNorms();				// calc norms, maybe
	  newlist.SetTexture(&dummytex);	
	  sprintf(objName, "dlrip%04d", numFound);

#ifndef DOS
	  if(DoPreview)
	   theViewer.ViewOpenGL(newlist, previewFlags, objName);
#endif

	  outfp = OpenNextFile(outfile, numFound);	// open next file
	  switch(OutputFormat)				// output
	  {
	   case DEF_OutFormat_Vtx:
		newlist.PrintVtx(objName , outfp);
		break;
	   case DEF_OutFormat_Inventor:
		newlist.PrintInventor(objName , outfp);
		break;
	   case DEF_OutFormat_OpenGL:
		newlist.PrintOGL(objName , outfp);
		break;
	  }
	  if(outfp != stdout)				// close file, maybe
	   fclose(outfp);
	  numFound++;					// inc counter
	 }
	}

	fflush(stderr);

	if(DoScan)
	{
	 scanOffset += ScanIncrement;
	 goto _scanStart;
	}
}



