Included in this ZIP are the key components for the 
Demo Disc Boot Strap mechanism.

Essentially demo's must adhere to the DEMOG.DOC 
document, and reserve the first 32K of memory for
BS.

BS Is fairly simple ... on starting it invokes
PIRATE.EXE (to produce the required ELSPA and 
DISCLAIMER files).   It isn't big enough to
use CDsearchfile, and instead relies on the
DISC Layout being...

	<start>
	PIRATE.EXE
	DD.EXE		/* the front end program */
	
PIRATE.EXE returns control back to BS, which then 
invokes DD.

DD Has all the menu code, and allows the user to choose
a game, and then find out, and passes back all of the
information concerning the executable to BS, by putting
information in the data area (data is a pointer to the
address APPDATA_POS).

DD exits, returning control to BS, which looks in APPDATA_POS
and then fires up the game.   After the game exits, again
BS will call back DD...

We've provided a few code fragments - we use a similar 
mechanism for both DemoOne (the inbox disc) and the
Official PlayStation Magazine Disc.

Let us know (via a Private mail on our BBS) if you have
any questions.

This is a first cut, and no doubt will be improved.

Paul Holman,
Mgr, Developer Support.
