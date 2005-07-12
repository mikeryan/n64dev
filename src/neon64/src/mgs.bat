copy gs.inc hardware.inc
..\n64 neon64
del hardware.inc
if errorlevel 2 goto end
..\rncpc p d -p 8096 .rnc neon64.bin
del neon64.bin

rem Relocatable version
..\n64 depack
if errorlevel 2 goto end
del neon64gs.bin
ren depack.bin neon64gs.bin

del neon64.rnc

:end

