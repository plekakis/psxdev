/* OUF ka8arise o topos... */


#include "helper.h"
#include "app.h"

int main()
{
    u_short padd;

    init_system(SCREEN_X, SCREEN_Y, 0, 0x800F8000, 0x00100000);

    // 256k of scratch
    INIT_MAIN_SCRATCH(1024 * 128);

    // and 128k of double buffered scratch. This memory is shared between the allocators, so each gets 64k
    INIT_MAIN_DB_SCRATCH(1024 * 64);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, SCREEN_X, SCREEN_Y, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters

	start();

	globals->frameIdx = 0;

	while (1) // draw and display forever
	{
	    globals->cdb = &globals->db[globals->frameIdx & 1];
        padd = PadRead(1);

	    FntPrint("Demo");
		FntFlush(-1);

		// reset scratch
        RESET_MAIN_SCRATCH();
		RESET_MAIN_DB_SCRATCH();

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

	SHUTDOWN_MAIN_DB_SCRATCH();
	SHUTDOWN_MAIN_SCRATCH();
	shutdown();

	PadStop();

	return 0;
}
