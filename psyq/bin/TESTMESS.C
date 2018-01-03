/*
** Fileserver Test Program.
**
** Run this on the Playstation to exercise fileserver functions
**
** Text output can be viewed in debugger message window
** or using the TESTMESS DOS program.
**
** Note: MESS1.COM TSR must be installed to view text output
*/ 
#include <libsn.h>
#define NUMCH 16


/* This is how you build your program for less than 8MB memory */
int   _ramsize=0x00200000;           /* 2MB for production PSX */
int _stacksize=0x00002000;           /* reserve 8K for stack   */

char buffer[32];

main()
{
	int	i,j,handle,len;

	for(;;)
	{
		handle=PCopen("c:\\autoexec.bat",0,0);

		do{
			len=PCread(handle,buffer,NUMCH);
			if(len)
			{
				for(i=0;i<len;i++)
				{
					PCwrite(-1,buffer+i,1);
					for(j=5000; j>0; j--) ;
				}
			}
			for(i=99999; i>0; i--) ;

			pollhost();

		} while(len>0);

		PCclose(handle);
	}
}

