$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$	
simple(jump) : SEQ playing

                     Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.

<Descriptions>
	This is a sample program using sound library.
	"simple" is playing the SEQ data file and displaying tempo,
	volume and status on screen.

	"jump" is using jump-table.

<How to execute>
       (simple)
        > psymake         compile
        > psymake load    load data
        > run simple      run

       (jump)
        > psymake         compile
        > psymake load    load data
        > run jump        run

<Controller Button Assignment>
        Up Direction Button    :volume up.
        Down Direction Button  :volume down.
        Right Direction Button :crescendo.
        Left Direction Button  :decrescendo.
        Cross Button           :replay SEQ data.
        Triangle Button        :pause SEQ data.
        L1 Button              :ritardando.
        R1 Button              :accelerando.
        Start Button           :play SEQ data.
        Select Button          :stop SEQ data.
