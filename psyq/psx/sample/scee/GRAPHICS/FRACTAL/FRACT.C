/***************************************************************************
Fractal Landscape viewer.
By Jason Page. (c)1997 SCEE
Use Pad to move forward/back and rotate.

History:
	Used, and butchered the Tuto10.C as a template.

***************************************************************************/


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>


int Rnd=1965328;					/* Used for random numbers */

#define Width 96					/* Map width */
#define Depth 2048				/* Map depth */
#define MAXHEIGHT 128			/* Max height of mountains */
#define SEA_LEVEL 64				/* Height of sea level */

char Map[Depth*2][Width*2];	/* Fractal map height array */

/* Size of each map cell: */
#define BG_CELLX	32
#define BG_CELLY	32

/* max width and height of screen display: */
#define BG_WIDTH	1536
#define BG_HEIGHT	720
	
/* number of cells:  */
#define BG_NX		(BG_WIDTH/BG_CELLX)
#define BG_NY		(BG_HEIGHT/BG_CELLY)	

/* The depth of OT : */
#define OTSIZE		BG_NY

/* screen size: */
#define SCR_W		256
#define SCR_H		240
#define SCR_Z		32


int CameraSpeed=8;			/* Speed we move forward/back*/
int CamAngSpd=0;				/* Speed camera tilts */

int FRTab[BG_NY][BG_NX];	/* RGB arrays */
int FGTab[BG_NY][BG_NX];
int FBTab[BG_NY][BG_NX];

/*
 * Define structure to deal BG
 */

typedef struct {
	SVECTOR		*ang;		/* rotation: */
	VECTOR		*vec;		/* translation: */
	SVECTOR		*ofs;		/* offset on map: */
	POLY_G4	cell[BG_NY*BG_NX];	/* BG cells: BG */
} BG;

/*
 * Define structure to deal BG
 */

typedef struct {
	DRAWENV		draw;		/* drawing environment: */
	DISPENV		disp;		/* display environment: */
	u_long		ot[BG_NY];	/* OT: */
	BG		bg0;		/* BG 0 */
} DB;



void InitPrim(DB *db,int dr_x, int dr_y, int ds_x, int ds_y, int w, int h);
void InitBackground(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs);
void UpdateBackground(u_long *ot, BG *bg);
void ReadPad(BG *bg);
void InitSystem(int x, int y, int z);
void SetPolysRGB(int ix,int iy,int mx,int my);
void CreateFract (void);
void InitFract(void);
int ModifyRnd(int f);




/*****************************************************************************
main
	 This is it:
*****************************************************************************/
main()
{
	DB		db[2];	/* double buffer: */
	DB		*cdb;		/* current buffer: */

	InitSystem(SCR_W/2, SCR_H/2, SCR_Z);		/* initialize GTE: */
	
	/* Set initial value of packet buffers.
	   and make links of primitive list for BG. */

	InitPrim(&db[0], 0,     0, 0, SCR_H, SCR_W, SCR_H);
	InitPrim(&db[1], 0, SCR_H, 0,     0, SCR_W, SCR_H);

	SetDispMask(1);	/* enable to screen: */

	InitFract();				/* Initialise fractal map */
	CreateFract();				/* Create fractal map */

	cdb = db;
	do{
		ReadPad(&cdb->bg0);
		cdb = (cdb==db)? db+1: db;	
		
		ClearOTag(cdb->ot, OTSIZE);

		UpdateBackground(cdb->ot, &cdb->bg0);	/* Draw background */

		FntPrint("fractal landscape \n");
		FntPrint("by jason page \n");
		FntPrint("scee 1997 \n\n");
		FntPrint("sync %d\n",VSync(1));

		FntFlush(-1);
		DrawSync(0);
		VSync(0);
		
		/* swap double buffer draw: */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);	
	}while(1);
}


/*****************************************************************************
UpdateBackground
		Calc poly positions.
*****************************************************************************/
void UpdateBackground(u_long *ot, BG *bg)
{
int r,g,b;

	static SVECTOR	Mesh[BG_NY+1][BG_NX+1];

	MATRIX		m;
	POLY_G4	*cell;
	SVECTOR		mp;
	int j;
	
	int		tx, ty;			/* current absolute position: */
	
	int		mx, my;			/* current left upper corner of map: */
	
	int		dx, dy;			/* current relative position: */
	
	int		ix, iy;			/* loop counters */
	int		xx, yy;
	long		dmy, flg;

	/* current postion of left upper corner of BG */

	tx = (bg->ofs->vx)&((Width*32)-1);
	ty = (bg->ofs->vy)&((Depth*32)-1);

	mx =  tx/BG_CELLX;
	my =  ty/BG_CELLY;
	dx = (tx%BG_CELLX);
	dy = -(ty%BG_CELLY);

	PushMatrix();

	RotMatrix(bg->ang, &m);		/* calculate matrix: */
	TransMatrix(&m, bg->vec);
	SetRotMatrix(&m);				/* set matrix: */
	SetTransMatrix(&m);


	mp.vy = -BG_HEIGHT + dy;					/* Initial Y Position */

	for (iy = 0; iy < BG_NY+1; iy++)
	{
		mp.vx =- (BG_WIDTH/2 + dx)*4;
		mp.vy += BG_CELLY;
		for (ix = 0; ix < BG_NX+1; ix++)
		{
			xx = (mx+ix);
			yy = (my+iy)&(Depth-1);

			mp.vz=(Map[yy][xx]);
			if (mp.vz<SEA_LEVEL)					/* Flattern sea level */
				mp.vz=SEA_LEVEL;

			j=mp.vz;
			j-=SEA_LEVEL;
			j-=32;
			j*=24;
			mp.vz=-j;								/* mp.vz = poly height */

			RotTransPers(&mp, (long *)&Mesh[iy][ix], &dmy, &flg);
			mp.vx += BG_CELLX*4;
		}
	}

/*-------*/

	for (iy = 0; iy < BG_NY; iy++)
	{
		for (ix = 0; ix < BG_NX; ix++)
		{
			SetPolysRGB(ix,iy,mx,my);			/* Calculate RGB for each poly */
		}
	}

/*-------*/

	cell = bg->cell;
	for (iy = 0; iy < BG_NY; iy++)
	{
		for (ix = 0; ix < BG_NX; ix++)
		{
			/* check if mesh is in display area or not */
			if (Mesh[iy  ][ix+1].vx <     0) continue;
			if (Mesh[iy  ][ix  ].vx > SCR_W) continue;
			if (Mesh[iy+1][ix  ].vy <     0) continue;
			if (Mesh[iy  ][ix  ].vy > SCR_H) continue;

			setXY4(cell,
			       Mesh[iy  ][ix  ].vx, Mesh[iy  ][ix  ].vy,	/* Set XY pos */
			       Mesh[iy  ][ix+1].vx, Mesh[iy  ][ix+1].vy,
			       Mesh[iy+1][ix  ].vx, Mesh[iy+1][ix  ].vy,
			       Mesh[iy+1][ix+1].vx, Mesh[iy+1][ix+1].vy);


			r=FRTab[iy][ix];
			g=FGTab[iy][ix];						/* Set RGB's */
			b=FBTab[iy][ix];
			setRGB0(cell, r, g, b);
			r=FRTab[iy][ix+1];
			g=FGTab[iy][ix+1];
			b=FBTab[iy][ix+1];
			setRGB1(cell, r, g, b);
			r=FRTab[iy+1][ix];
			g=FGTab[iy+1][ix];
			b=FBTab[iy+1][ix];
			setRGB2(cell, r, g, b);
			r=FRTab[iy+1][ix+1];
			g=FGTab[iy+1][ix+1];
			b=FBTab[iy+1][ix+1];
			setRGB3(cell, r, g, b);

			AddPrim(ot+iy, cell);				/* Add to OT */
			cell++;
		}
	}
	PopMatrix();
}


/*****************************************************************************
InitFract
		Init fractal map, setting random heights at 32*32 offsets.
*****************************************************************************/
void InitFract(void)
{
int x;
int y;
int r=64;
int b;

	for (y=0;y<Depth/32;y++)
	{
		for (x=0;x<Width/32;x++)
		{
			r=ModifyRnd(-1);
			Map[y*32][x*32]+=r;
			if (Map[y*32][x*32]>MAXHEIGHT)
				Map[y*32][x*32]=MAXHEIGHT;
		}
	}
}


/*****************************************************************************
CreateFract
		Create fractal map using seed points.
*****************************************************************************/
void CreateFract (void)
{
int x;
int y;
int f;
int xs;
int ys;
int xhs;
int yhs;
int Size;

	Size=32;
	while(Size!=0)
	{
		for (y=0;y<Depth;y+=Size)
		{
			for (x=0;x<Width;x+=Size)
			{
				xs=x+Size;
				ys=y+Size;
				xhs=x+(Size/2);
				yhs=y+(Size/2);

				f=Map[y][x];
				f+=Map[ys][x];
				f+=Map[ys][xs];
				f+=Map[y][xs];
				f/=4;

				Map[yhs][xhs]=ModifyRnd(f);
				Map[y][xhs]=ModifyRnd(f);
				Map[yhs][x]=ModifyRnd(f);
				if (ys>=Depth)
					Map[ys][xhs]=ModifyRnd(f);
			}
			Map[yhs][xs]=ModifyRnd(f);
		}
	Size/=2;
	}
}


/*****************************************************************************
SetPolysRGB
		Set each poly's RGB, depending on:
		a) Is it water?
		b) Angle to light source
		c) Distance from camera
*****************************************************************************/
void SetPolysRGB(int ix,int iy,int mx,int my)
{
int xx,yy;
int f2,f,r,g,b;

	xx = (mx+ix);//&(Width-1);
	yy = (my+iy)&(Depth-1);

	f=Map[yy][xx];	
	if (f<SEA_LEVEL)
	{
		r=0;
		g=0;
		b=(f*4);							/* Water - blue */
	}
	else
	{
		r=f;								/* Rocks */
		g=f/4;
		b=f/2;
		f2=Map[yy][xx-1]-f;			/* Add lighting */
		r+=f2*8;
		g+=f2*8;
		b+=f2*8;
		f2=Map[yy+1][xx]-f;
		g+=f2*4;
	}

	r-=((BG_NY-iy)*2);				/* Add distance lighting */
	g-=((BG_NY-iy)*2);
	b-=((BG_NY-iy)*3);

	if (r>255)							/* Limit RGB */
		r=255;
	else if (r<0)
		r=0;
	if (g>255)
		g=255;
	else if (g<0)
		g=0;
	if (b>255)
		b=255;
	else if (b<0)
		b=0;

	FRTab[iy][ix]=r;					/* Store RGB */
	FGTab[iy][ix]=g;
	FBTab[iy][ix]=b;
}


/*****************************************************************************
ReadPad
		Update player controls and dampen inertia.
*****************************************************************************/
void ReadPad(BG *bg)
{
u_long	padd;
int an2;
	
	padd = PadRead(1);

	bg->ofs->vy -= CameraSpeed;
	bg->ofs->vy &= ((Depth*32)-1);

	if(padd & PADLup)	CameraSpeed++;				/* Modify Speed */
	else if(padd & PADLdown)	CameraSpeed--;


	an2=bg->ang->vy;							/* Set Camera Angle Speed */
	if ((padd & PADLleft)&&(an2<256))
	 	CamAngSpd+=2;
	else if ((padd & PADLright)&&(an2>-256))
		CamAngSpd-=2;
	else if (CamAngSpd>0)							/* Damp Angle Speed if no L/R */
		CamAngSpd-=1;
	else if (CamAngSpd<0)
		CamAngSpd+=1;

	if (CamAngSpd>31)					/* Limit angle speed */
		CamAngSpd=31;
	else if(CamAngSpd<-31)
		CamAngSpd=-31;

	if (CameraSpeed>47)					/* Limit Speed */
		CameraSpeed=47;
	else if(CameraSpeed<-47)
		CameraSpeed=-47;

	bg->ang->vy +=CamAngSpd;		/* Limit camera angle / damp speed */
	if (bg->ang->vy >255)
		CamAngSpd-=2;
	else if (bg->ang->vy <-255)
		CamAngSpd+=2;
}


/*****************************************************************************
ModifyRnd
		Requires:
					n		-ve returns rand 0-255.
							+ve returns n+/- 0-4.
*****************************************************************************/
int ModifyRnd(int f)
{
int r;

	Rnd+=54321;

	if (f<0)
		f=Rnd&255;				/* Return 0-255 */
	else
	{
			Rnd+=f;				/* Add a bit of randomness otherwise.. */
			r=Rnd&3;
			r-=8;
			if (r>=0)
				r++;				/* r = -4 -> +4 */
			f+=r;					/* Original number +/- 0-4 */
			if (f<0)				/* Limit value */
				f=0;
			else if (f>MAXHEIGHT)
				f=MAXHEIGHT;
	}
	return (f);	
}


/*****************************************************************************
InitBackground
	Initialises location data. Generates primitive list.

	Requires:
		BG		*bg,	BG data: 
		int		x,y	location on screen: 
		VECTOR	*vec	translation vector: 
		SVECTOR	*ofs	map offset: 
*****************************************************************************/
void InitBackground(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs)
{
	POLY_G4	*cell;
	int		i, x, y, ix, iy;
	u_char		col;

	/* set location data: */
	bg->ang = ang;
	bg->vec = vec;
	bg->ofs = ofs;

	/* generate primitive list */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++)
	{
		for (ix = 0; ix < BG_NX; ix++, cell++)
		{
			SetPolyG4(cell);	
		}
	}
}


/*****************************************************************************
InitPrim
		Initialize prim double buffers.

		Requires:
			DB	*db,		primitive buffer:
			int	dr_x, dr_y	drawing area location:
			int	ds_x, ds_y	display area location:
			int	w,h		drawing/display  area:
*****************************************************************************/
void InitPrim(DB *db,int dr_x, int dr_y, int ds_x, int ds_y, int w, int h)
{
	/* Buffer BG location 
	 * GTE treat angles like follows: */

	static SVECTOR	ang = {-907, 0,       0};
	static VECTOR	vec = {0,      SCR_H/2, SCR_Z/2};
	static SVECTOR	ofs = {0,      2020*32,       0};

	/* set double buffer: */
	SetDefDrawEnv(&db->draw, dr_x, dr_y, w, h);
	SetDefDispEnv(&db->disp, ds_x, ds_y, w, h);

	/* set auto clear mode for background : */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* initialize for BG: */
	InitBackground(&db->bg0, &ang, &vec, &ofs);
}


/*****************************************************************************
InitSystem
			Init everything to get us up and running
*****************************************************************************/
void InitSystem(int x, int y, int z)
{
	PadInit(0);		
	
	/* reset graphic subsystem */
	ResetGraph(0);		
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
	
	/* set debug mode (0:off, 1:monitor, 2:dump) 
	SetGraphDebug(0);

	/* initialize geometry subsystem */
	InitGeom();			
	
	/* set geometry origin as (160, 120) */
	SetGeomOffset(x, y);	
	
	/* distance to veiwing-screen: */
	SetGeomScreen(z);		
}

