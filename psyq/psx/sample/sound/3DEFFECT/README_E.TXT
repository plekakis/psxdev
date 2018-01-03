$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
3deffect : 3d effect sample using libsnd and libspu

                      Copyright(C) 1997 Sony Computer Entertainment Inc.  
                                                    All Rights Reserved.

<Descriptions>
This Sample provides usage examples of all 3d sound key-on functions 
included in libsnd 4.1.

Compatability of libsnd key-on series with other libsnd functions is
demonstrated by using SsSetDetVVol() to change volume of sound keyed 
on with the libsnd key-on series of functions.

In addition, the following items are demonstrated:

Toggling reverb on and off using SsQueueReverb()
------------------------------------------------
In libsnd 4.1, this function now allows for easy reverb changes for 
currently sounding libsnd controlled voices. This function also fixes the 
libsnd-libspu  reverb incompatability bug.

Fast volume changes with SsQueueRegisters()
-------------------------------------------
Fast pitch and adsr changes can also be accomplished using this same method.


SEE ALSO:
For detailed descriptions of the new libsnd functions demonstrated in
this Sample, refer to the Basic Sound Library chapters of the Runtime
Library 4.1 Reference documentation.

Note that headphones provide the best experience of 3d sound location using 
this Sample.

    
Menu choices:
-------------
0)Key On Amb
         Keys on 2 tone, stereo pair, looping ambient rain effect using the
         libsnd key-on series.
         Demonstrates 1 method of using the libsnd key-on series. Many other
         methods [some examples:
         a) volume calculation rather than look-up tables b) pitch look-up
         tables rather than calculations c) vab header address storage and 
         offset to obtain tone data rather than using slower libsnd function 
         to accomplish same d) calculation of pan values, rather than 
         arbitrary settings] exist.

1) Key On Spatial
         Keys on a 2 tone, single-shot thunder effect, located according to
         the position set up in menu choice #3. Uses the libsnd key-on series. 
         Demonstrates one method of spatially locating sound. No sound 
         movement is included. No echo spatialization  is included.

2) Move Spat Loc
         Toggles through the spatial location grid. Does not change currently
         sounding spatial effects.

3) Key On Spu
         Uses libspu to key on a single tone, single-shot bullfrog effect on
         voice #12, which is outside of the voice allocation system of libsnd.

4) Dec Amb Vol
         Uses SsSetDetVol() to decrement volume of rain effect. Demonstrates
         ability of libsnd key-on series functions to tie into rest of libsnd.

5) Inc Amb Vol
         Uses SsQueueRegisters() to change the volume of the ambient rain
         effect. Also uses SsVoiceCheck() to ensure that the voices being 
         affected by volume change are the voices playing back the ambient 
         rain effect.

6) Toggle Spu Rev
         Toggles voice #12 reverb using SsQueueReverb(). Will place voice 12
         in the desired reverb state whether or not voice is currently 
         sounding (currently, bullfrog sample is not long enough to toggle 
         reverb during playback). Demonstrates solution to libspu-libsnd 
         reverb incompatability.

7) Toggle Amb Rev
         Toggles ambient rain effect reverb using SsQueueReverb(). Will not
         change reverb of rain effect if voices not currently sounding. 
         Demonstrates quick method to change reverb of currently sounding 
	 voices.

8) Key Off
         Key off all samples. Release rates for thunder and rain effects are
         very long.

<How to execute>
        > psymake         compile
        > psymake load    load data
        > run main          run

<Controller Button Assignment>
        Up & Down Directional Buttons    :menu choice
        Cross Button                     :for each highlighted menu choice
        Select Button                    :exit
