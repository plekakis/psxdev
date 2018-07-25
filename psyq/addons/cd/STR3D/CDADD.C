/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *   動画BGプログラム: ストリーミング部
 *
 *   Ver 1.00  1995 Feb 21
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 *   
 *  Thease codes are anim subroutines convenient for conbining graphics.
 *
 *  tuto0.c (contains main routine) calls thease subroutines.
 *
 *  Thease subroutine packages have higher priority for streaming than
 *  graphics , so you must allocate double buffers for whole frame image
 *  (16bit/pixcel) and double buffers for VLC decodeing.
 *
 *  If you don't have the priority for the speed of streaming ,
 *  you may reduce memory by preparing for the only single buffer.
 *
 *  The memory size you need for this program is changed
 *  by the size of the image. You must difine the size of the image
 *  by setting WIDTH & HEIGHT in the program. If the value of width
 *  and height written in STR header exceed the value WIDTH & HEIGT
 *  you set , program is halt , so you must set maximum value to
 *  WIDTH and HEIGHT in the program.
 *  
 *  inner function cdrom_play() seeks preset position and plays back
 *  STR format data. But it dosn't have fully error handling. You must
 *  rewrite that routine for the products.
 :
 *  このサブルーチン群は 動画をグラフィックスと組み合わせる時に便利な
 *  サブルーチンです。
 *
 *  メインルーチン tuto0.cから 公開関数を呼んでいます。
 *
 *  このサブルーチンパッケージは 動画の処理を優先的に行ないます。MDECが
 *  休まないように メモリもダブルバッファで持っています。そのため
 *  メインメモリ上に ２画面分の 16bit/pixel イメージデータと それを作るための
 *  ２画面分の VLCバッファをとります。
 *  動画のスピードを優先にせず メインメモリを節約したい場合は イメージデータ
 *  領域 及び VLCバッファ領域のダブルバッファは省略できます。省略した
 *  サブルーチンパッケージは次にリリースする予定です。
 *
 *  メインメモリ上に確保する領域は 動画のサイズによって増減します。
 *  WIDTH HIGHTに動画のサイズをいれ確保する領域の大きさを決定します。
 *  ただし 動画のデータの中に埋め込まれている 動画のサイズが WIDTH,HIGHT
 *  を超えると確保した領域を超えてメモリを破壊し暴走するので かならず
 *  扱う動画の最大のサイズを WIDTH,HIGHTで指定しておく必要があります。
 *
 *
 *  内部関数の cdrom_play()は 特定の場所にシークし アニメーションの再生を
 *  始める関数ですが このプログラムではシーク動作に失敗した時のエラー処理
 *  が完全ではありません。注意して下さい。
 */

/*
 * The words number of VLC
 * If you want limit the running time for VLC decoding , you set the
 * maximum words VLC decodes.
 * DecDCTvlc() returns main after decoding VLC_SIZE data.
 :
 * VLC のデコードワード数
 * VLC デコードが長時間他の処理をブロックすると不都合な場合は、一回の
 * デコードワードの最大値をここで指定する。
 * DecDCTvlc() は VLC_SIZEワードの VLC をデコードすると一旦制御をアプリ
 * ケーションに戻す。
 */
#define VLC_SIZE	2048

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libsnd.h>

/*********************** public functions : 公開関数 *******************/
void init_anim(void);
void start_anim(CdlFILE *,u_long);
void load_image_for_mdec_data(u_long,u_long);
void poll_anim(CdlFILE *);

/*********************** inner functions : 内部関数 *******************/
static void setup_frame();
static void out_callback();
static u_long *get_frame_movie(u_long);
static cdrom_play(CdlLOC *);


#define WIDTH 320		/* the maximum size of image (width) :
				   ストリーミングデータの最大の横 */
#define HIGHT 240		/* the maximum size of image (height) :
				   ストリーミングデータの最大の縦 */
#define SLICE  16		/* the width of slize image : タンザクの幅 */



/*
 *	DECODING ENVIRONMENT : デコード環境
 */
typedef struct {
        u_long	*vlcbuf[2];	/* VLC buffer （double） */
	int	vlcid;		/* current id of VLC buffer :
				   現在 VLC デコード中バッファの ID */
	u_short	*imgbuf[2];	/* decoded image buffer (double) :
				   デコード画像バッファ（ダブル）*/
	RECT	rect[2];	/* transfer area (double) :
				  転送エリア（ダブルバッファ） */
	int	rectid;		/* current transfer buffer id :
				   現在転送中のバッファ ID */
	RECT	slice;		/* slice image of one DecDCTout() :
				  １回の DecDCTout で取り出す領域 */
} DECENV;

static DECENV	dec;		/* instance of DECODE ENVIRONMENT :
				   デコード環境の実体 */
static int	MdecFree;	/* MDEC BUSY STATUS */
static int      Frame_ny;   /* STREAMING FLAME DATA NOT READY */
static int      slicew = 0;	/* work of width and height of the
				   streamed iamge : 画面の横と縦 */
static int      sliceh = 0;
static int      Vlc_size = 0;	/* the size of one time VLC decoding :
				   一度にVLCするサイズ */

static int      Rewind_Switch;	/* set 1 after playing end frame :
				   CDが終りまでいくと１になる */
static u_long   EndFrame = 0;	/* the last frame of the streaming :
				   ストリーミングを終わらせるフレーム */

#define RING_SIZE 24		/* the size of ring buffer (the number of
				   sectors) :
				   ストリーミングライブラリで使用される
				   リングバッファのサイズ */

u_long SECT_BUFF[RING_SIZE*SECTOR_SIZE]; /* the instance of ring buffer :
					    ストリーミングライブラリで使用
					    されるリングバッファの実体 */


/*
 * back ground process (callback function when DecDCTout() is finished) :
 * バックグラウンドプロセス 
 * (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void out_callback()
{
  MdecFree = 1;			/* when this function is callbacked ,
				   one frame decodeing is done. :
				   この関数がコールバックされる時には１枚の
				   絵のデコードが終わっている */
}


/*
 * forground process : フォアグラウンドプロセス
 */	
#define setRECT(r, _x, _y, _w, _h) \
	(r)->x = (_x),(r)->y = (_y),(r)->w = (_w),(r)->h = (_h)


DISPENV	disp;			/* DISPLAY ENVIRONMENT : 表示環境 */
u_long	vlcbuf0[WIDTH/2*HIGHT/2];	/* incorrect size : 大きさ適当 */
u_long	vlcbuf1[WIDTH/2*HIGHT/2];	/* incorrect size : 大きさ適当 */
u_short	imgbuf0[WIDTH*HIGHT];	/* 1 frame image buffer :
				   1画面分のフレームバッファ */
u_short	imgbuf1[WIDTH*HIGHT];


void init_anim()		/* initialize routines : 初期化ルーチン群 */
{
  RECT rect;
  int i;
  
  DecDCTReset(0);		/* reset MDEC : MDEC をリセット */
  MdecFree = 0;			/* reset the MDEC idle flag : 旗を下げる */
  Frame_ny = 0;
  Rewind_Switch = 0;		/* initialize stream end flag : 巻き戻し０ */
  Vlc_size = 0;
  
  /* set values of decoding structures : デコード構造体に値を設定 */
  dec.vlcbuf[0] = vlcbuf0;
  dec.vlcbuf[1] = vlcbuf1;
  dec.vlcid     = 0;
  dec.imgbuf[0]  = imgbuf0;
  dec.imgbuf[1]  = imgbuf1;
  dec.rectid    = 0;
  setRECT(&dec.rect[0], 0,   0, WIDTH, HIGHT);
  setRECT(&dec.rect[1], 0, HIGHT, WIDTH, HIGHT);
  setRECT(&dec.slice,   0,   0,  SLICE, HIGHT);

  setRECT(&rect, 0, 0, WIDTH, HIGHT*2);	/* clear display buff */
  ClearImage(&rect, 0, 0, 0);

  for(i=0;i<WIDTH*HIGHT;i++)	/* clear imgbuf */
    {
      imgbuf0[i]=0;
      imgbuf1[i]=0;
    }
  
  SsInit();		/* SPU lib init */
  SsSetSerialAttr(SS_SERIAL_A,SS_MIX,SS_SON); /* Audio out enable */
  SsSetSerialVol(SS_SERIAL_A,0x7fff,0x7fff);  /* Audio volume set */
  
  /* set the ring buffer :
     ストリーミングライブラリで使われるリングバッファの設定 */
  StSetRing(SECT_BUFF,RING_SIZE);
  
  /* set the callback of MDEC decodes 1 block :
    MDECが１デコードブロックを処理した時のコールバックを定義する */
  DecDCToutCallback(out_callback);
}


/*
 * start streaming by kicking CDROM
 * if the end frame is streamed , end_cdrom is called back.
 *
 * fp         file pointer of the streaming file
 * endFrame   end frame no , notice that you must set 2 or 3 frame
 *            before the real end frame , because if you set the
 *            last frame for end frame and unfortunately skips end frame,
 *            you can't get the end point of the streaming data.
 :
 * CDROMを倍速でスタートさせストリーミングを開始する
 * CDROMが最後までいったら end_cdrom() がコールバックされる
 *
 * fp          再生するストリーミングファイルのファイルポインタ
 * endFrame    ストリーミングの最後のフレーム番号（本当の終わりよりは
 *             2から3フレーム前に設定しないと フレームが読み飛ばされた時に
 *             終わりが検出できないので注意
 */
void start_anim(fp,endFrame)
CdlFILE *fp;
u_long  endFrame;
{
  int ret_val;
  u_long *frame_addr;
  
  StSetStream(0,1,0xffffffff,0,0);
  
  cdrom_play(&fp->pos);

  EndFrame = endFrame;
  
  /* get first frame : 最初の１フレームをとってくる */
  while((frame_addr = get_frame_movie(EndFrame))==0);
  DecDCTvlcSize(0);  
  /* decode VLC data : VLCデコードを行なう */
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  
  /* You must free one frame on the ring buffer because you finish decoding :
    VLCがデコードされたらリングバッファのそのフレームの領域は
     必要ないのでリングバッファのフレームの領域を解放する */
  StFreeRing(frame_addr);
  
  /* set up input of MDEC one whole frame data :
     １フレーム全てのデータをMDECへ入力するように設定する */
  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
  
  /* set up output of MDEC one whole frame data :
     1フレーム全てのデータをMDECからとり出すように設定する */
  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
  
  /* swap ID : ID をスワップ */
  dec.vlcid = dec.vlcid? 0: 1;
  
  /* get next data , if next data is error , returns 1. :
    次のフレームのデータをとってくる もし 次のフレームの
     データがエラーだったら 1が返る */
  /* if the data is error , free that earea and get next frame :
    エラーだったらリングバッファをクリアして
     次のフレームのデータをとってくる */
  while((frame_addr=get_frame_movie(EndFrame))==0);
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  StFreeRing(frame_addr);
}

/*
 * get next frame from ring buffer
 * if prepared one frame data , returns the top address of that data
 * if not prepared one frame data , returns NULL
 :
 * 次の１フレームのMOVIEフォーマットのデータをリングバッファからとってくる
 * 1フレーム分のデータが揃っていれば １フレームのデータの先頭アドレスを
 * まだ揃ってなければ NULLを返す
 */
static u_long *get_frame_movie(endFrame)
u_long endFrame;
{
  u_long   *addr;
  StHEADER *sector;
  
  if(StGetNext(&addr,(u_long **)&sector))
    return(0);			/* not yet prepared one frame data :
				   まだ１フレームのデータがリングバッファ
				   上に揃っていない */
  
  if(sector->frameCount > endFrame)
    Rewind_Switch = 1;
  
  /* if the resolution of previous frame is different of current rezolution,
     clear screen :
     画面の解像度が 前のフレームと違うならば ClearImage を実行 */
  if(slicew != sector->width || sliceh != sector->height) {
    RECT    rect;
    setRECT(&rect, 0, 0, WIDTH, HIGHT);
    ClearImage(&rect, 0, 0, 0);
    slicew  = sector->width;
    sliceh  = sector->height;
  }
  
  /* update DECODE ENVIRONMENT according to streaming header :
     ミニヘッダに合わせてデコード環境を変更する */
  dec.rect[0].w = dec.rect[1].w = slicew;
  dec.rect[0].h = dec.rect[1].h = sliceh;
  dec.slice.h   = sliceh;
  
  return(addr);
}


/*
 * transfer whole screen slice image to VRAM at once
 * arguments are the location of the VRAM :
 * メインメモリにタンザク順でならんでいるMDECによってデコードされた
 * イメージデータを タンザク毎に VRAMへ転送する
 * 引数は VRAMのアドレスを示す
 */
void load_image_for_mdec_data(xo,yo)
u_long xo,yo;
{
  int i,x;
  RECT tmprect;
  
  for(i=0;i<slicew/16;i++)
    {
      x = i*16;
      tmprect.x = x+xo;
      tmprect.y = yo;
      tmprect.w = 16;
      tmprect.h = sliceh;
      
      LoadImage(&tmprect, (u_long*)(dec.imgbuf[dec.rectid? 0: 1]+x*sliceh));
      DrawSync(0);		/* wait for complete transfering :
				   転送し終わるまで待つ */
    }
}


/* 
 *  start decoding next frame
 *  MdecFree and Frame_ny are status flags
 *  MdecFree is set when MDEC is idle
 *  Frame_ny is set when not preparing one frame data in the ring buffer :
 *  次のフレームのデコードを開始する
 *  ステイタスフラグとして MdecFreeとFrame_nyがある
 *  MdecFreeは MDECが画像のでコードをしていない時に１になる
 *  Frame_nyは リングバッファに１フレーム分のデータが用意できていない時に
 *  １になる
 *  
 */
static void setup_frame()
{
  static u_long *frame_addr;
  
  if(Vlc_size == 0)
    {
      if(Frame_ny == 0)
	{
	  dec.rectid = dec.rectid? 0: 1;
	  
	  /* tarnsfer VLC decoded data : VLC の完了したデータを送信 */
	  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
	  
	  /* prepare for recieving first decoded block :
	    最初のデコードブロックの受信の準備をする */
	  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
	  
	  /* swap id : ID をスワップ */
	  dec.vlcid = dec.vlcid? 0: 1;
	  MdecFree = 0;
	}
      
      if((frame_addr=get_frame_movie(EndFrame))==0)
	{
	  Frame_ny = 1;
	  return;
	}
      Frame_ny = 0;
      
      DecDCTvlcSize(VLC_SIZE);
      Vlc_size = DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
    }
  else
    Vlc_size = DecDCTvlc(0,0);
  
  if(Vlc_size==0)
    if(StFreeRing(frame_addr))
      printf("FREE ERROR\n");
}

/*
 * execute animation routine by polling
 * if MDEC is not idle and not finish next frame's VLC decoding
 * (MdecFree == 0 & Frame_ny == 0) , return imediately :
 * ポーリングでアニメーションのルーチンを起動する
 * MDECがデータのデコード中で次のフレームのVLCも完了している場合 すなわち
 * MdecFree == 0 かつ Frame_ny == 0 の場合は 何もしないで抜ける
 */
void poll_anim(fp)
CdlFILE *fp;
{
  if(Rewind_Switch) /* if after end frame , jumpt to start location :
		       巻き戻しが設定されていたら 最初にジャンプ */
    {
      cdrom_play(&fp->pos);
      Rewind_Switch = 0;
    }
  
  /* if you execute streaming routine , you call setup_frame() :
     ストリーミングルーチンを起動する必要がある場合は setup_frame()を呼ぶ */
  if(MdecFree || Frame_ny || Vlc_size)
    setup_frame();
}


/*
 * start animation by seeking loc
 * Notice that this routine have no error handling , so you must coding
 * error handling routine for the product
 : 
 * locにシークしアニメーションをスタートさせる
 * ただし CdRead2()でシークに失敗した場合のエラー処理が入っていないので
 * 注意すること（製品版では 入れる必要がある）
 */
static cdrom_play(CdlLOC *loc)
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
