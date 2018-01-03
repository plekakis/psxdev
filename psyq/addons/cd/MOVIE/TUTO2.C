/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  streaming movie from CD-ROM with memory streaming :
 *  CD-ROM からムービーをストリーミングするサンプル
 *  パッドの入力によって メモリーストリーミングに切り替える
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        Sep,26,1995     yutaka
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <kernel.h>


#define FILE_NAME   "\\DATA\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*
 *  16bit/pixel mode or 24 bit/pixel mode :  
 *  デコードする色数の指定(16bit/24bit)
 */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* pixel/short word :
		       １ショートワードに何ピクセルあるか */
#define DCT_MODE    3   /* 24bit decode :24bit モードでデコード */
#else
#define PPW 1       /* pixel/short word :
		       １ショートワードに何ピクセルあるか */
#define DCT_MODE    2   /* 16 bit decode : 16bit モードでデコード */
#endif

/*
 *  decode environment : デコード環境変数
 */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer （double）*/
    int vlcid;      /* current decode buffer id :
		       現在 VLC デコード中バッファの ID */
    u_short *imgbuf[2]; /* decode image buffer （double）*/
    int imgid;      /* corrently use buffer id :
		       現在使用中の画像バッファのID */
    RECT    rect[2];    /* double buffer orign(upper left point) address
			   on the VRAM (double buffer) :
			   VRAM上座標値（ダブルバッファ） */
    int rectid;     /* currently translating buffer id :
		       現在転送中のバッファ ID */
    RECT    slice;      /* the region decoded by once DecDCTout() :
			   １回の DecDCTout で取り出す領域 */
    int isdone;     /* the flag of decoding whole frame :
		       １フレーム分のデータができたか */
    int onmem;		/* flag for memory streaming :
			   メモリストリーミングかどうかのフラグ */
} DECENV;
static DECENV   dec;        /* instance of DECENV : デコード環境の実体 */

/*
 *  Ring buffer for STREAMING
 *  minmum size is two frame :
 *  ストリーミング用リングバッファ
 *  CD-ROMからのデータをストック
 *  最低２フレーム分の容量を確保する。
 *  メモリストリーミングのため大容量バッファを確保
 */
#define RING_SIZE   256      /* 32 sectors : 単位はセクタ */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*
 *  VLC buffer(double buffer)
 *  stock the result of VLC decode :
 *  VLCバッファ（ダブルバッファ）
 *  VLCデコード後の中間データをストック
 */
#define VLC_BUFF_SIZE 320/2*256     /* not correct value :
				       とりあえず充分な大きさ */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*
 *  image buffer(double buffer)
 *  stock the result of MDEC
 *  rectangle of 16(width) by XX(height) :
 *  イメージバッファ（ダブルバッファ）
 *  DCTデコード後のイメージデータをストック
 *  横幅16ピクセルの矩形毎にデコード＆転送
 */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];

/*
 *  その他の変数
 */
static int  StrWidth  = 0;  /* resolution of movie :
			       ムービー画像の大きさ（横と縦） */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* the end flag set after last frame :
			       終了フラグ:所定のフレームまで再生すると１になる */

/*
 *  prototypes :  
 *  関数のプロトタイプ宣言
 */
static int  anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

main()
{
    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    while(1) {
        if(anim()==0)
	   return 0;     /* animation subroutine :
			    アニメーションサブルーチン */
    }
}

/*
 *  animation subroutine forground process :  
 *  アニメーションサブルーチン フォアグラウンドプロセス
 */
static int anim()
{
    DISPENV disp;       /* display buffer : 表示バッファ */
    DRAWENV draw;       /* drawing buffer : 描画バッファ */
    int id;     /* display buffer id : 表示バッファの ID */
    CdlFILE file;
    int padd;
    
    /* search file : ファイルをサーチ */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
        PadStop();
	ResetGraph(3);
        StopCallback();
        return 0;
    }
    
    /* set the position of vram : VRAM上の座標値を設定 */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);
    
    /* init streaming system & kick cd : ストリーミング初期化＆開始 */
    strInit(&file.pos, strCallback);
    
    /* VLC decode the first frame : 最初のフレームの VLCデコードを行なう */
    strNextVlc(&dec);
    
    Rewind_Switch = 0;
    
    while (1) {
        /* start DCT decoding the result of VLC decoded data :      
	   VLCの完了したデータをDCTエンコード開始（MDECへ送信） */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare for recieving the result of DCT decode :
	   DCTデコード結果の受信の準備をする            */
        /* next DecDCTout is called in DecDCToutCallback :	
           この後の処理はコールバックルーチンで行なう */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* decode the next frame's VLC data :
	   次のフレームのデータの VLC デコード */
        strNextVlc(&dec);
        
        /* wait for whole decode process per 1 frame :	
	   １フレームのデコードが終了するのを待つ */
        strSync(&dec, 0);
        
        /* wait for V-Vlank : V-BLNK を待つ */
        VSync(0);
        
        /* swap the display buffer : 表示バッファをスワップ     */
        /* notice that the display buffer is the opossite side of
	   decoding buffer :
	   表示バッファはデコード中バッファの逆であることに注意 */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);
/*      SetDefDrawEnv(&draw, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);*/
        
#if IS_RGB24==1
        disp.isrgb24 = IS_RGB24;
        disp.disp.w = disp.disp.w*2/3;
#endif
        PutDispEnv(&disp);
/*      PutDrawEnv(&draw);*/
        SetDispMask(1);     /* display enable : 表示許可 */
        
        if(Rewind_Switch == 1)
	  break;
        
        if((padd = PadRead(1)) & PADk)
	  /* stop button pressed exit animation routine :
	     ストップボタンが押されたらアニメーションを抜ける */
	  break;

	/* start memory straming on pushed o button :
	   ○ボタンが押されたら メモリストリーミングを開始する */
        if(padd & PADRright)
	  StSetMask(0,0,0);

	/* stop memory streaming on pushed x button :
	   ×ボタンが押されたら メモリストリーミングをやめる */
        if(padd & PADRdown)
	  {
	    dec.onmem = 0;
	    Rewind_Switch = 1;
	    break;
	  }
      }
    
    /* post processing of animation routine : アニメーション後処理 */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
    if(Rewind_Switch==0) {
       PadStop();
       ResetGraph(3);
       StopCallback();
       return 0;
       }
    else
       return 1;
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
    dec->onmem     = 0;
}


/*
 * init the streaming environment and start the cdrom :  
 * ストリーミング環境を初期化して開始
 */
static void strInit(CdlLOC *loc, void (*callback)())
{
    static void repeatCallback();
  
    /* cold reset mdec : MDEC をリセット */  
    DecDCTReset(0);
  
    /* set the callback after 1 block MDEC decoding :
       MDECが１デコードブロックを処理した時のコールバックを定義する */
    DecDCToutCallback(callback);
    
    /* set the ring buffer : リングバッファの設定 */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* init the streaming library : ストリーミングをセットアップ */
    /* end frame is set endless : 終了フレーム=∞に設定   */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, (void *)repeatCallback);
    
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
  
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();     /* on the RGB24 bit mode , call
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
  
  /* transfer the decoded data to VRAM :
     デコード結果をフレームバッファに転送 */
  LoadImage(&snap_rect, (u_long *)dec.imgbuf[id]);
}


static void repeatCallback()
{
  dec.onmem = 1;
/*  CdControlF(CdlPause,0);*/
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
 *  get the 1 frame streaming data
 *  return vale     normal end -> top address of 1 frame streaming data
 *                  error      -> NULL :
 *  リングバッファからのデータの取り出し
 *  （返り値）  正常終了時＝リングバッファの先頭アドレス
 *          エラー発生時＝NULL
 */
static u_long *strNext(DECENV *dec)
{
    u_long      *addr;
    StHEADER    *sector;
    int     cnt = WAIT_TIME;
    
    /* get the 1 frame streaming data withe TIME-OUT :    
       データを取り出す（タイムアウト付き）
       メモリストリーミングの時は StGetNextS()を用いて取り出す */
    if(dec->onmem)
      while(StGetNextS((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
	  return(0);
      }
    else
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
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT*2);
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
