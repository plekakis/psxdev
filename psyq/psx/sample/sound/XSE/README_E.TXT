$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
xse : sound effect sample program

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	This is a sample program using sound effect(pitch bend, auto
	panning or auto volume).

	If you change the TONENO in main.c from 10 to 9, the voice will 
	change to the one with vibrato.

<How to Execute>
        > psymake        compile
        > psymake load   load data
        > run main       run

<Controller Button Assignment>
        Up Direction    :move up the tile.
        Down Direction  :move down the tile.
        L1              :move left the tile.
        R1              :move right the tile.
        Cross Button    :auto panning
        Triangle Button :auto volume
        Start           :key on
        Select          :key off
