@echo off
deltree /y SendTo
md SendTo
copy release\sendtov64.exe SendTo
copy readme64.txt SendTo
copy history.txt SendTo
copy setup.bat SendTo
start .

