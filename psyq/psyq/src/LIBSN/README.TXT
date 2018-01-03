FILES in this archive

LIBSN           <DIR>		directory containing source code for LIBSN
libsn    h         2,727	header file for Psy-Q Fn prototypes
libsn    lib      20,115	LIBSN main library file

---------------------------------------------------------
REVISION HISTORY


LIBSN dated 20-Sep-95

Updated to include Psy-Q replacement for library FlushCache routine.

The library version of flushcache has one minor bug in that
requires interrupts to be off before it is called.
This version which has been added to LIBSN.LIB is a replacement
function which does not have this problem. It is also considerably
faster.

Note that this version is declared in the new LIBSN.H:-

void	SNFlushCache(void);	/* fixed version of FlushCache() */

it has a different name to distinguish it from the version in the
other Playstation libraries.

