/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			    config
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,19,1995	suzu	
 *
 *			set basic configration
 *
 *	This function must be called before swapping double buffer
 */

#include "sys.h"

typedef struct {
	char	*label;			/* label */
	int	val;			/* target value */
	int	dval;			/* sensitivity */
	int	minval, maxval;		/* range */
} ITEM;
	
static ITEM item[] = {
	"draw triangle   ",	0,      1, 0, 1,
	"show map        ",	0,      1, 0, 1,
	"Fog Far         ",	0,   1000, 0, 32000,
	"Fog Near        ",	0,   1000, 0, 32000,
	"Div. Start Point",	0,    200, 0, 16000,
	"Div. Stop Size  ",	0,   1000, 0, 32000,
	"clip check      ",	0,      1, 0, 1,
	0,
};
	
void setMeshConfig(MESH *mesh, GEOMENV *genv)
{
	u_long	padd;
	int	id, n;
	DRAWENV	draw;
	DISPENV	disp;
	
	PutDispEnv(SetDefDispEnv(&disp, 0,  0, 320, 240));
	PutDrawEnv(SetDefDrawEnv(&draw, 0,  0, 320, 240));
	
	item[0].val = (mesh->debug&0x01)? 1: 0;
	item[1].val = (mesh->debug&0x02)? 1: 0;
	item[2].val = genv->fog_far;
	item[3].val = genv->fog_near;
	item[4].val = mesh->divz;
	item[5].val = mesh->size;
	item[6].val = mesh->clips->x == -SCR_X? 0: 1;
	
	while (PadRead(1));

	while (((padd = PadRead(1))&PADstart) == 0) {
		VSync(0);
		menu_update(item, padd);
	}


	while (PadRead(1));
	
	mesh->debug    = item[0].val? mesh->debug|0x01: mesh->debug&~0x01; 
	mesh->debug    = item[1].val? mesh->debug|0x02: mesh->debug&~0x02; 
	genv->fog_far  = item[2].val;
	genv->fog_near = item[3].val;
	mesh->divz     = item[4].val;
	mesh->size     = item[5].val;
	
	if (item[6].val) 
		setRECT(mesh->clips, -SCR_X/2, -SCR_Y/2, SCR_W/2, SCR_H/2);
	else
		setRECT(mesh->clips, -SCR_X, -SCR_Y, SCR_W, SCR_H);
	
	SetFogNearFar(genv->fog_near, genv->fog_far, SCR_Z);	
	areaClipZ(mesh->clips, mesh->clipw, genv->fog_far);
	divPolyClip(mesh->clips, mesh->size, mesh->ndiv);

}

static int	id = 0;
void menu_init(void)
{
	id = FntOpen(64, 80, 0, 0, 2, 256);	
}	

int menu_update(ITEM *item, u_long padd)
{
	static int	cp = 0, opadd = 0;	
	int i, ispress = 0;
	
	FntPrint(id, "    debug option    \n");
	FntPrint(id, "  R1/R2 ... +/- \n");
	FntPrint(id, "  start ... exit\n\n");
	for (i = 0; item[i].label; i++) {
		if (i == cp)	FntPrint(id, "~c888");
		else		FntPrint(id, "~c444");
		
		FntPrint(id, "%s %d\n", item[i].label, item[i].val);
	}
	if (opadd == 0 && (padd&PADLup))   cp--;
	if (opadd == 0 && (padd&PADLdown)) cp++;
	limitRange(cp, 0, i-1);
			
	if (opadd == 0 && (padd&(PADR1|PADL1))) {
		ispress = 1;
		item[cp].val += item[cp].dval;
	}
	if (opadd == 0 && (padd&(PADR2|PADL2))) {
		ispress = 1;
		item[cp].val -= item[cp].dval;
	}
	limitRange(item[cp].val, item[cp].minval, item[cp].maxval);

	opadd = padd;

	FntFlush(id);
	return(ispress? cp: -1);
}


