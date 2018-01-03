/*+--------+----------+----------+----------+----------+----------+---------+*/
/*                                                                           */
/* (C) Sony Computer Entertainment. All Rights Reserved.                     */
/*                                                                           */
/* Name:   ddp.h                                                             */                      
/* Author: Allan Murphy                                                      */                     
/* Date:   8/6/95                                                            */                       
/*                                                                           */
/* Description:                                                              */            
/*                                                                           */
/* Header file for DDP (demo disk program).                                  */
/*                                                                           */
/*+--------+----------+----------+----------+----------+----------+---------+*/

#ifndef __DDP_H
#define __DDP_H

#define INTERACTIVE        0
#define ATTRACT            1
#define ERROR              2

#define NO_PADS            0
#define PAD_ONE            1
#define PAD_TWO            2
#define BOTH_PADS          3

#define PAD_CHECK_TIME     100

#define MAX_ARGS           10

typedef struct
  {		
	 DRAWENV draw;		  /* drawing environment */
	 DISPENV disp;		  /* display environment */
  }
DB;

typedef struct
  {
    char selectionResult, transitionResult;
    int mode, timeout;
    short loadGame, cdb;
    DB db[2];
    CdlLOC pos;
    int args[MAX_ARGS];
  }
DDPData;

#define FSM_TRACK    0
#define FSM_MINUTE   0
#define FSM_SECOND   2
#define FSM_SECTOR   35

#endif
