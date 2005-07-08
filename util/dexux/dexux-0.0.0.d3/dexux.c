/* Dexux
 *
 * Copyright (C) 2000 William Ono <wmono@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/* Information found on http://www.easysw.com/~mike/serial/ was
 * invaluable in the production of this code.  Thanks to Michael Sweet.
 */

/* Portmon by Mark Russinovich, found on http://www.sysinternals.com,
 * was used to watch the serial port operations between the DexDrive and
 * the DexPlorer software.
 */

#include <stdio.h> /* Standard i/o definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <string.h> /* String function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <sys/stat.h>

#define TTYDEVICE "/dev/ttyS1"
#define STANDARDTIMEOUT 10  /* in 1/10 seconds */

int open_port(void); /* Create a file descriptor for the serial port */
int port_config(int fd); /* Set up the port's baud rate, etc. */
int wake_dex(int fd); /* Send initialization codes to the DexDrive */
int toggle_modem_lines(int fd); /* Toggle RTS+DTR lines for wake_dex() */
int clear_buffer(int fd); /* Read in and discard all waiting bytes */
int card_status(int fd); /* Check for card presence */
int open_card(int fd); /* Read in card metadata */
int close_port(int fd); /* Put DexDrive to sleep and close the fd */
int get_chunk(int fd, char *address, char *data); /* Get card data */
int dump_toc(int fd); /* Temporary function: Dump Block 0 to disk */
int set_read_timeout(int fd, int length); /* Change timeout */
int dump_block1_header(int fd); /* Temporary function: Dump Block 1 to disk */
int dump_block2_header(int fd); /* Temporary function: Dump Block 2 to disk */
int chunk_top(long chunkaddress); /* Get top half of chunkaddress */
int chunk_bottom(long chunkaddress); /* Get bottom half of chunkaddress */
int get_block(int fd, int blocknumber, char *blockdata); /* Recursively call get_chunk() to get a whole block */ 
int binary_reverse(int inbyte); /* Reverse the 8 digits of inbyte */
int mass_xor(char *instring, long stringlength); /* XOR all bytes together */
int copy_block_todisk(int fd, int blocknumber); /* Writes a block (and its header) to disk */
int copy_block_tocard(int fd, int blocknumber); /* Writes a block (and its header) to the card */
int write_chunk(int fd, char *address, char *data); /* Write a chunk of data */
int write_block(int fd, int blocknumber, char *blockdata); /* Recursively call write_chunk() to write a whole block */ 

int main(void) {
  int fd;

  printf("Dexux: DexDrive software for Linux.\n");

  printf("main: Opening %s...\n", TTYDEVICE);
  fd = open_port();
  
  printf("main: Configuring %s...\n", TTYDEVICE);
  port_config(fd);

  printf("main: Waking up the DexDrive...\n");
  if (wake_dex(fd)) {
	  printf("main: wake_dex() did not complete.  Bailing out!\n");
	  return(1);
  }

  printf("main: The green light should be illuminated.\n");

  printf("main: Waiting for a card...\n");
  while(card_status(fd)) {usleep(50000);}
  printf("main: I guess we have a card.\n");

//  printf("main: dumping block 0...\n");
//  dump_toc(fd);
  
//  printf("main: dumping block 1 header...\n");
//  dump_block1_header(fd);

//  printf("main: dumping block 2 header...\n");
//  dump_block2_header(fd);

  /****************************************************************************
   * Enable copy_block_todisk() to write Block 0 header and data to
   * the file dexux-copy_block in the current directory.  Warning: This
   * function will simply overwrite any existing file of that name.
   * 
   */

  //printf("main: copying header and data for block 1 to disk...\n");
  //copy_block_todisk(fd, 1);

  /****************************************************************************
   * Enable copy_block_tocard() to write Block 0 header and data to
   * the memory card in the DexDrive from the file dexux-copy_block
   * in the current directory.  Warning: This function will overwrite
   * Block 0 with whatever it manages to find in the file.  If the file
   * doesn't exist or is too short, unexpected things may happen.
   *
   */

  //printf("main: copying header and data for block 1 to card...\n");
  //copy_block_tocard(fd, 1);

  printf("main: Putting the DexDrive to sleep...\n");
  close_port(fd);

  return(0);
}

int open_port(void) {
  int fd; /* File descriptor for the port */
  fd = open(TTYDEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) 
    perror("open_port: unable to open TTYDEVICE.\n");
  else
    fcntl(fd, F_SETFL, 0);  /* Set fd's flags. See fcntl(2) */
  return(fd);
}

int port_config(int fd) {
  struct termios options;
  tcgetattr(fd, &options);

  cfsetispeed(&options, B38400);
  cfsetospeed(&options, B38400);
  options.c_cflag |= (CLOCAL | CREAD);
  
  options.c_cflag &= ~CSIZE;  /* Mask out character size */
  options.c_cflag |= CS8;     /* and then set it to 8 bits */

  options.c_cflag &= ~PARENB;  /* Remove parity bit enabled flag */
  options.c_cflag &= ~CSTOPB;  /* Remove "2 stop bits" flag */

  options.c_cflag &= ~CRTSCTS; /* Disable hw flow control */

  options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
    /* Disable: Canonical input, local echo, backspace -> BS space BS,
     * and SIGINTR, SIGSUSP, SIGDSUSP, SIGQUIT signals
     */

  options.c_iflag &= ~INPCK;  /* Disable parity check */
  options.c_iflag |= IGNPAR;  /* Ignore parity errors */
  options.c_iflag &= ~PARMRK; /* Do not mark parity errors */
    /* PARMRK causes parity errors to be 'marked' in the input stream using
     * special characters. If IGNPAR is enabled, a NUL character (000 octal)
     * is sent to your program before every character with a parity error.
     * Otherwise, a DEL (177 octal) and NUL character is sent along with
     * the bad character.
     *   -- http://www.easysw.com/~mike/serial/
     */

  options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow ctl */

  options.c_oflag &= ~OPOST; /* Do not process (mangle) output */

  options.c_cc[VTIME] = STANDARDTIMEOUT;  /* Timeout in 1/10 seconds on reads */

  return(tcsetattr(fd, TCSAFLUSH, &options));
}



int toggle_modem_lines(int fd) {
  int port_status;
  int temp;

  ioctl(fd, TIOCMGET, &port_status);
  port_status &= ~0x4000;

  usleep(100);
  port_status |= (TIOCM_RTS|TIOCM_DTR);
  ioctl(fd, TIOCMSET, &port_status);     /* Turn on RTS,DTR */
  ioctl(fd, TIOCMGET, &temp); printf("port status: addr %p, is %d, set to %d\n",&port_status,temp,port_status);

  usleep(100);
  port_status &= ~(TIOCM_RTS|TIOCM_DTR);
  ioctl(fd, TIOCMSET, &port_status);    /* Turn off RTS,DTR */
  ioctl(fd, TIOCMGET, &temp); printf("port status: addr %p, is %d, set to %d\n",&port_status,temp,port_status);

  usleep(100);
  port_status |= (TIOCM_RTS|TIOCM_DTR);
  ioctl(fd, TIOCMSET, &port_status);     /* Turn on RTS,DTR */
  ioctl(fd, TIOCMGET, &temp); printf("port status: addr %p, is %d, set to %d\n",&port_status,temp,port_status);

  usleep(100000);
  return(0);
}


int wake_dex(int fd) {
  char buffer[255];
  int readlength;
  int i; /* Loop counter */

  //toggle_modem_lines(fd);

  write(fd, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 32);

  readlength = read(fd, &buffer, 4);
  buffer[readlength]='\0';
  if (strncmp(buffer, "IAI!", 4)) {
	  fprintf(stderr, "wake_dex: I wanted to hear IAI! but the DexDrive said: %s\n",buffer);
	  return(1);
  }

printf("survived 32 Xs\n");
  
  write(fd, "IAI\x00\x10\x29\x23\xBE\x84\xE1\x6C\xD6\xAE\x52\x90\x49\xF1\xF1\xBB\xE9\xEB", 21);

  readlength = read(fd, &buffer, 9);
  buffer[readlength]='\0';
  if (strncmp(buffer, "IAI\x40\x1B\x50\x53\x58\x46",9)) {
	  fprintf(stderr, "wake_dex: I wanted to hear IAI\\x40\\x1B\\x50\\x53\\x58\\x46 but the DexDrive said: %s\n",buffer);
	  return(1);
  }

printf("after long arbitrary string exchange\n");
  
  for (i=0; i<10; i++) {
    write(fd, "IAI\x27",4);
    usleep(1000);
  }
  clear_buffer(fd);

//  write(fd, "IAI\x01",4);
//  readlength = read(fd, &buffer, 254);
//  buffer[readlength]='\0';
//  if (strncmp(buffer, "IAI\x22",4)) {
//	  fprintf(stderr, "wake_dex: I wanted to hear IAI\x22 but the DexDrive said: %s\n",buffer);
//	  return(1);
//  }
//printf("survived idle\n");
  
  write(fd, "IAI\x07\x01",5);
  readlength = read(fd, &buffer, 254);
  buffer[readlength]='\0';
  if (strncmp(buffer, "IAI",3)) {
	  fprintf(stderr, "wake_dex: I wanted to hear IAI but the DexDrive said: %s\n",buffer);
	  return(1);
  }
printf("survived real wakeup\n");
  
//  write(fd, "IAI\x02\x00\x00",6);
//  readlength = read(fd, &buffer, 254);
//  buffer[readlength]='\0';
//  if (strncmp(buffer, "IAI\x22",4)) {
//	  fprintf(stderr, "wake_dex: I wanted to hear IAI\x22 but the DexDrive said: %s\n",buffer);
//	  return(1);
//  }
  
  clear_buffer(fd);
  printf("All's well!\n");
  return(0);
} 


int clear_buffer(int fd) {
  char buffer[255];
  int available, discard, readlength;
  
  ioctl(fd, FIONREAD, &available);

  while (available > 0) {
    if (available>254) {discard=254;} else {discard=available;}
    readlength = read(fd, &buffer, discard);
    fprintf(stderr,"clear_buffer: saw %d bytes, threw out %d bytes\n",available,readlength);
    available -= readlength;
  }
  return(readlength);
}


int card_status(int fd) {  /* returns: 0 = card, 1 = no card, 2 = uh oh! */
  char buffer[255];
  int readlength;

  clear_buffer(fd);
  write(fd, "IAI\x01",4);

  readlength = read(fd, &buffer, 254);
  buffer[readlength]='\0';

  if (!strncmp(buffer, "IAI\x22",readlength)) {
	  fprintf(stderr,"card_status: IAI\\x22 = no card.\n");
	  return(1);
  }

  if (!strncmp(buffer, "IAI\x23\x10",readlength)) {
	  fprintf(stderr,"card_status: IAI\\x23\\x10 = card detected.\n");
	  return(0);
  }

  if (!strncmp(buffer, "IAI\x23\x00",readlength)) {
	  fprintf(stderr,"card_status: IAI\\x23\\x00 = card detected.\n");
	  return(0);
  }

  fprintf(stderr,"card_status: uh oh, something different was received: %s\n",buffer);
  return(2); /* It should not get to this point. */
}


int open_card(int fd) {
  char buffer[255];
  int readlength;
  int logfilefd;
  
  printf("open_card: dumping the first response to dexux-log.open_card.1\n");
  logfilefd = creat("dexux-log.open_card.1", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if (logfilefd == -1) 
    perror("open_card: unable to open dexux-log.open_card.1\n");

  clear_buffer(fd);
  write(fd, "IAI\x02\x00\x00",6);

  usleep(1000);
  readlength = read(fd, &buffer, 254);
  buffer[readlength]='\0';
  
  write(logfilefd, buffer, readlength);

  close(logfilefd);
  return(0);
}

int close_port(int fd) {

  write(fd, "IAI\x07\x00",5);
  
  close(fd);
  return(0);
}


int get_chunk(int fd, char *address, char *data) {
  int readlength;

printf("get_chunk: retrieving %.2x %.2x\n", *(address+0), *(address+1));

  clear_buffer(fd);
  set_read_timeout(fd, 0);
  write(fd, "IAI\x02", 4);
  write(fd, address+1, 1);
  write(fd, address+0, 1);
printf("get_chunk: request for chunk sent.\n");
  readlength = read(fd, data, 4);
  usleep(75000); /* Should be unnecessary, but fixes hang in get_block() */
  readlength = read(fd, data, 128);
  data[readlength]='\0';
  set_read_timeout(fd, STANDARDTIMEOUT);
printf("get_chunk: got %d bytes.\n", readlength);
  return(readlength);
}




int get_block(int fd, int blocknumber, char *blockdata) {   /*blockdata[8193]*/
  char *pblockdata;
  char *pchunktoget;
  char chunktoget[2];
  long chunkaddress;
  int i, j;
  char got_chunk[129];
  
  pblockdata = blockdata;
  pchunktoget = chunktoget;

  printf("get_block: starting chunk grab at %.2x\n", blocknumber*0x40 );
  printf("get_block: stopping chunk grab at %.2x\n", ((blocknumber+1)*'\x40')-1 );

  for (i = 0; i <= 63; i++) {
    chunkaddress = (blocknumber*0x40) + i;
    *(pchunktoget+0) = chunk_top(chunkaddress);
    *(pchunktoget+1) = chunk_bottom(chunkaddress);
    get_chunk(fd, chunktoget, got_chunk);
    /* Copy chunk to correct position in blockdata */
    for (j = 0; j <= 127; j++) {
      *(pblockdata + (i*128) + j) = got_chunk[j];
    }
  }

  *(pblockdata+8192) = '\0';
  return(0);
}


int dump_toc(int fd) {
  int logfilefd;
  char got_block[8193];
  
  logfilefd = creat("dexux-log.dump_toc.1", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  clear_buffer(fd);

  get_block(fd, 0, got_block);
  printf("dump_toc: got a block.\n");
  write(logfilefd, got_block, 8192);

  close(logfilefd);
  return(0);
}


int set_read_timeout(int fd, int length) {
  struct termios options;
  tcgetattr(fd, &options);
  options.c_cc[VTIME] = length;  /* Timeout in 1/10 seconds on reads */
  return(tcsetattr(fd, TCSAFLUSH, &options));
}


int dump_block1_header(int fd) {
  int logfilefd;
  char got_chunk[129];
  
  logfilefd = creat("dexux-log.dump_block1_header.1", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  clear_buffer(fd);

  get_chunk(fd, "\x00\x00", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x01", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x40", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x41", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x42", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x43", got_chunk);
  write(logfilefd, got_chunk, 128);

  close(logfilefd);
  return(0);
}


int dump_block2_header(int fd) {
  int logfilefd;
  char got_chunk[129];
  
  logfilefd = creat("dexux-log.dump_block2_header.1", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  clear_buffer(fd);

  get_chunk(fd, "\x00\x00", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x02", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x80", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x81", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x82", got_chunk);
  write(logfilefd, got_chunk, 128);

  get_chunk(fd, "\x00\x83", got_chunk);
  write(logfilefd, got_chunk, 128);

  close(logfilefd);
  return(0);
}

int chunk_top(long chunkaddress) {
  return(chunkaddress / 256);
}

int chunk_bottom(long chunkaddress) {
  return(chunkaddress % 256);
}


int binary_reverse(int inbyte) {
  int outbyte;

  outbyte =            (inbyte &   1) << 7;
  outbyte = outbyte + ((inbyte &   2) << 5);
  outbyte = outbyte + ((inbyte &   4) << 3);
  outbyte = outbyte + ((inbyte &   8) << 1);
  outbyte = outbyte + ((inbyte &  16) >> 1);
  outbyte = outbyte + ((inbyte &  32) >> 3);
  outbyte = outbyte + ((inbyte &  64) >> 5);
  outbyte = outbyte + ((inbyte & 128) >> 7);

  return(outbyte);
}

int mass_xor(char *instring, long stringlength) {
  int outbyte = 0;
  long i;  /* Loop counter */
  
  for (i = 0; i <= (stringlength - 1); i++) {
      outbyte =   (  (outbyte&  1)     ^  (*(instring+i)&  1)           ) +
                ( ( ((outbyte&  2)>>1) ^ ((*(instring+i)&  2)>>1) ) << 1) +
                ( ( ((outbyte&  4)>>2) ^ ((*(instring+i)&  4)>>2) ) << 2) +
                ( ( ((outbyte&  8)>>3) ^ ((*(instring+i)&  8)>>3) ) << 3) +
                ( ( ((outbyte& 16)>>4) ^ ((*(instring+i)& 16)>>4) ) << 4) +
                ( ( ((outbyte& 32)>>5) ^ ((*(instring+i)& 32)>>5) ) << 5) +
                ( ( ((outbyte& 64)>>6) ^ ((*(instring+i)& 64)>>6) ) << 6) +
                ( ( ((outbyte&128)>>7) ^ ((*(instring+i)&128)>>7) ) << 7);
  }

  return(outbyte);
}


int copy_block_todisk(int fd, int blocknumber) {
  char blockdata[8193];
  char headerdata[129];
  char chunkrequest[2], *pchunkrequest;
  int dumpfd;

  pchunkrequest = chunkrequest;

  *(pchunkrequest+0) = 0;
  *(pchunkrequest+1) = blocknumber;
  
  get_block(fd, blocknumber, blockdata);
  get_chunk(fd, chunkrequest, headerdata);

  dumpfd = creat("dexux-copy_block", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  write(dumpfd, headerdata, 128);
  write(dumpfd, blockdata, 8192);

  return(0);
}

int copy_block_tocard(int fd, int blocknumber) {
  char blockdata[8193];
  char headerdata[129];
  char chunk[2], *pchunk;
  int filefd;

  pchunk = chunk;

  *(pchunk+0) = 0;
  *(pchunk+1) = blocknumber;

  filefd = open("dexux-copy_block", O_RDONLY);
  read(filefd, headerdata, 128);
  read(filefd, blockdata, 8192);
  close(filefd);
  
  write_chunk(fd, chunk, headerdata); 
  write_block(fd, 1, blockdata);
  
  return(0);
}


int write_chunk(int fd, char *address, char *data) {
  char buffer[5];
  char sendstring[133], *psendstring;
  char checkbyte;
  int readlength;
  int i;  /* Loop counter */
  
  psendstring = sendstring;
  *(psendstring+132) = 0;

  *(psendstring+0) = *(address+0);
  *(psendstring+1) = *(address+1);
  *(psendstring+2) = binary_reverse( *(address+0) );
  *(psendstring+3) = binary_reverse( *(address+1) );
  
  for (i = 0; i <= 127; i++) {
    *(psendstring+4+i) = *(data+i);
  }
  
  clear_buffer(fd);
  printf("write_chunk: writing chunk %.2x %.2x to the card...\n", *(address+0), *(address+1) );

  checkbyte = mass_xor(sendstring, 132);
  write(fd, "IAI\x04", 4);
  write(fd, sendstring, 132);
  write(fd, &checkbyte, 1);

  readlength = read(fd, buffer, 4);
  if (strncmp(buffer, "IAI\x29", 4)) {
	  buffer[readlength] = 0;
	  fprintf(stderr, "wake_dex: I wanted to hear IAI\\x29 but the DexDrive said: %s\n",buffer);
	  return(1);
  }

  printf("write_chunk: chunk written successfully\n");

  set_read_timeout(fd, STANDARDTIMEOUT);
  return(readlength);
}


int write_block(int fd, int blocknumber, char *blockdata) {   /*blockdata[8193]*/
  char chunktoput[2], *pchunktoput;
  char chunkdata[129], *pchunkdata;
  long chunkaddress;
  int i, j;
  
  pchunktoput = chunktoput;
  pchunkdata = chunkdata;

  printf("write_block: starting at chunk %.2x\n", blocknumber*0x40 );
  printf("write_block: stopping at chunk %.2x\n", ((blocknumber+1)*0x40)-1 );

  for (i = 0; i <= 63; i++) {
    chunkaddress = (blocknumber*0x40) + i;
    *(pchunktoput+0) = chunk_top(chunkaddress);
    *(pchunktoput+1) = chunk_bottom(chunkaddress);
    
    /* Copy chunk from correct position in blockdata */
    for (j = 0; j <= 127; j++) {
      *(pchunkdata+j) = blockdata[ (i*128) + j ];
    }
    
    write_chunk(fd, chunktoput, chunkdata);
  }

  return(0);
}

