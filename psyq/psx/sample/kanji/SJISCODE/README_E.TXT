$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$


*** KANJI Code Viewer Program ***

< Overview >
    This sample program is used for mapping built-in KANJI data in 
    PlayStation as 16-bit texture on a rectangle polygon.
    Shift-JIS codes of the built-in fonts can be dispalyed. 
    Use for the referrence to user-defined character codes.  

< How to operate >	
	Left Up:	        Scrolling upward
	Left Down:	Scrolling downward
	L1:		Character color +
	L2:		Character color -
	R1:		Background color +
	R2:		Background color -

	Keeping any key pushed will increase the speed in 3 steps; 
        standard, double, 4 times.

< How to load the program >
	> psymake

	With the above command, codeview.cpe is created. 
	krom2tim.obj, kprintf.obj and ktrans.obj are needed to compile.
        These *.obj files will be built in libgpu.lib later.
        Use the following commands for the execution.

	> run patchw
	> run codeview

        Since KANJI font is not built in DTL-H2000, execute patchw
        before codeview execution.
	

