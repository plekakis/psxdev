/* ---------------------------------------------------------------------------
 * - (C) Sony Computer Entertainment. All Rights Reserved.
 * -
 * - Project:	Chrome Mapping Code V1.01
 * -
 * - Name:		tmd_shit.c
 * -
 * - Author:		Dave Virapen
 * -
 * - Date:		29th May 1997
 * -
 * - Description:	Don't bother to look at this code as it's just here ot
 * -			hack a tmd and calculate vertex normals etc.
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
#include "main.h"
#include "profile.h"
#include "my_macs.h"



void	tmd_vertex_nor(OBJ_GT3 *object)
{
    /* this is a tmd hacking procedure. In short it calculates each vertex
       normal by averaging each surrounding poly's normal
    */

    int		i, j, k, vc0, vc1, vc2, x, y, z, num_done, donev0, donev1, donev2;
    VECTOR	n0, n1, n2;
    SVECTOR	v0, v1, v2, centre, sn0, sn1, sn2;
    SVECTOR	norm_array0[20], norm_array1[20], norm_array2[20];
    SVECTOR 	beendone[200];

    num_done = 0;
    centre_point(object, &centre);

    for(i=0; i<object->n; i++) {
	copyVector(&v0, &object->vert[i].v0); 
	copyVector(&v1, &object->vert[i].v1); 
	copyVector(&v2, &object->vert[i].v2); 
	vc0 = vc1 = vc2 = 0;
	copyVector(&norm_array0[vc0], &object->vert[i].poly_norm);
	copyVector(&norm_array1[vc1], &object->vert[i].poly_norm);
	copyVector(&norm_array2[vc2], &object->vert[i].poly_norm);
	vc0++;	vc1++;	vc2++;
		
	donev0 = donev1 = donev2 = -1;	
	for (k=0; k<num_done; k++)	
	{
	    if (equal(&v0, &beendone[k]))
		donev0 = k;
	    if (equal(&v1, &beendone[k]))
		donev1 = k;
	    if (equal(&v2, &beendone[k]))
		donev2 = k;
	}	
	
		
	for(j=0; j<object->n; j++)
	{
	    if (j != i) {
		if ((donev0 == -1) && equal(&v0, &object->vert[j].v0))
		{
			copyVector(&norm_array0[vc0], &object->vert[j].poly_norm);
			vc0++;
		}	

		if ((donev1 == -1) && equal(&v1, &object->vert[j].v1))
		{
			copyVector(&norm_array1[vc1], &object->vert[j].poly_norm);
			vc1++;
		}	

		if ((donev2 == -1) && equal(&v2, &object->vert[j].v2))
		{
			copyVector(&norm_array2[vc2], &object->vert[j].poly_norm);
			vc2++;
		}	
	    }
	}

	// now we have all the surrounding norms we can get
	// the vertex normal

	n0.vx = n0.vy = n0.vz = 0;

	if (donev0 == -1)
	{
		for(x=0; x<vc0; x++)
		{
			n0.vx += norm_array0[x].vx; 
			n0.vy += norm_array0[x].vy; 
			n0.vz += norm_array0[x].vz; 
		}
		
		n0.vx = n0.vx/vc0;
		n0.vy = n0.vy/vc0;
		n0.vz = n0.vz/vc0;

		copyVector(&sn0, &n0);


		if (normal_ok(&sn0, &v0, &centre))
		{
			n0.vx = -n0.vx;
			n0.vy = -n0.vy;
			n0.vz = -n0.vz;
		} 
		donev0 = num_done;
		copyVector(&beendone[donev0], &v0);	
		num_done++;
		copyVector(&object->ship_norms.normal[donev0], &n0);
	}
				
	object->vert[i].ni0 = donev0;


	n1.vx = n1.vy = n1.vz = 0;

	if (donev1 == -1)
	{
		for(y=0; y<vc1; y++)
		{
			n1.vx += norm_array1[y].vx; 
			n1.vy += norm_array1[y].vy; 
			n1.vz += norm_array1[y].vz; 
		}
	
		n1.vx = n1.vx/vc1;
		n1.vy = n1.vy/vc1;
		n1.vz = n1.vz/vc1;

		copyVector(&sn1, &n1);
		if (normal_ok(&sn1, &v1, &centre))
		{
			n1.vx = -n1.vx;
			n1.vy = -n1.vy;
			n1.vz = -n1.vz;
		} 

		donev1 = num_done;
		copyVector(&beendone[donev1], &v1);	
		num_done++;
		copyVector(&object->ship_norms.normal[donev1], &n1);
	}
				
	object->vert[i].ni1 = donev1;


	n2.vx = n2.vy = n2.vz = 0;

	if (donev2 == -1)
	{
		for(z=0; z<vc2; z++)
		{
			n2.vx += norm_array2[z].vx; 
			n2.vy += norm_array2[z].vy; 
			n2.vz += norm_array2[z].vz; 
		}
	
		n2.vx = n2.vx/vc2;
		n2.vy = n2.vy/vc2;
		n2.vz = n2.vz/vc2;
		copyVector(&sn2, &n2);
		if (normal_ok(&sn2, &v2, &centre))
		{
			n2.vx = -n2.vx;
			n2.vy = -n2.vy;
			n2.vz = -n2.vz;
		} 

		donev2 = num_done;
		copyVector(&beendone[donev2], &v2);	
		num_done++;
		copyVector(&object->ship_norms.normal[donev2], &n2);
	}

	object->vert[i].ni2 = donev2;

    }
	object->ship_norms.n = num_done;
	printf("num done = %d\n", num_done);

}

int centre_point(OBJ_GT3 *obj, SVECTOR *cent_pnt)
{
    VECTOR	centre = {0, 0, 0};
    VECTOR	poly_av = {0, 0, 0};
    int		i = 0,no_poly;

	no_poly = obj->n;

	for(i = 0; i < no_poly; i++)
	{
	    poly_av.vx = poly_av.vy = poly_av.vz = 0;
	    poly_av.vx += (obj->vert[i].v0.vx + obj->vert[i].v1.vx + obj->vert[i].v2.vx);
	    poly_av.vy += (obj->vert[i].v0.vy + obj->vert[i].v1.vy + obj->vert[i].v2.vy);
	    poly_av.vz += (obj->vert[i].v0.vz + obj->vert[i].v1.vz + obj->vert[i].v2.vz);
	    poly_av.vx = poly_av.vx /3;
	    poly_av.vy = poly_av.vy /3;
	    poly_av.vz = poly_av.vz /3;

	    centre.vx += poly_av.vx;
	    centre.vy += poly_av.vy;
	    centre.vz += poly_av.vz;
	}

	cent_pnt->vx = centre.vx /no_poly;
	cent_pnt->vy = centre.vy /no_poly;
	cent_pnt->vz = centre.vz /no_poly;

}

int	normal_ok(SVECTOR *normal, SVECTOR *vertex, SVECTOR *centre)
{
//	if the normal is facing the right way 1 is returned
//	else 0 is returned

	SVECTOR	line;
	
	subVector(centre, vertex, &line);

	if (dot_product(&line, normal) >= 0)
		return 0;
	else
		return 1;
} 



int	equal(SVECTOR	*v1, SVECTOR	*v2)
{
//	returns true ie 1 if the svectors v1 and v2 are equal

	if ((v1->vx == v2->vx) && (v1->vy == v2->vy) && (v1->vz == v2->vz))
		return(1);
	else
		return(0);
} 
