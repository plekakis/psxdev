
PS-X Demonstration Program "bgfix32"
=====================================

* The program bgfix32 uses GsSortFixBg32, a libgs function that provides high-speed display of BG screens, which display the contents of BG data composed by the Sprite editor.

* Execution
psymake
resetps
psymake load
run main


* Description of the files
makefile.mak   makefile

main.c         main routine

sample.tim     texture data
sample.cel     BG cell data
sample0.bgd    BG map data
sample1.bgd         "
sample2.bgd         "
sample3.bgd         "


* Control pad operations are as follows


Up arrow key:  increase BG screen number
Down-arrow key:decrease BG screen number

SELECT button: exit



Copyright (C) 1994,1995 by Sony Computer Entertainment
All Rights Reserved

