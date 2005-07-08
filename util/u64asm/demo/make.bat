@echo off
..\n64 %1
if errorlevel 1 goto bad
copy /b ..\header + %1.bin %1.rom
del %1.bin
..\extend %1.rom
..\chksum64 %1.rom
:bad
