/* ---------------------------------------------------------------------------
 * - (C) Sony Computer Entertainment. All Rights Reserved.
 * -
 * - Project:	Chrome Mapping Code V1.03
 * -
 * - Name:		main.c
 * -
 * - Author:		Dave Virapen
 * -
 * - Date:		29th May 1997
 * -
 * - Description:	The two main procedures to look at here are
 * - 			calc_chrome_uvs and changeTexture
 * - ------------
 * ---------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <libcd.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <stdlib.h>
#include "ctrller.h"
#include "main.h"
#include "profile.h"
#include "my_macs.h"

//#include <libsn.h>
//#include <libmath.h>



/* ---------------------------------------------------------------------------
 * - PRIVATE FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------
 */

static void InitSys(void); 
static void InitEnvs(DB *db);

static	int 	pad_read(MATRIX *world);
void		copy_matrix(MATRIX *A, MATRIX *B);
void		loadTIM(TEXTURE_INFO *info);
int		loadTMD_GT3(u_long *addr, OBJ_GT3 *obj, TEXTURE_INFO *info);
void		calc_chrome_uvs(MATRIX *trans, object_norms *norms, int x_offset, int y_offset);
void		changeTexture(OBJ_GT3 *ship);
void		change_RGBS(OBJ_GT3 *ship);
void		init_background(POLY_G4 *background);
void		alt_background(POLY_G4 *background, RGB_STRUCT *changes);

extern void	tmd_vertex_nor(OBJ_GT3 *object);

typedef struct {
	VECTOR		object_pos;
	SVECTOR		object_rot;
	MATRIX		object1; 
	VECTOR		object2_pos;
	SVECTOR		object2_rot;
	MATRIX		object2; 
	VECTOR		object3_pos;
	SVECTOR		object3_rot;
	MATRIX		object3; 
	VECTOR		cam_pos;
	SVECTOR		cam_rot;
}	WORLD_STUFF;

/* ---------------------------------------------------------------------------
 * - GLOBAL DEFINITIONS 
 * ---------------------------------------------------------------------------
 */

#ifdef FINAL
u_long _ramsize   = 0x00200000;				// Use 2MB for final.
u_long _stacksize = 0x00004000;
#else
u_long _ramsize   = 0x00800000;				// Use 8MB for development.
u_long _stacksize = 0x00008000;
#endif

// global database structures
DB		db[2];
DB		*cdb;

WORLD_STUFF	world_coords;

ControllerPacket	buffer1,buffer2;

int			oldSp;
static long		fIdP;
volatile long		tCPU;

main()
{
    /* world screen matrix */
    MATRIX		ws;

    VERT_F3		*vert2;
    POLY_GT3		*poly2;

    TEXTURE_INFO	texture[3];

    int 		i, num_polys;

    // Init the display and drawing environments.
    InitSys();
    InitEnvs((void *)db);
    // initialise the cdb pointer
    cdb = &db[0];

    db[0].background_alt.change0 = db[0].background_alt.change2 = db[1].background_alt.change0 = db[1].background_alt.change2 = +5;
    db[0].background_alt.change1 = db[0].background_alt.change3 = db[1].background_alt.change1 = db[1].background_alt.change3 = -5;

    texture[0].addr = TEXADDR;
    texture[1].addr = TEXADDR2;
    texture[2].addr = TEXADDR3;


    loadTIM(&texture[0]); 					// load the texture
    loadTIM(&texture[1]); 					// load the texture
    loadTIM(&texture[2]); 					// load the texture
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[0].spaceship, &texture[0]);	// now load the 1st tmd
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[0].spaceship2, &texture[1]);	// now load the 2nd tmd
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[0].spaceship3, &texture[2]);	// now load the 3rd tmd
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[1].spaceship, &texture[0]);	// same again
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[1].spaceship2, &texture[1]);	// same again
    loadTMD_GT3(SHIP_MODEL_ADDRESS, &db[1].spaceship3, &texture[2]);	// same again

    init_background(&db[0].background);
    init_background(&db[1].background);

    ProfileInit(1);					// initialise the profiler

    /* enable display */
    SetDispMask(1);
    PutDrawEnv(&db[0].draw);				// set drawenv 
    PutDispEnv(&db[0].disp);				// set dispenv 
    InitPAD( &buffer1, MAX_CONTROLLER_BYTES, &buffer2, MAX_CONTROLLER_BYTES);
    StartPAD();

    /* main loop */
    while (pad_read(&ws) == 0)  
    {
	cdb = (cdb==db)? db+1: db;		// swap the current database

	ClearOTagR(cdb->world_ot, OTSIZE);	// clear the ot
	ClearOTagR(cdb->ship2_ot, OTSIZE);	// clear the ot
	ClearOTagR(cdb->ship3_ot, OTSIZE);	// clear the ot

	calc_chrome_uvs(&world_coords.object1, &cdb->spaceship.ship_norms,
			cdb->spaceship.x_offset, cdb->spaceship.y_offset);		// calc the new u,v vals for chrome mapping
	changeTexture(&cdb->spaceship);		// apply the new u,v vals to each of the polys
	change_RGBS(&cdb->spaceship);

	calc_chrome_uvs(&world_coords.object2, &cdb->spaceship2.ship_norms,
			cdb->spaceship2.x_offset, cdb->spaceship2.y_offset);		// calc the new u,v vals for chrome mapping
	changeTexture(&cdb->spaceship2);		// apply the new u,v vals to each of the polys

	calc_chrome_uvs(&world_coords.object3, &cdb->spaceship3.ship_norms,
			cdb->spaceship3.x_offset, cdb->spaceship3.y_offset);		// calc the new u,v vals for chrome mapping
	changeTexture(&cdb->spaceship3);		// apply the new u,v vals to each of the polys

	alt_background(&cdb->background, &cdb->background_alt);

	SetTransMatrix(&world_coords.object1);
	SetRotMatrix(&world_coords.object1);



	/* add the spaceship to the ordering table */
	num_polys = cdb->spaceship.n;
	vert2 = &cdb->spaceship.vert[0];	// pointer to 1st vertex struct
	poly2 = &cdb->spaceship.poly[0];	// pointer to 1st poly struct

	for ( i = 0; i < num_polys; i++, vert2++, poly2++)
	{
	    // process each poly and add to the ordering table
	
	    rotTransPers3(cdb->world_ot, OTSIZE, poly2, &vert2->v0, &vert2->v1, &vert2->v2);
 
	}	/* end for i */

	SetTransMatrix(&world_coords.object2);
	SetRotMatrix(&world_coords.object2);

	/* add the spaceship to the ordering table */
	num_polys = cdb->spaceship2.n;
	vert2 = &cdb->spaceship2.vert[0];	// pointer to 1st vertex struct
	poly2 = &cdb->spaceship2.poly[0];	// pointer to 1st poly struct

	for ( i = 0; i < num_polys; i++, vert2++, poly2++)
	{
	    // process each poly and add to the ordering table
	
	    rotTransPers3(cdb->ship2_ot, OTSIZE, poly2, &vert2->v0, &vert2->v1, &vert2->v2);
 
	}	/* end for i */


	SetTransMatrix(&world_coords.object3);
	SetRotMatrix(&world_coords.object3);

	/* add the spaceship to the ordering table */
	num_polys = cdb->spaceship3.n;
	vert2 = &cdb->spaceship3.vert[0];	// pointer to 1st vertex struct
	poly2 = &cdb->spaceship3.poly[0];	// pointer to 1st poly struct

	for ( i = 0; i < num_polys; i++, vert2++, poly2++)
	{
	    // process each poly and add to the ordering table
	
	    rotTransPers3(cdb->ship3_ot, OTSIZE, poly2, &vert2->v0, &vert2->v1, &vert2->v2);
 
	}	/* end for i */

	AddPrim(cdb->world_ot+254, &cdb->background);
	FntFlush(-1);				/* for debug */
	
	tCPU = VSync(1) - tCPU;
	ProfileReadCount();			// check the profiler count
	ProfileAddOT(cdb->world_ot);		// add the cpu bar
	DrawSync(0);				// wait for all drawing to finish
						// must have DrawSync(0) between ProfileAddOT
						// and ProfileAddDrawOT
	ProfileAddDrawOT(cdb->world_ot);	// add the gpu bar
	FntPrint(fIdP, "~cf0fCPU Time: %d\n", tCPU);
	FntFlush(fIdP);

	VSync(0);
	
	PutDrawEnv(&cdb->draw);
	PutDispEnv(&cdb->disp);

//	cdb->ship2_ot[0] = cdb->ship3_ot+OTSIZE-1;

	DrawOTag(cdb->world_ot+OTSIZE-1);	
	DrawOTag(cdb->ship2_ot+OTSIZE-1);	
	DrawOTag(cdb->ship3_ot+OTSIZE-1);

	ProfileStartCount();		// restart the profiler
	tCPU = VSync(1);
    }											/* end main loop */
    /* close controller */
    PadStop();
    ResetGraph(1);
    StopCallback();
//    SetSp(oldSp);
    return;
}						/* end main */



/* Analysing PAD and calculating matrix */

static int pad_read(MATRIX *ws)
{
    /* playstation treats angles as 360 = 4096 */

    static int 		scale = 2*ONE;
    VECTOR 			scale_vec;

    int 			ret = 0;

	    if ( PadKeyIsPressed(&buffer1,PAD_LU) != 0)  
	    {
			world_coords.object_pos.vy -= 10;
	    }
	    if ( PadKeyIsPressed(&buffer1,PAD_LD) != 0)  
	    {
			world_coords.object_pos.vy += 10;
	    }
	    if ( PadKeyIsPressed(&buffer1,PAD_LL) != 0)  
	    {
			world_coords.object_pos.vx -= 10;
	    }
	    if ( PadKeyIsPressed(&buffer1,PAD_LR) != 0)  
	    {
			world_coords.object_pos.vx += 10;
	    }
	    if ( PadKeyIsPressed(&buffer1, PAD_FLB) !=0 ) 
	    {
	    	world_coords.object_pos.vz += 10;
	    } 
	    if ( PadKeyIsPressed(&buffer1, PAD_FLT) !=0 ) 
	    {
	    	world_coords.object_pos.vz -= 10;
	    } 
	    if ( PadKeyIsPressed(&buffer1,PAD_RU) != 0)  
	    {
	    	world_coords.object_rot.vx += 10;
	    	world_coords.object2_rot.vx -= 10;
	    	world_coords.object3_rot.vx -= 20;
	    }
	    if ( PadKeyIsPressed(&buffer1,PAD_RD) != 0)  
	    {
	    	world_coords.object_rot.vx -= 10;
	    	world_coords.object2_rot.vx += 10;
	    	world_coords.object3_rot.vx += 20;
	    }
	  
	    if ( PadKeyIsPressed(&buffer1,PAD_RL) != 0)
	    {
			world_coords.object_rot.vy		-= 10;
			world_coords.object2_rot.vy		+= 10;
			world_coords.object3_rot.vy		+= 20;
	    }
	    if ( PadKeyIsPressed(&buffer1,PAD_RR) != 0)  
	    {
			world_coords.object_rot.vy		+= 10;
			world_coords.object2_rot.vy		-= 10;
			world_coords.object3_rot.vy		-= 20;
	    }
	    if ( PadKeyIsPressed(&buffer1, PAD_FRB) !=0 )  
	    {
			world_coords.object_rot.vz += 10;
			world_coords.object2_rot.vz -= 10;
			world_coords.object3_rot.vz -= 20;
	    }
	    if ( PadKeyIsPressed(&buffer1, PAD_FRT) !=0 )  
	    {
			world_coords.object_rot.vz -= 10;
			world_coords.object2_rot.vz += 10;
			world_coords.object3_rot.vz += 20;
	    }

	    if ( PadKeyIsPressed(&buffer1,PAD_SEL) != 0)  
	    {
			world_coords.object_rot.vx  = 600;
			world_coords.object_rot.vy  = 0;
			world_coords.object_rot.vz  = 0;
			world_coords.object_pos.vx  = 0;
			world_coords.object_pos.vy  = 0;
			world_coords.object_pos.vz  = SCR_Z; 

			world_coords.object2_rot.vx  = 600;
			world_coords.object2_rot.vy  = 0;
			world_coords.object2_rot.vz  = 0;
			world_coords.object2_pos.vx  = 128;
			world_coords.object2_pos.vy  = -40;
			world_coords.object2_pos.vz  = 768;

			world_coords.object3_rot.vx  = 600;
			world_coords.object3_rot.vy  = 0;
			world_coords.object3_rot.vz  = 0;
			world_coords.object3_pos.vx  = -128;
			world_coords.object3_pos.vy  = 40;
			world_coords.object3_pos.vz  = 768;
	    }

	    if ( PadKeyIsPressed(&buffer1,PAD_START))  
	    {
	    }


	world_coords.object_rot.vy		+= 40;
	world_coords.object2_rot.vy		-= 80;
	world_coords.object3_rot.vy		-= 60;

	setVector(&scale_vec, scale, scale, scale);
	RotMatrixYXZ(&world_coords.object_rot, &world_coords.object1);
	TransMatrix(&world_coords.object1, &world_coords.object_pos);
	ScaleMatrix(&world_coords.object1, &scale_vec);

	RotMatrixYXZ(&world_coords.object2_rot, &world_coords.object2);
	TransMatrix(&world_coords.object2, &world_coords.object2_pos);
	ScaleMatrix(&world_coords.object2, &scale_vec);

	RotMatrixYXZ(&world_coords.object3_rot, &world_coords.object3);
	TransMatrix(&world_coords.object3, &world_coords.object3_pos);
	ScaleMatrix(&world_coords.object3, &scale_vec);


    return(ret);
}

void calc_chrome_uvs(MATRIX *trans, object_norms *norms, int x_offset, int y_offset)
{
    /* this procedure applies the objects rotation matrix to each of
       the vertex normals storing the result in an array
    */

    MATRIX	temp;
    SVECTOR	v;
    SVECTOR	*norm, *uv_vec;

    VECTOR	result;
    int		num_norms, i, offset_x, offset_y;

    offset_x = x_offset;
    offset_y = y_offset;

    num_norms = norms->n;
    norm = &norms->normal[0];	// pointer to the 1st normal in the normal array
    uv_vec = &norms->rot_norm[0];	// pointer to the 1st element of the results array

    for (i = 0; i < num_norms; i++, norm++, uv_vec++)
    {
	copy_matrix(trans, &temp);
	copyVector(&v, norm);
	gte_ApplyMatrix(&temp, &v, &result);		// multiply the normal by the objects rotation matrix
//	shift_vec(&result, 6);				// shift the vector down by 8 bits so the value is between 0 and 32
	shift_vec(&result, 8);			// shift the vector down by 8 bits so the value is between 0 and 32
	result.vx = 32 - result.vx + offset_x;
	result.vy = 32 - result.vy + offset_y;
//	result.vz = 32 - result.vz;
//	result.vx = 128 - result.vx;
//	result.vy = 128 - result.vy;
	copyVector(uv_vec, &result);
    }
}


void changeTexture(OBJ_GT3 *ship)
{
/* this procedure should allow chrome mapping ie wrapping
   a texture round a model depending on it's orientation
   using the values calculated by calc_chrome_uvs 	  */

	int	index0, index1, index2, num_polys, i;
	POLY_GT3	*poly;
	VERT_F3		*vert;
	object_norms	*norm_struct;

	num_polys = ship->n;
	poly = &ship->poly[0];		// pointer to the 1st element of the poly array
	vert = &ship->vert[0];		// pointer to the 1st element of the vertex array
	norm_struct = &ship->ship_norms; // pointer to the location of the normal structer for the model

	for	(i = 0; i < num_polys; i++, poly++, vert++)
	{
		// get the index into the normal array
		index0 = vert->ni0;
		index1 = vert->ni1;
		index2 = vert->ni2;

		// use the index to get the precalculated values from the normal structure
		poly->u0 = norm_struct->rot_norm[index0].vx;
		poly->v0 = norm_struct->rot_norm[index0].vy;
		poly->u1 = norm_struct->rot_norm[index1].vx;
		poly->v1 = norm_struct->rot_norm[index1].vy;
		poly->u2 = norm_struct->rot_norm[index2].vx;
		poly->v2 = norm_struct->rot_norm[index2].vy;

/*		setRGB0(poly, norm_struct->rot_norm[index0].vx<<3,
				norm_struct->rot_norm[index0].vy<<3,
				norm_struct->rot_norm[index0].vz<<3);

		setRGB1(poly, norm_struct->rot_norm[index1].vx<<3,
				norm_struct->rot_norm[index1].vy<<3,
				norm_struct->rot_norm[index1].vz<<3);

		setRGB2(poly, norm_struct->rot_norm[index2].vx<<3,
				norm_struct->rot_norm[index2].vy<<3,
				norm_struct->rot_norm[index2].vz<<3);
*/
	} 

}

void	change_RGBS(OBJ_GT3 *ship)
{
/*	this procedure is me being silly and adding a rainbow
	gouraud effect for absolutly no reason at all */


	int 			index0, index1, index2, num_polys, i;
	POLY_GT3		*poly;
	VERT_F3			*vert;
	object_norms	*norm_struct;

	num_polys = ship->n;
	poly = &ship->poly[0];		// pointer to the 1st element of the poly array
	vert = &ship->vert[0];		// pointer to the 1st element of the vertex array
	norm_struct = &ship->ship_norms; // pointer to the location of the normal structer for the model


	for	(i = 0; i < num_polys; i++, poly++, vert++)
	{
		// get the index into the normal array
		index0 = vert->ni0;
		index1 = vert->ni1;
		index2 = vert->ni2;

		setRGB0(poly, norm_struct->rot_norm[index0].vx<<3,
				norm_struct->rot_norm[index0].vy<<3,
				norm_struct->rot_norm[index0].vz<<3);

		setRGB1(poly, norm_struct->rot_norm[index1].vx<<3,
				norm_struct->rot_norm[index1].vy<<3,
				norm_struct->rot_norm[index1].vz<<3);

		setRGB2(poly, norm_struct->rot_norm[index2].vx<<3,
				norm_struct->rot_norm[index2].vy<<3,
				norm_struct->rot_norm[index2].vz<<3);

	} 

}



void copy_matrix(MATRIX *A, MATRIX *B)
{
    /* this procedure copies the contents of matrix A to matrix B */

    int 	i,j;

    for (i = 0; i < 3; i++)  {
	B->t[i] = A->t[i];
	for (j = 0; j < 3; j++)  {
	    B->m[i][j] = A->m[i][j];
	}
    }
}



void alt_background(POLY_G4 *background, RGB_STRUCT *changes)
{
    //	changes the rgb vals of the gouraud background

    int	r, g, b;

    if (background->g0 >= 200)	changes->change0 = -5;
    if (background->g0 == 0)	changes->change0 = +5;
    if (background->r1 >= 200)	changes->change1 = -5;
    if (background->r1 == 0)	changes->change1 = +5;
    if (background->b2 >= 200)	changes->change2 = -5;
    if (background->b2 == 0)	changes->change2 = +5;
    if (background->g3 >= 200)	changes->change3 = -5;
    if (background->g3 == 0)	changes->change3 = +5;

    r = background->r0 - changes->change0;
    g = background->g0 + changes->change0;
    b = background->b0;

    setRGB0(background, r, g, b);

    r = background->r1 + changes->change1;
    g = background->g1;
    b = background->b1 - changes->change0;

    setRGB1(background, r, g, b);

    r = background->r2;
    g = background->g2 - changes->change0;
    b = background->b2 + changes->change2;

    setRGB2(background, r, g, b);

    r = background->r3 - changes->change0;
    g = background->g3 + changes->change3;
    b = background->b3;

    setRGB3(background, r, g, b);


}


void	loadTIM(TEXTURE_INFO *info)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(info->addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr) {	/* load CLUT (if needed) */
			LoadImage(image.crect, image.caddr);

			info->clut_x = image.crect->x;
			info->clut_y = image.crect->y;

//			printf("Clut X = %d , Clut Y = %d \n", image.crect->x, image.crect->y);
//			printf("Clut Width = %d , Clut= %d \n", image.crect->w, image.crect->h);

		}
		if (image.paddr) {	/* load texture pattern */
			LoadImage(image.prect, image.paddr);

			info->texture_x = image.prect->x;
			info->texture_y = image.prect->y;
			info->tpage	= image.prect->x>>6 + ((image.prect->y>>8) * 16);
			 // divide x by 64 to give tpage id then to see if 2nd row divide y by 256

//			printf("Texture X = %d , Texture Y = %d \n", image.prect->x, image.prect->y);
//			printf("Texture Width = %d , Height = %d \n", image.prect->w, image.prect->h);


		}
	}
}

/*
 * load TMD
 */


int		loadTMD_GT3(u_long *addr, OBJ_GT3 *obj, TEXTURE_INFO *info)
{
	VERT_F3		*vert;
	POLY_GT3	*prim;
	TMD_PRIM	tmdprim;
	int		col0, col1, col2, i, n_prim = 0;
	int		tpage_x, tpage_y, clut_x, clut_y, semi_trans;


	vert  = &obj->vert[0];
	prim = &obj->poly[0];

	tpage_x = (info->texture_x>>6)<<6;
	tpage_y = (info->texture_y>>8)<<8;
	clut_x  = info->clut_x;
	clut_y  = info->clut_y;
	semi_trans = info->semi_trans;

	obj->x_offset = info->texture_x - tpage_x;
	obj->y_offset = info->texture_y - tpage_y;
	
	/* open TMD */
	if ((n_prim = OpenTMD(addr, 0)) > OBJ_MAX_POLY)
		n_prim = OBJ_MAX_POLY;

	obj->n = n_prim;
	/*
	 * Set unchanged member of primitive here to deliminate main
	 * memory write access
	 */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {
		
		/* initialize primitive */
		SetPolyGT3(prim);

		prim->tpage = getTPage(1, 1, tpage_x, tpage_y);
		prim->clut  = getClut(clut_x, clut_y);

//		prim->tpage = getTPage(1 ,1 ,640, 0);
//		prim->clut = getClut(640 ,256);
		SetSemiTrans(prim, 1);
		
		/* copy normal and vertex */
		copyVector(&vert->poly_norm, &tmdprim.n0);

		copyVector(&vert->v0, &tmdprim.x0);
		copyVector(&vert->v1, &tmdprim.x1);
		copyVector(&vert->v2, &tmdprim.x2);

		// some crappy code to fudge gouraud

		col0 = (vert->v0.vx + vert->v0.vy + vert->v0.vz)/3;
		col1 = (vert->v1.vx + vert->v1.vy + vert->v1.vz)/3;
		col2 = (vert->v2.vx + vert->v2.vy + vert->v2.vz)/3;

		setRGB0(prim, col0, col0, col0);
		setRGB1(prim, col1, col1, col1);
		setRGB2(prim, col2, col2, col2);

		/* duplicate primitive for primitive double buffering
		 */  
//		memcpy(prim1, prim0, sizeof(POLY_F3));
		vert++, prim++;
	}

	tmd_vertex_nor(obj);		// calculate the tmd's vertex norms for the chrome mapping

	return(obj->n = i);
}
 

static void InitSys(void) {


/* - Type:	PRIVATE
 * -
 * - Usage: Init system. 
 */


    init_system(256, 128, SCR_Z, 1);

    fIdP = FntOpen(FRAME_X-210, 22, 150, 50, 0, 100);
    
    ResetCallback();
    ResetGraph(0);
    SetGraphDebug(0);

//    InitPAD( &buffer1, MAX_CONTROLLER_BYTES, &buffer2, MAX_CONTROLLER_BYTES);
//    StartPAD();

    SetBackColor(255, 255, 255);
    SetFarColor(0, 0, 0);

	// Video Mode.
#ifndef NTSC
	SetVideoMode(MODE_PAL);
#endif

}

static void InitEnvs(DB *db) {


/* - Type:	PRIVATE
 * -
 * - Usage: Init the drawing and display environments. Also init
 * -		the application and profiler FntPrint streams. 
 */


	// Init the display and drawing environments.
	SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);
	
	setRECT(&db[0].disp.screen, SCREEN_X, SCREEN_Y, 0, FRAME_Y);	
	setRECT(&db[1].disp.screen, SCREEN_X, SCREEN_Y, 0, FRAME_Y);	
	setRGB0(&db[0].draw, 0, 0, 0);
	setRGB0(&db[1].draw, 0, 0, 0);
	db[1].draw.isbg = db[0].draw.isbg = 0;


	// Init font environment.
/*	FntLoad(960, 256);	
	fIdA = FntOpen(18, 16, 310, 200, 0, 512);			// Applic stream.
	SetDumpFnt(fIdA);	
*/
}

void	init_background(POLY_G4 *background)
{

	SetPolyG4(background);
//	SetShadeTex(background, 1);
	setXYWH(background, 0, 0, 512, 256);
	setRGB0(background, 100, 100, 0);
	setRGB1(background, 100, 0, 100);
	setRGB2(background, 0, 100, 100 );
	setRGB3(background, 100, 100, 0);



}
