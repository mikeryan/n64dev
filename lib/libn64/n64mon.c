/* 
 n64mon.c - N64 specific code needed by the C library 
 This is based on dvemon.c and pmon.S etc. and defines stuff for the n64
 and your rom emulator (open() could use the zip disk on z64 etc.)
 TODO: implement linux ROMFS or similar, open gets ptr to cart file,
 then read and write dma data to/from the cart
 */

#include <mips.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libn64.h"
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>

void runCartNo() {}
void sendByteNo(char c) {}
char recvByteNo() {return 0;}
int readNo(int fd,void *buf,size_t nbytes) {return 0;}
int writeNo(int fd,const void *buf,size_t nbytes) {return nbytes;}

cartemu debug;

void initDebug(debugType type)
{
  switch (type)
    {
    case NONE:
    default:
      // no debugging
      debug.runCart=runCartNo;
      debug.sendByte=sendByteNo;
      debug.recvByte=recvByteNo;
      debug.read=readNo;
      debug.write=writeNo;
      break;
    case ONSCREEN:
      // on screen
      break;
    case CD64_PROCOMM:
      // cd64
      cd64_init(&debug);
      break;
    case V64JR:
      // v64jr
    case UPDOWNLOADER:
      // valery pudovs updownloader
    }
}

#include <errno.h>
#undef errno
extern int errno;

/* inbyte and outbyte are used by the GDB code in newlib.
   outbyte is called by write.c
   inbyte is called by read.c
   FIXME: they dont work yet, read and write should call these
   but I send reads/writes straight to n64term instead, this works
   for newlib but not gdb
*/

/* outbyte -- send a byte to the PC */
int outbyte(unsigned char c)
{
  // #ifdef GDB output 0x0f prefix so that GDB won't discard the output. 
  return c;
}

/* inbyte -- read a byte from the PC */
unsigned char inbyte()
{
  //  return (unsigned char) debug.recvByte();
  return 0;
}

/* N64 RDRAM 4MB default or 8 MB with the expansion pack */
/* FIXME: this should return the proper size, but I don't have an
   expansion pack or any tech details on it 
   actually I think the boot code writes the mem size somewhere..
*/

#define RAMSIZE             (caddr_t)0x400000

/* Structure filled in by get_mem_info.  Only the size field is
      actually used (by sbrk), so the others aren't even filled in.  */

struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};

void
get_mem_info(struct s_mem *mem)
{
  mem->size = (int)RAMSIZE;
}

/* read  -- read bytes from a file descriptor */

int read(int fd,void *buf,size_t nbytes)
{
  int rx_bytes=0;

  if (fd==STDIN_FILENO)
    rx_bytes=debug.read(fd,buf,nbytes);
  // FIXME: else cartridge FS

  return rx_bytes;
}

/* write bytes to a file descriptor */
int write(int fd,const void *buf,size_t nbytes)
{
  int tx_bytes=0;

  if (fd==STDOUT_FILENO || fd==STDERR_FILENO)
    tx_bytes=debug.write(fd,buf,nbytes);
  // FIXME: else dma stuff from the cartridge FS

    return (tx_bytes);
}

/*
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 */
int open(char *buf,int flags,int mode)
{
  errno = EIO;
  return (-1);
}

/*
 * close -- close a file descriptor. We don't need
 *          to do anything, but pretend we did.
 */
int close(int fd)
{
  return -1;
}

/*
 * environ -- environment variables
 *
 */

char *__env[1] = { 0 };
char **environ = __env;

int execve(const char *name, char * const argv[], char * const env[]){
  errno=ENOMEM;
  return -1;
}

int fork() {
  errno=EAGAIN;
  return -1;
}

/*
 * sbrk -- changes heap size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */

void* sbrk(size_t incr) { 
  extern char _end; // from ld 
  static char *heap_end; 
  char *prev_heap_end; 
  u32 stack_ptr;
  
  if (heap_end == 0) { 
    heap_end = &_end; 
  } 
  prev_heap_end = heap_end;

  // it took ages to figure I'd run out of memory, if only I'd had this 
  // earlier...
  asm ("move %0,$sp\n":"=g" (stack_ptr):);
  if ((u32)heap_end + incr > stack_ptr)
    {
      print("Heap and stack collision\n");
      errno=ENOMEM;
      return ((caddr_t)-1);
    }
  
  heap_end += incr; 
  return (caddr_t) prev_heap_end; 
}

/* Status of a file (by name). Minimal implementation */

int stat(const char *file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

/* times Timing information for current process. Minimal implementation */
#include <sys/times.h>
clock_t times(struct tms *buf){
  return -1;
}

/* unlink Remove a file's directory entry. Minimal implementation */

int unlink(const char *name){
  errno=ENOENT;
  return -1; 
}

/* wait Wait for a child process. Minimal implementation */

int wait(int *status) {
  errno=ECHILD;
  return -1;
}

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes and return a 1.
 */
int isatty(int fd)
{
  return (1);
}

/*
 * lseek -- move read/write pointer. Since a serial port
 *          is non-seekable, we return an error.
 */
off_t lseek(int fd,off_t offset,int whence)
{
  return 0;
}

/*
 * fstat -- get status of a file.
 *
 */
#include <sys/stat.h>
int fstat(int fd,struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

/*
 * getpid -- only one process, so just return 1.
 */
#define __MYPID 1
int
getpid()
{
  return __MYPID;
}

/*
 * kill -- go out via exit...
 */
int kill(int pid,int sig)
{
  if(pid == __MYPID)
    _exit(sig);
  errno=EINVAL;
  return(-1);
}

int link(const char *old,const char *new)
{
  errno=EMLINK;
  return -1;
}

unsigned sleep(unsigned int seconds)
{
  int retraces,i;
  switch (tvType)
    {
    case TV_PAL:
      retraces=50*seconds;
      break;
    case TV_NTSC:
    default:
      retraces=60*seconds;
    }

  for (i=0;i<retraces/2;i++) // /2?
    {
    while (IO_READ(VI_CURRENT_REG)!=512); // wait for vertical retrace
    while (IO_READ(VI_CURRENT_REG)!=0);
    }
  return 0;
}

/* DEBUGGING FUNCTIONS */

/* print -- do a raw print of a string */ 
void print(char *ptr)
{
  write(1,ptr,strlen(ptr));
}

/* putnum -- print a 32 bit number in hex */
int putnum (unsigned int num)
{
  char  buffer[9];
  int   count;
  char  *bufptr = buffer;
  int   digit;
  
  for (count = 7 ; count >= 0 ; count--) {
    digit = (num >> (count * 4)) & 0xf;
    
    if (digit <= 9)
      *bufptr++ = (char) ('0' + digit);
    else
      *bufptr++ = (char) ('a' - 10 + digit);
  }

  *bufptr = (char) 0;
  print (buffer);
  return 0;
}

void send_hex_u32(u32 h)
{
  int i;
  u8 t;
  printf("0x");
  for (i=28;i>=0;i-=4) {
    t=(h>>i)&0x0F;
    if (t<=9) putchar(t+'0'); else putchar(t-10+'A');
  }
}

void dump_mem(u32 *ptr,int n)
{
  int i=0;
  for (i=1;(i+3)<=n;i+=4) {
    send_hex_u32((u32)&(ptr[i-1])); printf(": ");
    send_hex_u32(ptr[i-1]); putchar(' ');
    send_hex_u32(ptr[i]); putchar(' ');
    send_hex_u32(ptr[i+1]); putchar(' ');
    send_hex_u32(ptr[i+2]); putchar(' '); putchar('\n');  
  }
  if (i<=n) {
    send_hex_u32((u32)&(ptr[i-1])); printf(": ");
    while (i<=n) {
      send_hex_u32(ptr[i++]); putchar(' ');
    }
    putchar('\n');
  }
}


/* THE CODE BELOW FIXES A BUG IN BINUTILS 2.8.1, THIS DOESN'T APPEAR
   TO HAPPEN IN 2.9.1 :-) SO THIS IS COMMENTED OUT */

/* FIXME: c++ constructors redefined here so I don't have to change the code
   in newlib. This is all because ld uses 64 bit SQUADs for CONSTRUCTORS 
   entries in mips-elf. Newlib wants 32 bit pointers. Why doesn't ld detect
   that we are using 32 bit pointers?  */
/*
typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern void __do_global_dtors();

void
__do_global_ctors()
{
  // call global constructors from array of 64 bit function pointers
func_ptr nptrs=__CTOR_LIST__[1];
u32 i;
for (i = (u32)nptrs; i >= 1; i--)
  __CTOR_LIST__[(i*2)+1] ();
__CTOR_LIST__[1]=(func_ptr)0xffff;
}

// this is called automatically from main()

void
__main()
{
  // Support recursive calls to `main': run initializers just once. 
  static int initialized;
  if (!initialized)
    {
      initialized = 1;
      __do_global_ctors();
    }
}
*/


