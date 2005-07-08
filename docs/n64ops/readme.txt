<<<www.dextrose.com>>>
Nintendo 64 Toolkit: opcodes v1.1 by anarko <anarko@flashback.net>

Readme file                                           released on 1999-04-21
----------------------------------------------------------------------------


   This is a collection of programming information for the Nintendo 64
   that i've gathered. It is currently splitted into these parts:

   n64ops#a.txt - Brief list of R4300i opcodes
   n64ops#b.txt - Detailed list of R4300i opcodes
   n64ops#c.txt - R4300i opcode matrix

   n64ops#d.txt - Brief list of RSP opcodes
   n64ops#e.txt - Detailed list of RSP opcodes
   n64ops#f.txt - RSP opcode matrix

   n64ops#g.txt - ROM header information
   n64ops#h.txt - Nintendo 64 memory map

   Contributed by other people:
   rcp.txt      - RCP information, opcodes etc.
   sound.txt    - How to play sound on the N64.
   controll.txt - How to access the N64 controller.

   If you find any errors in the text, or miss anything, please
   inform me about that, either by email <anarko@flashback.net>
   or ICQ #: 3299208.

   /anarko

   http://members.xoom.com/n64toolkit/ - Nintendo 64 Toolkit homepage.

----------------------------------------------------------------------------
Thank-you's to:
----------------------------------------------------------------------------
  Niki W. Waibel - Additional controller information
  Ben Stembridge - For his *great* support, many thanks!
  Fractal - For some contry codes.
  JL_Picard - For giving me some odd boot codes.
  Michael Tedder - For Project UnReality and a link on my page.
  Gordon Hollingworth - For his great support.
  René - For help with some pseudo opcodes.
  Zilmar - For everything you've done for me!
  And very much thank you to all Nintendo 64 programmers out there,
  keep on moohing!
  Titanik - for the controller info.

----------------------------------------------------------------------------
Resources:
----------------------------------------------------------------------------
  R4300i documentation from MIPS Technologies, Inc.
  R4000 Instructions from Silicon Graphics
  Project UnReality by Michael Tedder
  #n64dev and #n64emu on EFnet.
  Various emails and chats.


----------------------------------------------------------------------------
Version history:
----------------------------------------------------------------------------
v1.1 (99-04-21): | * Fixed description of ORI instruction.
                 | * Fixed DSRLV and DSRAV (they take a register for
                 |   variable 3 not a constant).
                 | * Removed CFC0, CTC0. Don't exist on r4300i
                 | * Removed all COP2 instrs. Don't exist on r4300i
                 | * Removed DMFC0, DMTC0. Don't exist on r4300i
                 | * Updated LI encoding in #A.
                 | * Removed BC0F, BC0T, BC0FL, BC0TL. Don't exist on r4300i
                 | * Updated COP1 opcode matrix in #C.
                 | * Changed the structure of the opcode list.
                 | * Added a separate RSP opcode matrix file (#f).
                 | * Changed the ROM header information a bit.
                 | * Removed developement device stuff from the memory map.
                 | * Fixed up the memory map a whole lot.
----------------------------------------------------------------------------
v1.0 (99-01-06): | * Added RSP instruction matrix in #C.
                 | * Changed format in #C.
                 | * Huge update of sections #F and #G (RSP opcodes).
----------------------------------------------------------------------------
v0.9 (98-11-26): | * Fixed ANDI instruction docs, thanx to Flaming Toast.
                 | * Fixed some bugs in Memory Map, thanx to Niki W. Waibel
                 | * Added more details on the controller, thanx to Niki W. Waibel
----------------------------------------------------------------------------
v0.8 (98-10-15): | * Added 0x1FC0 07C4 (Status of controller) in #e.
                 | * Added SOUND.TXT
                 | * Added RCP.TXT
                 | * Added CONTROLL.TXT
----------------------------------------------------------------------------
v0.7 (98-07-13): | * Added some RCP opcodes in #c.
                 | * Added section #f - RCP opcode list
                 | * Added section #g - RCP opcode encoding
                 | * Added more information in the RCP memory map in #e
----------------------------------------------------------------------------
v0.6 (98-05-22): | * Completed RCP memory map in #e.
                 | * Changed name to "Nintendo 64 ToolKit: opcodes"
                 | * Added "Pseudo opcodes" in #a.
                 | * Added pseudo opcodes details in #b.
----------------------------------------------------------------------------
v0.5 (98-05-06): | * Added ClockRate and Release info in #d.
                 | * Removed cc (MIPS IV) stuff from #b.
                 | * Fixed some register names in #a and #b.
                 | * Added explanations in #a.
                 | * Fixed formatting in #a.
                 | * Added "RCP memory map" section #e.
                 | * Added "RCP opcodes" section #f.
----------------------------------------------------------------------------
v0.4 (98-03-25): | * Added CFC0, CTC0, MFC2, MTC2 in #a and #b.
                 | * Added BC0F, BC0FL, BC0T, BC0TL in #b.
                 | * Added LDC2, LWC2, SDC2, SWC2 in #a and #b.
                 | * Added COP2 list in #c.
                 | * Added CFC2, CTC2, DMFC2, DMTC2 in #a and #b.
----------------------------------------------------------------------------
v0.3 (98-02-19): | * Added country codes for Germany and Australia in #d.
                 | * Fixed some stupid misses, i thought that a
                 |   "MIPS IV" instruction was the same as an instruction
                 |   for a MIPS R4xxx processor, but i was *very* wrong.
                 |   Information about PREF, MOVF, MOVZ and MOVN is removed.
                 | * Completed #c, *very satisfied now*
----------------------------------------------------------------------------
v0.2 (98-02-12): | * Added information about SYNC, PREF,
                 |   ABS, CEL.L, CEL.W, CVT.L, FLOOR.L,
                 |   FLOOR.W, ROUND.L, ROUND.W in #a & #b.
                 | * Added country code for Japan and Europe in #d.
                 | * Added compressed info in #d.
                 | * Added "R4300 Command set" section #c.
                 | * Removed some major errors in #b.
                 | * Added "Purpose" to all instructions in #b.
                 | * Changed format of instruction list
                 |   in #b, now much more complete.
                 | * Removed examples in #b, as they only
                 |   took up place.
                 | * Corrected LOTS of errors in #b, there should
                 |   only be minor errors like spelling left now.
                 | * Removed NOP as a separate instruction.
                 | * Corrected all information in #a.
                 | * Added "Description" to all instructions in #b.
                 | * Completed information about C.cond.fmt in #b.
----------------------------------------------------------------------------
v0.1 (98-02-05): | * First release
