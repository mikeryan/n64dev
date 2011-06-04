// contains some stuff related to symbols and registers

#define SYMBOL_SIZE 32
#define BUILTINSYMBOLS 107

struct Symbol {
   char name[SYMBOL_SIZE];
   unsigned long value;
   int type;
   bool certain;
   bool bexport;
};

Symbol *SymbolList;
unsigned int symcount;
unsigned int maxsymbols;

// registers are implemented as symbols

void InitRegs(void) {
   char * regnames[32] = {"r0 ","at ","v0 ","v1 ","a0 ","a1 ","a2 ","a3 ",
                          "t0 ","t1 ","t2 ","t3 ","t4 ","t5 ","t6 ","t7 ",
                          "s0 ","s1 ","s2 ","s3 ","s4 ","s5 ","s6 ","s7 ",
                          "t8 ","t9 ","k0 ","k1 ","gp ","sp ","s8 ","ra "};
   char * regnames2[32]={"zero ","r1 ", "r2 ", "r3 ", "r4 ", "r5 ", "r6 ", "r7 ",
                         "r8 ", "r9 ", "r10 ","r11 ","r12 ","r13 ","r14 ","r15 ",
                         "r16 ","r17 ","r18 ","r19 ","r20 ","r21 ","r22 ","r23 ",
                         "r24 ","r25 ","r26 ","r27 ","r28 ","r29 ","r30 ","r31 "};
   char * regnames0[10]={"r00 ","r01 ","r02 ","r03 ","r04 ","r05 ","r06 ","r07 ","r08 ","r09 "};
   char * cop0[32]={"index ","random ","entrylo0 ","entrylo1 ","context ",
                    "pagemask ","wired ","c07 ","badvaddr ","count ",
                    "entryhi ","compare ","status ","cause ","epc ","previd ",
                    "config ","lladdr ","watchlo ","watchhi ","xcontext ",
                    "c21 ","c22 ","c23 ","c24 ","c25 ","perr ","cacheerr ",
                    "taglo ","taghi ","errorepc ","c31 "};

   unsigned int c;
   for (c=0; c < 32; c++) {
      strcpy(SymbolList[c].name,regnames[c]);
      SymbolList[c].value = c;
      SymbolList[c].type = DTYPE_REGISTER;
      SymbolList[c].certain = true;
      SymbolList[c].bexport = false;
   }

   for (c=32; c < 64; c++) {
      strcpy(SymbolList[c].name,regnames2[c-32]);
      SymbolList[c].value = c-32;
      SymbolList[c].type = DTYPE_REGISTER;
      SymbolList[c].certain = true;
      SymbolList[c].bexport = false;
   }

   for (c=64; c < 74; c++) {
      strcpy(SymbolList[c].name,regnames0[c-64]);
      SymbolList[c].value = c-64;
      SymbolList[c].type = DTYPE_REGISTER;
      SymbolList[c].certain = true;
      SymbolList[c].bexport = false;
   }

   for (c=74; c < 106; c++) {
      strcpy(SymbolList[c].name,cop0[c-74]);
      SymbolList[c].value = c-74;
      SymbolList[c].type = DTYPE_CREGISTER;
      SymbolList[c].certain = true;
      SymbolList[c].bexport = false;
   }

   strcpy(SymbolList[106].name,"pc ");
   SymbolList[106].value = 0;
   SymbolList[106].type = DTYPE_INTEGER;
   SymbolList[106].certain = true;
   SymbolList[c].bexport = false;

   symcount = BUILTINSYMBOLS;
   return;
}

// Counts the number of symbols in the file, duplicate symbols will be
// counted repeatedly and thus not caught here, but they will be later.
// it counts as a symbol if the first character is not whitespace or if
// it is not an include
unsigned int CountSymbols(char * asmfile) {
   ifstream input(asmfile);
   char instr[256];
   unsigned int symbcount=0;
   
   if (!input) {
      cout << "Error opening input file " << asmfile << ".\n";
      exit(1); // because its supposed to return a numeric value
   }

   while (!input.eof()) {
   
      input.getline(instr,256);
   
      // Process comment ignorance

      if (instr[0] != ';' && instr[0] != '#') {
         // if the first character is not blank
         if (IsChar(instr[0])) symbcount++;
      }
   }
   input.close();
   return symbcount;
}

   
int LoadSymbols(char * rasmfile) {
   //InitRegs(); // Load the basic symbols (register names)
                 // already done in main()
   
   ifstream input(rasmfile);
   char asmfile[256];
   char instr[256];
   int c,lc,err,ic;
   unsigned int line=0,linecountfactor=1;

   strcpy(asmfile,rasmfile);
   
   if (!input) {
      cout << "Error opening input file " << asmfile << ".\n";
      exit(1); // because its supposed to return a numeric value
   }

   while (!input.eof()) {
   
      line+=linecountfactor;
      input.getline(instr,256);
      
      if (instr[0]=='#') {
         switch (instr[1]) {
            case 'e':
               linecountfactor=0;
               break;
            case 's':
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
               line=0;
               for (c=3; c < strlen(instr); c++) {
                  line*=10;
                  line += instr[c]-'0';
               }
               #ifdef DEBUG
               cout << "Line count changed to: " << line << '\n';
               #endif
               break;
            case 't':
               break;
            default:
               return Errhandler(ERR_ILLEGAL_DIRECTIVE,line,asmfile,instr);
         }
      } else if (instr[0] != ';' && instr[0] != '#') {

      if (IsChar(instr[0])) {
         for (lc=0; lc < symcount; lc++) {
            if (CheckEq(instr,0,SymbolList[lc].name,ic)) {
               return Errhandler(ERR_ALREADY_DEFINED,line,asmfile,instr);
            }
         }
         for (c=0; IsChar(instr[c]); c++) {
            #ifdef DEBUG
            cout << instr[c];
            #endif
            SymbolList[symcount].name[c] = LowerCase(instr[c]);
         }
         #ifdef DEBUG
         cout << "* ";
         #endif
         SymbolList[symcount].name[c]=' ';
         SymbolList[symcount].name[c+1]=0;
         SymbolList[symcount].certain=false;
         symcount++;
         #ifdef DEBUG
         cout << SymbolList[symcount-1].name << "*\n";
         #endif
      }
      }
   }
   input.close();
   return NO_ERROR;
}
   
