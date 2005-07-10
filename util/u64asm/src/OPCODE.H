// opcode.h: standardized instruction->opcode engine
//           Forgive me for using 'parameter' and 'operand' interchangably,
//           I usually mean parameter.

#define PTYPE_MASK      0xC0
#define PTYPE_INTEGER   0x00
#define PTYPE_REGISTER  0x40
#define PTYPE_IREGISTER 0x80
#define PTYPE_CREGISTER 0xC0

#define PSIZE_MASK      0x38
#define PSIZE_5         0x00
#define PSIZE_10        0x08
#define PSIZE_16        0x10
#define PSIZE_20        0x18
#define PSIZE_26        0x20

#define PLOC_MASK       0x07
#define PLOC_0          0x00
#define PLOC_6          0x01
#define PLOC_11         0x02
#define PLOC_16         0x03
#define PLOC_21         0x04

// Some common parameters
#define POFFSET         PSIZE_16|PLOC_0|PTYPE_INTEGER
#define PTARGET         PSIZE_26|PLOC_0|PTYPE_INTEGER
#define PIMMEDIATE      PSIZE_16|PLOC_0|PTYPE_INTEGER
#define PBASE           PSIZE_5|PLOC_21|PTYPE_IREGISTER
#define PRS             PSIZE_5|PLOC_21|PTYPE_REGISTER
#define PRT             PSIZE_5|PLOC_16|PTYPE_REGISTER
#define PRD             PSIZE_5|PLOC_11|PTYPE_REGISTER
#define PSA             PSIZE_5|PLOC_6|PTYPE_INTEGER
#define PFS             PSIZE_5|PLOC_11|PTYPE_CREGISTER
#define PFD             PSIZE_5|PLOC_6|PTYPE_CREGISTER
#define PFT             PSIZE_5|PLOC_16|PTYPE_CREGISTER
#define PCODE           PSIZE_20|PLOC_6|PTYPE_INTEGER
#define PSHORTCODE      PSIZE_10|PLOC_6|PTYPE_INTEGER
#define P_OP            PSIZE_5|PLOC_16|PTYPE_INTEGER

// Flags
#define FLS             0x01
#define FARITH          0x02
#define FBRK            0x04
#define FALLOC          0x08
#define FJMP            0x10
#define FBRANCH         0x20
#define FSPECIAL        0x40
#define FIMM            0x80

int db_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int dh_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int dw_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int dcb_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int dch_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int dcw_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int incbin_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int org_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int equ_fn(char *, int);
int equr_fn(char *, int);
int equne_fn(char *, int);
int li_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int la_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int move_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int obj_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int objend_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int report_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int offset_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int assert_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);
int watch_fn(char **, unsigned long *, int *, int, unsigned long &, bool, int);

struct instruction
{
   char * name;
   unsigned long base;
   int numparams;
   char paramtypes[3];
   char flags;
};

#define LOADSTORE(name,base)    {name,(unsigned long)base<<26,3,{PRT,POFFSET,PBASE},FLS}
#define ARITH(name,base)        {name,base,3,{PRD,PRS,PRT},FARITH}
#define IMMARITH(name,base)     {name,(unsigned long)base<<26,3,{PRT,PRS,PIMMEDIATE},FARITH|FIMM}
#define MULTDIV(name,base)      {name,base,2,{PRS,PRT},0}
#define SHIFT(name,base)        {name,base,3,{PRD,PRT,PSA},FARITH}
#define SHIFTV(name,base)       {name,base,3,{PRD,PRT,PRS},FARITH}
#define SETON(name,base)        {name,base,3,{PRD,PRS,PRT},0}
#define IMMSETON(name,base)     {name,(unsigned long)base<<26,3,{PRT,PRS,PIMMEDIATE},FIMM}
#define BRANCHEQ(name,base)     {name,(unsigned long)base<<26,3,{PRS,PRT,POFFSET},FIMM|FBRANCH}
#define BRANCH(name,base)       {name,base,2,{PRS,POFFSET},FIMM|FBRANCH}

#define NUMINSTRUCTIONS         124
const instruction instruct[NUMINSTRUCTIONS] = {
   LOADSTORE("lb ",32),LOADSTORE("lbu ",36),LOADSTORE("ld ",55),
   LOADSTORE("ldl ",26),LOADSTORE("ldr ",27),LOADSTORE("lh ",33),
   LOADSTORE("lhu ",37),LOADSTORE("ll ",48),LOADSTORE("lld ",52),
   LOADSTORE("lw ",35),LOADSTORE("lwl ",34),LOADSTORE("lwr ",38),
   LOADSTORE("lwu ",39),LOADSTORE("sb ",40),LOADSTORE("sc ",56),
   LOADSTORE("scd ",60),LOADSTORE("sd ",63),LOADSTORE("sdl ",44),
   LOADSTORE("sdr ",45),LOADSTORE("sh ",41),LOADSTORE("sw ",43),
   LOADSTORE("swl ",42),LOADSTORE("swr ",46),{"sync ",15,0,{0},0},
   ARITH("add ",32),IMMARITH("addi ",8),IMMARITH("addiu ",9),
   ARITH("addu ",33),ARITH("and ",36),IMMARITH("andi ",12),
   ARITH("dadd ",44),IMMARITH("daddi ",24),IMMARITH("daddiu ",25),
   ARITH("daddu ",45),MULTDIV("ddiv ",30),MULTDIV("div ",26),
   MULTDIV("divu ",27),MULTDIV("dmult ",28),MULTDIV("dmultu ",29),
   SHIFT("dsll ",56),SHIFT("dsll32 ",60),SHIFTV("dsllv ",20),
   SHIFT("dsra ",59),SHIFT("dsra32 ",63),SHIFTV("dsrav ",23),
   SHIFT("dsrl ",58),SHIFT("dsrl32 ",62),SHIFTV("dsrlv ",22),
   ARITH("dsub ",46),ARITH("dsubu ",47),{"lui ",(unsigned long)15<<26,2,{PRT,PIMMEDIATE},0}, // immediate removed because this will not be sign extended
   {"mfhi ",16,1,{PRD},0},{"mflo ",18,1,{PRD},0},{"mthi ",17,1,{PRS},0},
   {"mtlo ",19,1,{PRS},0},MULTDIV("mult ",24),MULTDIV("multu ",25),
   ARITH("nor ",39),ARITH("or ",37),IMMARITH("ori ",13),
   SHIFT("sll ",0),SHIFTV("sllv ",4),SETON("slt ",42),
   IMMSETON("slti ",10),IMMSETON("sltiu ",11),SETON("sltu ",43),
   SHIFT("sra ",3),SHIFTV("srav ",7),SHIFT("srl ",2),
   SHIFTV("srlv ",6),ARITH("sub ",34),ARITH("subu ",35), 
   ARITH("xor ",38),IMMARITH("xori ",14),       // 74 instructions
   BRANCHEQ("beq ",4),BRANCHEQ("beql ",20),
   BRANCH("bgez ",(unsigned long)1<<26|(unsigned long)1<<16),
   BRANCH("bgezal ",(unsigned long)1<<26|(unsigned long)17<<16),
   BRANCH("bgezall ",(unsigned long)1<<26|(unsigned long)19<<16),
   BRANCH("bgezl ",(unsigned long)1<<26|(unsigned long)3<<16),
   BRANCH("bgtz ",(unsigned long)7<<26),
   BRANCH("bgtzl ",(unsigned long)23<<26),
   BRANCH("blez ",(unsigned long)6<<26),
   BRANCH("blezl ",(unsigned long)22<<26),
   BRANCH("bltz ",(unsigned long)1<<26),
   BRANCH("bltzal ",(unsigned long)1<<26|(unsigned long)16<<16),
   BRANCH("bltzall ",(unsigned long)1<<26|(unsigned long)18<<16),
   BRANCH("bltzl ",(unsigned long)1<<26|(unsigned long)2<<16),
   BRANCHEQ("bne ",5),
   BRANCHEQ("bnel ",21),
   {"j ",(unsigned long)2<<26,1,{PTARGET},FJMP},
   {"jal ",(unsigned long)3<<26,1,{PTARGET},FJMP},
   {"jalr ",9,2,{PRS,PRD},0},
   {"jr ",8,1,{PRS},0},
   {"db ",0,0,{0},FSPECIAL|FALLOC},{"dh ",1,0,{0},FSPECIAL|FALLOC},
   {"dw ",2,0,{0},FSPECIAL|FALLOC},{"dcb ",3,2,{0},FSPECIAL},
   {"dch ",4,2,{0},FSPECIAL},{"dcw ",5,2,{0},FSPECIAL},
   {"incbin ",6,1,{0},FSPECIAL},{"org ",7,1,{0},FSPECIAL},
   {"nop ",0,0,{0},0},{"li ",8,2,{0},FSPECIAL},
   {"la ",9,2,{0},FSPECIAL},{"move ",10,2,{0},FSPECIAL},
   {"break ",13,1,{PCODE},FBRK},{"syscall ",12,1,{PCODE},FBRK},
   {"cache ",(unsigned long)47<<26,3,{P_OP,POFFSET,PBASE},0},
   {"eret ",(unsigned long)0x42000018,0,{0},0},
   {"mfc0 ",(unsigned long)0x10<<26,2,{PRT,PFS},0},
   {"mtc0 ",((unsigned long)4<<21)|((unsigned long)0x10<<26),2,{PRT,PFS},0},
   {"bnez ",(unsigned long)5<<26,2,{PRS,POFFSET},FBRANCH|FIMM},
   {"bnezl ",(unsigned long)21<<26,2,{PRS,POFFSET},FBRANCH|FIMM},
   {"beqz ",(unsigned long)4<<26,2,{PRS,POFFSET},FBRANCH|FIMM},
   {"beqzl ",(unsigned long)20<<26,2,{PRS,POFFSET},FBRANCH|FIMM},
   {"neg ",34,2,{PRD,PRT},FARITH},{"negu ",35,2,{PRD,PRT},FARITH},
   {"obj ",11,1,{0},FSPECIAL},{"objend ",12,0,{0},FSPECIAL},
   {"report ",13,0,{0},FSPECIAL},{"offset ",14,0,{0},FSPECIAL},
   {"assert ",15,1,{0},FSPECIAL},{"watch ",16,1,{0},FSPECIAL}
   };

int (*specialfcn[])(char **,unsigned long *,int *,int,unsigned long &,bool,int) = {
   db_fn,dh_fn,dw_fn,dcb_fn,dch_fn,dcw_fn,incbin_fn,org_fn,li_fn,la_fn,
   move_fn,obj_fn,objend_fn,report_fn,offset_fn,assert_fn,watch_fn};

int AsmInstr(unsigned long &pc, char * instr, int c, int outhandle) {
   unsigned long opcode;
   int icount,ic; // ic is the offset to start parameter searching at.
   int temp;
   bool found=false,watchfound=false;
   
   if (CheckEq(instr,c,"equ ",ic)) {
      return equ_fn(instr,ic);
   }
   if (CheckEq(instr,c,"equr ",ic)) {
      return equr_fn(instr,ic);
   }
   if (CheckEq(instr,c,"equne ",ic)) {
      return equne_fn(instr,ic);
   }

   for (icount=0; icount < NUMINSTRUCTIONS; icount++) {
      if (CheckEq(instr,c,instruct[icount].name,ic)) {
	 found=true;
	 break;
      }
   }
   if (!found) return ERR_UNKNOWN_INSTRUCTION;

   if (watchactive && pc==watchpoint) watchfound=true;
   
   char * pointers[64];
   unsigned long values[64];
   int types[64];
   int err;
   int numops;
   bool certain=true;
   err=FindOperands(instr,OperandOffset(instr,ic),pointers,values,types,numops,((instruct[icount].flags&FALLOC)?63:instruct[icount].numparams));
   if (err==ERR_UNCERTAIN) {
      if (!(instruct[icount].flags&FSPECIAL)) {pc+=4; return NO_ERROR;}
      certain=false;
   } else if (err) return err;
   if (instruct[icount].flags&FLS && numops < 3) {
     if (types[2] == DTYPE_IREGISTER) {
	types[3] = types[2];
	values[3] = values[2];
	types[2] = DTYPE_INTEGER;
	values[2] = 0;
	numops++;
     } else {
	types[3] = DTYPE_IREGISTER;
	values[3] = 0;
	numops++;
     }
   }
   if (instruct[icount].flags&FARITH && numops < instruct[icount].numparams) {
      if (instruct[icount].numparams==3) {
	 types[3] = types[2];
	 values[3] = values[2];
      }
      types[2] = types[1];
      values[2] = values[1];
      numops++;
   }
   
   opcode = instruct[icount].base;
   
   if (numops != instruct[icount].numparams && !(instruct[icount].flags&FALLOC) && !(instruct[icount].flags&FBRK && numops==0)) return ERR_MISSING_OPERAND;
   if (!(instruct[icount].flags&FSPECIAL)) {
   
   for (c=1; c < (numops+1); c++) {
      switch (instruct[icount].paramtypes[c-1]&PTYPE_MASK) {
	 case PTYPE_INTEGER:
	    if (types[c] != DTYPE_INTEGER) return ERR_BAD_OPERAND;
	    if (instruct[icount].flags&FIMM) {
	       if (instruct[icount].flags&FBRANCH) {
		  values[c] = (signed long)(values[c] - (pc+4))/4;
		  if (values[c] > 0x7fff && ((values[c]&0xffff8000) != 0xffff8000)) return ERR_BRANCH_RANGE;
	       } /* else {
		  if (values[c] > 0x7fff && ((values[c]&0xffff8000) != 0xffff8000)) return ERR_TOO_BIG_IMM;
	       } */
	    }
	    break;
	 case PTYPE_REGISTER:
	    if (types[c] != DTYPE_REGISTER) return ERR_BAD_OPERAND;
	    break;
	 case PTYPE_IREGISTER:
	    if (types[c] != DTYPE_IREGISTER) return ERR_BAD_OPERAND;
	    break;
	 case PTYPE_CREGISTER:
	    if (types[c] != DTYPE_CREGISTER) return ERR_BAD_OPERAND;
	    break;
	 default:
		return ERR_UNKNOWN_ERROR;
      }
      switch (instruct[icount].paramtypes[c-1]&PSIZE_MASK) {
	 case PSIZE_5:
	    values[c] &= 0x1f;
	    break;
	 case PSIZE_10:
	    values[c] &= 0x3ff;
	    break;
	 case PSIZE_16:
	    values[c] &= 0xffff;
	    break;
	 case PSIZE_20:
	    values[c] &= 0xfffff;
	    break;
	 case PSIZE_26:
	    if (instruct[icount].flags&FJMP) values[c] /= 4;
	    values[c] &= 0x3ffffff;
	    break;
	 default:
	    return ERR_UNKNOWN_ERROR;
      }
      switch (instruct[icount].paramtypes[c-1]&PLOC_MASK) {
	 case PLOC_0:
	    opcode |= values[c];
	    break;
	 case PLOC_6:
	    opcode |= values[c]<<6;
	    break;
	 case PLOC_11:
	    opcode |= values[c]<<11;
	    break;
	 case PLOC_16:
	    opcode |= values[c]<<16;
	    break;
	 case PLOC_21:
	    opcode |= values[c]<<21;
	    break;
	 default:
	    return ERR_UNKNOWN_ERROR;
      }
   }

   // put into big-endian format
   opcode = (opcode&0x000000ff)<<24 | (opcode&0x0000ff00)<<8 | (opcode&0x00ff0000)>>8 | (opcode&0xff000000)>>24;
   
   write(outhandle,&opcode,4);
   pc+=4; // doh!
   
   return watchfound?ERR_WATCH_FOUND:NO_ERROR;
   } else {
      temp=specialfcn[instruct[icount].base](pointers,values,types,numops,pc,certain,outhandle);
      if (temp) return temp;
      else if (watchfound) return ERR_WATCH_FOUND;
      return NO_ERROR;
   }
}

#define SHUTUP  pointers=pointers; values=values; types=types; numops=numops; pc=pc, certain=certain, outhandle=outhandle;

int db_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned int c;
   
   if (numops == 0) return ERR_MISSING_OPERAND;
   for (c=1; c <= numops; c++) {
      if (types[c]==DTYPE_STRING) {
	 if (pccertain && symbolcertain) write(outhandle,pointers[c],values[c]);
	 pc+=values[c];
      } else if (types[c]==DTYPE_INTEGER) {
	 if (pccertain && symbolcertain) write(outhandle,values+c,1);
	 pc++;
      } else return ERR_BAD_OPERAND;
   }
   SHUTUP
   return NO_ERROR;
}

// Halfword (2 bytes)
int dh_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned int c;
   
   if (numops == 0) return ERR_MISSING_OPERAND;
   for (c=1; c <= numops; c++) {
      if (types[c]==DTYPE_INTEGER) {
	 if (pccertain && symbolcertain) {
	    values[c] = ((values[c])&0xff00) >> 8 | ((values[c])&0x00ff) << 8;
	    write(outhandle,&(values[c]),2);
	 }
	 pc+=2;
      } else return ERR_BAD_OPERAND;
   }
   SHUTUP
   return NO_ERROR;
}

// Word (4 bytes)
int dw_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned int c;
		
   if (numops == 0) return ERR_MISSING_OPERAND;
   for (c=1; c <= numops; c++) {
      if (types[c]==DTYPE_INTEGER) {
	 if (pccertain && symbolcertain) {
	    values[c] = (values[c]&0x000000ff)<<24 | (values[c]&0x0000ff00)<<8 | (values[c]&0x00ff0000)>>8 | (values[c]&0xff000000)>>24;
	    write(outhandle,&(values[c]),4);
	 }
	 pc+=4;
      } else return ERR_BAD_OPERAND;
   }
   SHUTUP
   return NO_ERROR;
}

// Doubleword isn't possible right now, everything currently works with
// 4 bytes so 8 bytes is out of the question.

// Makes multiple bytes.  dcb <# bytes>, <byte value>
int dcb_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long c;
   
   if (types[1]==DTYPE_INTEGER && types[2]==DTYPE_INTEGER) {
      if (!certain) {
	 if (whenuncertain==1) {
	    pccertain=false;
	 } else {
	    pc+=values[1];
	 }
	 return NO_ERROR;
      }
      if (pccertain && symbolcertain) {
         for (c=0; c < values[1]; c++) {
	    write(outhandle, values+2, 1);
	 }
      }
      pc+=values[1];
      if (values[1]==0) return ERR_ZERO_SIZE;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int dch_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long c;
   
   if (types[1]==DTYPE_INTEGER && types[2]==DTYPE_INTEGER) {
      if (!certain) {
	 if (whenuncertain==1) {
	    pccertain=false;
	 } else {
	    pc+=values[1]*2;
	 }
	 return NO_ERROR;
      }
      if (pccertain && symbolcertain) {
	 for (c=0; c < values[1]; c++) {
	    write(outhandle,(char *)(values+2)+1,1);
	    write(outhandle,(char *)(values+2),1);
	 }
      }
      pc+=values[1]*2;
      if (values[1]==0) return ERR_ZERO_SIZE;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int dcw_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long c;
   
   if (types[1]==DTYPE_INTEGER && types[2]==DTYPE_INTEGER) {
      if (!certain) {
	 if (whenuncertain==1) {
	    pccertain=false;
	 } else {
	    pc+=values[1]*4;
	 }
	 return NO_ERROR;
      }
      if (pccertain && symbolcertain) {
	 for (c=0; c < values[1]; c++) {
	    write(outhandle,(char *)(values+2)+3,1);
	    write(outhandle,(char *)(values+2)+2,1);
	    write(outhandle,(char *)(values+2)+1,1);
	    write(outhandle,(char *)(values+2),1);
	 }
      }
      pc+=values[1]*4;
      if (values[1]==0) return ERR_ZERO_SIZE;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int incbin_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long c,size;
   int inhandle;
   char * pathfile;
   unsigned char * temp = NULL;
   unsigned char smalltemp;
   
   if (types[1]==DTYPE_STRING) {
      pointers[1][values[1]] = 0;
      inhandle = open(pointers[1], O_RDONLY|O_BINARY|O_EXCL, S_IREAD);
      if (inhandle <= 0) {
   
	 pathfile=new char[strlen(asmpath)+strlen(pointers[1])+1];
	 for (c=0; c < strlen(asmpath); c++) {
	    pathfile[c]=asmpath[c];
	 }
	 for (c=strlen(asmpath); c < strlen(asmpath)+strlen(pointers[1]); c++) {
	    pathfile[c]=pointers[1][c-strlen(asmpath)];
	 }
	 pathfile[c]=0;
	 inhandle = open(pathfile, O_RDONLY|O_BINARY|O_EXCL, S_IREAD);
	 delete [] pathfile;
	 if (inhandle <= 0) {return ERR_FILE_ERROR;}
      }
      
      size = lseek(inhandle,0,SEEK_END);
      lseek(inhandle,0,SEEK_SET);
      
      if (pccertain && symbolcertain) {
	 temp = new char [1024];
	 if (temp) {
	    for (c=1024; c <= size; c+=1024) {
	       read(inhandle,temp,1024);
	       write(outhandle,temp,1024);
	    }
	    if (c!=size) {
	       read(inhandle,temp,size-(c-1024));
	       write(outhandle,temp,size-(c-1024));
	    }
	    delete [] temp;
	 } else {
	    for (c=0; c < size; c++) {
	       read(inhandle,&smalltemp,1);
	       write(outhandle,&smalltemp,1);
	    }
	 }
      }
      pc+=size;

      close(inhandle);
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int org_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   pccertain=true;
   
   if (!certain) {pccertain=false; return NO_ERROR;}
   if (types[1]==DTYPE_INTEGER) {
      pc=values[1];
      if (isfirstorg) {firstorg=pc; isfirstorg=false;}
      if (inobj) return ERR_ORG_IN_OBJ;
      return NO_ERROR;
   }
   SHUTUP
   return ERR_BAD_OPERAND;
}

int equ_fn(char *instr, int firstbyte) {
   char * pointers[2];
   unsigned long values[2];
   int types[2];
   int numops;
   int err;

   int c,ic;
   bool equ_uncert=false;

   err=FindOperands(instr,firstbyte,pointers,values,types,numops,1);
   if (err==ERR_UNCERTAIN) {equ_uncert=true;}
   else if (err) return err;
   if (numops < 1) return ERR_MISSING_OPERAND;
   if (types[1]==DTYPE_INTEGER) {
      for (c=0; c < maxsymbols; c++) {
	 if (CheckEq(instr,0,SymbolList[c].name,ic)) break;
      }
      SymbolList[c].value = values[1];
      SymbolList[c].certain = !equ_uncert;
      SymbolList[c].export = true;
      return NO_ERROR;
   }
   
   return ERR_BAD_OPERAND;
}

int equr_fn(char *instr, int firstbyte) {
   char * pointers[2];
   unsigned long values[2];
   int types[2];
   int numops;
   int err;
   int c,ic;
   bool equr_uncert=false;

   err=FindOperands(instr,firstbyte,pointers,values,types,numops,1);
   if (err==ERR_UNCERTAIN) {equr_uncert=true;}
   else if (err) return err;
   if (numops < 1) return ERR_MISSING_OPERAND;
   if (types[1]==DTYPE_REGISTER || types[1]==DTYPE_CREGISTER || equr_uncert) {
      for (c=0; c < maxsymbols; c++) {
	 if (CheckEq(instr,0,SymbolList[c].name,ic)) break;
      }
      SymbolList[c].value = values[1];
      SymbolList[c].certain = !equr_uncert;
      SymbolList[c].type = types[1];
      SymbolList[c].export = false; // equrs aren't exported
      return NO_ERROR;
   }
   
   return ERR_BAD_OPERAND;
}

// ne=no export, keeps these declarations out of headers
int equne_fn(char *instr, int firstbyte) {
   char * pointers[2];
   unsigned long values[2];
   int types[2];
   int numops;
   int err;

   int c,ic;
   bool equ_uncert=false;

   err=FindOperands(instr,firstbyte,pointers,values,types,numops,1);
   if (err==ERR_UNCERTAIN) {equ_uncert=true;}
   else if (err) return err;
   if (numops < 1) return ERR_MISSING_OPERAND;
   if (types[1]==DTYPE_INTEGER) {
      for (c=0; c < maxsymbols; c++) {
	 if (CheckEq(instr,0,SymbolList[c].name,ic)) break;
      }
      SymbolList[c].value = values[1];
      SymbolList[c].certain = !equ_uncert;
      SymbolList[c].export = false;
      return NO_ERROR;
   }
   
   return ERR_BAD_OPERAND;
}

// Used for 16-bit immediate values (halfwords)
// or something that fits into one instruction
int li_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long opcode,imm,reg;
   
   if (!symbolcertain || (types[1]==DTYPE_REGISTER && types[2]==DTYPE_INTEGER)) {
      if (pccertain && symbolcertain) {
	 imm=values[2];
	 reg=values[1];
	 if (imm < 0x10000) {
	    
	    if (imm & 0x8000) {
	       opcode=((unsigned long)13 << 26) | (reg << 16) | imm;
	    } else {
	       opcode=((unsigned long)9 << 26) | (reg << 16) | imm;
	    }
	    
	    //opcode=((unsigned long)13 << 26) | (reg << 16) |imm;
	 } else {
	    /*
	    if ((imm & 0xffff8000) != 0xffff8000) {
	       return ERR_TOO_BIG_IMM;
	    } else {
	    */
	       // otherwise its just a signed value
	       opcode=((unsigned long)9 << 26) | (reg << 16) | (imm & 0xffff);
	    //}
	 }
	 opcode = (opcode&0x000000ff)<<24 | (opcode&0x0000ff00)<<8 | (opcode&0x00ff0000)>>8 | (opcode&0xff000000)>>24;
	 write(outhandle,&opcode,4);
      }
      pc+=4;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

// Used for 32-bit immediate values (words)
int la_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long opcode=(unsigned long)15 << 26;
   
   if (!symbolcertain || (types[1]==DTYPE_REGISTER && types[2]==DTYPE_INTEGER)) {
      if (pccertain && symbolcertain) {
	 opcode|=((values[2]&0xFFFF0000) >> 16) | (values[1] << 16);
	 opcode = (opcode&0x000000ff)<<24 | (opcode&0x0000ff00)<<8 | (opcode&0x00ff0000)>>8 | (opcode&0xff000000)>>24;
	 write(outhandle,&opcode,4);
	 opcode=(unsigned long)13 << 26;
	 opcode|=(values[2]&0xFFFF) | (values[1] << 16) | (values[1] << 21);
	 opcode = (opcode&0x000000ff)<<24 | (opcode&0x0000ff00)<<8 | (opcode&0x00ff0000)>>8 | (opcode&0xff000000)>>24;
	 write(outhandle,&opcode,4);
      }
      pc+=8;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int move_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   unsigned long opcode=32;
   
   if (!symbolcertain || (types[1]==DTYPE_REGISTER && types[2]==DTYPE_REGISTER)) {
      if (pccertain && symbolcertain) {
	 opcode|=(values[1] << 11) | (values[2] << 16);
	 opcode = (opcode&0x000000ff)<<24 | (opcode&0x0000ff00)<<8 | (opcode&0x00ff0000)>>8 | (opcode&0xff000000)>>24;
	 write(outhandle,&opcode,4);
      }
      pc+=4;
   } else return ERR_BAD_OPERAND;
   SHUTUP
   return NO_ERROR;
}

int obj_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   objcert=pccertain;
   pccertain=true;
   
   if (inobj) return ERR_NEST_OBJ;
   if (!certain) {pccertain=false; return NO_ERROR;}
   if (types[1]==DTYPE_INTEGER) {
      objstart=pc;
      pc=values[1];
      objinit=pc;
      inobj=true;
      return NO_ERROR;
   }
   SHUTUP
   return ERR_BAD_OPERAND;
}
int objend_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   pccertain=objcert;
   
   if (!inobj) return ERR_OBJEND;
   pc=(pc-objinit)+objstart;
   inobj=false;
   SHUTUP
   return NO_ERROR;
}

int report_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   if (pccertain && symbolcertain) {
      cout.flags(ios::hex|ios::showbase);
      cout << "PC = " << pc << ".\n";
      cout.flags(ios::dec);
   }
   SHUTUP
   return NO_ERROR;
}
int offset_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   if (pccertain && symbolcertain) {
      cout.flags(ios::hex|ios::showbase);
      cout << "Offset = " << lseek(outhandle,0,SEEK_CUR) << ".\n";
      cout.flags(ios::dec);
   }
   SHUTUP
   return NO_ERROR;
}
int assert_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   if (pccertain && symbolcertain) {
      if (types[1]==DTYPE_INTEGER) {
	 if (pc!=values[1]) {
	    cout.flags(ios::hex|ios::showbase);
	    cout << "PC = " << pc << ".\n";
	    cout.flags(ios::dec);
	    return ERR_ASSERT_FAIL;
	 }
      } else return ERR_BAD_OPERAND;
   }
   SHUTUP
   return NO_ERROR;
}
int watch_fn(char ** pointers, unsigned long * values, int * types, int numops, unsigned long &pc, bool certain, int outhandle) {
   if (pccertain && symbolcertain) {
      if (types[1]==DTYPE_INTEGER) {
	 watchpoint=values[1];
	 watchactive=true;
      } else return ERR_BAD_OPERAND;
   }
   SHUTUP
   return NO_ERROR;
}
