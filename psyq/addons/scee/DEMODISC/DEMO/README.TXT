
These directories include the various bits and pieces you will need to rebuild
your program for use in the demo disk. Personally, I reckon this isn't too
hard a process (at the time of writing it has been done once already), but
if you have problems, feel free to ask.

There are 5 directories:

bs:             Bootstrap program source, and a readme. 
                This boots the machine, and will load and run your program 
                under the conditions it will have on the demo disk.

docs:		Contains an MS Word and Text version of the document
		describing how the playable game segments should be
		constructed.

none2:          This directory contains a reworking of the SCEI none2.obj,
                which is a replacement for libsn.lib, allowing your program
                to return to the calling program (ie bs.exe) properly.
                This version includes the libsn variables __heapbase et al.

setsp:          This directory contains 2 PC utilities for working with
                PlayStation .EXE files - dumpexe, which dumps the information
                from the XF_HDR info at the start of an EXE file, and setsp,
                which can be used to set the correct stack pointer in a
                PlayStation .EXE file (as the cpe2x converter does not, and
                the demo disk needs to know where the stack goes).

example:        This directory has an example program which runs under bs.exe,
                and shows how to retrieve arguments from the parent, and set
                up and close down the various subsystems without breaking
                anything.

IMPORTANT:
----------
1: Please note that for Demo 2 (which will be released around Winter 95) the
meaning and number of arguments passed to the demo segment has slightly
changed. Please make sure you are familiar with these alterations, otherwise
you're demo will not work! A description of the new standard can be found in
demog.doc.

2: If you haven't filled out the questionnaire about your game demo, please
do! I will have a lot of difficulty laying the disk out if you don't !

Thanks, and best of luck,

Allan.

                        
