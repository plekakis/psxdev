#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "window.h"

#define NO_PAL_256 		// PAL 256 line mode

DB				db[2] ;
DB				*cdb ;
u_short			frameTPage ;
DR_TWIN			frameTWinOn,
				frameTWinOff ;
RECT			tWinRectOn = {224, 192, 32, 32} ;
RECT			tWinRectOff = {0, 0, 0, 0} ;

#ifdef PAL_256
 #define FRAME_X		320*2
 #define FRAME_Y		256
 #define SCREEN_X 		0
 #define SCREEN_Y		20
#else
 #define FRAME_X		320*2
 #define FRAME_Y		240
 #define SCREEN_X	 	0
 #define SCREEN_Y		16
#endif

void InitializeGraphics (void)
{
  RECT			rect = {0, 0, 1024, 512} ;

  ResetGraph (0) ;

  #ifdef PAL_256
   SetVideoMode (MODE_PAL) ;
  #endif
  SetDefDrawEnv (&db[0].drawenv, 0, 0, FRAME_X, FRAME_Y) ;
  SetDefDispEnv (&db[0].dispenv, 0, FRAME_Y, FRAME_X, FRAME_Y) ;
  SetDefDrawEnv (&db[1].drawenv, 0, FRAME_Y, FRAME_X, FRAME_Y) ;
  SetDefDispEnv (&db[1].dispenv, 0, 0, FRAME_X, FRAME_Y) ;

  db[1].dispenv.screen.x = db[0].dispenv.screen.x = SCREEN_X ;
  db[1].dispenv.screen.y = db[0].dispenv.screen.y = SCREEN_Y ;

  db[1].dispenv.screen.h = db[0].dispenv.screen.h = FRAME_Y ;
  db[1].dispenv.screen.w = db[0].dispenv.screen.w = 0 ;

  cdb = &db[0] ;
  SetDispMask (1) ;
  db[0].drawenv.isbg = 1 ;
  db[1].drawenv.isbg = 1 ;
  setRGB0 (&db[0].drawenv, 0x10, 0x10, 0x10) ;
  setRGB0 (&db[1].drawenv, 0x20, 0x20, 0x20) ;
  frameTPage = GetTPage (2, 0, TEX_X, TEX_Y) ;
//  SetTexWindow (&frameTWinOn, &tWinRectOn) ;
//  SetTexWindow (&frameTWinOff, &tWinRectOff) ;
}

void DeinitializeGraphics (void)
{
}

void SetFrameWindow (FRAMEWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle)
{
  int 		loop ;

  SetPolyGT4 (&window->main) ;
  setXYWH (&window->main, rect->x, rect->y, rect->w, rect->h) ;
//  setRGB0 (&window->main, 0x10, 0x10, 0x10) ;
//  setRGB1 (&window->main, 0x20, 0x20, 0x20) ;
//  setRGB2 (&window->main, 0x30, 0x30, 0x30) ;
//  setRGB3 (&window->main, 0x10, 0x10, 0x10) ;
  setRGB0 (&window->main, 0x10, 0x10, 0x10) ;
  setRGB1 (&window->main, 0x90, 0x90, 0x90) ;
  setRGB2 (&window->main, 0x50, 0x50, 0x50) ;
  setRGB3 (&window->main, 0x10, 0x10, 0x10) ;
  window->main.tpage = frameTPage ;
  setUVWH (&window->main, 0, 0, rect->w, rect->h) ;
//  setUVWH (&window->main, 0, 0, 255, 255) ;
  SetTexWindow (&window->tWinOn, &tWinRectOn) ;
  SetTexWindow (&window->tWinOff, &tWinRectOff) ;

  for (loop=0; loop<4; loop++)
  {
    SetPolyF4 (&window->border[loop]) ;
	SetSemiTrans (&window->border[loop], 1) ;
  }
  setXY4 (&window->border[0], rect->x, rect->y, 
  							  rect->x + rect->w, rect->y, 
					  		  rect->x + borderSize, rect->y + borderSize, 
					  		  rect->x + rect->w - borderSize, rect->y + borderSize) ;
  setXY4 (&window->border[1], rect->x + rect->w, rect->y, 
  							  rect->x + rect->w, rect->y + rect->h,
					  		  rect->x + rect->w - borderSize, rect->y + borderSize, 
					  		  rect->x + rect->w - borderSize, rect->y + rect->h - borderSize) ;
  setXY4 (&window->border[2], rect->x, rect->y, 
  							  rect->x + borderSize, rect->y + borderSize, 
  					  		  rect->x, rect->y + rect->h, 
  					  		  rect->x + borderSize, rect->y + rect->h - borderSize) ;
  setXY4 (&window->border[3], rect->x, rect->y + rect->h, 
  							  rect->x + rect->w, rect->y + rect->h,
				      		  rect->x + borderSize, rect->y + rect->h - borderSize, 
				      		  rect->x + rect->w - borderSize, rect->y + rect->h - borderSize) ;
  
  if (borderStyle == BORDER_UP)
  {
    setRGB0 (&window->border[0], 0x40, 0x40, 0x40) ;
    setRGB0 (&window->border[1], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[2], 0x40, 0x40, 0x40) ;
    setRGB0 (&window->border[3], 0x00, 0x00, 0x00) ;
  }
  else
  {
    setRGB0 (&window->border[0], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[1], 0x40, 0x40, 0x40) ;
    setRGB0 (&window->border[2], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[3], 0x40, 0x40, 0x40) ;
  }

  ClearOTag (window->orderTable, OT_SIZE_FRAME) ;
  AddPrim (&window->orderTable[0], &window->tWinOn) ;
  AddPrim (&window->orderTable[1], &window->main) ;
  AddPrim (&window->orderTable[2], &window->tWinOff) ;
  for (loop=0; loop<4; loop++)
  {
    AddPrim (&window->orderTable[loop+3], &window->border[loop]) ;
  }
}

void SetIconWindow (ICONWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect)
{
  int 		loop ;

  SetPolyFT4 (&window->main) ;
  setXYWH (&window->main, rect->x, rect->y, rect->w, rect->h) ;
  setRGB0 (&window->main, 0x80, 0x80, 0x80) ;
/*  setUVWH (&window->main, otUV[optType].x, otUV[optType].y, 
  						  otUV[optType].w, otUV[optType].h) ;*/
  setUVWH (&window->main, iconRect->x, iconRect->y, 
  						  iconRect->w, iconRect->h) ;
  window->main.tpage = GetTPage (2, 0, TEX_X, TEX_Y) ;

  for (loop=0; loop<4; loop++)
  {
    SetPolyF4 (&window->border[loop]) ;
	SetSemiTrans (&window->border[loop], 1) ;
  }
  setXY4 (&window->border[0], rect->x, rect->y, 
  							  rect->x + rect->w, rect->y, 
					  		  rect->x + borderSize, rect->y + borderSize, 
					  		  rect->x + rect->w - borderSize, rect->y + borderSize) ;
  setXY4 (&window->border[1], rect->x + rect->w, rect->y, 
  							  rect->x + rect->w, rect->y + rect->h,
					  		  rect->x + rect->w - borderSize, rect->y + borderSize, 
					  		  rect->x + rect->w - borderSize, rect->y + rect->h - borderSize) ;
  setXY4 (&window->border[2], rect->x, rect->y, 
  							  rect->x + borderSize, rect->y + borderSize, 
  					  		  rect->x, rect->y + rect->h, 
  					  		  rect->x + borderSize, rect->y + rect->h - borderSize) ;
  setXY4 (&window->border[3], rect->x, rect->y + rect->h, 
  							  rect->x + rect->w, rect->y + rect->h,
				      		  rect->x + borderSize, rect->y + rect->h - borderSize, 
				      		  rect->x + rect->w - borderSize, rect->y + rect->h - borderSize) ;

  if (borderStyle == BORDER_UP)
  {
    setRGB0 (&window->border[0], 0x80, 0x80, 0x80) ;
    setRGB0 (&window->border[1], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[2], 0x80, 0x80, 0x80) ;
    setRGB0 (&window->border[3], 0x00, 0x00, 0x00) ;
  }
  else
  {
    setRGB0 (&window->border[0], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[1], 0x80, 0x80, 0x80) ;
    setRGB0 (&window->border[2], 0x00, 0x00, 0x00) ;
    setRGB0 (&window->border[3], 0x80, 0x80, 0x80) ;
  }

  ClearOTag (window->orderTable, OT_SIZE_ICON) ;
  AddPrim (&window->orderTable[0], &window->main) ;
  for (loop=0; loop<4; loop++)
  {
    AddPrim (&window->orderTable[loop+1], &window->border[loop]) ;
  }
}

void SetStdWindow (STDWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect)
{
  RECT				rect2 ;

  SetFrameWindow (&window->frame, rect, borderSize, borderStyle) ;
  rect2.x = rect->x + rect->w - borderSize - ICON_WIDTH ;
  rect2.y = rect->y + borderSize ;
  rect2.w = ICON_WIDTH ; 
  rect2.h = ICON_HEIGHT ;
  SetIconWindow (&window->icon, &rect2, borderSize, borderStyle, iconRect) ;
  ClearOTag (window->orderTable, OT_SIZE_STD) ;
  AddPrims (window->orderTable, window->frame.orderTable, window->frame.orderTable + OT_SIZE_FRAME - 1) ;
  AddPrims (window->orderTable+1, window->icon.orderTable, window->icon.orderTable + OT_SIZE_ICON - 1) ;
}

void SetOKWindow (OKWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect) 
{
  RECT				rect2 ;
  int				x, y ;

/*  rect2.w = otUV[iconOKOpt].w ;
  rect2.h = otUV[iconOKOpt].h ;*/
  rect2.w = iconRect->w ;
  rect2.h = iconRect->h ;
  rect2.x = rect->x + ((rect->w - rect2.w) / 2) ;
  rect2.y = rect->y + (((rect->h - rect2.h) / 3) * 2) ;
  SetFrameWindow (&window->frame, rect, borderSize, borderStyle) ;
  SetIconWindow (&window->okIcon, &rect2, BUTTON_BORDER, BORDER_UP, iconRect) ;
  ClearOTag (window->orderTable, OT_SIZE_OK) ;
  AddPrims (window->orderTable, window->frame.orderTable, window->frame.orderTable + OT_SIZE_FRAME - 1) ;
  AddPrims (window->orderTable+1, window->okIcon.orderTable, window->okIcon.orderTable + OT_SIZE_ICON - 1) ;

/*  x = rect->x + ((rect->w - otUV[errorInCardOpt].w) / 2) ;
  y = rect->y + (((rect->h - otUV[errorInCardOpt].h) / 3) * 1) ;
  setXYWH (&optionsPoly[errorInCardOpt], x, y, otUV[errorInCardOpt].w, otUV[errorInCardOpt].h) ;*/
}

void DrawFrameWindow (FRAMEWINDOW *window, u_int zOrder)
{
  AddPrims (cdb->orderTable+zOrder+1, window->orderTable, window->orderTable + OT_SIZE_FRAME - 1) ;
}									 

void DrawStdWindow (STDWINDOW *window, u_int zOrder) 
{
  AddPrims (cdb->orderTable+zOrder, window->orderTable, window->orderTable + OT_SIZE_STD - 1) ;
}

void DrawOKMessageWindow (OKWINDOW *window, u_int zOrder)
{
  AddPrims (cdb->orderTable+zOrder, window->orderTable, window->orderTable + OT_SIZE_OK - 1) ;
}

void SwapBuffers (void)
{
  DrawSync (0) ;
  VSync (0) ;
  cdb = (cdb==db) ? db+1 : db ;
  PutDispEnv (&cdb->dispenv) ;
  PutDrawEnv (&cdb->drawenv) ;
}

void ClearOrderTable (void)
{
  ClearOTagR (cdb->orderTable, OT_SIZE) ;
}

void DrawOrderTable (void) 
{
  DrawOTag (cdb->orderTable+OT_SIZE-1) ;
}

