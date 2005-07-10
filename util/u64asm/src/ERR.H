// Error handling subroutine
// Reports to the user when there is an error, line number and file,
//  and the seriousness. If the error is recoverable (a 'warning'), it keeps
//  track of how many recoverable errors there have been, this is reported
//  at the end of assembly by main().

int Errhandler(int error, unsigned long line, char * filename, char * instr) {
   int errlev;
   switch (error) {
      case NO_ERROR:
         return NO_ERROR;
      case ERR_UNKNOWN_INSTRUCTION:
         errlev = FATAL_ERROR;  // can't just put a NOP there
         break;
      case ERR_TOO_MANY_OPERANDS:
         errlev = FATAL_ERROR;    // nothing is stored
         break;
      case ERR_UNEXPECTED_CHARS:
         errlev = FATAL_ERROR;          // crashes FindOps
         break;
      case ERR_LINE_TOO_LONG:
         errlev = FATAL_ERROR;          // unsure how input handles this
         break;                         // in different situations
      case ERR_MISSING_OPERAND:
         errlev = FATAL_ERROR;          // can't assemble an opcode without
         break;                         // an operand
      case ERR_COMPLEX_EXPRESSION:
         errlev = FATAL_ERROR;          // all sorts of recursiveness messed
         break;                         // up
      case ERR_NOT_ALLOWED_IN_DELAY_SLOT:
         errlev = FATAL_ERROR;          // because it won't run right
         break;
      case ERR_NOT_DEFINED:
         errlev = FATAL_ERROR;  // can't just make something up
         break;
      case ERR_BAD_OPERAND:
         errlev = FATAL_ERROR;  // don't want to squeeze something to fit
         break;
      case ERR_ALREADY_DEFINED:
         errlev = FATAL_ERROR;    // stops Assemble() before opcode
         break;
      case ERR_UNTERMINATED_STRING:
         errlev = FATAL_ERROR;    // It can't work
         break;
      case ERR_NOT_A_DIGIT:
         errlev = FATAL_ERROR;          // number is permanently messed up
         break;
      case ERR_EQU_NEEDS_LABEL:
         errlev = FATAL_ERROR;  // messes up otherwise
         break;
      case ERR_VALUE_TOO_BIG:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_NO_REG_MATH:
         errlev = FATAL_ERROR;
         break;
      case ERR_INDIRECTION:
         errlev = FATAL_ERROR;
         break;
      case ERR_UNCERTAIN:
         errlev = NO_ERROR;
         break;
      case ERR_FILE_ERROR:
         errlev = FATAL_ERROR;
         break;
      case ERR_ILLEGAL_DIRECTIVE:
         errlev = FATAL_ERROR;
         break;
      case ERR_HANDLED:
         errlev = FATAL_ERROR;
         break;
      case ERR_OUT_OF_MEMORY:
         errlev = FATAL_ERROR;
         break;
      case ERR_NESTED_INCLUDE:
         errlev = FATAL_ERROR;
         break;
      case ERR_NUM_PARAMETERS:
         errlev = FATAL_ERROR;
         break;
      case ERR_TOO_BIG_IMM:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_BRANCH_RANGE:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_OBJEND:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_NEST_OBJ:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_ORG_IN_OBJ:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_ASSERT_FAIL:
         errlev = FATAL_ERROR;
         break;
      case ERR_MACRO_NAME:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_ZERO_LENGTH:
         errlev = FATAL_ERROR;
         break;
      case ERR_WATCH_FOUND:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_ZERO_SIZE:
         errlev = RECOVERABLE_ERROR;
         break;
      case ERR_DBZ:
         errlev = FATAL_ERROR;
         break;
      case ERR_UNKNOWN_ERROR:
      default:
         errlev = FATAL_ERROR;  // the assembler itself is messed up
   }
   if (line > 0) {
      if (errlev == FATAL_ERROR) cout << "\nError " << filename << ' ' << line << ": ";
      if (errlev == RECOVERABLE_ERROR) {cout << "\nWarning " << filename << ' ' << line << ": "; errtot++;}
   } else {
      if (errlev == FATAL_ERROR) cout << "\nError: ";
      if (errlev == RECOVERABLE_ERROR) {cout << "\nWarning: "; errtot++;}
   }
   if (errlev == NO_ERROR) return NO_ERROR;
   
   switch (error) {
      case ERR_UNKNOWN_INSTRUCTION:
         cout << "Unknown instruction.\n";
         break;
      case ERR_TOO_MANY_OPERANDS:
         cout << "Too many parameters.\n";
         break;
      case ERR_UNEXPECTED_CHARS:
         cout << "Unexpected characters at end of line.\n";
         break;
      case ERR_LINE_TOO_LONG:
         cout << "Line too long.\n";
         break;
      case ERR_MISSING_OPERAND:
         cout << "Parameter expected.\n";
         break;
      case ERR_COMPLEX_EXPRESSION:
         cout << "Error in complex expression.\n";
         break;
      case ERR_NOT_ALLOWED_IN_DELAY_SLOT:
         cout << "Instruction not allowed in delay slot.\n";
         break;
      case ERR_NOT_DEFINED:
         cout << "Symbol not defined.\n";
         break;
      case ERR_BAD_OPERAND:
         cout << "Wrong type of parameter.\n";
         break;
      case ERR_ALREADY_DEFINED:
         cout << "Symbol already defined.\n";
         break;
      case ERR_UNTERMINATED_STRING:
         cout << "Unterminated string.\n";
         break;
      case ERR_NOT_A_DIGIT:
         cout << "Non-digit character found in numeric value.\n";
         break;
      case ERR_EQU_NEEDS_LABEL:
         cout << "EQU and EQUR need to be used with a label.\n";
         break;
      case ERR_VALUE_TOO_BIG:
         cout << "Value too big, 32-bit maximum.\n";
         break;
      case ERR_NO_REG_MATH:
         cout << "Math operations cannot be performed on registers.\n";
         break;
      case ERR_INDIRECTION:
         cout << "Syntax error in indirect addressing mode (reg).\n";
         break;
      case ERR_FILE_ERROR:      
         cout << "File not found.\n";
         break;
      case ERR_ILLEGAL_DIRECTIVE:
         cout << "Illegal directive.\n";
         break;
      case ERR_HANDLED:
         break;
      case ERR_OUT_OF_MEMORY:
         cout << "Out of memory.\n";
         break;
      case ERR_NESTED_INCLUDE:
         cout << "A file cannot be included from within itself.\n";
         break;
      case ERR_NUM_PARAMETERS:
         cout << "Wrong number of macro parameters.\n";
         break;
      case ERR_TOO_BIG_IMM:
         cout << "Immediate value is too large.\n";
         break;
      case ERR_BRANCH_RANGE:
         cout << "Branch out of range.\n";
         break;
      case ERR_OBJEND:
         cout << "OBJEND without OBJ.\n";
         break;
      case ERR_NEST_OBJ:
         cout << "Cannot nest OBJs.\n";
         break;
      case ERR_ORG_IN_OBJ:
         cout << "An ORG within an OBJ can cause unwanted results.\n";
         break;
      case ERR_ASSERT_FAIL:
         cout << "Assert failed.\n";
         break;
      case ERR_MACRO_NAME:
         cout << "Macro name contains another macro name.\n";
         break;
      case ERR_ZERO_LENGTH:
         cout << "Macro parameter cannot have zero length.\n";
         break;
      case ERR_WATCH_FOUND:
         cout << "PC == watchpoint.\n";
         break;
      case ERR_ZERO_SIZE:
         cout << "Attempt to make a zero sized string.\n";
         break;
      case ERR_DBZ:
         cout << "Divide by zero.\n";
         break;
      case ERR_UNKNOWN_ERROR:
      default:
         cout << "Unknown error #" << error << '\n';
   }
   if (instr) cout << instr << '\n';
   
   return errlev;
}

