/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  CD-ROM からムービーをストリーミングするサンプル
 *
 *   Version    Date
 *  --------------------------------------------------------------
 *	1.00		Jul,14,1994	yutaka
 *	1.10		Sep,01,1994	suzu
 *	2.00		Feg,07,1996     suzu
 *      2.01            Sep,06,1997     yutaka (倍速対策）
 *
 *animInit	バックグラウンド動画処理を初期化
 *
 *	形式	int animInit(char *name, int x, int y)
 * 
 *	引数	name	動画ファイル名
 *		x, y	解凍画像を転送するフレームバッファのアドレス
 *			（左上端点）
 * 	
 *	解説	CD-ROM 上の name で指定されたファイルを動画像ファイル
 *		として再生し結果をフレームバッファの (x,y) で始まる矩
 *		形領域に転送する。動画像の大きさ（は幅・高さ）は、再生
 *		するデータに依存する。
 *	
 *	返り値	成功すると 0, 失敗すると非零
 *
 * animPoll	バックグラウンド動画処理
 *
 *	形式	int animPoll(void)
 *
 *	引数	なし
 *	
 *	解説	バックグラウンドアニメーションを監視するためのポーリン
 *		グ関数。描画バッファの切替えにあたって一回呼び出すこと
 *
 *	返り値	0	CD-ROM からのデータ待ち中 （動画復号処理は行な
 *			われていない） 
 *		1	動画復号処理中
 *		2	CD-ROM シーク中	（動画復号処理は行なわれていない）
 *
 */
 
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>


#define RING_SIZE	32      	/* ring buffer size */
#define VLC_BUFF_SIZE	8196		/* VLC buffer size */
#define SLICE_IMG_SIZE	(16*256)	/* slice image buffer size */

#define START_FRAME	1		/* start frame ID */
#define END_FRAME   	577		/* end frame ID */
#define SCR_WIDTH   	0		/* movie width */
#define SCR_HEIGHT  	0		/* movie height */

#define PPW		1     		/* pixel per short-word */
#define IS_RGB24	0		/* RGB_24 flag is 0 */
#define DCT_MODE	2 		/* 16 bit decode */



/*
 *  decode environment : デコード環境変数
 */
typedef struct {
    u_long	*vlcbuf[2];	/* vlc buffer (double) */
    int		vlcid;      	/* vlc buffer ID */
    u_short	*imgbuf[2];	/* decode image buffer （double）*/
    int		imgid;    	/* corrently use buffer id */
    RECT	rect[2];	/* double buffer orign (upper left point) */
    int		rectid;		/* currently translating buffer id */
    RECT	slice;		/* the region decoded by once DecDCTout() */
    int		isdone;		/* the flag of decoding whole frame */
} DECENV;

static DECENV   dec;       
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
static int	StrWidth  = 0;  /* resolution of movie */
static int	StrHeight = 0;	/* flags indicating image height change */
static int	Rewind_Switch;  /* flags indicating end of the movie */
static CdlFILE	FileID;		/* CD file handle */

/*
 *  prototypes :
 *  関数のプロトタイプ宣言
 */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);


/*
 *  animation subroutine forground process :
 *  アニメーションサブルーチン フォアグラウンドプロセス
 */
int animInit(char *name, int x, int y)
{
    /* search file : ファイルをサーチ */
    if (CdSearchFile(&FileID, name) == 0) {
        printf("file not found\n");
        return 1;
    }
    
    /* set the position of vram : VRAM上の座標値を設定 */
    strSetDefDecEnv(&dec, x, y, x, y);

    /* init streaming system & kick cd : ストリーミング初期化＆開始 */
    strInit(&FileID.pos, strCallback);
    
    /* VLC decode the first frame : 最初のフレームの VLCデコードを行なう */
    strNextVlc(&dec);
    
    /* initialize flags: フラグを初期化 */
    Rewind_Switch = 0;
    dec.isdone = 1;
}

int animPoll(void)
{
    static int strNextSync();
    
    /* フレームの処理が終了しているか */
    if (dec.isdone == 0)
	    return(0);

    /* リングバッファにデータが用意されているか */
    if (strNextSync())
	    return(0);
    
    /* VLC をデコード開始 */
    dec.isdone = 0;
    
    /* start DCT decoding the result of VLC decoded data :
       VLCの完了したデータをDCTデコード開始（MDECへ送信） */
    DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
    /* prepare for recieving the result of DCT decode :
       DCTデコード結果の受信の準備をする            */
    /* next DecDCTout is called in DecDCToutCallback :
       この後の処理はコールバックルーチンで行なう */
    DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
    /* decode the next frame's VLC data :
       次のフレームのデータの VLC デコード */
    strNextVlc(&dec);

    /* check rewind switch: 巻き戻しフラグを点検 */
    if(Rewind_Switch == 1) {
	    Rewind_Switch = 0;
	    strKickCD(&FileID.pos);
	    return(2);
    }
    
    /* return with busy */
    return(1);
}

/*
 * init DECENV    buffer0(x0,y0),buffer1(x1,y1) :
 * デコード環境を初期化
 *  x0,y0 デコードした画像の転送先座標（バッファ０）
 *  x1,y1 デコードした画像の転送先座標（バッファ１）
 *
 */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1)
{

    dec->vlcbuf[0] = vlcbuf0;
    dec->vlcbuf[1] = vlcbuf1;
    dec->vlcid     = 0;

    dec->imgbuf[0] = imgbuf0;
    dec->imgbuf[1] = imgbuf1;
    dec->imgid     = 0;

    /* width and height of rect[] are set dynamicaly according to STR data :
      rect[]の幅／高さはSTRデータの値によってセットされる */
    dec->rect[0].x = x0;
    dec->rect[0].y = y0;
    dec->rect[1].x = x1;
    dec->rect[1].y = y1;
    dec->rectid    = 0;

    dec->slice.x = x0;
    dec->slice.y = y0;
    dec->slice.w = 16*PPW;

    dec->isdone    = 0;
}


/*
 * init the streaming environment and start the cdrom :
 * ストリーミング環境を初期化して開始
 */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* cold reset mdec : MDEC をリセット */
    DecDCTReset(0);
    
    /* set the callback after 1 block MDEC decoding :
       MDECが１デコードブロックを処理した時のコールバックを定義する */
    DecDCToutCallback(callback);
    
    /* set the ring buffer : リングバッファの設定 */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* init the streaming library : ストリーミングをセットアップ */
    /* end frame is set endless : 終了フレーム=∞に設定   */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    /* start the cdrom : ストリーミングリード開始 */
    strKickCD(loc);
}

/*
 *  back ground process
 *  callback of DecDCTout() :
 *  バックグラウンドプロセス
 *  (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void strCallback()
{
  RECT snap_rect;
  int  id;
  u_long *ot, *BreakDraw();
  
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* on the RGB24 bit mode , call
			       StCdInterrupt manually at this timing :
			       RGB24の時はここで起動する */
        StCdIntrFlag = 0;
    }
#endif
    
  id = dec.imgid;
  snap_rect = dec.slice;
  
    /* switch the id of decoding buffer : 画像デコード領域の切替え */
    dec.imgid = dec.imgid? 0:1;

    /* update slice(rectangle) position :
      スライス（短柵矩形）領域をひとつ右に更新 */
    dec.slice.x += dec.slice.w;
    
    /* remaining slice ? : 残りのスライスがあるか？ */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* prepare for next slice : 次のスライスをデコード開始 */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
    }
    /* last slice ; end of 1 frame : 最終スライス＝１フレーム終了 */
    else {
        /* set the decoding done flag : 終ったことを通知 */
        dec.isdone = 1;
        
        /* update the position on VRAM : 転送先座標値を更新 */
        dec.rectid = dec.rectid? 0: 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y;
    }
  
  
  /* interrupt current drawing: 実行中の描画を一旦停止する */
  ot = BreakDraw();
  
  /* transfer the decoded data to VRAM :
     デコード結果をフレームバッファに転送 */
  LoadImage(&snap_rect, (u_long *)dec.imgbuf[id]);

  /* restart drawing: 再開 */
  if (ot)
	  DrawOTag(ot);		
}


/*
 *  execute VLC decoding
 *  the decoding data is the next frame's :
 *  VLCデコードの実行
 *  次の1フレームのデータのVLCデコードを行なう
 */
static void strNextVlc(DECENV *dec)
{
    int cnt = WAIT_TIME;
    u_long  *next;
    static u_long *strNext();

    /* get the 1 frame streaming data : データを１フレーム分取り出す */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return;
    }
    
    /* switch the decoding area : VLCデコード領域の切替え */
    dec->vlcid = dec->vlcid? 0: 1;

    /* VLC decode */
    DecDCTvlc(next, dec->vlcbuf[dec->vlcid]);

    /* free the ring buffer : リングバッファのフレームの領域を解放する */
    StFreeRing(next);

    return;
}

/*
 *  get the status of 1 frame streaming data
 *  return vale     already setup 1 frame -> 0
 *                  not yet setup 1 frame -> 1 :
 *  リングバッファからのデータの取り出し
 *  （返り値）  リング上に１フレーム分のデータが揃った  ＝ 0
 *              まだリング上にデータが揃わない          ＝ 1
 */
static u_long   *addr;
static StHEADER *sector;

static int strNextSync(void)
{
   if (StNextStatus((u_long **)&addr,(u_long **)&sector)==StCOMPLETE)
     return (0);
   else
     return (1);
}

static u_long *strNext(DECENV *dec)
{
    int     cnt = WAIT_TIME;

    /* get the 1 frame streaming data withe TIME-OUT :
       データを取り出す（タイムアウト付き） */
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
      if (--cnt == 0)
	return(0);
    }
    
    /* if the frame number greater than the end frame, set the end switch :
       現在のフレーム番号が指定値なら終了  */
    if(sector->frameCount >= END_FRAME) {
        Rewind_Switch = 1;
    }
    
    /* if the resolution is differ to previous frame, clear frame buffer :
       画面の解像度が 前のフレームと違うならば ClearImage を実行 */
    if(StrWidth != sector->width || StrHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT);
        ClearImage(&rect, 0, 0, 0);
        
        StrWidth  = sector->width;
        StrHeight = sector->height;
    }
    
    /* set DECENV according to the data on the STR format :
       STRフォーマットのヘッダに合わせてデコード環境を変更する */
    dec->rect[0].w = dec->rect[1].w = StrWidth*PPW;
    dec->rect[0].h = dec->rect[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*
 *  wait for finish decodeing 1 frame with TIME-OUT :
 *  １フレームのデコード終了を待つ
 *  時間を監視してタイムアウトをチェック
 */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;
	    
    /* wait for the decod is done flag set by background process :
       バックグラウンドプロセスがisdoneを立てるまで待つ */
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            /* if timeout force to switch buffer : 強制的に切替える */
            printf("time out in decoding !\n");
            dec->isdone = 1;
            dec->rectid = dec->rectid? 0: 1;
            dec->slice.x = dec->rect[dec->rectid].x;
            dec->slice.y = dec->rect[dec->rectid].y;
        }
    }
    dec->isdone = 0;
}


/*
 *  start streaming :
 *  CDROMを指定位置からストリーミング開始する
 */
static void strKickCD(CdlLOC *loc)
{
  u_char param;

  param = CdlModeSpeed;
  
 loop:
  /* seek to the destination : 目的地まで Seek する */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
  while (CdControl(CdlSetmode, &param, 0) == 0);
  VSync(3);  /* wait for 3 VSync when changing the speed :
		倍速に切り替えてから ３V待つ必要がある */
    /* out the read command with streaming mode :
       ストリーミングモードを追加してコマンド発行 */
  if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
