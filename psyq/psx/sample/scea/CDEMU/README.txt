Basic example 
Date: August 11, 1997.

This tutorial shows you how to create an executable and run it on the CD-Emulator from the MS-DOS 
console, how to launch it directly from the CD-Emulator as if it were a gold boot disk, and (briefly) how to 
transfer it to a gold disk and run it.   This is just to get your feet wet; more information can be found in the 
"CD Emulator" user's guide, which comes with your CD-Emulator distribution, and also on the Technical 
Reference CD (do a multi-document search on "buildcd" and see what comes up).  I assume that you've 
already set up your emulator correctly and that you have gone through the tutorial in the "Readme.doc" 
distributed with the CD-Emulator diskette. 

I hope this helps, if you have any questions or comments, email me at 
Devtech_support@playstation.sony.com.  If you have any questions or comments about the emulator 
programs, please contact support@snsys.com.  You can visit their Web Site at http://www.snsys.com, and 
thereby get access to their FTP site, which contains the latest and greatest materials.  Finally, I'm not the CD 
guru around here.  Email your tough questions to Devtech_support@playstation.sony.com.

 If your CD-Emulator is already up and running, follow these steps to launch the example program 
"main.cpe":

Step 0. Launch the drivers for your development board.   Open up an MS-DOS console window.   If 
you have a DTL-H2000, run "dexbios".  If you have a DTL-H2500, run "h25bios".  Make sure the emulator 
hard drive is on, and activate the emulator driver "cdbios"  by running the command "cdbios /a<address> 
/I<interrupt> /D<dma channel>", such as 

cdbios /a398 /i5 /d7

  (For more information on launching "cdbios", read the tutorial in the "README.DOC" included in this 
distribution).

Step 1. Place your data files in the directory structure that will appear on the CD.  In this tutorial, 
"text1.txt" and "\dir1\text2.txt" have already been placed in their proper positions.  (By the way, pronounce 
"dir1" as "dir-one", not "dir-el".)

 

The figure on the left is the directory tree we currently have.  In Steps 2 and 3, we will build the emulator's 
hard drive to correspond to the  figure on the right.

Step 2.  Build a ".cti" file. The "hello.cti" file has already been created for this tutorial using "CDGEN" 
and "CCS2CTI", although there are several ways to create a ".cti" file: 

Method 1: By hand.  Follow the formatting instructions in the  CD Emulator User's Guide. 
Method 2: By using CDGEN.   Follow the instructions in the "CD Generator User's Guide" to 
save the file as a ".ccs" file. Then use "ccs2cti" to convert the file to a ".CTI" file:

ccs2cti hello.ccs

This command converts "hello.ccs" into "hello.cti".

Method 3: By using gencti.  This is still a version 1.0beta.  Read the "GenCTI Instructions" in the 
"Readme.doc" of this distribution.  If you would like an update of this program, contact SN 
Systems at support@snsys.com.


Let's examine the "hello.cti" file more closely.


 

 The LicenseA.Dat is the licensing file used to create gold disks for North American territories.  
If you are just using the CD Emulator for now, and you don't have access to this file, you can 
delete this line.  You can find copies of  the licensing files "licenseA.dat", "licenseJ.dat", and 
"licenseE.dat" on the Programmer Tools CD, in the directory "\Cdgen\LcnsFile".  However, if you 
are going to create a gold disk, you should get the appropriate file for your territory anyways.
  The "PLAYSTATION" system identifier and appplication identifiers  are required fields.
  These Identifiers aren't optional, but their values are.
  In this sample directory (sitting on your compuer)   is a sub-directory called "dir1", and it 
contains the file "text2.txt".   Hence, the "source" for this file is "DIR1\TEXT2.TXT".  When the 
cd emulator image is built in Step 3 (below), the file will be placed in to the "Directory DIR1" as 
"File TEXT2.TXT".  The above file declaration used a relative pathname -- it was assumed that 
the parent directory of "DIR1" is the current directory, but alternatively, you can specify the entire 
"source" path.  For example, instead of "Source DIR1\TEXT2.TXT", you could use "Source 
C:\ps\cdemu\sample\DIR1\TEXT2.TXT", as in the following snippet:

Directory DIR1
          File TEXT2.TXT
            XAFileAttributes Form1 Data
            Source C:\ps\cdemu\sample\DIR1\TEXT2.TXT
          EndFile
        EndDirectory

  In this sample directory (sitting on your computer) is a lone file called "text1.txt".  Hence the 
"source" for this file is "TEXT1.TXT", and it will be placed in the Root directory of the CD 
Emulator.
  In this sample directory (sitting on your computer) is a lone file called "psx.exe".   This is the 
bootable executable version of the "main.cpe" file.  You will learn how to boot off of the 
cdemulator in Step 6 (option 2).


Following the syntax in the "Cd Emulator" user's guide, you should be able to cut and paste from this file.  
However, be aware that these sample files are regular data only -- not CD-DA files or sub-header files (such 
as XA-audio), which require a different syntax.  Refer to the examples in the Programmer Tools CD, in 
\psx\sample\scee\cd for examples of ".cti" files for these types of data files.

Step 3.  Copy the directory structure to the emulator's hard drive.  With the cdbios active, run buildcd 
to process the ".cti" file to copy your directory tree's data to the CD emulator's harddrive. Make sure you are 
in the correct directory.  Assuming that the full path of this  "basic" directory is "c:\ps\cdemu\sample\basic", 
change to the directory:

cd c:\ps\cdemu\sample\basic


Now run the "buildcd" command. In the following MS-DOS command, the emulator's hard drive is at SCSI 
ID "2", and data is written to partition "1":


 buildcd -s2:1 hello.cti

(You should have already activate the cdbios in Step 0 of this tutorial).  In practice, you would probably 
want to put this into a batch file, such as the included "build.bat".

Step 4.  Verify that the files are on the emulator's hard drive.  With the cdbios active, run cddisk.  In the 
following MS-DOS command, the emulator's hard drive is at SCSI ID "2".

cddisk 2

Within the cddisk program, set the active partition to "1".  Choose "3" to "View Partition Contents", then 
press "1" (to see inside partition 1).  You should see the directory listing of the contents of the emulator's 
hard drive.  Press "Esc" to exit.

Step 5.  Select the emulator.  Type the following (assuming that snpatch is in c:\ps\pssn\bin; your directory 
tree may be different):

resetps 1
run c:\ps\pssn\bin\snpatch.cpe   (only if running on a DTL-H2000)
run c:\ps\pssn\bin\selemu.cpe	 (you only need to do this command once for any given session of 
h25bios or dexbios)

Step 6 (option 1) .   Launch the executable from the MS-DOS prompt.  Assuming that you are in the 
correct directory, type the following:

resetps 1
run c:\ps\pssn\bin\snpatch   (only if running on a DTL-H2000)
run main.cpe

By running your executable from the MS-DOS prompt, you can step through your code using  SN's 
dbugpsx:

resetps 1
run c:\ps\pssn\bin\snpatch   (only if running on a DTL-H2000)
dbugpsx  main /e

You should see the following words on your television monitor, on a red background:

	THE CURRENT BUFFER:  0
	FROM FILE 1: CONTENTS OF TEXT1.TXT. HI THERE!

	FROM FILE 2: GREETINGS FROM \DIR\TEXT2!


Step 6 (option 2).  Launch the executable directly from the CD emulator.  The file "psx.exe" is already 
included in this distribution, and was included in the "hello.cti" file.    This is the method used when 
creating gold disks.

Resetps 1
run c:\ps\pssn\bin\cdexec.cpe

You should see the following words on your television monitor, on a red background:

	THE CURRENT BUFFER:  0
	FROM FILE 1: CONTENTS OF TEXT1.TXT. HI THERE!

	FROM FILE 2: GREETINGS FROM \DIR\TEXT2!


"psx.exe" was created from the "main.cpe" file using the "cpe2x" command by doing the following:

cpe2x /cA main.cpe
move main.cpe psx.exe

In the above example, the "/cA" option embeds information in the "psx.exe" file that the executable will run 
on American PlayStations.  If we had used "/cE", the executable would be only suitable for European 
PlayStations.  

To debug using this method, you will have to rely on "printf" statements in your code.   You can see the 
output using the TSR "mess1.com" and "testmess", included on your Programmer Tools CD.

Resetps 1
mess1.com		  Starts the printf handler 
run c:\ps\pssn\bin\cdexec
testmess			 Dumps out the printf's to your MS-DOS command prompt



Summary of the basic steps for the emulator
Let's review what we've learned.  You can launch the ".cpe" file from the MS-DOS prompt, which will 
allow you to debug the program using "dbugpsx".  Alternatively, you can launch the ".exe" file directly 
from the cd-emulator, which is also the method for launching the program from your gold disk and the 
"black box" (DTL-H2010 or the DTL-H2510, which are CD drives for the DTL-H2000/DTL-H2500 
development boards).  The following summarizes the steps:

To launch the ".cpe" file:

Launch the drivers for the development board and the emulator.
Create the program and data.
Create a CTI file.
Create a CD-ROM image  using  C:> buildcd *.cti -s2:1 (SCSI channel 2, partition 1)
Activate the partition, and view the partition's content  .  C:>cddisk  2 (SCSI channel 2)
Tell the dev boards to choose the CD-ROM emulator   using  C:>run selemu
Reset the hardware using  C:>resetps 1
For DTL-H2000 only, run the bug-fix to the ROM  using C:> run c:\ps\pssn\bin\snpatch.cpe
Execute the program  using C:>run *.cpe or C:>dbugpsx main /e


To launch the ".exe" file:
Launch the drivers for the development board and the emulator.
Create the program and data.
Create the .exe file using c:>cpe /cA *.cpe (for North America)
Create a CTI file.
Create a CD-ROM image  using  C:> buildcd *.cti -s2:1 (SCSI channel 2, partition 1)
Activate the partition, and view the partition's content  .  C:>cddisk  2  (SCSI channel 2)
Tell the dev boards to choose the CD-ROM emulator using  C:>run selemu.  
Reset the hardware using  C:>resetps 1
For DTL-H2000 only, run the bug-fix to the ROM  using C:> run c:\ps\pssn\bin\snpatch.cpe
Execute the program  using C:>run c:\ps\pssn\bin\cdexec

Building a CD
To build a CD, use the "CDGEN" program in conjunction with your burner.  (Note that "CDGEN" is not 
produced by SN Systems.)  You can also try using CutCD, which is included in the CD Emulator 
distribution diskette.  The following is a basic summary of the steps required to burn a CD using CGEN, 
taken from the Technical Reference CD in the document FAQ\CD4.pdf as well as other sources (using the 
Adobe Acrobat multi-document search tool on "buildcd", and Coombe's talk on "My Gold Disk Doesn't 
Work", which appears in the downloading section of the Web Site, under "Files:Documentation 
Updates:DVConf97.zip".):

Step 1.  Remove functions such as pollhost, Pcinit, Pcopen, Pclseek, Pcread, Pcwrite, Pcclose and  
PSYQpause.
Step 2. Make your program fit into two meg minus 64K used by the Rom kernel  
Remember, your development boards have 8 megabytes of RAM, but the PlayStation 
only has 2 megabytes.   Generate and check your map file to be sure executables fit if 
you are unsure. Check your malloc() calls to be sure they succeed. Remove references to 
memory outside the 2Meg memory map. Your 2Meg memory map is between 
0x80010000 (the 0x10000 is the 64K used by the kernel) and 0x801FFFFF. Use 
_ramsize and _stacksize(specify as static). Just because it compiles  does not mean it 
will fit in 2 Meg. That is up to you.
Step 3.  Perform a cpe2x /C[area] on the .cpe file.
Step 4.  Add files to the CDGEN.

a) Choose the correct file type for XA items.
?  Use Mode 2 Form 1 for game data.
?  Use Mode 2 Form 2 for XA files.
?  Select both Form 1 and Form 2 for a combined Audio and Video file.
?  Use the file type button to set it for each file.
?  Standard file is Mode 2 Form 1.

Step 5. Using the Additional Information dialog button in the volume panel of 
CDGEN, set  the System Area File to the path of your company's license.dat file. 
Example:

c:\cdgen\licenseA.dat

Note: You do not need to actually have "licenseA.dat" appear in the root directory of 
your CD-ROM image.  The information will be burned into the lead track (and hence 
become invisible).

Note: You can find the license files on the Programmer Tools CD  in  the directory 
"cdemu\LcsnFile".

Step 6. Using the Master dialog button in the Layout panel of CDGEN, set the License 
Area to:

? J if you have a Japanese debug station.
? A if you have an American debug station.
? E if you have an European debug station.

Step 7. Set the minutes to 74 minutes. However, you should use 71 minute media. A 74 
minute media might work but is not supported. You must use the CD-R71PS for the 
mastering process for submissions to SONY. 
Step 8.  Press the record (REC) button.  


Running the CD on your developer's CD-ROM 
Now that you've burned the gold disk, you can run it on the CD-ROM drives of your development boards.

Step 0. Make sure your DTL-H2000 or DTL-H2500 is hooked up, and that the CD-ROM drives are hooked 
up to the boards. The DTL-H2010 CD drive is used by the DTL-H2000, and the DTL-H2510 CD drive is 
used by the DTL-H2500.  Run dexbios (for DTL-H2000 boards) or h25bios (for DTL-H2500 boards) in 
your DOS console.
Step 1..  Place the CD (gold disk or commercial) into your DTL-H2010 or DTL-H2510  CD-ROM drives.
Step 2.  Notify the boards that you will be using the CD-ROM drive rather than the CD-Emulator:

	run c:\ps\psyq\bin\selcd.cpe

This selects the CD mode.

Step 3. Type "resetps 1" for good measure. 
Step 4. Type the following:

	run c:\ps\psyq\bin\cdexec

This should start the executable.   
Step 5: If you're curious about the cdexec.cpe file, look at \psx\sample\module\cdexec, which contains the 
few lines of code that launch the executable.

 
In general, if you have difficulties running the gold disk, recheck your work (Step 2 of "Building the CD").  
If it still doesn't work, then you should  reference Dave Coombe's talk on "My Gold Disk Doesn't Work", 
which appears in the downloading section of the Web Site, under "Files:Documentation 
Updates:DVConf97.zip".








 





