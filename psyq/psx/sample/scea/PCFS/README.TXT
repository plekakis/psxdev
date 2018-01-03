This is an example written by Mike Fulton of SCEA which demonstrates
how to use the PCFS on the development boards.

To run on the DTL-H2500 or the DTL-H2000 machines:
1. Open up a DOS-box.
2. Run dexbios.com or h25bios.com in the DOS-box.  For example, type

	c:\ps\pyq\bin\h25bios.com

or 

	c:\ps\bin\dexbios /a1340 /i10

3. Run mess1.com, the TSR message handler. Type

	mess1.com

in the DOS-box.

4. Run your program -- in this case, 

	run hello.cpe

5. Type "testmess" to see the output.

