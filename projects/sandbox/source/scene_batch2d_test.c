#include "embedded/bgtex.c"
#include "embedded/fnt.c"

uint16 tpage, clut;
uint16 fnt_tpage, fnt_clut;

CVECTOR whiteColor = { 255, 255, 255 };

///////////////////////////////////////////////////
void start()
{
	clut = LoadClut(bgtex, 0, 480);
	tpage = LoadTPage(bgtex + 0x80, 0, 0, 640, 0, 256, 256);
	
	// Put the font after the previous bg tex
	fnt_clut = LoadClut2(s_test_font, 640, 385);
	fnt_tpage = LoadTPage(s_test_font + 0x80, 0, 0, 640, 257, 128, 32);
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void DrawBatch_Textured()
{
	typedef struct
	{
		DR_TPAGE	fontPage1;
		CHAR2D		text1[64];
		DR_TPAGE	characterTPageMode;
		SPRT		knight[4];
		SPRT		princess[4];
		DR_MODE		grassMode;
		SPRT		tiledGrass;
		DR_MODE		resetMode;
	}Textured2D;

	Gfx_BeginSubmission(OT_LAYER_OV);
	{
		Batch2D batch;
		uint32 index;
		Gfx_BeginBatch2D(&batch, sizeof(Textured2D));

		// A bit of explanation text
		{
			DVECTOR textPosition = { Gfx_GetDisplayWidth()/2-16, 140-16 };
			Gfx_Batch2D_AddTPageDirect(&batch, MODE_FLAG_NONE, fnt_tpage);
			Gfx_Batch2D_AddString(&batch, "A few sprites with/without texture window", &textPosition, NULL, &whiteColor, fnt_clut, PRIM_FLAG_NONE);
		}

		// Specify our tpage
		Gfx_Batch2D_AddTPageDirect(&batch, MODE_FLAG_NONE, tpage);

		// Add 4 sprites, pick the knight hero
		for (index = 0; index < 4; ++index)
		{
			DVECTOR position = { Gfx_GetDisplayWidth() -32, 140 + index * 32 };
			DVECTOR size = { 32, 32 };
			TVECTOR uv = { 32 * index, 32 };

			Gfx_Batch2D_AddSpriteDirect(&batch, &position, &size, &uv, &whiteColor, clut, PRIM_FLAG_NONE);
		}

		// Add 4 more, pick the princess
		for (index = 0; index < 4; ++index)
		{
			DVECTOR position = { Gfx_GetDisplayWidth() - 64, 140 + index * 32 };
			DVECTOR size = { 32, 32 };
			TVECTOR uv = { 32 * index, 64 };

			Gfx_Batch2D_AddSpriteDirect(&batch, &position, &size, &uv, &whiteColor, clut, PRIM_FLAG_NONE);
		}

		// Finally, demonstrate a way to tile a sprite using texture windows
		// Setting a rect around the area we want to tile and adding a mode to the batch does it. In this case, pick a grass-like block, half its original size and tile it across
		// Notice you don't have to specify a uv, this is infered from the sprite size.
		{
			DVECTOR position = { Gfx_GetDisplayWidth()-330, 140 };
			DVECTOR size = { 256, 128 };
			RECT rect;
			setRECT(&rect, 128, 96, 16, 16);

			Gfx_Batch2D_AddModeDirect(&batch, MODE_FLAG_NONE, &rect, tpage);
			Gfx_Batch2D_AddSpriteDirect(&batch, &position, &size, 0, &whiteColor, clut, PRIM_FLAG_NONE);
		}

		Gfx_EndBatch2D(&batch);
	}
	Gfx_EndSubmission();
}

///////////////////////////////////////////////////
void DrawBatch_Untextured()
{
	typedef struct
	{
		DR_TPAGE	fontPage1;
		CHAR2D		text1[20];
		TILE		tile;
		CHAR2D		text2[32];
		TILE		tiles[2048];
		CHAR2D		text3[64];
		LINE_F2		line0[100];
		CHAR2D		text4[64];
		LINE_G2		line1[100];
	}Untextured2D;

	const uint32 c_count = 2048;
	CVECTOR tileColor = { 255, 0, 0 };
	DVECTOR tileSize = { 256, 24 };
	DVECTOR basePos = { 32, Gfx_GetDisplayHeight()/2 - 100 };
	DVECTOR textPosition = { basePos.vx, basePos.vy - 16 };
	uint32 i = 0;

	Gfx_BeginSubmission(OT_LAYER_OV);

	{
		Batch2D batch;
		Gfx_BeginBatch2D(&batch, sizeof(Untextured2D));

		Gfx_Batch2D_AddTPageDirect(&batch, MODE_FLAG_NONE, fnt_tpage);
		Gfx_Batch2D_AddString(&batch, "A single color tile", &textPosition, NULL, &whiteColor, fnt_clut, PRIM_FLAG_NONE);

		textPosition.vy += 16;
		Gfx_Batch2D_AddTile(&batch, &textPosition, &tileSize, &tileColor, PRIM_FLAG_NONE);

		textPosition.vy += 40;
		Gfx_Batch2D_AddString(&batch, "2048 randomly colored tiles", &textPosition, NULL, &whiteColor, fnt_clut, PRIM_FLAG_NONE);

		textPosition.vy += 16;
		for (i = 0; i < c_count; ++i)
		{
			DVECTOR pos = { textPosition.vx + 4 * (i % 64), textPosition.vy + 4 * (i / 64) };
			DVECTOR size = { 4, 4 };
			CVECTOR color = { rand() % 255, rand() % 255, rand() % 255 };

			Gfx_Batch2D_AddTile(&batch, &pos, &size, &color, PRIM_FLAG_NONE);
		}

		textPosition.vy += 136;
		Gfx_Batch2D_AddString(&batch, "100 F2 vertical lines", &textPosition, NULL, &whiteColor, fnt_clut, PRIM_FLAG_NONE);
				
		for (i=0; i<100; ++i)
		{
			DVECTOR lineStart0 = { textPosition.vx + i * 4, textPosition.vy + 16 };
			DVECTOR lineEnd0 = { lineStart0.vx, lineStart0.vy + 32 };
			
			CVECTOR lineColor0 = { 255 - i, i, 255 / ((i >> 1) + 1) };

			Gfx_Batch2D_AddLineF(&batch, &lineStart0, &lineEnd0, &lineColor0, PRIM_FLAG_NONE);
		}

		textPosition.vy += 64;
		Gfx_Batch2D_AddString(&batch, "100 G2 vertical lines", &textPosition, NULL, &whiteColor, fnt_clut, PRIM_FLAG_NONE);
		for (i = 0; i < 100; ++i)
		{
			DVECTOR lineStart0 = { textPosition.vx + i * 4, textPosition.vy + 16 };
			DVECTOR lineEnd0 = { lineStart0.vx, lineStart0.vy + 32 };

			CVECTOR lineColor0 = { 255 - i, i, 255 / ((i >> 1) + 1) };
			CVECTOR lineColor1 = { i, 255 - i, i << 1 };

			Gfx_Batch2D_AddLineG(&batch, &lineStart0, &lineEnd0, &lineColor0, &lineColor1, PRIM_FLAG_NONE);
		}

		Gfx_EndBatch2D(&batch);
	}

	Gfx_EndSubmission();
}

void render()
{
	DrawBatch_Untextured();
	DrawBatch_Textured();
}