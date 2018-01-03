Mike's Controller Demo
Version 1.1
November 15, 1996
Copyright (c) 1996 Sony Computer Entertainment America




Introduction
------------
This sample program is intended to demonstrate the various controllers
which are available for the Sony PlayStation game console.  It
recognizes a variety of controllers including the standard Sony
controller, analog joysticks, the Namco neGcon, MadCatz Steering Wheel,
the Sony Mouse, the Konami and Interact light guns, and more.

If the program sees an unknown controller type then it makes a minimal
attempt to deal with it.  It assumes a D-pad and buttons, and checks for
up to 4 channels of analog data.



Starting The Program
--------------------
First make sure your PlayStation development environment is configured
properly and then execute the program as you would any other PlayStation
program:

	run pad.cpe

If you have received a CD-ROM containing just the controller sample
program, then boot the disc.  Note that gold CD-ROM discs will work
properly only on a development PlayStation or on a development system, not
on a standard retail PlayStation.



Using The Program
-----------------
Using the program is very simple.  Simply plug in a controller and
manipulate it.  If the controller is one recognized by the program, then a
square sprite will be moved around on screen in response to the controller
actions.

The appearance of the sprite will depend on what type of controller is
used.  For example, the light gun sprite has a "G" on it.  The steering
wheel has a "W".  And so on.

If the controller type is not recognized by the program, then it's hard to
predict what the results will be.  If the controller includes a standard
D-PAD, then the program will try to deal with it in a similar manner to
the D-PAD on a standard Sony controller.

The program will display a colorful background with text and sprites
overlaid on top.  The text will indicate what types of controllers are
connected and what the current values returned by the controller.   The
sprites will indicate a "player" position based on the controller input.

Note that the default position for a sprite is the top left corner of the
display.  Depending on the overscan settings for your system, you may not
see the sprite at first.  Move the controller down and to the right and it
should become visible.  Certain controller types such as analog joysticks
default to a center position and sprite will be shown at the corresponding
screen position when the joystick is at rest.

Some controllers use analog pressure-sensitive buttons.  In such cases,
typically one button controls the X-axis position of a sprite while
another analog button controls the Y-axis position.  Experiment: press all
the buttons and see what happens.



Multi Tap Multi-Player Adapter
------------------------------
If the program detects that a Multi-Tap adapter is connected to either
port, it will be recognized and up to 4 controllers may be plugged in.
Please note the following limitations of the hardware and software:

* You cannot plug a Multi-Tap into another Multi-Tap.

* The light gun may only be plugged into slot "A" of a Multi-Tap



Light Gun Notes
---------------
In order for the light gun to be properly recognized on a development
system with the DTL-H2080 controller adapter box, a modification is
required.

The DTLH2080.BMP picture file shows the required modification.  It's
quite simple: a 1k ohm resistor must be soldered between two leads.  You
can probably do this yourself in less than 10 minutes, but if you don't
feel comfortable attempting it, then please contact Sony Developer
Support.



Source Code
-----------
The source code for the sample is provided in the SOURCE directory.  Note
that this program was written to use version 3.6 of the Sony PlayStation
libraries.  Earlier versions of the libraries could not use use a light
gun and the Multi Tap multi-player adapter in the same program.  Later
versions of the libraries should work without change, or with minimal
changes.

Note that the TIM texture map files have been included both as the
original TIM files and as C language files containing data declarations.
The C files are included by the main source file as a shortcut around
other means of loading the textures at runtime.



Contact Information
-------------------
Please direct comments or questions to:

Mike Fulton, R&D
Sony Computer Entertainment America
919 E. Hillsdale Ave.
Foster City, CA  94404
EMAIL: mfulton@interactive.sony.com

Developer Hotline: (415) 655-8181

