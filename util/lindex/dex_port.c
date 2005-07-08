// --------------------------------------------------------------------------
//
// LinDex - DexDrive Utilities For Linux
// Copyright (C) 2001 Dan Page [http://www.phoo.org]
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// --------------------------------------------------------------------------

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "lindex.h"

#include "dex_frame.h"
#include "dex_block.h"
#include "dex_card.h"
#include "dex_port.h"
#include "dex_string.h"
#include "dex_util.h"

// {{{ dex_portOpen
UINT32 dex_portOpen( UINT8 device[] )
{
  // Define local variables.
  struct termios options;

  UINT32 port   = 0;
  UINT32 status = 0;

  // Open the named port in read/write mode without becomming the
  // controlling process and without initially waiting for the 
  // other end to be ready.
  port = open( device, O_RDWR | O_NOCTTY | O_NDELAY );

  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portOpen( ... ) [port=0x%02X, device=%s]\n", port, device );
  }

  // Check if the port was opened sucessfully.
  if( port != -1 )
  {
    // Set the port up so that there is no blocking performed when  
    // there is no data availible.
    fcntl( port, F_SETFL, 0 );

    // Get the current attributes for the port.
    tcgetattr( port, &options );

    // Set the input and output baud rates for the port.
    cfsetispeed( &options, B38400 );
    cfsetospeed( &options, B38400 );

    // Set the port up for 8N1 style communication and without any
    // hardware flow control.
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;

    // Set the local serial driver to raw mode to stop it mucking 
    // around with the data we read from the device.
    options.c_lflag &= ~ICANON;
    options.c_lflag &= ~ECHO;
    options.c_lflag &= ~ECHOE;
    options.c_lflag &= ~ISIG;

    // Set the input flags up so there is no parity checking and no
    // software flow control.
    options.c_iflag &= ~INPCK; 
    options.c_iflag |= IGNPAR; 
    options.c_iflag &= ~PARMRK;

    options.c_iflag &= ~IXON;
    options.c_iflag &= ~IXOFF;
    options.c_iflag &= ~IXANY;

    // Set the output flags up so there is no mangling of output data
    // in terms of newline translation etc.
    options.c_oflag &= ~OPOST; 

    // Set the minimum number of characters to read and the minimum time
    // to wait for input.  This ensures that read operations will return
    // fairly quickly if there isn't any data to read.
    options.c_cc[ VMIN  ] = 0; 
    options.c_cc[ VTIME ] = 10; 

    // Set the new attributes for the port.
    tcsetattr( port, TCSAFLUSH, &options );

    // Toggle DSR and RTS on for 200ms.
    ioctl( port, TIOCMGET, &status );
    status &= TIOCM_LE;
    status &= TIOCM_RTS;
    ioctl( port, TIOCMSET, status );
    usleep( 200 );

    // Toggle DSR and RTS off for 200ms.
    ioctl( port, TIOCMGET, &status );
    status &= ~TIOCM_LE;
    status &= ~TIOCM_RTS;
    ioctl( port, TIOCMSET, status );
    usleep( 200 );

    // Toggle DSR and RTS on for 200ms.
    ioctl( port, TIOCMGET, &status );
    status &= TIOCM_LE;
    status &= TIOCM_RTS;
    ioctl( port, TIOCMSET, status );
    usleep( 200 );

    // Return the port port.
    return port;
  }
  else
  {
    // Return an error.
    return DEX_PORT_OPEN_ERROR;
  }
}
// }}}

// {{{ dex_potClose
UINT32 dex_portClose( UINT32 port )
{
  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portClose( ... ) [port=0x%02X]\n", port );
  }

  // Close the specified port port.
  close( port );
}
// }}}

// {{{ dex_portIFlush
UINT32 dex_portIFlush( UINT32 port )
{
  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portIFlush( ... ) [port=0x%02X]\n", port );
  }

  // Flush any input data.
  return tcdrain( port );
}
// }}}
// {{{ dex_portOFlush
UINT32 dex_portOFlush( UINT32 port )
{
  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portOFlush( ... ) [port=0x%02X]\n", port );
  }

  // Flush any output data.
  return tcflush( port, TCIFLUSH );
}
// }}}

// {{{ dex_portPut
UINT32 dex_portPut( UINT32 port, UINT8 message[], UINT32 length )
{
  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portPut( ... ) [port=0x%02X]\n", port );
  }

  // Perform the write and return the result.
  return write( port, message, length );
}
// }}}
// {{{ dex_portGet
UINT32 dex_portGet( UINT32 port, UINT8 message[], UINT32 length )
{
  // Define local variables.
  UINT32 availThis = 0;
  UINT32 availPrev = 0;

  UINT16 retry     = 0;

  // Check the port to see how much input is availible.
  ioctl( port, FIONREAD, &availThis );

  // Print debugging information if we need to.
  if( dex_debug )
  {
    printf( "dex_portGet( ... ) [port=0x%02X, retry=0x%02X, availThis=0x%02X, availPrev=0x%02X]\n", port, retry, availThis, availPrev );
  }

  // Loop while there isn't enough input and we still feel the 
  // need to keep retrying. 
  while( ( availThis < length ) && ( retry < DEX_PORT_RETRY ) )
  {
    // Increment the retry count.
    if( availThis == availPrev )
    {
      retry++;
    }

    // Sleep for a while and check the port again.
    usleep( DEX_PORT_SLEEP );
    ioctl( port, FIONREAD, &availThis );

    // Print debugging information if we need to.
    if( dex_debug )
    {
      printf( "dex_portGet( ... ) [port=0x%02X, retry=0x%02X, availThis=0x%02X, availPrev=0x%02X]\n", port, retry, availThis, availPrev );
    }

    // Store the previous availibility count.
    availPrev = availThis;
  }

  // Perform the read and return the result.
  return read( port, message, length );
}
// }}}
