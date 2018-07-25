#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>


void init_system(int x, int y, int z, int level)
{

	/* initialize controller */
	PadInit(0);             
	
	/* reset graphic subsystem */
	ResetGraph(0);		
	FntLoad(960, 256);	
//	SetDumpFnt(FntOpen(32, 32, 320, 64, 2, 512));
	
	/* set debug mode (0:off, 1:monitor, 2:dump) */
	SetGraphDebug(level);	

	/* initialize geometry subsystem */
	InitGeom();			
	
	/* set geometry origin as (160, 120) */
	SetGeomOffset(x, y);	
	
	/* distance to veiwing-screen */
	SetGeomScreen(z);		
}
