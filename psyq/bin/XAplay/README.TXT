Multi-channel XA Audio player V1.1b - 16/11/1998
-------------------------------------------------------

Copyright 1998 Sony Computer Entertainment Europe

This program will play back a multi-channel XA audio file on a Windows PC.
The XA file must be in 2336 byte sector format.  This program will not read
an XA file direct from a PlayStation disc.


Required:

486-class PC
Windows 95, 98 or NT (not tested)
Sound card with 16-bit playback at 18.9KHz & 37.8KHz or 22.05Khz & 44.1KHz

Recommended:

Pentium
Sound Blaster 16 or AWE

Version history:

V1.1b
Fixed a bug where XAPlay would sometimes hang at the end of playback.
Fixed a bug when opening files.

V1.1a
Added resampling to allow the use of cheap sound cards which don't
support 18.9KHz & 37.8KHz playback.
Open dialog now remembers the path without having to quit.
Now works on Dave's PC! (Dave upgraded his drivers).

V1.0a
32-channel support!
Fixed a bug with channels ending at different times

Known problems with this version:

Channel changing only occurs when decoding of the next 6 sectors begins.
Position slider is only accurate to 6 sectors when re-positioning.
Audio pauses when moving the window.  BAD Windows.  BAD.

Thomas Daniel,
SCEE Technology Group
