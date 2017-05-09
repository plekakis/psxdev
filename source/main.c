/* OUF ka8arise o topos... */
#include "helper.h"
#include "alkis.h"
#include "mike.h"
#include "which.h"

#ifndef USE_MIKE_CODE
	#define _start alkisStart
	#define _update alkisUpdate
#else
	#define _start mikeStart
	#define _update mikeUpdate
#endif

int main()
{
    u_short padd;
	
    init_system(ORIGIN_X, ORIGIN_Y, 512, 0, 0x800F8000, 0x00100000);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters
	
	_start();

	globals->frameIdx = 0;
	
	while (1) // draw and display forever
	{
	    globals->cdb = &globals->db[globals->frameIdx % MAX_BUFFERS];
        padd = PadRead(1);

	    FntPrint("Demo\n");
		FntFlush(-1);

	    _update();

        DrawSync(0);

        /* vsync and swap frame double buffer
		 *  set the drawing environment and display environment. */
		VSync(0);
		PutDrawEnv(&globals->cdb->draw);
		PutDispEnv(&globals->cdb->disp);

		/* start Drawing */
		DrawOTag(globals->cdb->ot);

		++globals->frameIdx;
	}

	DrawSync(0);

	PadStop();

	return 0;
}
