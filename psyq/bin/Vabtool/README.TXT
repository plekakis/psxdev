-----------------------------------------------------------------------
VAB/VAG PC Sound Tools  V2.1 beta

by Mark Breugelmans

24 November 1998

(c) 1998 Sony Computer Entertainment Europe
-----------------------------------------------------------------------

Overview:

The following tools have been created to help PC users contruct sound files
more easily for the PlayStation. Most of the current sound tools are geared
towards Mac users which can cause problems for developers without access to
an Apple Mac computer.

Tools:

1. Wav2Vag - This is a simple program which stips off the header from
             Windows format '.wav' files and then shells the Aiff2Vag
             program with the correct options. It requires 16bit mono
             wav files. It runs as a DOS program with command line
             parameters.

2. VABTool - This is a windows program to contruct/edit VAB files

             a) You can edit/add programs and tones.
             b) You can set ADSR settings for tones.
             c) You can add VAG format files to the sound data section
                of the VAB file.
             d) You can edit '.VH' format VAB Headers.
             e) You can join and split VH and VB format to VAB format.
             f) You can preview sounds as you adjust them.
             g) Supports the PCI Sound Artist Card.
             h) DevKit preview tool added.
    - NEW -  i) SEQ preview on PCI Sound Artist Card added.

Instructions/Notes:

1. The VXD driver file for the PCI sound artist card must go into the
   windows/system directory.

2. Most of the bugs have been fixed as far as I know - most of these were in
   the ADSR graphs. The preview system was totally rewritten to be integrated
   with the other tools rather than launching a separate application, this
   seems to work better but hasn't been tested extensively - so be carefull.

3. Wav2Vag may not work with some of the newer format 'wav' files if they use
   non standard format. 

4. You will need your screen set to at least 800x600 to view the dialogs on
   VABTool.

5. You can double click the grids on VABTool to access other dialogs more
   quickly.

6. SoundBlaster preview only represents an approximation of the sound output
   on the PSX in particular features such as reverb and ADSR are ignored.

7. You will need to set the path for 'PREVIEW.BAT' to use it with the new
   DevKit preview tool. This runs as a normal DevKit program like the
   other examples, you can run it from windows95  so long as there are no
   other DOS windows running H25BIOS.COM. This program supports PAD and
   mouse controllers. 

   If you want to use it on its own just type in
           PREVIEW MyFile.VAB
   from your dos window.

   Use the direction keys/mouse to move the cursor. Left mouse button or 'X'
   on the pad presses the piano keys or the program more/less arrows. The
   right mouse button or 'O' on the pad toggles moves key position up/down
   two octaves.

8. Reverb is not previewed in any of the tools.

9. There's some pretty intensive processing going on to decode the VAG sound
   data and modify sampling rates etc, so you may find this needs a fast
   PC. I'll be interested to get feedback on this.

10.You can now playback SEQ files. The feature is only available on the
   sound artist PCI card (H800). It had hoped to emulate playback in
   software for SoundBlaster also, however my PC simply isn't fast enough
   to decode and mix 24 voices fast enough to keep good timing. Pitch
   bend messages are currently igonored.

Fixed in this version:

1. Fixed bug in the tones dialog which corrupted files when adding tones
   to new programs.

Support/Fixes:

Please email me: mark_breugelmans@playstation.sony.com with any comments,
requests or suggestions I'll be happy to hear any feedback.


