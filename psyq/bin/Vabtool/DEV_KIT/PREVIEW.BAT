@echo off
@echo Previewing file: %1
h25bios.com
mess1.com
pqbload mouse.tim      80090000
pqbload keyboard.tim   80090100
pqbload progbutn.tim   8009A000
pqbload %1             80100000
run sndtool
