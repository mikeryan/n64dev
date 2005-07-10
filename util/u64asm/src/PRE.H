// macro/include preprocessor v0.9
// now allowing nested macros (and correctly echoing directives)
// optimized a bit by using single chars to represent parameters to eliminate
//  the need to check *every* character against the parameters
// attempting to make another type of include, imacros, and other small
//  speedups
// fixed crash with zero length parameter
// trying to allow parenthesis within macro parameters

int PreProcessor (char * asmfile) {
   unsigned long defcnt=0,totlines=0;
   unsigned int c,c2,c3,c4;
   int numdone;
   bool nest=false,contain;
   
   ofstream output("temp.tmp");
   if (!output) {cout << "\nError opening output file temp.tmp\n"; return 1;}
   if (CopyFile(asmfile,output,defcnt,totlines,false)) {return 1;}
   output << "#t " << totlines << '\n';
   output.close();

   #ifdef DEBUG
   cout << defcnt << " defines\n";
   #endif

   Macro *macros = new Macro[defcnt];
   if (!macros) {cout << "\nOut of memory.\n"; return 1;}

   cout << " .";
   ifstream input("temp.tmp");
   if (!input) {cout << "\nError opening input file temp.tmp.\n"; return 1;}
   if (LoadMacs(macros,input)) {return 1;}
   input.close();
   
   #ifdef DEBUG
   for (c=0; c < defcnt; c++) {
      cout << "Macro: \"" << macros[c].name << "\"\n";
      cout << "Text:\n\"";
      for (c2=0; c2 < strlen(macros[c].text); c2++) {
         if (macros[c].text[c2] <= macros[c].parnum) {
            cout << "[parameter " << (int)macros[c].text[c2] << ']';
         } else cout << macros[c].text[c2];
      } 
      cout << "\"\n";
   }
   #endif   

   cout << " .";
   for (c=0; c < defcnt; c++) {
      contain=false;
      for (c2=0; c2 < defcnt; c2++) {
         if (c2!=c) {
            for (c3=0; c3 < strlen(macros[c].name) && !contain; c3++) {
               contain = contain || CheckMac(macros[c].name,c3,macros[c2].name,c4);
            }
         }
      }
      if (contain) Errhandler(ERR_MACRO_NAME,0,NULL,macros[c].name);
   }

   do {
      cout << " .";
      input.open("temp.tmp");
      output.open("temp2.tmp");
      if (!input) {cout << "\nError opening input file temp.tmp\n"; return 1;}
      if (!output) if (!output) {cout << "\nError opening output file temp2.tmp\n"; return 1;}
      numdone=0;
      if (DoMacs(macros,defcnt,numdone,input,output,nest)) {return 1;}
      output.close();
      input.close();
      if (numdone > 0) {
         unlink("temp.tmp");
         rename("temp2.tmp","temp.tmp");
      }
      nest=true;
   } while (numdone>0);
   cout << '\n';

   return 0;
}

int CopyFile(char * infile, ofstream & output, unsigned long &defcnt, unsigned long &totlines, bool macfile) {
   int c,c2;
   unsigned long linecnt=0;
   char * pathfile;
   char instr[256], *filenamepass,stringinnage;
   bool inastring=false,willtolive;
   ifstream input(infile);
   if (!input) {
      pathfile=new char[strlen(asmpath)+strlen(infile)+1];
      for (c=0; c < strlen(asmpath); c++) {
         pathfile[c]=asmpath[c];
      }
      for (c=strlen(asmpath); c < strlen(asmpath)+strlen(infile); c++) {
         pathfile[c]=infile[c-strlen(asmpath)];
      }
      pathfile[c]=0;
      input.open(pathfile);
      delete [] pathfile;
      if (!input) {cout << "\nError opening input file " << infile << '\n'; return 1;}
   }

   output << "#f " << infile << '\n';
   output << "#l 0\n";
   if (macfile) output << "#m\n";

   while (!input.eof()) {
      linecnt++;
      totlines++;
      input.getline(instr,255);
      if (strlen(instr) >= 254) {return Errhandler(ERR_LINE_TOO_LONG,linecnt,infile,NULL);}
      willtolive=true;
      for (c=0; c < strlen(instr) && willtolive; c++) {
         if (instr[c] == ';' && !inastring) {
            instr[c] = 0;
            willtolive=false; // no point in reading any further
         } else if (instr[c] == stringinnage && inastring) {
            inastring = false;
         } else if (instr[c] == '\"'|| instr[c] == '\'' && !inastring) {
            inastring = true;
            stringinnage = instr[c];
         }
      }
      if (instr[0]=='#' && LowerCase(instr[1])=='i' &&
          ((LowerCase(instr[2])=='n' && LowerCase(instr[3])=='c' &&
          LowerCase(instr[4])=='l' && LowerCase(instr[5])=='u' &&
          LowerCase(instr[6])=='d' && LowerCase(instr[7])=='e') || 
          (LowerCase(instr[2])=='m' && LowerCase(instr[3])=='a' &&
          LowerCase(instr[4])=='c' && LowerCase(instr[5])=='r' &&
          LowerCase(instr[6])=='o' && LowerCase(instr[7])=='s')) &&
          instr[8]==' ') {
         for (c=9; instr[c]==' ' && c < strlen(instr); c++) {}
         if (instr[c]=='\"') {
            c++;
            for (c2=c; instr[c2]!='\"' && c2 < strlen(instr); c2++) {}
         } else if (instr[c]=='\'') {
            c++;
            for (c2=c; instr[c2]!='\'' && c2 < strlen(instr); c2++) {}
         } else {
            for (c2=c; instr[c2]!=' ' && c2 < strlen(instr); c2++) {}
         }
         instr[c2]=0;
         filenamepass=instr+c;
         if (!strcmp(filenamepass,infile)) {return Errhandler(ERR_NESTED_INCLUDE,linecnt,infile,NULL);}
         if (LowerCase(instr[2])=='m') {if (CopyFile(filenamepass,output,defcnt,totlines,true)) return 1;}
         else {if (CopyFile(filenamepass,output,defcnt,totlines,false)) return 1;}
         output << "#f " << infile << '\n';
         output << "#l " << linecnt << '\n';
         continue;
      } else if (instr[0]=='#' && LowerCase(instr[1])=='d' &&
          LowerCase(instr[2])=='e' && LowerCase(instr[3])=='f' &&
          LowerCase(instr[4])=='i' && LowerCase(instr[5])=='n' &&
          LowerCase(instr[6])=='e' && instr[7]==' ') {
         defcnt++;
      } else if (instr[0]=='#') {
         return Errhandler(ERR_ILLEGAL_DIRECTIVE,linecnt,infile,instr);
      }
      output << instr << '\n';
   }
   input.close();

   return 0;
}

int LoadMacs(Macro *macros, ifstream &input) {
   char instr[256],innerstr[256],asmfile[256];
   char ** parameters;
   unsigned int c,c2,c3,mc,cold,thislen,pc,cstrlen;
   int sc;
   unsigned long line=0;
   unsigned int maccount=0;
   streampos spos;
   bool willtolive,willtolive2,continuate;
   while (!input.eof()) {
      parameters=NULL;
      line++;
      input.getline(instr,255);
      
      if (instr[0]=='#') {
         switch (instr[1]) {
            case 'f':
               for (c=3; c < strlen(instr); c++) {
                  asmfile[c-3]=instr[c];
               }
               asmfile[c-3]=0;
               break;
            case 'l':
               line=0;
               for (c=3; c < strlen(instr); c++) {
                  line*=10;
                  line+=instr[c]-'0';
               }
               break;
            default:
              
      if (LowerCase(instr[1])=='d' &&
          LowerCase(instr[2])=='e' && LowerCase(instr[3])=='f' &&
          LowerCase(instr[4])=='i' && LowerCase(instr[5])=='n' &&
          LowerCase(instr[6])=='e' && instr[7]==' ') {
         for (c=8; c < strlen(instr) && instr[c] == ' '; c++) {}
         // find size of macro name
         for (c2=0; c+c2 < strlen(instr) && instr[c+c2] != ' ' && instr[c+c2] != '('; c2++) {}
         // allocate mem
         macros[maccount].name = new char[c2+1];
         if (!macros[maccount].name) {cout << "\nOut of memory.\n"; return 1;}
         for (c2=0; c+c2 < strlen(instr) && instr[c+c2] != ' ' && instr[c+c2] != '('; c2++) {
            macros[maccount].name[c2]=instr[c+c2];
         }
         macros[maccount].name[c2]=0;
         for (mc=0; mc < maccount; mc++) {
            if (!strcmp(macros[maccount].name,macros[mc].name)) {
               return Errhandler(ERR_ALREADY_DEFINED,line,asmfile,instr);
            }
         }
         
         macros[maccount].parnum=0;
         cold=c+c2;
         c=c+c2;
         if (instr[c] == '(') {
            #ifdef DEBUG
            cout << "\nCounting parameters for " << macros[maccount].name << '\n';
            #endif
            macros[maccount].parnum++;
            // count # of parameters
            willtolive=true;
            for (c=cold+1; willtolive && c < strlen(instr); c++) {
               switch (instr[c]) {
                  case ',':
                     macros[maccount].parnum++;
                     break;
                  case ')':
                     willtolive=false;
                     break;
                  default:
                     break;
               }
            }
            
            parameters = new char*[macros[maccount].parnum];
            if (!parameters) {cout << "\nOut of memory.\n"; return 1;}
            
            // count the length of each parameter
            c=cold+1;
            for (c2=0; c2 < macros[maccount].parnum; c2++) {
               willtolive=true;
               thislen=0;
               for (; willtolive && c < strlen(instr); c++) {
                  switch (instr[c]) {
                     case ' ':
                        break;
                     case ',':
                        willtolive=false;
                        break;
                     case ')':
                        willtolive=false;
                        break;
                     default:
                        thislen++;
                  }
               }
               if (thislen==0) {Errhandler(ERR_ZERO_LENGTH,line,asmfile,instr); return 1;}
               parameters[c2] = new char[thislen+1];
               if (!parameters[c2]) {cout << "\nOut of memory.\n"; return 1;}
            }
            // load each parameter into the array
            // parameters are case sens.
            c=cold+1;
            for (c2=0; c2 < macros[maccount].parnum; c2++) {
               willtolive=true;
               thislen=0;
               for (; willtolive && c < strlen(instr); c++) {
                  switch (instr[c]) {
                     case ' ':
                        break;
                     case ',':
                        willtolive=false;
                        break;
                     case ')':
                        willtolive=false;
                        break;
                     default:
                        parameters[c2][thislen]=instr[c];
                        thislen++;
                  }
               }
               parameters[c2][thislen]=0;
            }

         } // end of "if (instr[c] == '(')"
         
         for (; c < strlen(instr) && instr[c] == ' '; c++) {}
         macros[maccount].multiline=false;
         // find size of macro
         willtolive=true;
         thislen=0;
         strcpy(innerstr,instr);
         spos = input.rdbuf()->seekoff(0, ios::cur, ios::in);
         cold=c;
         willtolive=false;
         cstrlen=strlen(innerstr);
         for (sc=c; sc < cstrlen; sc++) {
            thislen++;
            if (innerstr[sc]=='\\') {
               macros[maccount].multiline=true;
               input.getline(innerstr,255);
               cstrlen=strlen(innerstr);
               sc=-1;
            }
         }
         input.rdbuf()->seekoff(spos,ios::beg,ios::in);
         
         // allocate mem for it
         macros[maccount].text = new char[thislen+1];
         
         if (!macros[maccount].text) {cout << "\nOut of memory.\n"; return 1;}
         willtolive=true;
         thislen=0;
         strcpy(innerstr,instr);
         c=cold;
         while (willtolive) {
            willtolive=false;
            for (c2=c; c2 < strlen(innerstr) && !willtolive; c2++) { 
               if (innerstr[c2]=='\\') {
                  macros[maccount].text[thislen]=0xa;
                  willtolive=true;                 
               } else {
                  willtolive2=true;
                  for (pc=0; (pc < macros[maccount].parnum) && willtolive2; pc++) {
                     if (CheckMac(innerstr,c2,parameters[pc],c3)) {
                        willtolive2=false;
                        macros[maccount].text[thislen]=(pc+1)|0x80;
                        c2=c3-1;
                     }
                  }
                  if (willtolive2) macros[maccount].text[thislen]=innerstr[c2];
               }
               thislen++;
            }
            if (willtolive) {
               line++;
               input.getline(innerstr,255);
               if (strlen(innerstr) > 254) return Errhandler(ERR_LINE_TOO_LONG,line,asmfile,NULL);
               c=0;
            }
         }
         
         for (pc=0; pc < macros[maccount].parnum; pc++) {
            delete [] parameters[pc];
            parameters[pc]=NULL;
         }
         if (macros[maccount].parnum > 0) {delete [] parameters; parameters=NULL;}

         macros[maccount].text[thislen] = 0;
         
         maccount++;
      } // end of if define
      } // end of big select
      } // end of if #
   } // end of while
   return 0;
}

int DoMacs(Macro * macros, unsigned long defcnt, int &numdone, ifstream &input, ofstream &output, bool nest) {
   unsigned long linenum=0;
   unsigned int c,c2=0,c3,c4,cold,mc,pc,thislen;
   char instr[256],asmfile[256];
   char **parameters;
   unsigned int parnum,parencount;
   bool macfound,macfound2,willtolive,linecount=true,macfile=false;

   while (!input.eof()) {
      if (linecount) linenum++;
      input.getline(instr,255);
      
      if (instr[0]==0) {
         if (!macfile) output << '\n';
      } else if (instr[0]=='#') {
      if (instr[1]=='m') {
         macfile=true;
         if (!nest) output << instr << '\n';
      }
      if (instr[1]=='f') {
         macfile=false;
         for (c=3; c < strlen(instr); c++) {
            asmfile[c-3]=instr[c];
         }
         asmfile[c-3]=0;
         output << instr << '\n';
      } else if (instr[1]=='e') {output << instr << '\n'; linecount=false;}
      else if (instr[1]=='s') {output << instr << '\n'; linecount=true;}
      else if (instr[1]=='l') {
         linenum=0;
         for (c=3; c < strlen(instr); c++) {
            linenum*=10;
            linenum+=instr[c]-'0';
         }
         output << instr << '\n';
      // take any #defines out of the output stream (define absorbtion)
      } else if (LowerCase(instr[1])=='d' &&
          LowerCase(instr[2])=='e' && LowerCase(instr[3])=='f' &&
          LowerCase(instr[4])=='i' && LowerCase(instr[5])=='n' &&
          LowerCase(instr[6])=='e' && instr[7]==' ') {
         output << '\n';
         willtolive=true;
         for (c=0; c < strlen(instr) && willtolive; c++) {if (instr[c]=='\\') willtolive=false;}
         while (instr[c-1]=='\\') {
            linenum++; input.getline(instr,255); output << '\n';
            willtolive=true;
            for (c=0; c < strlen(instr) && willtolive; c++) {if (instr[c]=='\\') willtolive=false;}
         }
      } else if (instr[1]=='t') {
         output << instr << '\n';
      } 
      } else { // end of if '#'
         if (!macfile) { // nothing gets echoed in a macro file
         
         // make an early pass to find if any macros are in the line
         macfound2=false;
         for (c=0; c < strlen(instr) && !macfound2; c++) {
            if (instr[c]==' ') continue;
            for (mc=0; mc < defcnt && !macfound2; mc++) {
               if (CheckMac(instr,c,macros[mc].name,c2)) macfound2=true;
            }
         }
         if (macfound2 && !nest) output << "#e\n";
         for (c=0; macfound2 && c < strlen(instr); c++) {
            if (instr[c]==' ') {output << ' '; continue;}
            macfound=false;
            for (mc=0; mc < defcnt && !macfound; mc++) {
               if (CheckMac(instr,c,macros[mc].name,c2)) {
                  macfound=true;
                  numdone++;
                  if (macros[mc].parnum==0) {
                     output << macros[mc].text;
                  } else {
                     if (instr[c2]=='(') {
            // BEGIN PARAMETER RECOGNITION
            // count # of parameters
            willtolive=true;
            parnum=1;
            parencount=1;
            cold=c2;
            for (c3=cold+1; willtolive && c3 < strlen(instr); c3++) {
               switch (instr[c3]) {
                  case ',':
                     if (parencount==1) parnum++;
                     break;
                  case '(':
                     parencount++;
                     break;
                  case ')':
                     parencount--;
                     if (parencount==0) willtolive=false;
                     break;
                  default:
                     break;
               }
            }
            if (parnum != macros[mc].parnum) {
               #ifdef DEBUG
               cout << "Should be " << macros[mc].parnum << ", is " << parnum << ".\n";
               #endif
               return Errhandler(ERR_NUM_PARAMETERS,linenum,asmfile,instr);
            }
            
            parameters = new char*[parnum];
            if (!parameters) {cout << "\nOut of memory.\n"; return 1;}
            // count the length of each parameter
            c3=cold+1;
            parencount=1;
            for (pc=0; pc < parnum; pc++) {
               willtolive=true;
               thislen=0;
               for (; willtolive && c3 < strlen(instr); c3++) {
                  switch (instr[c3]) {
                     //case ' ':
                     //   break;
                     case ',':
                        if (parencount==1) willtolive=false;
                        else thislen++;
                        break;
                     case '(':
                        parencount++;
                        thislen++;
                        break;
                     case ')':
                        parencount--;
                        if (parencount==0) willtolive=false;
                        else thislen++;
                        break;
                     default:
                        thislen++;
                  }
               }
               if (thislen==0) {Errhandler(ERR_ZERO_LENGTH,linenum,asmfile,instr); return 1;}
               parameters[pc] = new char[thislen+1];
               if (!parameters[pc]) {cout << "\nOut of memory.\n"; return 1;}
            }
            // load each parameter into the array
            // parameters are case sens.
            c3=cold+1;
            parencount=1;
            for (pc=0; pc < parnum; pc++) {
               willtolive=true;
               thislen=0;
               for (; willtolive && c3 < strlen(instr); c3++) {
                  switch (instr[c3]) {
                     //case ' ':
                     //   break;
                     case ',':
                        if (parencount==1) willtolive=false;
                        else {parameters[pc][thislen]=instr[c3]; thislen++;}
                        break;
                     case ')':
                        parencount--;
                        if (parencount==0) willtolive=false;
                        else {parameters[pc][thislen]=instr[c3]; thislen++;}
                        break;
                     case '(':
                        parencount++;
                        parameters[pc][thislen]=instr[c3];
                        thislen++;
                        break;
                     default:
                        parameters[pc][thislen]=instr[c3];
                        thislen++;
                  }
               }
               parameters[pc][thislen]=0;
            }
            c2=c3; // tell the above loop that the macro name ended where the
                   // parenthesis ended
            for (c3=0; c3 < strlen(macros[mc].text); c3++) {
               if (((unsigned char)macros[mc].text[c3]) > 0x80) {
                  output << parameters[(macros[mc].text[c3]&0x7f)-1];
               } else output << macros[mc].text[c3];
            }

            #ifdef DEBUG
            cout << "Called with parameters:\n";
            #endif
            for (pc=0; pc < parnum; pc++) {
               #ifdef DEBUG
               cout << parameters[pc] << '\n';
               #endif
               delete [] parameters[pc];
            }
            delete [] parameters;
            //END PARAMETER RECOGNTION
                     } else {return Errhandler(ERR_NUM_PARAMETERS,linenum,asmfile,instr);}
                  }
                  c=c2-1;
               }
            }
            if (!macfound) output << instr[c];
         }
         if (!macfound2) output << instr;
         output << '\n';
         if (macfound2 && !nest) output << "#s\n";
         }
      } // end of else '#'
   } // end of main while
   return 0;
}         

bool static CheckMac(char *array, unsigned int aoff, char *sstring, unsigned int &ic) {
   unsigned int c;
   ic=aoff;
   for (c=0; c < strlen(sstring); c++) {
      if (array[ic] != sstring[c]) return false;
      ic++;
   }
   return true;
}

// Telepancreasectomesis - The removal of a person's pancreas with
//                         one's mind without physical contact from a
//                         distance.


// Complifyificationnessitudeocitation - opposite of simplification   
