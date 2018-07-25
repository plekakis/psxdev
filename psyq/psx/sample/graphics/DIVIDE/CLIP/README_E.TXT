$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

clip: Sample program showing use of the Divide function

                       Copyright(C) 1995,1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	The Divide function (of libgte) inhibits texture distortion caused 
	by polygon division.  
	
	In this sample program, division is performed on the 
	POLY_FT4 flat texture quadrilateral using the DivideFT4() function.  

<How to Execute>
	> psymake all		compile
	> psymake load		load data
	> run tuto0		run

<Controller Button Assignment>
	Directional Buttons          : move up/down, left/right
	L1,L2                        : move front/back
	
	R1,R2                        : rotate
	circle,triangle,square,cross : rotate
	START + Directional Buttons  : change number of divisions
	SELECT + Directional Buttons : change picture area

<Remarks>

	The initial value for division depth is 2 (4 x 4 division) and the 
	initial size of the picture area is 540 (w) x 170 (h).

	To make the division process easy to understand, this sample program
	limits its operation to a region inside the screen, rather than using
	the full screen which is the normal picture area.
	
	In this example, the picture area is that area enclosed by the white
	line of the injector.  Division is not performed from outside this
	area up to the protruding section.
	
	Divided polygons are directly registered to the specified OT entry
	when the Divide function is called. They are registered to all of the
	same entries, independently of the otz, after division.
	
	The Divide function judges normal clipping only once before division.
	This means that large polygons that have approached a viewpoint are
	all clipped prior to divison.  This can produce situations where a
	section which is expected to be drawn is not. To prevent this from
	happening, use the RCpoly function instead of the Divide function as
	shown below.
	 

u_long *DivideFT4(v0,v1,v2,v3,uv0,uv1,uv2,uv3,rgbc,s,ot,divp)
SVECTOR *v0,*v1,*v2,*v3;
u_long *uv0,*uv1,*uv2,*uv3;
CVECTOR *rgbc;
POLY_FT4 *s;
u_long *ot;
DIVPOLYGON4 *divp;  

{
    int i;
    u_short sz;
    long p,flag;
    RVECTOR *r0,*r1,*r2,*r3;
    

    r0= &divp->r0;
    r1= &divp->r1;
    r2= &divp->r2;
    r3= &divp->r3;
    divp->cr[0].r0 = r0;		
    divp->cr[0].r1 = r1;		
    divp->cr[0].r2 = r2;		
    divp->cr[0].r3 = r3;		
    r0->v = *v0;
    r1->v = *v1;
    r2->v = *v2;
    r3->v = *v3;

    if(RotAverageNclip4
       (
	&r0->v,&r1->v,&r2->v,&r3->v,
	&r0->sxy,&r1->sxy,&r2->sxy,&r3->sxy,
	&p,&sz,&flag)>0 )
	{
	    ReadSZfifo4(&r0->sz,&r1->sz,&r2->sz,&r3->sz);
	    divp->ot = ot;
	    divp->rgbc = *rgbc;
	    divp->clut = (*uv0)>>16;
	    divp->tpage = (*uv1)>>16;
	    *(u_long *)(r0->uv) = *uv0;
	    *(u_long *)(r1->uv) = *uv1;
	    *(u_long *)(r2->uv) = *uv2;
	    *(u_long *)(r3->uv) = *uv3;
	    s =(POLY_FT4 *)RCpolyFT4(s,divp);
	}
    return((u_long *)s);
}
