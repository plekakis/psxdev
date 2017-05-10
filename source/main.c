/* OUF ka8arise o topos... */
#include "alkis.h"
#include "mike.h"
#include "helper.h"
#include "which.h"

int main()
{
    u_short padd;

    init_system(ORIGIN_X, ORIGIN_Y, 512, 0, 0x800F8000, 0x00100000);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters

	start();

	globals->frameIdx = 0;

	while (1) // draw and display forever
	{
	    globals->cdb = &globals->db[globals->frameIdx % MAX_BUFFERS];
        padd = PadRead(1);

	    FntPrint("Demo\n");
		FntFlush(-1);

		/* clear all OT entries */
        ClearOTag(globals->cdb->ot, MAX_OT_ENTRIES);

	    update();

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

	shutdown();

	PadStop();

	return 0;
}
