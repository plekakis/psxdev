--------------------------------------------------------------------
$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

              Memory card sample code    Memory Card Manager

                                   1997.05 (C)SCEI

[Overview]

This sample program uses a menu to perform various kinds of accesses
on the slot 1 (left) memory card.

     1. FILE LIST
Displays a list of the files saved on the card.

     2. INFO
Displays information on whether a card is present, formatting information,
etc.

     3. FORMAT
Formats a card.

     4. UNFORMAT (QUICK)
Takes a formatted card and returns it to an unformatted state. Initializes the
FAT only.

     5. UNFORMAT (QUICK)
Takes a formatted card and returns it to an unformatted state. Unformats all
sectors.

     6. CREATE 15 FILES
Saves 15 empty, 1-block files.

     7. EASY FORMAT TEST
Checks to see if a card is formatted.

     8. MAKE CARD DATA
Saves sample code data using the filename "HAND".

     9 EXIT
Terminates the program.


[Compiling the code]

Run psymake

     >psymake [RETURN]

[Executing the code]

     >resetps [RETURN]
     >run card [RETURN]  * Run patchj here if board requires patchj


