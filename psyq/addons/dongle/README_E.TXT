$PSDocId: Document Release 1.0 for Runtime Library Release 3.x$

                            2nd October 1996
	
                          Texture Window Stuff
	
	    Copyright (C) 1994-1996, Sony Computer Entertainment
                           All right reserved
	

    Here is a sample program to illustrate using the memory card as
    a dongle.  Although the dongle 'encryption'method used is crude,
    it can be replaced easily with a more complicated method.

    Keys:
      Main Menu
        up            previous option
        down          next option
        cross         select option

      Entering Dongle Info
        up            choose above edit box  
        down          choose below edit box
        left          move to previous character
        right         move to next character

        triangle      previous menu
        square        increase character value
        circle        decrease character value
        L1            increase character value X 6
        R1            decrease character value X 6
        L2            previous dongle info (from PC file)
        R2            next dongle info (from PC file)
        start         create dongle file

      Memory Card Options
        triangle      previous menu
        up            choose previous file
        down          choose next file

        square        delete file
        circle        view file contents

      View File Contents
        triangle      previous menu
        up            scroll up 128 bytes
        down          scroll down 128 bytes 
        left          scroll up 1024 bytes
        right         scroll down 1024 bytes 

        ==================================================================
	program		contents				
        ------------------------------------------------------------------
    main        the dongle creation program
        -------------------------------------------------------------------
