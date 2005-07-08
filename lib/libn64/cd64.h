/* cd64.h - CD64 specific functions */

extern void cd64_runcart();
extern void cd64_init();
extern void cd64_sendbyte(char c);
extern char cd64_recvbyte();
extern void cd64_pc_connect();
extern void cd64_mode(int mode);
int cd64_read(int fd,void *buf,size_t nbytes);
int cd64_write(int fd,const void *buf,size_t nbytes);
