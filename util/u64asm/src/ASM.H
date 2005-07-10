// asm.h
// The real body of the assembler, the biggest function is FindOperands
// which parses operands and returns a nice list for the opcode function
// to deal with

// What is the point of this list? I'm not sure.
// v0.0a - Last modified 2/22/02
// v0.0b - 2/22/02
// v0.0c - 2/24/02
// v0.0d - 2/24/02-2/25/02
// v0.0e - 2/27/02
// v0.0f - 2/27/02-2/28/02
// v0.0g - 3/14/02
// v0.0h - 3/14/02
// v0.0i - 3/15/02
// v0.0j - 3/15/02
// v0.0k - 3/16/02-3/17/02
// v0.0l - 3/17/02
// v0.0m - 3/17/02
// v0.0n - 3/18/02-3/19/02
// v0.0o - 3/21/02
// v0.0p - 3/21/02
// v0.0q - 3/22/02-3/24/02
// v0.0r - 3/24/02-3/25/02
// v0.0s - 3/26/02
// v0.0t - 4/1/02-4/2/02
// v0.0u - 4/2/02
// v0.0v - 4/3/02
// v0.0w - 4/5/02-4/8/02
// v0.0x - 4/9/02-4/14/02
// v0.0y - 4/20/02-5/9/02
// v0.0z - 5/10/02-5/17/02
// v0.1 - 5/17/02-5/21/02
// v0.1a - 5/22/02
// v0.1b - 6/60/02-7/4/02
// v0.1c - 7/5/02
// v0.1d - 7/6/02-7/7/02
// v0.1e - 7/9/02-7/10/02
// v0.1f - 7/10/02-7/11/02
// v0.1g - 7/16/02
// v0.1h - 7/17/02-7/21/02
// v0.1i - 7/22/02-7/23/02
// v0.1j - 8/12/02-8/13/02
// v0.1k - 8/20/02-8/21/02
// v0.1l - 8/23/02
// v0.1m - 8/29/02
// v0.1n - 10/2/02
// v0.1o - 2/25/03

#include "opcode.h"

bool IsChar(char c) {
   return ((c!=' ') && (c!=0) && (c!=':'));
}
char LowerCase(char c) {
   if (c >= 'A' && c <= 'Z') return c+('a'-'A');
   else return c;
}
bool CheckEq(char *array, int aoff, char *sstring, int &ic) {
   int c;
   ic=aoff;
   for (c=0; c < strlen(sstring); c++) {
      if (!IsChar(array[ic]) && sstring[c]==' ') return true;
      if (LowerCase(array[ic]) != sstring[c]) return false;
      ic++;
   }
   return true;
}
bool CheckSym(char *array, int aoff, char *sstring, int &ic) {
   int c;
   ic=aoff;
   #define OECSIZE 17
   char okendchars[OECSIZE] = {0,',','+','-','/','%','*','|','&','!','^','<','>','@',')','(',':'};
   for (c=0; c < strlen(sstring); c++) {
      if (LowerCase(array[ic]) != sstring[c]) {
         if (!(sstring[c]==' ' && (InSet(okendchars,OECSIZE,array[ic]) < OECSIZE))) return false;
      }
      ic++;
   }
   return true;
}

int OperandOffset(char *thestring, int cchar) {
   int c;
   for (c=cchar; (c < strlen(thestring)) && !IsChar(thestring[c]); c++) {}
   return c;
}

// Returns error code
int FindOperands(char *instr,int firstbyte, char **pointers, unsigned long *values, int *types, int &numops, int maxops) {
   int c, c2, c3;
   unsigned long total;
   int errl;
   bool inastring=false,nextop=true,indreg=false,wasp=false;
   numops = 0;
   thiscertain=true;    // what if a later operand is certain?
   whenuncertain=0;
   for (c=firstbyte; c < strlen(instr); c++) {
      switch (instr[c]) {
         case '\"':
            if (numops >= maxops && nextop) return ERR_TOO_MANY_OPERANDS;
            if (nextop) nextop=false;
            else return ERR_UNEXPECTED_CHARS;
            numops++;
            inastring=true;
            c2=c;
            *(pointers+numops) = instr+c+1;
            types[numops] = DTYPE_STRING;
            while ((c < strlen(instr)) && inastring) {
               c++;
               if (instr[c] == '\"') inastring=false;
            }
            values[numops] = c-c2-1;
            if (inastring) return ERR_UNTERMINATED_STRING;
            break;
         case '\'':
            if (numops >= maxops && nextop) return ERR_TOO_MANY_OPERANDS;
            if (nextop) nextop=false;
            else return ERR_UNEXPECTED_CHARS;
            numops++;
            inastring=true;
            c2=c;
            *(pointers+numops) = instr+c+1;
            types[numops] = DTYPE_STRING;
            while ((c < strlen(instr)) && inastring) {
               c++;
               if (instr[c] == '\'') inastring=false;
            }
            values[numops] = c-c2-1;
            if (inastring) return ERR_UNTERMINATED_STRING;
            break;
         case ',':
            // expects another operand
            if (nextop) return ERR_MISSING_OPERAND;
            nextop=true;
         case ' ':      // ignore spaces
            break;
         // Assumed to be a symbol
         default:
            if (numops >= maxops && nextop) return ERR_TOO_MANY_OPERANDS;
            if (nextop) nextop=false;
            else return ERR_UNEXPECTED_CHARS;
            ExpError=false;
            ExpErrorV=ERR_COMPLEX_EXPRESSION;
            IsRegister=false;
            IsIndex=false;
            IsCOPRegister=false;
            wasp = (instr[c]=='(');
            if (thiscertain) {
               total = Evaluate(instr,c);
               if (!thiscertain) whenuncertain=numops+1;
            } else total = Evaluate(instr,c);
            if (ExpError) return ExpErrorV;
            numops++;
            if (instr[c]=='(') {
               if (indreg) return ERR_INDIRECTION;
               indreg=true;
               nextop=true;
            } else {
               if (indreg && !IsIndex) return ERR_INDIRECTION;
               if (IsIndex) {
                  if (!indreg && !wasp) return ERR_INDIRECTION;
                  indreg=false;
               }
            }
            values[numops]=total;
            types[numops]=(IsIndex?DTYPE_IREGISTER:(IsRegister?(IsCOPRegister?DTYPE_CREGISTER:DTYPE_REGISTER):DTYPE_INTEGER));
            #ifdef DEBUG
            cout.flags(ios::hex|ios::showbase);
            cout << "Result of expression: " << total << '\n';
            cout.flags(ios::dec);
            #endif
            c--;
            break;
      }
   }
   if (nextop && !(numops==0)) return ERR_MISSING_OPERAND;
   if (!thiscertain) return ERR_UNCERTAIN;
   return NO_ERROR;
}

int GetSymbol(char * instr, int &c, int &symnum) {
   bool inastring=true;
   int c2, c3;
   for (c2=0; (c2 < maxsymbols)&&inastring; c2++) {
      // Check the first char, theres no need to call CheckSym
      // if not even the 1st char matches.
      if (SymbolList[c2].name[0] == LowerCase(instr[c])) {
         if (CheckSym(instr,c,SymbolList[c2].name,c3)) {
            c=c3-2;    // it makes sense, think about it
            if (!SymbolList[c2].certain) {thiscertain=false; symbolcertain=false; numuncertain++;}
            symnum=c2;
            inastring=false;
            #ifdef DEBUG
            cout.flags(ios::showbase | ios::hex);
            cout << "Symbol " << SymbolList[c2].name << "'s value: " << SymbolList[c2].value << '\n';
            cout.flags(ios::dec);
            #endif
         }
      }
   }
   #ifndef DEBUG
   if (inastring) return ERR_NOT_DEFINED;
   #else
   if (inastring) {
      cout << "Bad symbol: " << instr+c <<"*\n";
      return ERR_NOT_DEFINED;
   }
   #endif
   return NO_ERROR;
}

int Assemble(char *instr, int outhandle, unsigned long &pc) {
   SymbolList[106].value=pc;
   SymbolList[106].certain=pccertain;
   bool instring, labeled=false, blankline=false;
   char stringinage;
   int c, lc, ic;
   int fnret;

   #ifdef DEBUG
   cout.flags(ios::showbase | ios::hex);
   cout << setw(11) << pc << ' ';
   cout << setw(11) << lseek(outhandle,0,SEEK_CUR) << ' ';
   cout.flags(ios::dec);
   #endif
   
   #ifdef DEBUG
   cout.flags(ios::left);
   cout << setw(49) << instr << setw(0) << "*\n";
   #endif

   c=0;
   
   // if the first character is not blank, there is a label, skip ahead
   // to the actual command
   if (IsChar(instr[0])) {
      for (; (c < strlen(instr))&&IsChar(instr[c]); c++) {}
      labeled=true;
   }
   
   for (; (c < strlen(instr))&&!IsChar(instr[c]); c++) {}
   blankline = (c==strlen(instr));
   if (blankline && !labeled) return NO_ERROR;

   // equ can correct this itself
   if (labeled) {
      for (lc=0; lc < maxsymbols; lc++) {
         if (CheckEq(instr,0,SymbolList[lc].name,ic)) break;
      }
      SymbolList[lc].value = pc;
      SymbolList[lc].type = DTYPE_INTEGER;
      SymbolList[lc].certain = pccertain;
      SymbolList[lc].export = true;
   }
   if (blankline) return NO_ERROR;

   fnret = AsmInstr(pc,instr,c,outhandle);
   
   return fnret;
}

// One for all and every man for himself!
