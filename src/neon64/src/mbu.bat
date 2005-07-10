copy backup.inc hardware.inc
..\n64 neon64
del hardware.inc
if errorlevel 2 goto end
copy /b ..\header + neon64.bin neon64bu.rom
del neon64.bin
:end
