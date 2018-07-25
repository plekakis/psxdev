$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
Wave and Clock: texture address modulation sample, tuning sample with DTL-H2700

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Descriptions>

        This sample shows the pseudo environment map of the wave reflection
        of a clock by modulating texture address in real time, and the 
        refraction of a clock under the water.

        tuto1, tuto2 and tuto3 are tuned with DTL-H2700 and Performance 
        analyzer. "main" has auto-demo mode that changes three scenes.

        tuto1  clock in a swimming pool
               with DMPSX
        tuto2  clock in a swimming pool
               tune CPU process (scratch pad is assigned to stack)
        tuto3  clock in a swimming pool
               tune GPU process (divide OT)
        main   2-minute auto demo including three scenes


<How to Execute>

    tuto1, tuto2, tuto3:

        > psymake tuto1.cpe  compile
        > psymake tuto1      load data and run

    main:

        > psymake main.cpe   compile
        > psymake main       load data and run


<Controller Button Assignment>

    tuto1, tuto2, tuto3:

     Controller One
        Up Directional Button   : transfer XY of the surface of the water
        Down Directional Button : (ditto)
        Right Directional Button: (ditto)
        Left Directional Button : (ditto)
        Square Button           : rotation of the surface of the water
        Cross Button            : (ditto)
        Triangle Button         : (ditto)
        Circle Button           : (ditto)
        L1 Button               : refraction rate up
        L2 Button               : refraction rate down
        > R1 Button               : transfer Z of the water surface
        R2 Button               : (ditto)
        Start Button            : (unused)
        Select Button           : quit

     Controller Two
        Up Directional Button   : transfer XY of the clock
        Down Directional Button : (ditto)
        Right Directional Button: (ditto)
        Left Directional Button : (ditto)
        Square Button           : stop wave movement
        Cross Button            : reduction of the clock
        Triangle Button         : magnification of the clock
        Circle Button           : resume wave movement
        L1 Button               : reflection rate up
        L2 Button               : reflection rate down
        R1 Button               : transfer Z of the clock
        R2 Button               : (ditto)
        Start Button            : change textures of the water bottom
        Select Button           : change rendering modes

    main:

     Controller One
        Up Directional Button   : rotation of the water surface 
        Down Directional Button : (ditto)
        Right Directional Button: (ditto)
        Left Directional Button : (ditto)
        Square Button           : change patterns of the wave (three patterns)
        Cross Button            : stop wave movement (stop auto demo)
        Triangle Button         : resume wave movement
        Circle Button           : resume wave movement (auto demo too)
        L1 Button               : refraction rate up
        L2 Button               : refraction rate down
        R1 Button               : reflection rate up
        R2 Button               : reflection rate down
        Start Button            : change three scenes
        Select Button           : quit

     Controller Two
        Up Directional Button   : transfer XY of the clock
        Down Directional Button : (ditto)
        Right Directional Button: (ditto)
        Left Directional Button : (ditto)
        Square Button           : (unused)
        Cross Button            : reduction of the clock
        Triangle Button         : magnification of the clock
        Circle Button           : (unused)
        L1 Button               : (unused)
        L2 Button               : (unused)
        R1 Button               : transfer Z of the clock
        R2 Button               : (ditto)
        Start Button            : (unused)
        Select Button           : (unused)


<Remarks>

        The model data "CLOCK.TMD" is made from the data distributed by
        Newtek Incorporated.

