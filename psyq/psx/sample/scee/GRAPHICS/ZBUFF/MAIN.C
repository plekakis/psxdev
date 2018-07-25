/***************************************************************
*                                                              *
*                         Z-Buffer Example                     * 
*															   *
*     Copyright (C) 1997 by Sony Computer Entertainment Europe *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley Nov/Dec 97                    *
*                                                              *
***************************************************************/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <inline_c.h>
#include <gtemac.h>
#include <libgpu.h>

#define SCR_Z	307		// distant to screen
#define OTLEN	8
#define OTSIZE	(1<<OTLEN)
#define START_POLYS 1000
#define MAX_POLYS 6000

// TMD is loaded into this structure before being manipulated for use in the OT
typedef struct
	{
	SVECTOR x0,x1,x2;
	unsigned char r0,g0,b0,code;
	unsigned char u0,v0;
	unsigned short clut;
	unsigned char u1,v1;
	unsigned short tpage;
	unsigned char u2,v2;
	unsigned short ot;
	} POLY;

// buffer structure
typedef struct
	{		
	u_long		ot[OTSIZE];		// ordering table
	POLY_F3		p[MAX_POLYS];   // model
	long tl;					// top line of background to redraw
	long bl;					// bottom line of background to redraw
	} DB;

extern unsigned long gtmd[];	// model
extern DR_LOAD bg[];			// background (DR_LOAD lists here)
extern unsigned long TIMaddr[];	// background TIM
extern unsigned long Zbuffer[];	// Z-buffer
extern unsigned long tex[];		// texture for model

VECTOR v;							// position vector
SVECTOR a = {0,0,0};				// angle of model
POLY polys[START_POLYS];			// polys of model (remain unchanged)
POLY copy_of_polys[START_POLYS];	// polys of model (get changed)
volatile short vsyncflag;			// wait for a vsync or not
volatile short ode;					// odd or even phase
long loopcnt = 0;					// number of times round main loop
volatile unsigned long pad;			// pad status
MATRIX vt;							// model transformation matrix
MATRIX t2;							// direction matrix
short abl[2];						// used previous b/g update bottom line
short obl[2];						// previous b/g update bottom line
short otl[2];						// previous b/g update top line
short d0,d1;						// drawn even and odd phase
char mu,md,ml,mr;					// movement flags

DB	db[3];			// buffers
DB	*cdb;			// current db 

// vsync callback routine, includes controls for movement of model and
// drawing of OT
static void vsync(void)
	{
	DB *ddb;			// draw db
	DR_LOAD *bgp = bg;	// background pointer

	if (cdb == db)
		ddb = db + 1;
	else if (cdb == db + 1)
		ddb = db + 2;
	else
		ddb = db;

	// controls, with some simple limits
	pad = PadRead(0);

	if ((pad & PADLup) && ((a.vy > 0 && a.vy < 800) || ml) &&
			((a.vy > 3100 && a.vy < 3900) || mr) &&
	 		(!(a.vy > 1024 && a.vy < 3072) || (v.vz > 250000 && mu && md)))
		applyVector(&v, t2.m[0][0], t2.m[0][1], t2.m[0][2], -=);

	if ((pad & PADLdown) && ((a.vy > 2048 && a.vy < 2848) || ml) &&
			((a.vy > 1052 && a.vy < 1852) || mr) &&
	 		((a.vy > 1024 && a.vy < 3072) || (v.vz > 250000 && mu && md)))
		applyVector(&v, t2.m[0][0], t2.m[0][1], t2.m[0][2], +=);

	if ((pad & PADRup) && mu)
		applyVector(&v, vt.m[1][0], vt.m[1][1], vt.m[1][2], -=);

	if ((pad & PADRdown) && md)
		applyVector(&v, vt.m[1][0], vt.m[1][1], vt.m[1][2], +=);

	if (pad & PADLright)
		a.vy = (a.vy + 16) & 0xfff;

	if (pad & PADLleft)
		a.vy = (a.vy - 16) & 0xfff;

 	// don't draw until buffer prepared
 	// only draw odd and even phase once for each buffer
 	if (loopcnt > 1 && (d0 == 0 || d1 == 0))
		{
		// repair previously altered links
		(bgp + ode * 25 + (abl[ode] >> 1) * 50 - 26)->tag =
					0x0b000000 | ((unsigned long)(bgp + ode * 25 +
					(abl[ode] >> 1) * 50) & 0xffffff);
		// place background into OT
		if (ddb->bl > obl[ode])
			abl[ode] = ddb->bl;
		else
			abl[ode] = obl[ode];

		(bgp + ode * 25 + (abl[ode] >> 1) * 50 - 26)->tag =
					0x0b000000 | ((unsigned long)&ddb->ot[OTSIZE-2] & 0xffffff);

		if (ddb->tl < otl[ode])
			ddb->ot[OTSIZE-1] = ((unsigned long)(bgp + ode * 25 +
								(ddb->tl >> 1) * 50) & 0xffffff);
		else
			ddb->ot[OTSIZE-1] = ((unsigned long)(bgp + ode * 25 +
								(otl[ode] >> 1) * 50) & 0xffffff);

		DrawOTag(ddb->ot + OTSIZE - 1);	// draw

		obl[ode] = ddb->bl;
		otl[ode] = ddb->tl;
		}

	if (ode == 0)
		d0 = 1;
	else
		d1 = 1;

	if (vsyncflag > 0)
		vsyncflag--;

	ode ^= 1;
	}


// make 2 lists of DR_LOAD primatives to draw the background for odd and even
// phases in interlaced mode
static void makebg()
	{
	short i,j,k;
	unsigned long *pic = TIMaddr + 5;	// b/g TIM pixel info address
	DR_LOAD *bgp = bg;					// b/g pointer
	RECT rect;							// LoadImage rectangle

	// each line is made up of 25 DR_LOADs
	for(j = 0; j < 480; j++)
		{
		// 24 DR_LOADs of 26 pixel length 
		for(i = 0; i < 24; i++)
			{
			setRECT(&rect, i * 26, j, 26, 1);
			SetDrawLoad(bgp,&rect);

			// send 16 words to GPU and point to next group of pixels
			bgp->tag = 0x10000000 | ((unsigned long)(bgp + 1) & 0xffffff);

			for(k = 0; k < 13; k++)
				bgp->p[k] = *pic++;

			bgp++;
			}

		// 1 DR_LOAD of 16 pixel length
		setRECT(&rect, i * 26, j, 16, 1);
		SetDrawLoad(bgp,&rect);

		// this DR_LOAD sends 11 words to GPU and points over the next line
		bgp->tag = 0x0b000000 | ((unsigned long)(bgp + 26) & 0xffffff);

		for(k = 0; k < 8; k++)
			bgp->p[k] = *pic++;

		bgp++;
		}
	}


// load a TMD made up of FT3's only, into a structure that can be used by
// zbuffering code
// IN: TMD address and structure pointer
// RETURN: number of polygons loaded
static short loadTMD_FT3(u_long *tmd, POLY *p)
	{
	TMD_PRIM tp;			// TMD data
	short i;
	short np = 0;			// number of primatives

	if ((np = OpenTMD(tmd, 0)) > MAX_POLYS) 
		np = MAX_POLYS;
	
	for (i = 0; i < np && ReadTMD(&tp) != 0; i++)
		{
		copyVector(&p->x0, &tp.x0);
		copyVector(&p->x1, &tp.x1);
		copyVector(&p->x2, &tp.x2);

		setRGB0(p, tp.r0, tp.g0, tp.b0);
		setRGB0(p,127,127,127);
		p->code = 0x26; 			// FT3 semi trans
		setUV3(p, tp.u0, tp.v0, tp.u1, tp.v1, tp.u2, tp.v2);
		p->tpage = tp.tpage;
		p->clut = tp.clut;
		p->ot = 0;
		
		p++;
		}
	return np;
	}

// load a TIM into it's given position in VRAM
// IN: TIM address
static void initTexture(unsigned long *timAddr)
	{
	unsigned long bnum;		// number of bytes
	RECT rect;				// LoadImage rectangle

	timAddr++;

	if (*timAddr & 8)				// check CLUT flag
		{
		timAddr++;					// load CLUT info

		bnum = *timAddr;

		timAddr++;

		rect.x = *timAddr & 0xffff;
		rect.y = *timAddr >> 0x10;

		timAddr++;

		rect.w = *timAddr & 0xffff;
		rect.h = *timAddr >> 0x10;

		timAddr++;

		LoadImage(&rect,timAddr);

		timAddr += (bnum >> 2) - 2;
		}
	else
		timAddr += 2;

	rect.x = *timAddr & 0xffff;		// load pixel info
	rect.y = *timAddr >> 0x10;

	timAddr++;

	rect.w = *timAddr & 0xffff;
	rect.h = *timAddr >> 0x10;

	timAddr++;

	LoadImage(&rect,timAddr);
	DrawSync(0);
	}

// Z-buffer originally in Alias format (distance from view point and 32bit),
// convert to a Z depth from view plane and 16bit
static void convert_zbuffer()
	{
	unsigned long zx;		// Z data
	unsigned char *ch;		// byte of Z data
	unsigned long *pp;		// pointer to Z data
	short *zb;				// converted Z value
	short i,j;

	pp = Zbuffer;
	pp += 2;

	zb = (short *)Zbuffer;

	for(i = 0; i < 240; i++)
		for(j = 0; j < 320; j++)
			{
			ch = (unsigned char *)pp++; // convert endian
			zx = *ch++;
			zx <<= 8;
			zx += *ch++;
			zx <<= 8;
			zx += *ch++;
			zx <<= 8;
			zx += *ch;

			*zb++ = (short)((SquareRoot0(((zx - 0x40000000) / 18668) *
					((zx - 0x40000000) / 18668) - (long)abs(120 - i) * 
					abs(120 - i) - (long)abs(160 - j) * abs(160 - j))) - 614);
			}
	}

int main(void)
	{
	short i,j,k;
	MATRIX t;				// transform matrix
	volatile short nptmd;	// number of primatives in TMD
	DRAWENV	drawenv;
	DISPENV	dispenv;
	VECTOR v2;				// translation vector

	ResetCallback();

	VSyncCallback(vsync);

	ResetGraph(0);				// reset graphic subsystem (0:cold,1:warm)
	SetGraphDebug(0);			// set debug mode (0:off, 1:monitor, 2:dump)
	
	PadInit(0);             	// initialise PAD

	InitGeom();					// initialise geometry subsystem
	SetGeomOffset(320, 240);	// set geometry origin
	SetGeomScreen(SCR_Z);		// distance to viewing-screen

	SetDefDrawEnv(&drawenv, 0, 0, 640, 480);  	// initialise environment
	SetDefDispEnv(&dispenv, 0, 0, 640, 480);

	dispenv.isinter = 1;		// interlaced
	drawenv.isbg = 0;			// don't clear background

	SetDispMask(1);				// enable display (0:inhibit, 1:enable)

	PutDrawEnv(&drawenv); 		// update drawing environment
	PutDispEnv(&dispenv); 		// update display environment

	// initial alignment of axis to scene
	vt.m[0][0] = -1522;	vt.m[1][0] = 0;		vt.m[2][0] = -3803;
	vt.m[0][1] = 0;   	vt.m[1][1] = 4096;	vt.m[2][1] = 0;
	vt.m[0][2] = 3803;	vt.m[1][2] = 0;		vt.m[2][2] = -1522;

	setVector(&v,0,0,512000);	// initialise model position/rotation
	setVector(&a,0,0,0);

	initTexture(TIMaddr);		// load TIMs
	initTexture(tex);

	nptmd = loadTMD_FT3(gtmd,polys);	// copy TMD to usable form

    for(i = 0; i < MAX_POLYS; i++)		// initialise primitives
		{
		SetPolyF3(&db[0].p[i]);
		SetPolyF3(&db[1].p[i]);
		SetPolyF3(&db[2].p[i]);
		}

	convert_zbuffer();			// make Z-buffer usable

	vsyncflag = 0;

	makebg();					// setup b/g

	ode = GetODE();				// current interlace phase

	do
		{
		if (cdb == db)						// swap current buffer
			cdb = db + 1;
		else if (cdb == db + 1)
			cdb = db + 2;
		else
			cdb = db;

		ClearOTagR(cdb->ot, OTSIZE);		// clear ordering table

		RotMatrix(&a,&t);					// positioning of model
		gte_MulMatrix0(&vt,&t,&t2);
		
		TransposeMatrix(&t2,&t2);
		gte_SetRotMatrix(&t);
		setVector(&v2,v.vx/1024,v.vy/1024,v.vz/1024);
		gte_SetTransVector(&v2);

		for(i = 0; i < nptmd; i++)			// copy polys
			copy_of_polys[i] = polys[i];

		d0 = d1 = 0;						// set phase drawn flags

		zdiv(&copy_of_polys[0], Zbuffer, &cdb->ot[0], &cdb->p[0], nptmd);

		while(vsyncflag > 1);

		mu = md = ml = mr = 1;

		// screen top and bottom of model
		cdb->tl = 307 * v2.vy / v2.vz - 33000 / v2.vz + 240;
		cdb->bl = 307 * v2.vy / v2.vz + 33000 / v2.vz + 240;
		
		if (cdb->tl < 0)
			mu = 0;
		if (cdb->bl > 480)
			md = 0;
		if (307 * v2.vx / v2.vz - 33000 / v2.vz + 320 < 0)
			ml = 0;
		if (307 * v2.vx / v2.vz + 33000 / v2.vz + 320 > 640)
			mr = 0;

		if (cdb->bl - cdb->tl < 40)
			{
			cdb->bl += 20;
			cdb->tl -= 20;
			}

		if (cdb->tl < 0)
			cdb->tl = 0;

		if (cdb->bl > 480)
			cdb->bl = 480;

		vsyncflag++;
		loopcnt++;

		DrawSync(0);	// wait for end of drawing
		}
	while (!(pad & PADselect));

    PadStop();

	VSyncCallback(0);
	StopCallback();
	ResetGraph(3);
	return 0;
	}
