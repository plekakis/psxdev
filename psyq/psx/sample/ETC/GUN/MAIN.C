/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			gun: sample program *
 *		Copyright (C) 1996 by Sony Corporation
 *			All rights Reserved
 */
#include <r3000.h>
#include <asm.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libapi.h>
#include <libgun.h>

/*
 * Primitive Buffer
 */
#define OTSIZE		16			/* size of ordering table */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
} DB;

#define PADMAX        2
#define PADBUFFLEN    34
#define PADID_CON     0x41
#define PADID_GUN     0x31

typedef struct {
        u_short    v;
        u_short    h;
}HV_CNT;

typedef struct {
	char result;
	char id;
	char data[32];
}PAD;

typedef struct {
	char dummy;
	char count;
        HV_CNT	hv[20];
}GUN;

static GUN  	gun[2];
static PAD	pad[2];
static u_short	v_count[2];
static u_short	h_count[2];
static	LINE_F2 line[2][2];

static init_prim(DB *db);  /* preset unchanged primitive members */
static pad_read(DB *db);    /* parse controller */
static long ReadGun(long);
static u_short ReadGunX(GUN *),ReadGunY(GUN *);
static u_long _PadRead();

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */
	u_long	*ot;		/* current OT */
	int	cnt;	/* work */
	int i;
	
	ResetCallback();
	InitGUN((char *)&pad[0], PADBUFFLEN, (char *)&pad[1], PADBUFFLEN,(char *)&gun[0],(char *)&gun[1],20);
	StartGUN();
	ChangeClearPAD(0);

	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
	/* init font environment */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	init_prim(&db[0]);		/* initialize primitive buffers #0 */

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	cdb = db;
	while (1){
		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */

 		/* clear ordering table */
		ClearOTag(cdb->ot, OTSIZE);	
		
		/* update sprites */
		ot = cdb->ot;
		
		if (pad_read(cdb)&PADselect)
			break;
		for(i=0; i<2; i++)
		{
			setXY2(&line[i][0], h_count[i]-4, v_count[i],h_count[i]+4,v_count[i]);
			AddPrim(ot, &line[i][0]);
			setXY2(&line[i][1], h_count[i], v_count[i]-4,h_count[i],v_count[i]+4);
			AddPrim(ot, &line[i][1]);
		}
		FntPrint("X:%3d Y:%3d \n",h_count[0],v_count[0]);
		FntPrint("X:%3d Y:%3d \n",h_count[1],v_count[1]);
		FntFlush(-1);
		DrawOTag(cdb->ot);
		DrawSync(0);		/* wait for end of drawing */
		cnt = VSync(1);		/* check for count */
		VSync(0);		/* wait for V-BLNK */
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
	}
	StopGUN();
	RemoveGUN();
	StopCallback();
}

/*
 * Initialize drawing Primitives
 */

static init_prim(db)
DB	*db;
{
	int i;

	/* inititalize double buffer */
	for(i=0; i<PADMAX; i++)
	{
		(db+i)->draw.isbg = 1;
		setRGB0(&(db+i)->draw, 0, 0, 0);
		gun[i].count=0;
		h_count[i] = 100*(i+1);
		v_count[i] = 100;
		SetLineF2(&line[i][0]);
		SetLineF2(&line[i][1]);
		setRGB0(&line[0][i],255,0,0);
		setRGB0(&line[1][i],0,255,0);
	}
}	

/*
 * Read controll-pad
 */
static pad_read(DB *db)
{
	static	u_long	old_padd=0;
	u_long	padd = _PadRead();

	if(~old_padd & padd & (PADRleft|PADRleft<<16))
	{
		ClearImage(&db->draw.clip,255,255,255);
	}
	if(~old_padd & padd & (PADRleft<<16)) 
		SelectGUN(1,1);
	else
		SelectGUN(1,0);
	if(~old_padd & padd & PADRleft) 
		SelectGUN(0,1);
	else
		SelectGUN(0,0);
	old_padd = padd;
	return padd;
}		

static
u_long _PadRead()
{
        u_short		data[2];
        u_char          id;
        long		i;
	u_long		gdata;

        for(i=0; i<PADMAX; i++)
        {
                if(pad[i].result != 0)
                        id = 0;
                else
                        id = pad[i].id;
                switch(id)
                {
                        case PADID_GUN :
			gdata = ReadGun(i);
			if(gdata!=0)
			{
				h_count[i]=(gdata&0xffff);
				v_count[i] = (gdata>>16)&0xffff;
			}
                        case PADID_CON :
                                data[i] = (u_short)((((pad[i].data[0]&0xff)<<8))|(pad[i].data[1])&0xff);
				data[i] = ~data[i]&0xffff;
                                break;
                        default :
                                data[i] = 0;
                                id      = 0;
                }
        }
	return (data[0]&0xffff)|(((data[1])<<16)&0xffff0000);
}

static
long ReadGun(long n)
{
	if(gun[n].count == 0)
		return 0;
	gun[n].count = 0;
	return (ReadGunX(&gun[n])&0xffff)|((ReadGunY(&gun[n])<<16)&0xffff0000);
}

static
u_short ReadGunX(GUN *gun)
{
	return gun->hv[0].h/5;
}

static
u_short ReadGunY(GUN *gun)
{
	return gun->hv[0].v;
}
