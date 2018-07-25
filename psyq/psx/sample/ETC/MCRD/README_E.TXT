$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

libmcrd Tutorial

                     Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.

tuto0:
======

<Descriptions>
  Card data in the following format is saved in slot 1 (left slot).

  Filename: "HAND"
  Number of blocks used: 1
  Animation: 3 frames (rock, paper, scissors)

<How to execute>
  psymake	compile
  run main	run


tuto1:
======

<Descriptions>
  Displays file data from card in slot 1.
  Checks continuously for the presence of a card. Handling is performed
  when a card is inserted or removed.

<How to execute>
  psymake	compile
  run main	run

<Remarks>
  If a card is inserted into slot 1, it is immediately recognized and a list of
  files is displayed.


tuto2:
======

<Descriptions>
  Selectively copies data from slot 1 to slot 2.

     * The insertion and removal of card is handled for both slots.
     * If the copy destination is an unformatted card, the user is queried
       and formatting is performed.
     * The available space in the copy destination is checked.
     * If an identically named file already exists in the copy destination,
       the user is queried and the file is overwritten.

<How to execute>
  psymake	compile
  run main	run

<Controller Button Assignment>
  Up Directional Button,
    Down Directional Button : select file
  Left Directional Button,
    Right Directional Button : select YES or NO
  Circle Button, Cross Button : execute

<Remarks>
  The sample code can be tested by inserting an unformatted card or a card
  filled to capacity instead of a regular card.

