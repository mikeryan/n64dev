================================================================================
SendToV64 v1.1a by Christopher Field                                  05/10/1998
================================================================================

*** Important ***
~~~~~~~~~~~~~~~~~
There will be no more releases of SendToV64 from me (Christopher Field). I am
hereby throwing the source code open to the public domain. Hopefully some one
can sort out ECP/EPP support, because I have lost all motivation to continue.
If you do, then you must send me a copy of the modified code, and a copy of
the new .exe. Thank you.

Please read Source.txt also.

Introduction:
~~~~~~~~~~~~~
SendToV64 is a Windows 95/98 utility for sending Nintendo 64 ROM images to the
Doctor v64 backup unit made by Bung. It does not, at present, work
with Windows NT.


Installation:
~~~~~~~~~~~~~
Unzip it using WinZip (www.winzip.com) or similar to any directory of your
pleasing.


Running:
~~~~~~~~
The first time you run the program, it will bring up the configuration screen
so that you may specify what parallel port to use, and also associate the
program with .v64, .z64 and/or .rom files if you wish.
It's highly recommended that you DO associate with these extensions.

Also provided (as of version 1.1a) is the ability to associate the new
extensions supported in BIOS 1.90 or newer. Be warned. If you associate these
files with SendToV64 then you do so at your own risk, and I will not take
blame if it stops other programs working. This is only likely to happen
if you associate SendToV64 with .MP3 files, in this case, you might have
problems loading them into WinAmp etc...

Provided you have associated the extensions, you can now just double click
on any of the .v64 etc... files to send.

If you wish to change your parallel port, or un-associate any extension
then just right click on a ROM image, and select "Config". If you haven't
associated any extensions, then you will have to run "SendToV64 /config".

Once SendToV64 has loaded, just click send, and sit back while SendToV64
does the hard work of sending the rom, and flipping to V64 format if needed.

Details:
~~~~~~~~
SendToV64 supports Doctor v64 and Z64/SP64/WC file formats, and will flip
the image if needs be. Hopefully this will stop a few of the lamers out there
from complaining when you get a black screen...

At present the program only supports sending via SPP. This is an area that is
being addressed. So please, no mail on that front :)

Contact:
~~~~~~~~
E-Mail  sendto64@netcomuk.co.uk
WWW     http://www.netcomuk.co.uk/~debug/sendtov64.html
IRC	ircnet or efnet, nickname = [prizna]

Thanks To:
~~~~~~~~~~
Trancer for testing, support and heaps of suggestions.
Bung and Andreas Sterbenz for releasing the source code to their send programs.

Disclaimer:
~~~~~~~~~~~
I will not be held responsible for any damage caused either directly or
indirectly, due to the use of this program. YOU USE IT AT YOUR OWN RISK!

Having said that, if you do manage to fry your v64 BIOS, then please do let
me know (I wont fix it for you, but I might be able to stop others doing
the same). Also if you fry your BIOS, then there is a utility to fix it on
Bung's web page (www.bung.com.hk).
