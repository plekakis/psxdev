#include <sys/types.h>
#include <libetc.h>
#include <libcd.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


/***********************************************\
  Author: Chia-Ming_Wang@sony.interactive.com
  Date: August 1997

  Shows how to read from the CD, using the function:

  void LoadFileFromCD(char *lpszFilename, void *dst )
  Example:
  
  	char buf[1024];
	MyLoadFileFromCD("\\MY_DATA\\MYFILE;1", buf); 

\****************************************************/




/**************************************************************\
		   IMPLEMENATION
\**************************************************************/

int  ReadFromCD(long nbytes , void *dst, int mode)
// "nbytes" = number of bytes to read
// "dst" = address in memory where items will be written.
{
  int nsection, cnt, nsector;
  unsigned char com;
  
  nsector = (nbytes+2047)/2048;

  com=mode;
  CdControlB( CdlSetmode, &mode, 0);
  VSync(3);

  // Start reading.
  CdRead(nsector, dst, mode);

  while ((cnt = CdReadSync(1, 0)) >0)
  	VSync(0);

  return (cnt);

}


void MyLoadFileFromCD(char *lpszFilename, void *dst )
{
  CdlFILE fp;  // File

  int i, cnt;

  // Try 10 times to find the file.
  for (i=0; i<10; i++)
  {
	if (CdSearchFile(&fp, lpszFilename)==0)
		continue;  // Failed read, retry.
  
  	// Set target position
  	CdControl(CdlSetloc, (u_char*) &(fp.pos), 0);

  	cnt = ReadFromCD(fp.size, (void*) dst, CdlModeSpeed);  
  }
}

