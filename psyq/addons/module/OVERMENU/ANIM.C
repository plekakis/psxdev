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
 *  ------------------------------------------
 *  1.00        Jul,14,1994     yutaka
 *  1.10        Sep,01,1994     suzu
 *  1.20        Oct,24,1994     yutaka(anim subroutine化)
 *  1.30        Jun,02,1995     yutaka(後処理)
 *  1.40        Jul,10,1995     masa(imgbufダブルバッファ化)
 *  1.50        Jul,20,1995     ume(画面クリア改良)
 *  1.50a	Aug,03,1996     yoshi (for overlay)
 *  1.50b	Mar,04,1996     yoshi (added English comments)
 *====================================================================
 *    This was rewritten as a child process. Compile conditionally with OVERLAY.
 *    Called while parent is emitting sound.
 *    Playback is performed at 24bits. :
 * 子プロセス用に書き直した。OVERLAYで条件コンパイルする。
 * 親でサウンドを鳴らしたままの状態で呼ばれる。
 * 再生は 24bit  で行っている。
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
 *  specification of the number of colors to decode (16bit/24bit) : 
 *  デコードする色数の指定(16bit/24bit)
 */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* how many pixels in 1 short word? : 
                       １ショートワードに何ピクセルあるか */
#define DCT_MODE    3   /* decode in 24bit mode : 24bit モードでデコード */
#else
#define PPW 1       /* how many pixels in 1 short word? : 
                       １ショートワードに何ピクセルあるか */
#define DCT_MODE    2   /* decode in 16 bit mode : 16bit モードでデコード */
#endif

/*
 *  decode environment variable : デコード環境変数
 */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double buffer) : VLC バッファ（ダブルバッファ） */
    int vlcid;      /* Cureent buffer ID for VLC decode operation : 
                       現在 VLC デコード中バッファの ID */
    u_short *imgbuf[2]; /* decode image buffer (double buffer) : 
                           デコード画像バッファ（ダブルバッファ）*/
    int imgid;      /* ID of image buffer being used currently : 
                       現在使用中の画像バッファのID */
    RECT    rect[2];    /* coordinates on VRAM (double buffer) : 
                           VRAM上座標値（ダブルバッファ） */
    int rectid;     /* area retrieved for one DecDCTout : 現在転送中のバッファ ID */
    RECT    slice;      /* area retrieved for one DecDCTout : 
                           １回の DecDCTout で取り出す領域 */
    int isdone;     /* is one frame's worth of data prepared? : 
                       １フレーム分のデータができたか */
} DECENV;
static DECENV   dec;        /* actual decode environment : 
                               デコード環境の実体 */

/*
 * ring buffer for streaming
 * stocks data from the CD-ROM
 * reserve capacity for at least 2 frames' worth
 *  ストリーミング用リングバッファ
 *  CD-ROMからのデータをストック
 *  最低２フレーム分の容量を確保する。
 */
#define RING_SIZE   32      /* 単位はセクタ */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*
 *  VLC buffer (double buffer)
 *  stock intermediate data after VLC decoding
 *  VLCバッファ（ダブルバッファ）
 *  VLCデコード後の中間データをストック
 */
#define VLC_BUFF_SIZE 320/2*256     /* とりあえず充分な大きさ */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*
 * image buffer (double buffer)
 * stock image data from after DCT decoding
 * decode and transfer for rectangles 16 pixels wide
 *  イメージバッファ（ダブルバッファ）
 *  DCTデコード後のイメージデータをストック
 *  横幅16ピクセルの矩形毎にデコード＆転送
 */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
/*
 *  other variables : その他の変数
 */
static int  StrWidth  = 0;  /* size of movie image (horizontal and vertical) : 
                               ムービー画像の大きさ（横と縦） */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* end flag: set to 1 when playback has reached a specified frame : 
                               終了フラグ:所定のフレームまで再生すると１になる */

/*
 *  function prototype declaration : 関数のプロトタイプ宣言
 */
static void anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

#ifdef OVERLAY
child_anim()
#else
main()
#endif
{
    RECT rct;

    /* initialization other than animation are all the same : 
       アニメーション それ以外 共通の初期化 */
#ifdef OVERLAY
/* some tricks are used to allow smooth screen translation : 
   画面切り替えがスムーズに行くように少し工夫している */
    VSync(0);
    SetDispMask(0);
    ResetGraph(1);
    setRECT(&rct,0,0,1024,512);
    ClearImage(&rct,0,0,0);
    DrawSync(0);
#else
    ResetCallback();
    CdInit();
    PadInit(0);     /* reset PAD : PAD をリセット */
    ResetGraph(0);      /* reset GPU : GPU をリセット */
    SetGraphDebug(0);   /* set debug level : デバッグレベル設定 */
#endif
    
    anim();     /* animation subroutine : アニメーションサブルーチン */

	return(0);
}


/*
 *  animation subroutine foreground process : 
 *  アニメーションサブルーチン フォアグラウンドプロセス
 */
static void anim()
{
    DISPENV disp;       /* display buffer : 表示バッファ */
    DRAWENV draw;       /* drawing buffer : 描画バッファ */
    int id;     /* ID of display buffer : 表示バッファの ID */
    CdlFILE file;
    
    /* search file : ファイルをサーチ */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
        StopCallback();
        PadStop();
        exit();
    }
    
    /* set coordinates on VRAM : VRAM上の座標値を設定 */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);

    /* initalize and begin streaming : ストリーミング初期化＆開始 */
    strInit(&file.pos, strCallback);
    
    /* peform VLC decoding of the initial frame : 
       最初のフレームの VLCデコードを行なう */
    strNextVlc(&dec);
    
    Rewind_Switch = 0;
    
    while (1) {
        /* DCT encoding is begun on the data for which VLC has been completed 
           (send to MDEC) : 
           VLCの完了したデータをDCTエンコード開始（MDECへ送信） */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare to receive DCT decode results*/
        /* processing after this is performed with call back routines : 
           DCTデコード結果の受信の準備をする            
             この後の処理はコールバックルーチンで行なう */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* VLC decoding of the data for the next frame : 
           次のフレームのデータの VLC デコード */
        strNextVlc(&dec);
        
        /* wait for decoding of one frame to finish :
           １フレームのデコードが終了するのを待つ */
        strSync(&dec, 0);
        
        /* wait for V-BLNK : V-BLNK を待つ */
        VSync(0);
        
        /* swap display buffer  
           note that the display buffer is the reverse of the buffer being decoded :
           表示バッファをスワップ                                
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
        SetDispMask(1);     /* display permission : 表示許可 */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* exit animation if the stop button is pressed : 
                                   ストップボタンが押されたらアニメーション
                       を抜ける */
            break;
    }
    
    
    /* post-processing of animation :
       アニメーション後処理 */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
}


/*
 *    initialize decode environment
 *  the transfer coordinates of the image decoded from x0,y0 (buffer 0)
 *  the transfer coordinates of the image decoded from x1,y1 (buffer 1)
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

    /* the width/height of rect[] is set from the STR data values :
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
 * initialize streming environment and begin : 
   ストリーミング環境を初期化して開始
 */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* reset MDEC : MDEC をリセット */
    DecDCTReset(0);

    /* define call back for when MDEC has processed one decode block :
       MDECが１デコードブロックを処理した時のコールバックを定義する */
    DecDCToutCallback(callback);
    
    /* set ring buffer : 
       リングバッファの設定 */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* set up streaming  
       set final frame = infinity :
       ストリーミングをセットアップ   
        終了フレーム=∞に設定   */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    
    /* begin streaming read : 
       ストリーミングリード開始 */
    strKickCD(loc);
}


/*
 *    background process
 *  (call back function that is called when DecDCTout() is finished) :
 *  バックグラウンドプロセス 
 *  (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void strCallback()
{
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* run here if RGB24 : RGB24の時はここで起動する */
        StCdIntrFlag = 0;
    }
#endif
    
    /* transfer decode results to the frame buffer :
       デコード結果をフレームバッファに転送 */
    LoadImage(&dec.slice, (u_long *)dec.imgbuf[dec.imgid]);
    
    /* switch image decode area :
       画像デコード領域の切替え */
    dec.imgid = dec.imgid? 0:1;

    /* update slice (rectangular strip) area to next one on the right :
       スライス（短柵矩形）領域をひとつ右に更新 */
    dec.slice.x += dec.slice.w;
    
    /* any remaining slices? :
       残りのスライスがあるか？ */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
	    /* begin decoding next slice : 
           次のスライスをデコード開始 */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
    }
    /* final slice = 1 frame completed : 最終スライス＝１フレーム終了 */
    else {
        /* indicate completion : 終ったことを通知 */
        dec.isdone = 1;
        
        /* update transfer coordinates : 転送先座標値を更新 */
        dec.rectid = dec.rectid? 0: 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y;
    }
}


/*
 *  perform VLC decoding
 *  perform VLC decoding for next frame :
 *  VLCデコードの実行
 *  次の1フレームのデータのVLCデコードを行なう
 */
static void strNextVlc(DECENV *dec)
{
    int cnt = WAIT_TIME;
    u_long  *next;
    static u_long *strNext();


    /* retrieve one frame's worth of data : データを１フレーム分取り出す */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return;
    }
    
    /* switch VLC decode area : VLCデコード領域の切替え */
    dec->vlcid = dec->vlcid? 0: 1;

    /* VLC decode : VLCデコード */
    DecDCTvlc(next, dec->vlcbuf[dec->vlcid]);

    /* free up frame area of ring buffer : 
       リングバッファのフレームの領域を解放する */
    StFreeRing(next);

    return;
}

/*
 *    retrieve data from the ring buffer
 *    (return value)      normal completion = starting address of ring buffer
 *    error occurred = NULL
 *  リングバッファからのデータの取り出し
 *  （返り値）  正常終了時＝リングバッファの先頭アドレス
 *          エラー発生時＝NULL
 */
static u_long *strNext(DECENV *dec)
{
    u_long      *addr;
    StHEADER    *sector;
    int     cnt = WAIT_TIME;


    /* retrieve data (with timeout) :  
       データを取り出す（タイムアウト付き） */  
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }

    /* if current frame number is the specified value, then end :   
       現在のフレーム番号が指定値なら終了  */   
    if(sector->frameCount >= END_FRAME) {
        Rewind_Switch = 1;
    }
    
    /* if the resolution of the frame is different from that of the previous frame,
          perform ClearImage : 
       画面の解像度が 前のフレームと違うならば ClearImage を実行 */
    if(StrWidth != sector->width || StrHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT*2);
        ClearImage(&rect, 0, 0, 0);
        
        StrWidth  = sector->width;
        StrHeight = sector->height;
    }
    
    
    /* change decode environment to match the STR format header :
       STRフォーマットのヘッダに合わせてデコード環境を変更する */
    dec->rect[0].w = dec->rect[1].w = StrWidth*PPW;
    dec->rect[0].h = dec->rect[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*
 *  wait for the decoding of one frame to finish
 *  monitor time and check for a timeout
 *  １フレームのデコード終了を待つ
 *  時間を監視してタイムアウトをチェック
 */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;

    /* wait until the background process sets 'is done' :      
       バックグラウンドプロセスがisdoneを立てるまで待つ */      
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            /* timeout:  forcibly switch : timeout: 強制的に切替える */
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
 *  begin streaming the CDROM from the specified position :
 *  CDROMを指定位置からストリーミング開始する
 */
static void strKickCD(CdlLOC *loc)
{
 loop:
  /* Seek the target position : 目的地まで Seek する */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
    
    /* add streaming mode and issue the command : 
       ストリーミングモードを追加してコマンド発行 */
  if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}

