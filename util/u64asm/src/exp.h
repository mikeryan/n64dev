// Expression evaluation

unsigned long Evaluate(char * s, int &c) {
   unsigned long total = 0;
   unsigned long op2,op3;
   signed long c2;
   
   total = EvOp(s,c);

   while (c < strlen(s)) {
     
      switch (s[c]) {
         case ' ':
            c++;
            continue;   // from beginning of while loop
         case ',':
            return total;
         case '+':
            c++;
            op2 = EvOp(s,c);
            total += op2;
            break;
         case '-':
            c++;
            op2 = EvOp(s,c);
            total -= op2;
            break;
         case '*':
            c++;
            op2 = EvOp(s,c);
            total *= op2;
            break;
         case '/':
            c++;
            op2 = EvOp(s,c);
            if (op2 == 0) {ExpError=true; ExpErrorV=ERR_DBZ; return 0;}
            total /= op2;
            break;
         case '%':      // overloaded
            c++;
            op2 = EvOp(s,c);
            total %= op2;
            break;
         case '|':
            c++;
            op2 = EvOp(s,c);
            total |= op2;
            break;
         case '&':
            c++;
            op2 = EvOp(s,c);
            total &= op2;
            break;
         case '!':
            c++;
            op2 = EvOp(s,c);
            total ^= op2;
            break;
         case '^':
            c++;
            op2 = EvOp(s,c);
            op3 = 1;
            if (((signed long)op2) < 0) {
               if (total == 0) {ExpError=true; return 0;}
               for (c2=0; c2 > ((signed long)op2); c2--) {
                  op3/=total;
               }
            } else {
               for (c2=0; c2 < op2; c2++) {
                  op3*=total;
               }
            }
            total=op3;
            break;
         case '@':
            c++;
            op2 = EvOp(s,c);
            if (((signed long)op2)<=0) {ExpError=true; return 0;}
            if (((signed long)total)<=0) {ExpError=true; return 0;}
            total=Root(total,op2);
            break;
         case '<':
            c++;
            op2 = EvOp(s,c);
            total = total << op2;
            break;
         case '>':
            c++;
            op2 = EvOp(s,c);
            total = total >> op2;
            break;
         case ')':
            return total;
         case '(':
            return total;
         default:
            ExpError=true;
            return 0;
      }
      if (symbolcertain && IsRegister) {ExpErrorV=ERR_NO_REG_MATH; ExpError=true; return 0;}
   }
   return total;
}

// Set, setsize, character in question
int InSet(char * set, int setsize, char ciq) {
   int c;
   for (c=0; c < setsize; c++) {if (LowerCase(ciq)==set[c]) return c;}
   return c;
}

// Evaluates a single operand
// Unary operators (-, ~) are covered here
unsigned long EvOp(char * s, int &c) {
   // Valid characters for an operand
   unsigned long total;
   int d;
   int c2;
   
   bool negflag=false, notflag=false, stillsearching=true;
   while (stillsearching && c < strlen(s)) {
      switch (s[c]) {
         case '-':
            negflag = !negflag;
            c++;
            break;
         case '~':
            notflag = !notflag;
            c++;
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
         case '%':
         case '$':
         case '#':
            total = EvInt(s,c);
            stillsearching=false;
            break;
         case '(':
            c++;
            total = Evaluate(s,c);
            stillsearching=false;
            if (s[c] != ')') ExpError=true;
            if (IsRegister) IsIndex=true;
            c++;
            break;
         case ' ':      // ignore spaces
            c++;
            break;
         default:
            if (s[c]==')') {ExpError=true; return 0;}
            if (GetSymbol(s,c,c2)) {ExpErrorV=ERR_NOT_DEFINED; ExpError=true; return 0;}
            c++;
            total = SymbolList[c2].value;
            if (SymbolList[c2].type == DTYPE_REGISTER) IsRegister=true;
            if (SymbolList[c2].type == DTYPE_CREGISTER) {IsRegister=true; IsCOPRegister=true;}
            stillsearching=false;
            break;
      }
   }
   if (stillsearching) ExpError=true;
   return notflag?~(negflag?-total:total):(negflag?-total:total);
}

// Evaluates an integer in decimal, hex ($), bin (%), or octal (#)
unsigned long EvInt(char * s, int &c) {
   int base=10,d;
   char charset[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
   //bool neg=false;
   unsigned long total = 0, ltotal=0;

   if (s[c] == '$') {base = 16; c++;}
   else if (s[c] == '%') {base = 2; c++;}
   else if (s[c] == '#') {base = 8; c++;}
   else if (s[c] == '0' && s[c+1] == 'x') {base = 16; c+=2;} // 0x10 and such
   
   if (InSet(charset, base, s[c]) == base) {ExpErrorV=ERR_NOT_A_DIGIT; ExpError=true; return 0;}
   for (; c < strlen(s); c++) {
      if ((d=InSet(charset, base, s[c])) == base) return total;
      ltotal=total;
      total=total*base+d;
      if (ltotal > total) {ExpErrorV=ERR_VALUE_TOO_BIG; ExpError=true;}
   }
   return total;
}

unsigned long DivRep(unsigned long divisor, unsigned long dividend, unsigned long times) {
   for (unsigned long c=0; c < times; c++) {divisor /= dividend;}
   return divisor;
}

unsigned long Root(unsigned long n, unsigned long groot) {
   unsigned long root,err,olderr;

   root=n/2;
   err = (root-DivRep(n,root,groot-1));
   do {
      root -= err/groot;
      olderr=err;
      err = (root-DivRep(n,root,groot-1));
   } while (olderr != err && err != 0);
   return root;
}
