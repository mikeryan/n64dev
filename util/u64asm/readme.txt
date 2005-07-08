5/14/03
A demo of the U64ASM assembler.

In the demo directory type "make demo" and the finished ROM will appear in
demo.rom

The contents of this archive are as follows:
* chksum64.exe - an N64 ROM image checksummer by Andreas Sterbenz
* extend.exe - pads a file with zeroes until it is a multiple of 2 MBs
* header - a standard N64 header file, with the title "HCS N64 Demo",
           origin unknown
* n64.exe - U64ASM, an full-featured MS-DOS R4000 assembler
* readme.txt - this file
* demo\control.inc - routines for interfacing controllers
* demo\demo.asm - the main demo program
* demo\dos.raw - a standard 8x8 monochrome bitmap font
* demo\macros.inc - some basic macros for use with the demo
* demo\make.bat - runs all needed programs to produce a complete N64
                  ROM image
* demo\rsp.inc - routines for interfacing the RSP
* demo\screen.inc - routines for initializing video
* demo\text.inc - a sample text blitter for the RSP

Ask and ye shall receive.

-hcs
http://here.is/halleyscomet
halleyscometsoftware@hotmail.com

All contents of this archive are (c) 2002-2003 Halley's Comet Software
unless otherwise noted.

P.S.
U64ASM is bloated, poorly written, and slow; thus it should only be used for
educational purposes.
