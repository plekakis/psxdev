SNDTOOL  - A DEV Kit VAB file preview tool
by Mark Breugelmans 1 November 1997
(Sony Computer Entertainment Europe)
------------------------------------------

SNDTOOL is a program which previews VAB sound files on the
PlayStation Development Kit. It supports both mouse and
PAD controllers.

Instructions:


1) Setting the VAB that you wish to preview

You can run the program 'PREVIEW.BAT' in Windows95 with a
command line parameter which is the name of the file to
preview. This will boot up your DEV kit and start the
preview tool for you in a DOS box. (Note : Make sure there
are no other DOS sessions already running)

Alternatively you can run it from a DOS window like the other
DEV kit samples. To use a different VAB file just change the
file 'makefile.mak'. The forth item is the VAB file which is
to be loaded into memory at address 80100000. Then just type
in 'psymake load' and then 'run sndtool'.


2) Playing sounds:

You can use a PAD controller or a PlayStation mouse to move
the mouse cursor over the music keyboard to select the note
that you which to key-on and key-off. The note that you are
over is shown in text above the keyboard.

When the note is keyed On with the left mouse button or 'X'
key on the PAD controller the key is highlighted on the
screen and the sound is played.

If you want to move the keyboard sideways to get to higher
notes press the right mouse button or the 'O' button on the
mouse controller.

3) Selecting a program

You can select a different program by clicking on the left
and right arrow icons above the keyboard to get the next or
previous program. Or with the pad controller you can also use
the L1 and R1 buttons.

