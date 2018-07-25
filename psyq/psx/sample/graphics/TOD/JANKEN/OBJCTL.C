/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *
 *	TOD functions (part1) */
/*
 *		Version 1.30	Apr,  17, 1996
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "tod.h"

/*
 *  Prototype definition */
void TodInitObjTable();
GsDOBJ2 *TodSearchObjByID();
GsDOBJ2 *TodCreateNewObj();
GsDOBJ2 *TodRemoveObj();
u_long *TodSearchTMDByID();

/*
 *  Initialize Object table */
void TodInitObjTable(tblP, objAreaP, objCoordP, objParamP, nObj)
TodOBJTABLE *tblP;		/* pointer to table*/
GsDOBJ2 *objAreaP;		/* object (DOBJ2) area*/
GsCOORDINATE2 *objCoordP;	/* local coordinate area*/
GsCOORD2PARAM *objParamP;	/* local coordinate parameter area*/
int nObj;			/* Maximum number of object */
{
	int i;

	tblP->nobj = 0;		/* number of available object*/
	tblP->maxobj = nObj;	/* maximum number of object*/
	tblP->top = objAreaP;	/* object area*/
	i = 0;
	for(i = 0; i < nObj; i++) {
		/* Initialize members*/
		objAreaP->attribute = 0x80000000;
		objAreaP->id = TOD_OBJ_UNDEF;
		objAreaP->coord2 = objCoordP;
		objAreaP->coord2->param = objParamP;
		objAreaP->tmd = NULL;
		objAreaP++;
		objCoordP++;
		objParamP++;
	}
}

/*
 *  Search object in the object table by ID */
GsDOBJ2 *TodSearchObjByID(tblP, id)
TodOBJTABLE *tblP;	/* pointer to object table*/
u_long id;		/* ID number for search*/
{
	GsDOBJ2 *objP;
	int i;

	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(id == objP->id) break;
		objP++;
	}
	if(i == tblP->nobj) {
		/* Not Found */
		return(NULL);
	}
	else {
		/* Return pointer */
		return(objP);
	}
}

/*
 *  Create new object in the object table */
GsDOBJ2 *TodCreateNewObj(tblP, id)
TodOBJTABLE *tblP;	/* pointer to object table*/
u_long id;		/* ID number to create*/
{
	GsDOBJ2 *objP;
	int i;

	/* Search undefined area in the table(ID=TOD_OBJ_UNDEF) */
	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(objP->id == TOD_OBJ_UNDEF) break;
		objP++;
	}

	/* Search deleted area */
	if(i < tblP->nobj) {
		objP->id = id;
		objP->attribute = 0;
		GsInitCoordinate2(WORLD, objP->coord2);
		objP->tmd = NULL;
		return(objP);
	}
	else {
		/* Add new object to bottom of the table */
		if(i < tblP->maxobj) {		/* Table is Full? */
			objP = tblP->top+tblP->nobj;
			tblP->nobj++;
			objP->id = id;
			objP->attribute = 0;
			GsInitCoordinate2(WORLD, objP->coord2);
			objP->tmd = NULL;
			return(objP);
		}
		else {
			/* Table is full*/
			return(NULL);
		}
	}
}

/*
 *  Delete object from object table */
GsDOBJ2 *TodRemoveObj(tblP, id)
TodOBJTABLE *tblP;	/* Object table*/
u_long id;		/* ID number to delete*/
{
	GsDOBJ2 *objP;
	int i;

	/* Search object*/
	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(objP->id == id) break;
		objP++;
	}

	/* Delete it*/
	if(i < tblP->nobj) {
		objP->id = TOD_OBJ_UNDEF;
		if(i == tblP->nobj-1) {
			while(objP->id == TOD_OBJ_UNDEF) {
				tblP->nobj--;
				objP--;
			}
		}

		/* return object addr*/
		return(objP);
	}
	else {
		/* retuen NULL when not found*/
		return(NULL);
	}
}

/*
 *  Search modeling data in TMD */
u_long *TodSearchTMDByID(tmdP, idListP, id)
u_long *tmdP;
int *idListP;
u_long id;
{
	int n;

	tmdP++;		/* Skip header*/
	n = *tmdP++;	/* Number of object*/

	while(n > 0) {
		if(id == *idListP) break;
		tmdP += 7;	/* next object*/
		idListP++;
		n--;
	}
	if(n == 0) {
		/* return NULL when not found*/
		return(NULL);
	}
	else {
		/* return TMD addr*/
		return(tmdP);
	}
}
