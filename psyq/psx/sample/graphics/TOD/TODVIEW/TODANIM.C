/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	TOD functions (part2) */
/*
 *		Version 1.30	Apr, 17, 1996
 *		Version 1.31	Oct, 14, 1997
 *			- New branch "TOD_MATRIX" is added.
 *
 *		Copyright (C) 1995-1997 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "tod.h"

extern GsF_LIGHT pslt[3];		/* Light source*/

/*
 *  Prototype definitions*/
u_long *TodSetFrame();
u_long *TodSetPacket();

/*
 *  TOD process for 1 frame */
u_long *TodSetFrame(currentFrame, todP, objTblP, tmdIdP, tmdTblP, mode)
int currentFrame;	/* Current time*/
u_long *todP;		/* Current TOD*/
TodOBJTABLE *objTblP;	/* Object table*/
int *tmdIdP;		/* TMD ID list*/
u_long *tmdTblP;	/* TMD data addr*/
int mode;		/* Playing mode*/
{
	u_long hdr;
	u_long nPacket;
	u_long frame;
	int i;

	/* Get frame info.*/
	hdr = *todP;			/* get frame header*/
	nPacket = (hdr>>16)&0x0ffff;	/* Number of packet*/
	frame = *(todP+1);		/* time (frame number) */

	/* Wait until specified frame time */
	if(frame > currentFrame) return todP;

	/* Execute each packet in a frame */
	todP += 2;
	for(i = 0; i < nPacket; i++) {
		todP = TodSetPacket(todP, objTblP, tmdIdP, tmdTblP, mode);
	}

	/* Return next frame addr. */
	return todP;	
}

/*
 *  Execute single packet */
u_long *TodSetPacket(packetP, tblP, tmdIdP, tmdTblP, mode)
u_long *packetP;
TodOBJTABLE *tblP;
int *tmdIdP;
u_long *tmdTblP;
int mode;
{
	u_long *dP;		/* Current TOD addr*/

	u_long hdr;
	u_long id;
	u_long flag;
	u_long type;
	u_long len;

	/* Current object*/
	GsDOBJ2 *objP;		/* pointer to object*/
	GsCOORD2PARAM *cparam;	/* Coordinate value(RST)*/
	MATRIX *coordp;		/* Coordinate matrix*/
	GsDOBJ2 *parentP;	/* Parent object*/
	VECTOR v;
	SVECTOR sv;

	/* Dummy object*/
	GsDOBJ2 dummyObj;
	MATRIX dummyObjCoord;
	GsCOORD2PARAM dummyObjParam;


	/* Get Packet Info*/
	dP = packetP;
	hdr = *dP++;
	id = hdr&0x0ffff;	/* ID number */
	type = (hdr>>16)&0x0f;	/* Packet type (TOD_???) */
	flag = (hdr>>20)&0x0f;	/* Flags */
	len = (hdr>>24)&0x0ff;	/* Word lengthfor packet*/

	/* Search specified object from ID */
	objP = TodSearchObjByID(tblP, id);
	if(objP == NULL) {
		/* Use dummy object when not found */
		objP = &dummyObj;
		coordp = &dummyObjCoord;
		cparam = &dummyObjParam;
	}
	else {
		coordp = &(objP->coord2->coord);
		cparam = (objP->coord2->param);
		objP->coord2->flg = 0;
	}

	/* process by packet type*/
	switch(type) {
	    case TOD_ATTR:/* Update attribute*/
		objP->attribute = (objP->attribute&*dP)|*(dP+1);	
		dP += 2;
		break;

	    case TOD_COORD:/* Update Coordinate value */

		/* Update 'coordinate' member of current object
		   using 'cparam' member. */

		if(flag&0x01) {

			/* Differencial value*/

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx += (*(((long *)dP)+0))/360;
				cparam->rotate.vy += (*(((long *)dP)+1))/360;
				cparam->rotate.vz += (*(((long *)dP)+2))/360;
				dP += 3;
			}
			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx
				 = (cparam->scale.vx**(((short *)dP)+0))/4096;
				cparam->scale.vy
				 = (cparam->scale.vy**(((short *)dP)+1))/4096;
				cparam->scale.vz
				 = (cparam->scale.vz**(((short *)dP)+2))/4096;
				dP += 2;
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx += *(((long *)dP)+0);
				cparam->trans.vy += *(((long *)dP)+1);
				cparam->trans.vz += *(((long *)dP)+2);
				dP += 3;
			}

			RotMatrix(&(cparam->rotate), coordp);
			ScaleMatrix(coordp, &(cparam->scale));
			TransMatrix(coordp, &(cparam->trans));
		}
		else {
			/* Immediate value*/

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx = (*(((long *)dP)+0))/360;
				cparam->rotate.vy = (*(((long *)dP)+1))/360;
				cparam->rotate.vz = (*(((long *)dP)+2))/360;
				dP += 3;
				RotMatrix(&(cparam->rotate), coordp);
			}

			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx = *(((short *)dP)+0);
				cparam->scale.vy = *(((short *)dP)+1);
				cparam->scale.vz = *(((short *)dP)+2);
				dP += 2;
				if(!(flag&0x02))
					RotMatrix(&(cparam->rotate), coordp);
				ScaleMatrix(coordp, &(cparam->scale));
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx = *(((long *)dP)+0);
				cparam->trans.vy = *(((long *)dP)+1);
				cparam->trans.vz = *(((long *)dP)+2);
				dP += 3;
				TransMatrix(coordp, &(cparam->trans));
			}
		}
		break;

	    case TOD_MATRIX:
		*coordp = *(MATRIX *)dP;
		dP += 8;
		break;

	    case TOD_TMDID:	/* Link to TMD*/
		if(tmdTblP != NULL) {
			GsLinkObject4((u_long)TodSearchTMDByID(tmdTblP, tmdIdP,
					(unsigned long)(*dP&0xffff)), objP, 0);

		}
		break;

	    case TOD_PARENT:	/* Set parent*/
		if(mode != TOD_COORDONLY) {
			if((*dP == NULL)||(*dP == 0xffff)) {
				objP->coord2->super = NULL;
				dP++;
			}
			else {
				parentP = TodSearchObjByID(tblP, *dP++);
				objP->coord2->super = parentP->coord2;
			}
		}
		break;

	    case TOD_OBJCTL:
		/* in the objtable*/
		if(tblP != NULL) {
			switch(flag) {
			    case 0:
				/* Create new object*/
				TodCreateNewObj(tblP, id);
				break;
			    case 1:
				/* Delete object*/
				TodRemoveObj(tblP, id);
				break;
			}
		}
		break;

	    case TOD_LIGHT:
		/* Light source control*/
		if(flag&0x02) {
			if(flag&0x01) {
				pslt[id].vx += *(((long *)dP)+0);
				pslt[id].vy += *(((long *)dP)+1);
				pslt[id].vz += *(((long *)dP)+2);
			}
			else {
				pslt[id].vx = *(((long *)dP)+0);
				pslt[id].vy = *(((long *)dP)+1);
				pslt[id].vz = *(((long *)dP)+2);
			}
			dP += 3;
		}
		if(flag&0x04) {
			if(flag&0x01) {
				pslt[id].r += *(((u_char *)dP)+0);
				pslt[id].g += *(((u_char *)dP)+1);
				pslt[id].b += *(((u_char *)dP)+2);
			}
			else {
				pslt[id].r = *(((u_char *)dP)+0);
				pslt[id].g = *(((u_char *)dP)+1);
				pslt[id].b = *(((u_char *)dP)+2);
			}
			       dP++;
		}
		GsSetFlatLight(id, &pslt[id]);
		break;

	    case TOD_USER0:
		/* Describe user process here */
		break;
	}
	/* Return next packet addr*/
	return packetP+len;
}

