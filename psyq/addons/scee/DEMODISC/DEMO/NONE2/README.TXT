This directory contains a replacement for none2.obj.

As you may know, if you link your code with none2.obj instead of libsn.lib,
it will not start properly, but another program can load and run a program
linked with none2.obj and regain control when the none2 program (child program)
has completed. The differences are that none2 doesn't set up a new stack
pointer, clear the bss, or re-initialise the heap, and at the end of the
program (ie when you fall out of main()), libsn does a break $1, whereas
none2.obj does a nop.

There is a problem though ! Newer versions of libsn have the hand variables
_ramsize, _stacksize (for artifically setting the sizes of these things),
and __heapbase, __heapsize, etc etc for telling where various segments of
your program are (for those who program in C) (like me). But none2.obj doesn't
have these variables.

Luckily I have constructed a bastard offspring of the two, none2.lib, which
doesn't do all the things none2 doesn't do (stack, heap, bss) and does do
some of the nice things libsn does (__heapbase, etc).

So now you can link up your child programs, but also tell where everything
is. Great.



