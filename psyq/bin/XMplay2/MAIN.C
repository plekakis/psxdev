/*****************************************************************************
Extended MOD (XM) Player for Sony Playstation
Written By Jason Page
Last Update 07/12/98
(C)1998 Sony Computer Entertainment.

*****************************************************************************/


#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libspu.h>

#include "xmplay.h"
#include "ctrller.h"		/* JoyPad stuff */
#include "graph.h"		/* Graphics routines */

void main(void);
void Drawbar(int Depth,int YPos,int r,int g,int b);
void InitSPUHardware(void);
void DisplayInfo(void);

/**** XM and VAB ADDRESSES */

extern unsigned char VHData[]; 
extern unsigned char VBData[]; 
extern unsigned char XMData[]; 

/**** JOYPAD DATA ****/

DB	   db[2];		/* packet double buffer */
DB* 	cdb;			/* current db */
ControllerPacket buffer1, buffer2;
char Key_LL=0;
char Key_LR=0;
char Key_LU=0;
char Key_LD=0;
char Key_RD=0;
char Key_RU=0;
char Key_RL=0;
char Key_RR=0;
char LKey_LD=0;
char LKey_LU=0;
char LKey_LL=0;
char LKey_LR=0;
char LKey_RD=0;
char LKey_RU=0;
char LKey_RL=0;
char LKey_RR=0;



/**** Initialise SPU data ****/

#define MALLOC_MAX 128
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

/**** FEEDBACK SCRUCTURES ****/

XM_Feedback Feedback;
XM_Feedback *FB;
XM_HeaderInfo HInfo;
XM_HeaderInfo *Inf;
XM_VABInfo VInfo;
XM_VABInfo *VInf;


#define XM_ID1 0
int VABID[8];
int Song_ID1 =-1;

int S3M_Mode=0;			/* Old S3M or XM flag */

/*****************************************************************************
main
		This does everything.
*****************************************************************************/

void main(void)
{
#define YOff 24+7			/* Scan time Y start offset */

int vs2=0;					/* Maximum scan lines */
int Frame=0;				/* Frame counter */
int i;

u_char *XMAddr[24];
int SongSize;
int FileHSize;
u_char *FHAddr;

	ResetCallback();
	InitPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
	StartPAD();
	init_graph();

	InitSPUHardware();			/* Start up SPU */
	XM_OnceOffInit(XM_NTSC);	/* NTSC/PAL */
	XM_SetStereo();				/* or XM_SetMono for mono output */

/**** NEW MALLOC STUFF...DO THIS FOR AS MANY SONG_ID'S YOU USE ****/

	FileHSize=XM_GetFileHeaderSize();
	FHAddr=malloc(FileHSize);			// Do for each XM File you are using..
	XM_SetFileHeaderAddress(FHAddr);	// (max 8 files initialised at once)

	SongSize=XM_GetSongSize();
	for (i=0; i<1;i++)
	{
		XMAddr[i]=malloc(SongSize);	// Do for however many songs you want to
		XM_SetSongAddress(XMAddr[i]);	// play at once..(max 24)
	}

	VABID[0]=XM_VABInit(VHData,VBData);	// Initialise VAB.

	InitXMData(XMData,XM_ID1,XM_UseS3MPanning); 	/* Address, XM_ID, PanType */

	Inf=&HInfo;
	XM_GetHeaderInfo(0,Inf);	/* XM_ID,HeaderInfo structure */

	VSyncCallback(XM_Update); 	/* HOOK XM UPDATE ROUTINE INTO 60FPS INTERRUPT */

	Song_ID1=XM_Init				/* PLAY A SONG */
				(VABID[0],	/* VAB ID returned from XM_VABInit */
				XM_ID1,		/* XM ID */
				0,				/* SongID (-1 to let the player allocate an ID*/
				0,				/* First Channel to playback on */
				XM_Loop,		/* Looping? */
				-1,			/* Play bitmask (-1 = all channels playing)*/
				XM_Music,	/* Music or SFX?*/
				0);			/* Start offset or SFX Pattern */

	while(1)
	{
		cdb = (cdb==db)? db+1: db;
		ClearOTagR(cdb->ot, OTSIZE);

		FB=&Feedback;
		XM_GetFeedback(0,FB);	/* SongID,Feedback structure */

		if (XM_SCAN>vs2)
			vs2=XM_SCAN;			/* Maximum scan lines */

		DisplayInfo();

		DrawBar(XM_SCAN,YOff,200,0,200);
		DrawBar(1,YOff+vs2,255,255,255);

	  	Key_LL= PadKeyIsPressed(&buffer1,PAD_LL);
	  	Key_LR= PadKeyIsPressed(&buffer1,PAD_LR);
	  	Key_LU= PadKeyIsPressed(&buffer1,PAD_LU);
	  	Key_LD= PadKeyIsPressed(&buffer1,PAD_LD);
	  	Key_RD= PadKeyIsPressed(&buffer1,PAD_RD);
	  	Key_RR= PadKeyIsPressed(&buffer1,PAD_RR);
	  	Key_RL= PadKeyIsPressed(&buffer1,PAD_RL);
	  	Key_RU= PadKeyIsPressed(&buffer1,PAD_RU);


		if (Key_LL>LKey_LL)
			XM_SetSongPos(0,Feedback.SongPos-1);	/* SongID, SongPos */
		if (Key_LR>LKey_LR)
			XM_SetSongPos(0,Feedback.SongPos+1);

		if (Key_RD>LKey_RD)
		{
			if (Feedback.Status==XM_PLAYING)
				XM_Pause(Song_ID1);
			else if (Feedback.Status==XM_PAUSED)
				XM_Restart(Song_ID1);
		}


		if ((Key_RU>LKey_RU)||(Key_RL>LKey_RL))
		{
			XM_Exit();							// STOP TUNE PLAYING
			XM_FreeAllSongIDs();				// FREE SONG ID's
			XM_FreeAllFileHeaderIDs();		// FREE HEADER ID's

			free(FHAddr);
			for (i=0;i<1;i++)					// De-Allocate memory
			{
				free(XMAddr[i]);
			}

			FileHSize=XM_GetFileHeaderSize();	// Re-initialise everything again
			FHAddr=malloc(FileHSize);
			XM_SetFileHeaderAddress(FHAddr);

			SongSize=XM_GetSongSize();		// GET SONG STRUCTURE SIZE
			for (i=0;i<1;i++)
			{
				XMAddr[i]=malloc(SongSize);		// MALLOC THAT SIZE
				XM_SetSongAddress(XMAddr[i]);	// SET ADDRESS OF MALLOCED MEMORY
			}
			VABID[0]=XM_VABInit(VHData,VBData);
	
			if (Key_RU>LKey_RU)
			{
				S3M_Mode=1;
				InitXMData(XMData,XM_ID1,XM_UseS3MPanning);	// Init as S3M
			}
			else
			{
				S3M_Mode=2;
				InitXMData(XMData,XM_ID1,XM_UseXMPanning);		// Init as XM
			}

			Song_ID1=XM_Init
						(VABID[0],	/* VAB ID returned from XM_VABInit */
						XM_ID1,		/* XM ID */
						0,				/* Song ID */
						0,				/* First Channel */
						XM_Loop,		/* Looping? */
						-1,			/* Play bitmask */
						XM_Music,	/* Music or SFX?*/
						0);			/* Start offset or SFX Pattern */
			vs2=0;
		}

		LKey_LL=Key_LL;	/* Gate joypad */
		LKey_LR=Key_LR;
		LKey_LU=Key_LU;
		LKey_LD=Key_LD;
		LKey_RD=Key_RD;
		LKey_RR=Key_RR;
		LKey_RU=Key_RU;
		LKey_RL=Key_RL;

		Frame++;
	   FntFlush(-1);
	   DrawSync(0);
      VSync(0);
	   PutDrawEnv(&cdb->draw);
	   PutDispEnv(&cdb->disp);
	   DrawOTag(cdb->ot+OTSIZE-1);

	}
	XM_Exit();
}



void DrawBar(int Depth,int YPos,int r,int g,int b)
{
static POLY_G4 Bar;

	SetPolyG4(&Bar);
	setXYWH(&Bar,0,YPos,512,Depth);
	setRGB0(&Bar,r/2,g/2,b/2);
	setRGB1(&Bar,r/2,g/2,b/2);
	setRGB2(&Bar,r,g,b);
	setRGB3(&Bar,r,g,b);
	DrawPrim(&Bar);
}

void DisplayInfo(void)
{
	FntPrint("\n");
	FntPrint("0-\n8-\n16-\n24-\n32-\n40-\n");
	FntPrint("        xm player 2.0 (c)scee. written by jason page\n");
	FntPrint("                music by peter 'skaven' hajba\n\n");

	FntPrint("status = ");
	if (Feedback.Status==XM_STOPPED)
		FntPrint("stopped ");
	else if (Feedback.Status==XM_PLAYING)
		FntPrint("playing ");
	else if (Feedback.Status==XM_PAUSED)
		FntPrint("paused ");

	if (S3M_Mode==1)
		FntPrint("- old s3m");
	else if (S3M_Mode==2)
		FntPrint("- xm file");
	FntPrint("\n");

	FntPrint("sngpos = %d/%d\n",Feedback.SongPos,Feedback.SongLength);
	FntPrint("patpos = %d\n",Feedback.PatternPos);
	FntPrint("pattern= %d\n",Feedback.CurrentPattern);
	FntPrint("bpm/spd= %d/%d ",Feedback.SongBPM,Feedback.SongSpeed);
	FntPrint("(original = %d/%d)\n",HInfo.BPM,HInfo.Speed);
	FntPrint("keyon voices %d\n",Feedback.ActiveVoices);
	FntPrint("master volume/pan %d/%d\n",Feedback.Volume,Feedback.Panning);
	FntPrint("\npad l/r to move through song\n");
	FntPrint("pad x to pause/resume\n");
	FntPrint("pad triangle - init as old s3m\n");
	FntPrint("pad square - init as xm\n");
}


void InitSPUHardware(void)
{

/** Do Some SPU Hardware stuff **/

	SpuInit ();
	SpuSetCommonMasterVolume(0x3fff,0x3fff);
	SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);
}

