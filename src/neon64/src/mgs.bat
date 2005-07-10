copy gs.inc hardware.inc
..\n64 neon64
del hardware.inc
if errorlevel 2 goto end
..\rncpc p d -p 8096 .rnc neon64.bin
del neon64.bin
..\n64 depackgs
if errorlevel 2 goto end
del neon64gs.bin
ren depackgs.bin neon64gs.bin

rem Europe version
..\n64 depackge
if errorlevel 2 goto end
del neon64ge.bin
ren depackge.bin neon64ge.bin

del neon64.rnc

:end

