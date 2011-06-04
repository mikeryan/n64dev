// Level 1 error types
#define NO_ERROR                0       // there was no error
#define RECOVERABLE_ERROR       1       // aka a warning
#define FATAL_ERROR             2       // a real error, assembly must halt
#define INCLUDE_ERROR           3       // not an error at all, just a
                                        // signal from Assemble()

// Level 2 error types, more specific about the error. Errhandler decides
// if each of these is a fatal error and prints the error message.
#define ERR_UNKNOWN_INSTRUCTION         1
#define ERR_TOO_MANY_OPERANDS           2
#define ERR_UNEXPECTED_CHARS            3
#define ERR_LINE_TOO_LONG               4
#define ERR_MISSING_OPERAND             5
#define ERR_COMPLEX_EXPRESSION          6
#define ERR_NOT_DEFINED                 7
#define ERR_NOT_ALLOWED_IN_DELAY_SLOT   8
#define ERR_BAD_OPERAND                 9
#define ERR_ALREADY_DEFINED             10
#define ERR_UNTERMINATED_STRING         11
#define ERR_NOT_A_DIGIT                 12
//#define ERR_TOO_MANY_SYMBOLS            13
#define ERR_EQU_NEEDS_LABEL             14
//#define ERR_INCLUDE                     15      // not an actual error!
#define ERR_VALUE_TOO_BIG               16
#define ERR_NO_REG_MATH                 17
#define ERR_INDIRECTION                 18
#define ERR_UNCERTAIN                   19
#define ERR_IRRESOLVABLE_UNCERTAINTY    20
#define ERR_FILE_ERROR                  21
#define ERR_ILLEGAL_DIRECTIVE           22
#define ERR_HANDLED                     23 // already reported
#define ERR_OUT_OF_MEMORY               24
#define ERR_NESTED_INCLUDE              25
#define ERR_NUM_PARAMETERS              26
#define ERR_TOO_BIG_IMM                 27
#define ERR_BRANCH_RANGE                28
#define ERR_OBJEND                      29
#define ERR_NEST_OBJ                    30
#define ERR_ORG_IN_OBJ                  31
#define ERR_ASSERT_FAIL                 32
#define ERR_MACRO_NAME                  33
#define ERR_ZERO_LENGTH                 34
#define ERR_WATCH_FOUND                 35
#define ERR_ZERO_SIZE                   36
#define ERR_DBZ                         37
#define ERR_UNKNOWN_ERROR               0xFF

#define DTYPE_INTEGER   0
#define DTYPE_STRING    1
#define DTYPE_REGISTER  2
#define DTYPE_IREGISTER 3 // index register
#define DTYPE_CREGISTER 4
