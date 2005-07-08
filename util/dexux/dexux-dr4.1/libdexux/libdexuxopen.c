/* Dexux
 *
 * Copyright (C) 2000 William Ono <wmono@users.sourceforge.net>
 * Copyright (C) 2000 Ben Galliart <bgallia@users.sourceforge.net>
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "libdexux.h"

/* Create a file descriptor and configure serial port */
int dexuxopen(char *devname) {
  int fd; /* File descriptor for the port */
  struct termios options;

  if (devname[0] != 0) {
    fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY);
  } else if (getenv("DEXUXPORT") != NULL) {
    fd = open(getenv("DEXUXPORT"), O_RDWR | O_NOCTTY | O_NDELAY);
  } else { 
    fd = open(DEXUXTTYDEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
  } 
  if (fd == -1) {
    perror("open_port: unable to open device for dex drive\n");
    return(fd);
  }

  fcntl(fd, F_SETFL, 0);  /* Set fd's flags. See fcntl(2) */

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

  options.c_cc[VTIME] = DEXUXTIMEOUT;  /* Timeout in 1/10 seconds on reads */

  tcsetattr(fd, TCSAFLUSH, &options);

  return(fd);
}
