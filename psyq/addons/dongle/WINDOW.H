#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define OT_SIZE				1024 
#define OT_SIZE_FRAME		8
#define OT_SIZE_ICON		6
#define OT_SIZE_STD 		3
#define OT_SIZE_OK			3
#define	OT_SIZE_YESNO		4

#define BORDER_UP			0
#define BORDER_DOWN			1

#define BUTTON_BORDER		3

#define ICON_WIDTH			32
#define ICON_HEIGHT			16

#define FONT_WIDTH			8
#define FONT_HEIGHT			8

#define TEX_X				640
#define TEX_Y				256

#define OPTION_QUIT				0
#define OPTION_UP				1
#define OPTION_DOWN				2
#define OPTION_LEFT				3
#define OPTION_RIGHT			4
#define OPTION_TRIANGLE			5
#define OPTION_SQUARE			6
#define OPTION_CROSS			7
#define OPTION_CIRCLE			8
#define OPTION_SELECT			9
#define OPTION_START			10
#define OPTION_LEFT1			11
#define OPTION_LEFT2			12
#define OPTION_RIGHT1			13
#define OPTION_RIGHT2			14
#define OPTION_NONE				99

/* Type Declarations */

typedef struct 
{
  DRAWENV		drawenv ;
  DISPENV		dispenv ;
  u_long		orderTable[OT_SIZE] ;
} DB ;

typedef struct 	 
{
  POLY_GT4		main ;
  POLY_F4		border[4] ;
  DR_TWIN		tWinOn,
  				tWinOff ;
  u_long		orderTable[OT_SIZE_FRAME] ;
} FRAMEWINDOW ;

typedef struct 
{
  POLY_FT4		main ;
  POLY_F4		border[4] ;
  u_long		orderTable[OT_SIZE_ICON] ;
} ICONWINDOW ;

typedef struct 
{
  FRAMEWINDOW	frame ;
  ICONWINDOW	icon ;
  u_long		orderTable[OT_SIZE_STD] ;
} STDWINDOW ;

typedef struct
{
  FRAMEWINDOW	frame ;
  ICONWINDOW	okIcon ;
  u_long		orderTable[OT_SIZE_OK] ;
} OKWINDOW ;

typedef struct
{
  FRAMEWINDOW	frame ;
  ICONWINDOW	yesIcon ;
  ICONWINDOW	noIcon ;
  u_long		orderTable[OT_SIZE_YESNO] ;
} YESNOWINDOW ;

void InitializeGraphics (void) ;
void DeinitializeGraphics (void) ;
void SetFrameWindow (FRAMEWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle) ;
void SetIconWindow (ICONWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect) ;
void SetStdWindow (STDWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect) ;
void SetOKWindow (OKWINDOW *window, RECT *rect, u_int borderSize, u_int borderStyle, RECT *iconRect) ;
void DrawFrameWindow (FRAMEWINDOW *window, u_int zOrder) ;
void DrawStdWindow (STDWINDOW *window, u_int zOrder) ;
void DrawOKMessageWindow (OKWINDOW *window, u_int zOrder) ;
void SwapBuffers (void) ;
void ClearOrderTable (void) ;
void DrawOrderTable (void) ;


extern DB			db[2] ;
extern DB			*cdb ;
extern DR_TWIN		frameTWinOn,
					frameTWinOff ;


