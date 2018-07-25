/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *      Dungeon */
#include <stdio.h>
#include <stdlib.h>
#include <rand.h>


#define XMAX  100			/* Size of dungeon (even)  */
#define YMAX  100			/* Size of dungeon (even) */
#define MAXSITE  (XMAX*YMAX/4)		/* Max site number */
static char map[XMAX+1][YMAX+1];	/* Map */
static unsigned short nsite = 0;	/* Registered site number */
static unsigned short xx[MAXSITE];	/* Registered site coordination */
static unsigned short yy[MAXSITE];	/* Registered site coordination */
int dx[4] = { 2, 0, -2,  0 };		/* Displacement vector */
int dy[4] = { 0, 2,  0, -2 };		/* Displacement vector */
int dirtable[24][4] = {			/* Direction table */
	{0,1,2,3},{0,1,3,2},{0,2,1,3},{0,2,3,1},{0,3,1,2},{0,3,2,1},
	{1,0,2,3},{1,0,3,2},{1,2,0,3},{1,2,3,0},{1,3,0,2},{1,3,2,0},
	{2,0,1,3},{2,0,3,1},{2,1,0,3},{2,1,3,0},{2,3,0,1},{2,3,1,0},
	{3,0,1,2},{3,0,2,1},{3,1,0,2},{3,1,2,0},{3,2,0,1},{3,2,1,0},
};
static short xmax;
static short ymax;
/**************************************************************************

**************************************************************************/
void add(int i, int j)
{
	if( nsite>= MAXSITE ) {
		printf( "site over\n" );
		return;
	}
	xx[nsite] = i;  yy[nsite] = j;  nsite++;
}

int select(int *i, int *j)
{
	int r;

	if (nsite == 0) return 0;
	nsite--;  r = (int)(nsite * (rand() / (RAND_MAX + 1)));
	*i = xx[r];  xx[r] = xx[nsite];
	*j = yy[r];  yy[r] = yy[nsite];  return 1;
}

void make_maze_data( int mx, int my )
{
	int i, j, i1, j1, d, t, *tt;

	nsite = 0;
	xmax = mx+5;
	ymax = my+5;

	for (i = 0; i <= xmax; i++)
		for (j = 0; j <= ymax; j++) map[i][j] = 1;
	for (i = 3; i <= xmax - 3; i++)
		for (j = 3; j <= ymax - 3; j++) map[i][j] = 0;
	map[2][3] = 0;  map[xmax-2][ymax-3] = 0;

	for( i=4; i <= xmax - 4; i += 2) {  /* Add site */
		add(i, 2);  add(i, ymax - 2);
	}
	for (j = 4; j <= ymax - 4; j += 2) {
		add(2, j);  add(xmax - 2, j);
	}
	while (select(&i, &j)) {
		for ( ; ; ) {
			tt = dirtable[rand()%24];
			for (d = 3; d >= 0; d--) {
				t = tt[d];  i1 = i + dx[t];  j1 = j + dy[t];
				if (map[i1][j1] == 0) break;
			}
			if (d < 0) break;
			map[(i + i1) / 2][(j + j1) / 2] = 1;
			i = i1;  j = j1;  map[i][j] = 1;  add(i, j);
		}
	}
}

int get_maze_data( int px, int py )
{
	return((int)map[px+3][py+3]);
}
