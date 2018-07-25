Hi,
	This is the code for the exception handler that I wrote for aiding debugging
on debug stations.
	The code can catch most, but not all errors. It will not detect stack errors
(It needs the stack to get the screen etc.) or Breakpoints. (This is since when
developing it I kept getting a debug exception when running (not debugging) on the
development kit.)
	It has been tested with Libraries 3.1 and 3.2 on both the dev kit and on a
PAL debugging station. (I haven't run it on a NTSC debug machine yet.)
	
	On installation the code can detect for the SN Systems debug stub. To do this
remove the comment from line 88 in vsex.s This checks the debug stub address that
a PSX patched to 4.04 uses but it should work on other patch levels. This means
that you can keep the code in but it won't take effect unless run a debug or
release machine. If the debug stub address changes then the debugger won't work.
So if the debugger stops working then it could be the exception handler. (As 
shipped the exception handler has the debug stub check disabled.)

	I have added a little bus error test. Compile (Makefile and link file
included..) then run. You should see a red screen with a dump of the stack and the
regsiters saying that there was an address error exception on a load or fetch at
0x800108d0. If you then compile with the debug stub check decommented and run
under the debugger : the debugger should stop at 0x800108d0.

	Now the Legal Bit!

1.	This code is for use by Sony Playstation developers and support personnel
	only.

2.	This code and documentation is supplied without any warranty either express
	or implied.

3.	This code may not be used for production code. If we descover its use in 
	final code then we will expect payment.

4.	By using this code you are agreeing to supply any changes/bug fixes to the
	place you obtained this code.

5.	If you find this code useful then you should give credit in your apllication.
	(A tenner would be nice too :)

	As stated in the legal stuff I am not supporting this. If you send mail
to "VISUAL PSX" on the BBS then if I have time then I will try to help if I have
time though this will be on my own time not company time.

	If you want a different handler then its best to change the 'C' function
CExceptionHandler rather than the assembler.
	The version of the assembler hook presented here is basically ripped from
the SN Systems debug stub.. I hope they don't mind!

	Thanks to Andy@Sn Systems for help with this and Debug Stub calls...
			  Allan Murphy for promising to help :)
			  My Boss (Russel) for letting be release this...

	Hope you find this useful!

	Brian Marshall, Visual Sciences Ltd. 09/08/95
	Unit 12,
	Prospect III,
	Dundee Technology Park,
	Dundee,
	DD3 1SW

	Phone: +44 (0) 1382 561101
	Fax  : +44 (0) 1382 561244
	

