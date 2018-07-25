/////////////////////////////////////////////////////////////////////////////
// HELLO.CC
/////////////////////////////////////////////////////////////////////////////
// PC Host communication & file access sample program for PlayStation
//
// Written by Mike Fulton
// Last modified: 5:41pm, 1/29/97
//
// Copyright (c) 1997 Sony Computer Entertainment America
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <libsn.h>

/////////////////////////////////////////////////////////////////////////////

char string[] = "Written by PlayStation!\015\012";

/////////////////////////////////////////////////////////////////////////////

void main()
{
int fhand, bytes_read, bytes_written, filepos;
char buf[300];

// Initialize PC host communication

	PCinit();

// Open file for read/write access (file must be

	fhand = PCopen( "TESTFILE.TXT", 2, 0 );
	printf( "File handle for TESTFILE.TXT = %d\n", fhand );

// If file opened sucessfully

	if( fhand > 0 )
	{
		// Read 100 bytes from start of file
		
		bytes_read = PCread( fhand, buf, 100 );
		printf( "Tried to read 100 bytes, got %d bytes\n", bytes_read );
		printf( "Contents of buffer (1st 100 bytes of file) are:" );
		printf( "\n-------------------------\n" );

		// Null-terminate and display buffer

		buf[99] = 0;
		printf( buf );

		// Seek to end of file

		filepos = PClseek( fhand, 0, 2 );
		printf( "\n-------------------------\n" );
		printf( "PClseek returns file position of %d when seeking to EOF\n", filepos );

		// Write out a string of text to file

		bytes_written = PCwrite( fhand, string, strlen(string) );
		printf( "Tried to write %d bytes, wrote %d bytes\n", strlen(string), bytes_written );

		// Close file

		PCclose( fhand );
	}
	else
	{
		// If can't open, error is probably that file not
		// found, so print an error message to that effect.

		printf( "File /042TESTFILE.TXT/042 not found!\n" );
		printf( "Must be located in current directory on PC!\n" );
	}
}

