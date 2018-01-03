/*	Test file for Exception Handler */

#include "vsdie.h"

void
main(void)
{
	DWORD Temp;
	DWORD *pTemp;

	_EX_Init();		/* Install the Exception Handler */

	pTemp = (DWORD*)0x1;	/* An Odd Address... */
	Temp = *pTemp;			/* This should generate a load bus error */

	_EX_Quit();		
}	/*	End Main */
