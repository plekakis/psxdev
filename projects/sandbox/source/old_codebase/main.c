#include "helper.h"
#include "app.h"

unsigned long ramsize =   0x00200000; // 2 Megabytes of RAM
unsigned long stacksize = 0x800F8000; // 16 Kilobytes of Stack

int main()
{
    u_short padd;

    init_system(SCREEN_X, SCREEN_Y, 0, ramsize, stacksize);

    // 128k of scratch
    INIT_MAIN_SCRATCH(1024 * 128);

    // and 64k of double buffered scratch. This memory is shared between the allocators, so each gets 32k
    INIT_MAIN_DB_SCRATCH(1024 * 64);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, SCREEN_X, SCREEN_Y, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters

	start();

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

		ClearImage(&globals->cdb->draw.clip, globals->clearColor.r, globals->clearColor.r, globals->clearColor.b);

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
