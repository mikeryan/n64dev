// This project started 2/20/02 in order to do Neon64
// Written on a 10 y/o machine very much like the N64: 100mhz, 7 MB RAM
// But it has a 329 MB hard drive!

#define VERSION "0.1p"
#define CDATE "9/2/2003"

// v0.0a - First code, only support for db strings. 2/22/02
// v0.0b - Revised error codes, standardized operand reading 2/22/02
// v0.0c - comment ignorance, operand recognition, intergal operands  
//         (decimal, hex and binary) fixed some error reporting 2/24/02
// v0.0d - enforced line too long error, debug stuff, speed reporting 2/25/02
// v0.0e - added org, changed to correct method of argument access 
//         added negation (only for decimal values) 2/27/02
// v0.0f - began to set up symbol recognition (max 256 symbols of length
//         31 characters each for now, symbols must be stored in lowercase)
//         Note: org does not immediately change the pc, the change
//         only takes effect on the next line.
//         Note: Something new will need to be done about the
//         offset(base) operand in the load/store instructions. Maybe treat
//         the opening parentesis as a comma and ignore the closing one?
//         Or just use a simpler syntax: lb t0, $100, t1. The () are unneeded,
//         its not as if there are really any addressing modes!
// v0.0g - Cleared up some symbol stuff, added quotes.
// v0.0h - Fixed error in CheckEq, made include, cleaned up Errhandler
//         I ended this version before I acutally finished working tonight
//         because I feared that the next step might mess up my work.
// v0.0i - Added "1st pass", which counts the number of symbols so that
//         memory can be allocated for however many. Added r1-r31 reg names.
//         Cleaned out some commented code. Turned off obsolete warning. :-)
//        Rearranged a lot. Added some more quotes, put them in seperate file.
// v0.0j - Fixed major error in previous version caused by r1-r31 change.
// v0.0k - Added a lot of quotes, wrote complex expression evaluator but
//         haven't included it yet.
// v0.0l - Added expression evaluation but not with symbols yet.
// v0.0m - Made symbols in expression evaluation work, added '0x' prefix for
//         hexadecimal. Made equr and fixed equ and equr's error reporting.
//         Made expression error reporting more specific. Only macros now...
// v0.0n - Actual opcodes (load+store), added macros where they were sorely
//         needed, so code is easy to write although the compiled prog is big.
//         Added the indexed () addressing mode, made it required for load/
//         store instructions. Added dh and dw. Switched to medium mem model.
//         Labels can end in a colon that will be ignored.
// v0.0o - About, changed UI a bit, added quotes. Didn't want to mess up this
//         nice new version ...
// v0.0p - Put in the multi-pass certainty thing. It could use some more
//         testing, probably.
// v0.0q - Fixed bug in symbol name loading with include files.
//         Fixed bug that allowed a symbol to be defined in terms of itself.
//         Fixed improper reporting of ERR_EQU_NEEDS_LABEL
//         Increased speed by not writing if uncertain.
// v0.0r - Worked on uncertainty a bit more, cleaned up some too many labels
//         errors. Added dc*, incbin. Closed file before end. Made pointers
//         char instead of void.
// v0.0s - Added Spaceballs quotes, revised about screen.
// v0.0t - Incorporated preprocessor, made includes able to have single,
//         double, or no quotes. Added some Biblical and dragon warrior
//         quotes. Macros are done, all that remains is to add the rest
//         of the opcodes.
// v0.0u - Fixed bug that prevented macro parameters from being deallocated.
//         Found a bug in n64asm that assembles dsllv wrong.
//         Added all opcodes (except the "special" and exception opcodes.)
//         test.asm assembles more or less the same as in asmn64, except
//         that it uses addiu in li instead of ori. Why?
// v0.0v - Used a more complex method of calculating li, to accomodate signed
//         values. Made warnings type 1 errors.
//         Actually did some test runs!
//         As soon as some style warnings are added it'll be time for version
//         1.0!
// v0.0w - Added *more* opcodes. (break, mfc0, mtc0). Revised li again.
//         Did some more tests, got RSP working!
// v0.0x - Cleaned up preprocessor error reporting a little.
//         Added PC to debug reporting.
//         Fixed bug that didn't limit load/store immeidate values to
//         16 bits.
//         Realized that move had been in here since v0.0u :-)
// v0.0y - Added NEG opcode, made it possible for include files to be in the
//         same directory as the assembler instead of where it is run from.
//         Made non-debug version not include "Counting Symbols . . ."
//         Two months ago this project began . . .
// v0.0z - Another quote
// v0.1  - Standardized opcode generation. Found big bug
//         in xor. Added obj/objend. Can't figure out the register math error.
// v0.1a - Worked with arguments, added header file generation.
//         Added report to tell you what offset you're at, to aid in making
//         headers.
// v0.1b - Debugging erroneous or with 0x20000000 for sw and sd. Which didn't
//         actually exist. Added offset, which actually tells you what offset
//         you're at. report gives you the PC. assert makes sure that it is
//         at the given PC, for use in headers, generates an error otherwise.
//         Made assert a part of the header.
// v0.1c - Fixed a bug that checked for symbol type even when the symbol
//         was uncertain, only a problem in 'special' instructions.
//         Would it be so hard to have macros within macros? The answer
//         turned out to be no.
// v0.1d - I added the symbol pc, which is the PC of the beginning of a line,
//         and fixed a problem with the macro nesting where internal
//         directives weren't being mirrored.
//         errors are now printed with the line that caused them. Removed the
//         "Immediate value too large" error, it just doesn't work.
// v0.1e - Trying to speed up preprocessor. Exported Errhandler to another
//         file. Also made the line continuation character available anywhere
//         in the line, so if there's a comment afterward it should still be
//         OK. I also discovered that when deallocating an array you use
//         delete [], so I went through the program (mostly the preprocessor)
//         fixing that. Added a warning if a macro name contains another
//         macro name, made errtot (# of warnings) a global and had
//         Errhandler incrememnt it for an accurate count.
// v0.1f - Made another type of include that expects a file with nothing
//         but macros, so we don't have to wade through all the blank lines.
//         This and some other tweaks made macro processing considerably
//         faster (Almost twice as fast now!)
// v0.1g - Trying to speed up binary inclusion by loading the entire file
//         to memory. It worked.
// v0.1h - A bit of a bug in macros ignores spaces in parameters.
//         Another bug crashed the program with a zero-sized macro parameter.
//         Fixed incbin error reporting and gave it a bad operand error.
//         Took "first org" comment out of headers.
//         Made equne to keep those labels out of headers.
//         Added cdate to program title, since I've had several versions of
//         v0.1h.
// v0.1i - Large incbins cause errors...
//         A failed assert now tells you what the PC actually is.
//         Header no longer generated if assemble fails.
// v0.1j - Maybe register math error is caused by uncertainty? Since I made it
//         only report the error if symbolcertain, the problem hasn't come
//         up again. I also fixed an error of the cache instruction, the
//         cache op was at the wrong bit position.
//         Fixed an error in eret, thanks to my good friend the MIPS R4000
//         User's Manual.
// v0.1k - I found the CRC source, so now I have the assembler output
//         a completely checksummed N64 ROM image. (CRC code from stan).
//         Also added a modified version of Bung's drjr send utility, with
//         built-in byteswapping. U64ASM is now an all-in-one utility!
// v0.1l - Extend generates an error if it it runs out of hard drive space.
// v0.1m - Added watch instruction, reports which line contains a certain PC.
// v0.1n - Checking for request to make zero items, prevented error
//         on zero assemble time.
// v0.1o - Trying to find the source of some pain-in-the-ass errors. By
//         making the preprocessor only replace a \ with 0xa instead of 0xd
//         and 0xa I seem to have stopped it...
// v0.1p - Added support for parenthesis in macro parameters to preprocessor
//         Found a problem with macros with 10 or more parameters: $0a, the
//         line feed character, would be interpreted as parameter 10...
//         I think a good solution would be to use characters 128 and up,
//         which of course excludes them from being used in any macro.
//         Now catch divide by zero error in divide operation (I had all the
//         special cases handled for all the other ops, but not the obvious
//         one.)

//#define DEBUG
//#define QUOTES
#pragma warn -obs

using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "defines.h"

bool ExpError;
int  ExpErrorV;

int errtot=0;

bool IsRegister;
bool IsIndex;
bool IsCOPRegister;

unsigned long watchpoint;
bool watchactive=false;
unsigned long this_line_count=0;

bool pccertain=true;
bool symbolcertain=true;
bool thiscertain=true;
unsigned int numuncertain;
unsigned int whenuncertain;

bool isfirstorg=true;
unsigned long firstorg=0;

unsigned long objstart; // where the object started
unsigned long objinit;  // initial value for the object
bool objcert;
bool inobj=false;

char * asmpath;

struct Macro {
   char *name;
   unsigned int parnum;
   char *text;
   bool multiline;
};

void InitRegs(void);
int LoadSymbols(char * asmfiles, unsigned int line);
unsigned int CountSymbols(char *);
int Errhandler(int, unsigned long, char *, char *);
void Syntax(char *);
int AssembleFile(char *, int, unsigned long &,unsigned long &, bool &);
bool IsChar(char);      // Returns false if the character is a space or null
char LowerCase(char);   // Converts character to lowercase
bool CheckEq(char *, int, char *, int &);       // Sees if 2nd str is in
                                                // 1st str at int
bool CheckSym(char *, int, char *, int &);      // slightly different from
                        // checkeq because it allows chars after the string
                        // when there are spaces in the search string
int Assemble(ifstream &, int, unsigned long &); // Main assembly function
                                               // returns error code
int OperandOffset(char *, int);                // Finds the offset of the
                                                // first operand
int FindOperands(char *,int, char **, unsigned long *, int *, int &, int);
int GetSymbol(char *, int &, int &);

// Expression evaluation prototypes
int InSet(char *, int, char);
unsigned long EvOp(char *, int &);
unsigned long EvInt(char *, int &);
unsigned long Evaluate(char *, int &);
unsigned long DivRep(unsigned long, unsigned long, unsigned long);
unsigned long Root(unsigned long, unsigned long);

// Preprocessor prototypes
int CopyFile(char *, ofstream &, unsigned long &, unsigned long &, bool);
char inline LowerCase(char);
int LoadMacs(Macro *, ifstream &);
int DoMacs(Macro *, unsigned long, int &, ifstream &, ofstream &, bool);
int InSet(char * set, int setsize, char ciq);
bool static CheckMac(char *array, unsigned int aoff, char *sstring, unsigned int &ic);
int PreProcessor (char *);

int crc(int); // Takes a file handle
int drjrsend(char *); // Parameter is name of file to send.
#include "symbols.h"
#include "asm.h"
#include "exp.h"
#include "pre.h"
#include "err.h"
#include "crc.h"

#ifdef WIN32
#include "drjr.h"
#endif

// get time in milliseconds
unsigned long gettime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char *argv[]) {
   // fatalerror means the program was actually stopped
   // anerr means that a 'minor' error occured and was ignored
   bool fatalerror=false,willtolive,header=false,romout=false,drsend=false;
   unsigned long bios_start_time=gettime();
   int err,c,c2;
   int outhandle,headhandle;
   unsigned long program_counter;
   unsigned long line_count;
   unsigned int numpasses=1;
   unsigned int lastnumuncertain;   
   unsigned char * temp;
   char * pathfile;
   char smalltemp;

   // *** BEGIN ARGUMENT PROCESSING

   char *asmfile = "temp2.tmp";
   char *realfile = (char *)0;
   char *outfile = (char *)0;
   char *headfile = (char *)0;
   
   cout << "U64ASM " << VERSION;
   #ifdef DEBUG
   cout << " (Internal debug version)";
   #endif
   cout << " (" << CDATE << ")\n";
   cout << "(c) 2002-2003 Halley's Comet Software\n\n";
   if (argc == 0) {
      Syntax(argv[0]);
      return 1;
   }
   
   for (c=1; c < argc; c++) {
      if (!strcmp(argv[c],"-about") || !strcmp(argv[c],"-credits")) {
         #include "about.h"
         return 0;
      } else if (argv[c][0] == '-' && argv[c][1] == 'o') {
         outfile = argv[c]+2;
      } else if (argv[c][0] == '-' && argv[c][1] == 'h') {
         header = true;
         headfile = argv[c]+2;
         if (argv[c][2]=='\0') headfile = (char *)0;
      } else if (argv[c][0] == '-' && argv[c][1] == 'r') romout=true;
#ifdef WIN32
      else if (argv[c][0] == '-' && argv[c][1] == 's') {drsend=true; romout=true;}
#endif
      else {
         realfile = argv[c];
         willtolive=true;
         for (c2=0; c2 < strlen(realfile) && willtolive; c2++) {
            if (realfile[c2]=='.') willtolive=false;
         }
         if (willtolive) {
            realfile = new char[strlen(realfile)+5];
            strcpy(realfile,argv[c]);
            c2 = strlen(realfile);
            realfile[c2]='.';
            realfile[c2+1]='a';
            realfile[c2+2]='s';
            realfile[c2+3]='m';
            realfile[c2+4]='\0';
         }
      }
   }
   if (!realfile) {
      Syntax(argv[0]);
      return 1;
   }
   if (!outfile) {
      outfile = new char[strlen(realfile)+5];
      willtolive=true;
      for (c=0; c < strlen(realfile) && willtolive; c++) {
         outfile[c]=realfile[c];
         if (realfile[c]=='.') willtolive=false;
      }
      outfile[c]='b';
      outfile[c+1]='i';
      outfile[c+2]='n';
      outfile[c+3]='\0';
   }
   if (header && !headfile) {
      headfile = new char[strlen(realfile)+1];
      willtolive=true;
      for (c=0; c < strlen(realfile) && willtolive; c++) {
         headfile[c]=realfile[c];
         if (realfile[c]=='.') willtolive=false;
      }
      headfile[c]='h';
      headfile[c+1]='\0';
   }
   
   // Find the path of the assembler, where files will be looked for if
   //  they are not found in the current dir.
   // "Pathfinder" :-)

   for (c=strlen(argv[0])-1; c > 0; c--) {
      if (argv[0][c]=='\\') break;
   }
   asmpath = new char[c+2];
   for (c2=0; c2<=c; c2++) {
      asmpath[c2]=argv[0][c2];
   }
   asmpath[c2]=0;

   // ******* END PROCESSING ARGUMENTS

   cout << "Preprocessing .";
   if (PreProcessor(realfile)) return 2;

   cout << "Loading symbols . . .\n";
   #ifdef DEBUG
   cout << "Counting symbols . . .\n";
   #endif
   maxsymbols=CountSymbols(asmfile)+BUILTINSYMBOLS;
   SymbolList = new Symbol[maxsymbols];
   if (!SymbolList) {
      cout << "Out of memory.\n";
      return 1;
   }

   InitRegs(); // Load the basic symbols (register names)
   if (LoadSymbols(asmfile)) {cout << "Fatal error, assembly must halt.\n"; return 2;}

   cout << "Assembling " << realfile << " to " << outfile << " . . .\n";
   
   // ************ MAIN ASSEMBLY LOOP ****************
   // Once per pass.
   do {

   outhandle = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
   if (outhandle < 0) {
      cout << "Error opening output file.\n";
      return 1;
   }
   close(outhandle);
   outhandle = open(outfile, O_RDWR|O_EXCL, S_IREAD|S_IWRITE);
   
   if (outhandle < 0) {
      cout << "Error opening output file.\n";
      return 1;
   }

   // A ROM needs a header.
   if (romout) {
      headhandle = open("header", O_RDONLY|O_EXCL, S_IREAD);
      if (headhandle <= 0) {
   
         pathfile=new char[strlen(asmpath)+7];
         for (c=0; c < strlen(asmpath); c++) {
            pathfile[c]=asmpath[c];
         }
         pathfile[c]='h'; pathfile[c+1]='e'; pathfile[c+2]='a';
         pathfile[c+3]='d'; pathfile[c+4]='e'; pathfile[c+5]='r';
         pathfile[c+6]=0;
         headhandle = open(pathfile, O_RDONLY|O_EXCL, S_IREAD);
         delete [] pathfile;
         if (headhandle <= 0) {cout << "Error opening N64 header.\n"; return 1;}
      }
      
      temp = new unsigned char [4096];
      if (temp) {
         read(headhandle,temp,4096);
         write(outhandle,temp,4096);
         delete [] temp;
      } else {
         for (c=0; c < 4096; c++) {
            read(headhandle,&smalltemp,1);
            write(outhandle,&smalltemp,1);
         }
      }
      close(headhandle);
   }

   // Actual assembly begins
   cout << "Pass " << numpasses << " . . .\n";
   
      program_counter=0;
      symbolcertain=true;
      pccertain=true;
      lastnumuncertain=numuncertain;
      numuncertain=0;
      line_count=1;
      if (AssembleFile(asmfile, outhandle, line_count, program_counter, fatalerror)) return 1;
      #ifdef DEBUG
      cout << "Total # uncertain: " << numuncertain << '\n';
      #endif
      if (numpasses==1) lastnumuncertain=numuncertain+1;
      numpasses++;
      if (numuncertain >= lastnumuncertain) {cout << "Error: Irresolvable uncertainty.\nFatal error, assembly must halt.\n"; return 2;}
      
      program_counter = lseek(outhandle,0,SEEK_CUR);// gives offset of last
                                                    // byte, size of file
      // Make a ROM image.
      if (romout && !fatalerror && symbolcertain && pccertain) {
         cout << "Extending " << outfile << " from " << program_counter << " bytes to ";
         cout << (((program_counter/2097152)+1)*2097152) << " bytes . . .\n";
         program_counter=lseek(outhandle,(((program_counter/2097152)+1)*2097152)-1, SEEK_SET)+1;
         smalltemp=0;
         write(outhandle, &smalltemp, 1);
         cout << "Calculating checksum . . .\n";
         if(crc(outhandle)) return 1;
      }
   
      close(outhandle);
   } while ((!symbolcertain || !pccertain) && !fatalerror);
   // ********** END OF MAIN ASSEMBLY LOOP ****************8

#ifdef WIN32
   if (drsend && !fatalerror) {
      cout << "Sending";
      if (drjrsend(outfile)) return 1;
   }
#endif

   if (header && !fatalerror) {
      cout << "Creating header " << headfile << " . . .\n";
      ofstream headerfile(headfile);
      headerfile.flags(ios::hex|ios::showbase);
      headerfile << "; U64ASM v" << VERSION << " header file.\n";
      headerfile << " assert " << firstorg << '\n';
      for (c=BUILTINSYMBOLS; c < maxsymbols; c++) {
         if (SymbolList[c].type == DTYPE_INTEGER && SymbolList[c].bexport) {
            headerfile << SymbolList[c].name << " equ " << SymbolList[c].value << '\n';
         }
      }
      headerfile << " incbin \"" << outfile << "\"\n";
      headerfile.close();
   }

   cout << flush;
   if (!fatalerror) {
      float total_time = ((float)(gettime()-bios_start_time))/1000.0;
      cout << "\n" << line_count << " lines assembled in ";
      cout << total_time << " seconds into " << program_counter << " bytes.\n";
      if (total_time==0) {
         cout << "That's really fast!\n";
      } else {
          cout << "That's " << line_count/total_time << " lines or " << program_counter/total_time << " bytes per second.\n";
      }
      if (errtot > 0) {
         cout << errtot << " warning" << (errtot==1?" ":"s ") << "generated.\n";
         return 2;  // 1 is for file errors and the like.
         //return 1;  // not as big an error
         //return 2;  // still an error
      }
   } else return 3; // an acutal assembly error

   #ifndef DEBUG
   unlink("temp.tmp");
   unlink("temp2.tmp");
   #endif

   return 0;
}

int AssembleFile(char * rasmfile, int outhandle, unsigned long &line_count,
                 unsigned long &program_counter, bool &fatalerror) {
   char instr[256], asmfile[256];
   int errl,linecountfactor=1,c;
   this_line_count=0;

   strcpy(asmfile,rasmfile);   

   ifstream input(asmfile);
   if (!input) {
      cout << "Error opening input file " << asmfile << ".\n";
      return 1;
   }
   
   while (!input.eof() && !fatalerror) {
     
      this_line_count+=linecountfactor;
      input.getline(instr,255);
      
      #ifdef DEBUG
      cout << this_line_count << ": ";
      #endif

      if (strlen(instr)==254) {errl=Errhandler(ERR_LINE_TOO_LONG,this_line_count,asmfile,NULL);}
      else {
         if (instr[0]=='#') {
            errl=NO_ERROR;
            switch (instr[1]) {
               case 'e':
                  #ifdef DEBUG
                  cout << '\n';
                  #endif
                  linecountfactor=0;
                  break;
               case 's':
                  #ifdef DEBUG
                  cout << '\n';
                  #endif
                  linecountfactor=1;
                  break;
               case 'f':
                  for (c=3; c < strlen(instr); c++) {
                     asmfile[c-3]=instr[c];
                  }
                  asmfile[c-3]=0;
                  #ifdef DEBUG
                  cout << "File name changed to: " << asmfile << '\n';
                  #endif
                  break;
               case 'l':
                  this_line_count=0;
                  for (c=3; c < strlen(instr); c++) {
                     this_line_count*=10;
                     this_line_count += instr[c]-'0';
                  }
                  #ifdef DEBUG
                  cout << "Line count changed to: " << this_line_count << '\n';
                  #endif
                  break;
               case 't':
                  line_count=0;
                  for (c=3; c < strlen(instr); c++) {
                     line_count*=10;
                     line_count+=instr[c]-'0';
                  }
                  #ifdef DEBUG
                  cout << "Total line count = " << line_count << '\n';
                  #endif
                  break;
               default:
                  errl=ERR_ILLEGAL_DIRECTIVE;
            }
         } else errl=Assemble(instr,outhandle,program_counter);
         errl=Errhandler(errl,this_line_count,asmfile,instr);
      }
      if (errl==FATAL_ERROR) {
         cout << "Fatal error, assembly must halt.\n";
         fatalerror=true;
      }
   }
   input.close();
   #ifdef DEBUG
   cout << "End of " << asmfile << '\n';
   #endif
   return 0;
}

void Syntax(char * exename) {
   cout << "Syntax is:\n";
   cout << exename << " <asm file> [optional switches]\n";
   cout << "<asm file>:         The file containing the assembly source. If no extension\n";
   cout << "                    is used, .asm is assumed.\n";
   cout << "-o<bin output>:     Use this switch to specify the name of the binary output\n";
   cout << "                    file. If this switch is not used, the source file name,\n";
   cout << "                    with a .bin extension, will be used. If no extension is\n";
   cout << "                    used, .bin is assumed.\n";
   cout << "-h<header file>:    Use this switch to cause the generation of a header file,\n";
   cout << "                    which will be written with the values of any defined labels\n";
   cout << "                    in the program and an incbin instruction at the bottom to\n";
   cout << "                    include the assembled code. An assert statement is placed at";
   cout << "                    the top of the file, causing the assembler to check that the";
   cout << "                    preassembled code is at the proper location in the program.\n";
   cout << "                    If no file name is given, the source file name with a .h\n";
   cout << "                    extension is used. If no extension is given, .h is assumed.\n";
   cout << "-r:                 This switch causes the output to be a complete, extended,\n";
   cout << "                    checksummed N64 ROM image. The assembler expects the N64\n";
   cout << "                    boot header to be in a file called \"header\" in either the\n";
   cout << "                    same directory as the source or the assembler.\n";
#ifdef WIN32
   cout << "-s:                 This switch sends the N64 ROM image to a V64jr backup unit.\n";
   cout << "                    -s activates the -r switch automatically.\n";
#endif
   cout << "-about or -credits  Gives some information about the program and those who made\n";
   cout << "                    it possible.\n";
   return;
}

// Reading this code constitutes a federal offense.
// This source code is the exclusive property of Halley's Comet Software
// (c) 2002

// Written entirely by Adam Gashlin (a.k.a. themind) in edit.com, compiled
// with Borland C++ and A LOT OF PATIENCE.

// HalleysCometSoftware@hotmail.com
