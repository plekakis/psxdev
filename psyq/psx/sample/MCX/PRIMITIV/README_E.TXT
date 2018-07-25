$PSDocId: Document Release 1.0 for Run-time Library Release 4.4.1$

Sample for PDA (libmcx)

                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<Description>

	  This program shows the way to call every function in libmcx.

	 * Explanation of Index on the Screen

	 1: ALL INFO		Test for McxAllInfo()
	 2: CURRENT CONTROL	Test for McxCurrCtrl()
	 3: FLASH ACCESS	Test for McxFlashAcs()
	 4: SET EXEC FLAG	Test for McxExecFlag()
	 5: EXEC APPLI		Test for McxExecApl()
	 6: GET APPLI		Test for McxGetApl()
	 7: SET TIME		Test for McxSetTime()
	 8: GET TIME		Test for McxGetTime()
	 9: SET LED		Test for McxSetLED()
	10: GET SERIAL NO	Test for McxGetSerial()
	11: WRITE DEVICE	Test for McxWriteDev()
	12: READ DEVICE		Test for McxReadDev()
	13: SHOW TRANS		Test for McxShowTrans()
	14: HIDE TRANS		Test for McxHideTrans()
	15: SET MEMORY		Test for McxSetMem()
	16: GET MEMORY		Test for McxGetMem()
	17: SET USER IFS	Test for McxGetUIFS()
	18: GET USER IFS	Test for McxGetUIFS()

	L1: Select next term.
	L2: Select previous term.
	START: Call function.
	SELECT: Finish this program.


	* Explanation of the Contents of Each Term

	- Commonly used messages

	NO PARAMETER NEEDED: No parameter is needed to run the function.

	PRESS START TO EXEC COMMAND: No data to display the result of   
			the test.  Press the Start button to acquire it.


	- Result message of the testing function (buttom of the screen)

	COMMAND SUCCEEDED: Succeeded in the communication with PDA.
	COMMAND SUCCEEDED (CARD CHANGED): Succeeded in communication,
		but the check flag has not been set or PDA was exchanged 
		from the last communication.
	CARD NOT CONNECTED: No memory card or PDA is found in front ports.
	COMMAND FAILED: Failed in communication.  (Not PDA but Memory Card 
		is inserted in the front slot, or PDA is defective.)


	
	*1: ALL INFO	Acquire all the PDA status information.

		CURRENT APPLICATION: xx
			PDA application number which is running. 
			(Set by 5:EXEC APPLI)
		PDA APPLI FLASH ACCESS : xxxxxx
			Gives priority to the FLASH write by PDA 
			application.
			(Set by 3:FLASH ACCESS)
		CURRENT CONTROL    SOUND: xx   INFRED: xx  LED: xx
			Limits power capacity.
			(Set by 2:CURRENT CONTROL)
		SERIAL NO. : xxxxxxxx
			Serial number of the PDA.
		DATA: 199x/xx/xx xxx xx:xx:xx
			Displays the present year, month, day, day of 
			the week, hour, minute and second.


	*2: CURRENT CONTROL	Limit functions in high current capacity. 

		R2: Speaker ON.
		R1: Speaker OFF.
		Triangle: Enable infrared transmission.
		Square:   Disable infrared transmission.
		Circle:   Enable LED.
		Cross:    Disable LED.


	*3: FLASH ACCESS	Give priority to the FLASH write by PDA 
				application.

		Circle: Give priority to FLASH.  (SUPERIOR)
		Cross:  Give priority to the communication with PS.
			(FLASH write enabled: INFERIOR)


	*4: SET EXEC FLAG	Set the flag (to distinguish between 
				Memory Card data and PDA application) 
				to FAT (file allocation table) data 
				of the specified file.

		Triangle: Increase application number.
		Square:   Decrease application number.
		Circle:   Set flag.  (PDA application)
		Cross:    Clear flag.  (Memory Card data)


	*5: EXEC APPLI		Call PDA application.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		Cross:    -1

		Set the application number (the block number for the 
		file stored in the Memory Card file system) to launch 
		and the argument to send to that application, then 
		press the Start button.


	*6: GET APPLI		Acquire the PDA application number 
				which is running now.

		CURRENT APPLICATION : xx
			The application number that was launched by 
			"5:EXEC APPLI" or other function calls.


	*7: SET TIME		Set real time clock.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		Cross:    -1


	*8: GET TIME		Acquire real time clock data.

		DATA: 199x/xx/xx xxx xx:xx:xx


	*9: SET LED		Turn on/off LED lamp.

		Circle: Turn the LED on.
		Cross: Turn the LED off.


	*10: GET SERIAL		Acquire serial number of the PDA.

		SERIAL NO. : xxxxxxxx


	*11: WRITE DEVICE	Write to the registered PDA device
				driver sub-routine.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		Cross:    -1

		Set the device driver number to call to "DEVICE NO: xx"
		section, the parameter to "FIX" section and the write 
		data to "VAR" section.  Press the Start button, then 
		the data in VAR section will be written to the device 
		driver sub-routine.  (System-reserved device drivers 
		are 0:RTC device, 1:PDA memory read/write device and 
		2:read/write User Interface Status.  User definable 
		device drivers have a driver number that is larger 
		than 0x80.)

		The length of the FIX section parameter varies depending 
		on the device driver.  The VAR section data varies 
		depending on the content of its FIX parameter.


	*12: READ DEVICE	Read out from the registered PDA device
				driver sub-routine.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		Cross:    -1

		Set the device driver number to call to "DEVICE NO : xx"
		section, the parameter to "FIX" section and the write 
		data to "VAR" section.  Press the Start button, then 
		the data from the device driver sub-routine will be 
		displayed on the screen.  (System-reserved device drivers 
		are 0:RTC device, 1:PDA memory read/write device and 
		2:read/write User Interface Status.  User definable 
		device drivers have a driver number that is larger than 
		0x80.)

		The length of the FIX section parameter varies depending 
		on the device driver.  The VAR section data varies 
		depending on the content of its FIX parameter.


	*13: SHOW TRANS		Start animation of transferring data 
				between PlayStation and PDA.

		Triangle: Set transmission direction from PS to PDA.
		Square:   Set transmission direction from PDA to PS.
		Circle:   Increase timeout limit.
		Cross:    Decrease timeout limit.


	*14: HIDE TRANS		Stop animation of transferring data
				between PlayStation and PDA.

		No result data to display.


	*15: SET MEMORY		Write to PDA memory.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		cross:    -1

		Set the write start address, the size of the data to write 
		and the contents of data, and then press the Start button.  
		The size of the data setting area varies depending on 
		the LEN setting. 

		When an address other than 0x0xxxxxx, 0x6xxxxxx and 
		0xA000000 to 0xDFFFFFF is included in the area where a write 
		access is made, no data is written.
		


	*16: GET MEMORY		Read out from PDA memory.

		Directional button: Select the location to change value.
		Triangle: +0x10
		Square:   -0x10
		Circle:   +1
		Cross:    -1

		Set the read start address and the size of the data to 
		read, and then press the Start button. 

		When an address other than 0x0xxxxxx, 0x2xxxxxx, 0x4xxxxxx, 
		0x6xxxxxx and 0xA000000 to 0xDFFFFFF is included in the area 
		where a read access is made, no data is read.

		When attempting to read 0x2xxxxxx without setting a virtual 
		Flash Memory, a bus error occurs to the PDA and it results 
		in stall.


	*17: SET USER IFS	Write to User I/F status of PDA.

		Directional button: Select the location to change value.

		[While ALARM TIME Selected]
		Triangle: +0x10
		Square: -0x10
		Circle: +1
		Cross: -1

		[Other Cases]
		R2: Alarm ringer ON.
		R1: Alarm ringer OFF.
		Triangle: Hold key lock.
		Square:   Release key lock.
		Circle:   Speaker off --> Low volume --> High volume
		Cross:    High volume --> Low volume --> Speaker off

	*18: GET USER IFS	Read out User I/F status of PDA.

		ALARM TIME:  xx:xx	Time to ring alarm.
		ALARM:		xx	Alarm ON/OFF status.
		KEY LOCK:	xx	Key lock status.
		SPEAKER:	xx	Volume level of speaker.
		AREA CODE:	xx	Area code.
		RTC SET:        xx	Flag for real time clock already set.
		FONT ADDRESS:	xx	Address to the font data.



<How to execute> 
	> psymake	:Build executable file.
	> run main	:Start program.
