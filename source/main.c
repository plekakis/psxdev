/* OUF ka8arise o topos... */

#include "helper.h"

RENDERABLE triangle;

int main()
{
    SVECTOR x[3];
    u_short padd;
    VECTOR cameraPos = {0,0,1000};
    SVECTOR cameraRot = {0,0,100};
    TRANSFORM t;

    init_system(ORIGIN_X, ORIGIN_Y, 512, 0, 0x800F8000, 0x00100000);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters

	setVector(&x[0], -256, 128, 0);
    setVector(&x[1],  256, 128, 0);
    setVector(&x[2], 0,  -128, 0);


    t.position.vx = 0;
    t.position.vy = 0;
    t.position.vz = 0;

    t.rotation.vx = 0;
    t.rotation.vy = 0;
    t.rotation.vz = 0;

	init_renderable(&triangle, &t, 1, x, sizeof(SVECTOR));

	globals->frameIdx = 0;
	while (1) // draw and display forever
	{
	    globals->cdb = &globals->db[globals->frameIdx % MAX_BUFFERS];
        padd = PadRead(1);

	    FntPrint("Demo\n");
		FntFlush(-1);

	    /* clear all OT entries */
		ClearOTag(globals->cdb->ot, MAX_OT_ENTRIES);

		update_camera(&cameraPos, &cameraRot);
		add_renderable(globals->cdb->ot, &triangle);

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
