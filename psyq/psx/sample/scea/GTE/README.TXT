-------------------------------------------------
GTE Optimization Demo
by Mike Acton, Sony Interactive Studios America

Contact	  :  	macton@sonyinteractive.com
Publisher :  	SISA 
Submitted :  	20 Oct 97 

-----------------------
DESCRIPTION:
-----------------------
This example is suitable for  Advanced programmers
and can best be categorized as a
sample that demonstrates (GTE) Optimization. 
 


-----------------------
INSTRUCTIONS:
----------------------
To compile the sample program,  execute the following
commands:
 
	"psymake" (uses 'makefile.mak' default)
	or "maker"   (borland maker uses 'makefile' default)



In order to run the sample program,  execute the following
commands:
 
	resetps 1
	run main.cpe

You'll see the following on the screen:

	"CUBE GTE OPTIMIZATION DEMO" at the top/left corner
	and a spinning cube (GT4s) taking about 1/6 the screen.


---------------------------
EXPLANATION:
--------------------------
Cube GTE Optimization Demo

Visually there is nothing stunning being displayed. The intent is
to demonstrate a few optimization methods using the GTE. I chose a
cube since it was trivial to implement and it kept the examples
very clean. 

There are two source files - main.c and cube.c. All the optimization
happens within cube.c. 

The concepts demonstrated in cube.c are:

1. Using the "nop" time in a transform function.
   gte_rtpt is basically translated into a "nop" then a hw "rtpt" since
   there must be at least a one cycle delay between loading the gte
   registers and transforming them. But it's often better to fill that
   cycle with something useful (like an add) rather than a nop.

2. Using the transform time (in rtpt,specifically) to limit main ram wait
   time.
   The most significant optimization problem is the fact that main ram
   access is very slow. Limiting that is almost certainly for the better
   when it comes to speed. 

   A normal transform might look something like,
   gte_ldv3c(vp);
   gte_rtpt();
   gte_stsxy3c(sp);

   assuming *vp is in main ram this is interpreted as...
   gte register <- main ram (v0)
   gte register <- main ram (v1)
   gte register <- main ram (v2)
   nop
   rtpt
   wait
   main ram <- gte register (s0)
   main ram <- gte register (s1)
   main ram <- gte register (s2)

   in this situation there are two significant time hits:
   1. the main ram to gte register copies
   2. the rtpt wait

   there's nothing to be done about (2), but we can take advantage of
   that by reading main ram during the wait. 
   you can't modify the gte registers while they are being transformed,
   so we need to store the main ram data temporarily in cpu registers.
   this adds one extra step (gte register <- cpu register <- main ram)
   but it's worth it in time.

   the result of this is...

   gte register <- cpu register (v0)
   gte register <- cpu register (v1)
   gte register <- cpu register (v2)
   rtpt
   cpu register <- main ram (v0)
   cpu register <- main ram (v1)
   cpu register <- main ram (v2)
   main ram <- gte register (s0)
   main ram <- gte register (s1)
   main ram <- gte register (s2)
    

3. Transforming groups of shared vertecies effectively.  
   This is fairly obvious, but worth noting. It is significantly
   cheaper to transform groups of shared vertecies rather than
   tranform vertecies on a per-poly basis.   
 
   Doing this though introduces one additional step (assuming you
   are using gte_nclip). Since doing backface clipping requires the
   vertecies for the particular polygon to be loaded in the gte
   registers (in order), you will have to reload them yourself.

   in other words,
 
   a normal transform loop (for quads) might look something like,

   for each poly...

   gte_ldv3c(vp);
   gte_rtpt();
   gte_stsxy3c(&poly->x0)

   gte_ldv0(vp+3);
   gte_rtps();
   gte_stsxy(&poly->x3)

   gte_nclip(); ...


   we would change this to,

   for each three vertecies...

   gte_ldv3c(vp);
   gte_rtpt();
   gte_stsxy3c(sxy_table);

   I'll note here two things:
   1. if you can keep your shared vertex groups small enough, there is
      a significant speed advantage to having the sxy_table be in the
      dcache.
   2. it's almost always better to simply pad your vertecies so that
      they are a multiple of three - since a triple tranform is still faster
      than a double, and the branch checks each iteration of the loop 
      tend to outweigh the advantage of doing one single transform at the
      end.
   
   for each poly...

   gte_ldsxy0(sxy_table[polydef->ndx0]);  // you do have the main ram hit
   gte_ldsxy1(sxy_table[polydef->ndx1]);  // for looking up the index, but
   gte_ldsxy2(sxy_table[polydef->ndx2]);  // it is still WAY faster.

   gte_nclip(); ...
   
4. Using the ccpsx syntax to declare a variable as a specific register.
   This is not really an optimization, but a note: rather than hard-coding
   the cpu registers into macros (load cpu reg 16 (cpu_ld16),etc...) I
   have opted to use the 'register <type> <name> asm("$<regnum>")' to 
   declare (and name) a specific register and pass it as an argument
   to cpu register macros (cpumac.h).


That's about it.

Mike Acton
macton@sonyinteractive.com

