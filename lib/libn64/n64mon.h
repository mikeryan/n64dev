#ifndef _DEBUGDECL_H
#define _DEBUGDECL_H

typedef struct cartemu {
  void (*runCart)();
  void (*sendByte)(char c);
  char (*recvByte)();
  int (*read)(int fd,void *buf,size_t nbytes);
  int (*write)(int fd,const void *buf,size_t nbytes);
} cartemu;

/* keep this a multiple of 4, align all variables with a 4 byte boundary.
   The n64 doesn't like reading across the end of a 16 byte boundary like
   0xb008d93e (u16 read) 
*/
#define FILENAME_LEN 24
typedef struct fs { // sizeof=2*16
  u8 filename[FILENAME_LEN];
  u32 length;
  u32 next_file;
} fs_struct;

typedef enum debugType { NONE,ONSCREEN,CD64_PROCOMM,V64JR,UPDOWNLOADER } debugType;

extern cartemu debug;

void initDebug(debugType type);
void print(char *string);
int putnum (unsigned int num);
void send_hex_u32(u32 h);
void dump_mem(u32 *ptr,int n);

#endif /* _DEBUGDECL_H */
