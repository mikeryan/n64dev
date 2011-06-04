#!/bin/sh
n64 codes -h
n64 staticdl -h
n64 logo

cp -a backup.inc hardware.inc
n64 neon64 || echo uh oh
rm hardware.inc

cat ../header neon64.bin > neon64bu.rom
rm neon64.bin
