/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto6 simplest sample
 *
 *	Complete background on-memory movie decompression (with frame
 *	double buffering). Notice VLC decoding is done in CPU (foreground)
 *	
 :	バックグラウンド動画の再生（フレームダブルバッファ付き）
 *		ただし、VLC のデコードは表で実行する
 *	
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

/*
 * Decode Environment. notice that image buffer in main memory is single
 *デコード環境: メインメモリ上の解凍画像バッファはひとつなことに注意
 */
typedef struct {
	/* VLC buffer (double): VLC バッファ（ダブルバッファ） */
	u_long	*vlcbuf[2];	
	
	/* current VLC buffer ID: 現在 VLC デコード中バッファの ID */
	int	vlcid;		
	
	/* decoded image buffer (single): デコード画像バッファ（シングル）*/
	u_long	*imgbuf;	
	
	/* frame buffer area (double):  転送エリア（ダブルバッファ） */
	RECT	rect[2];	
	
	/* current decompressing buffer ID: 現在転送中のバッファ ID */
	int	rectid;		
	
	/* one slice area of one DecDCTout: １回の DecDCTout で取り出す領域 */
	RECT	slice;		
	
} DECENV;
static DECENV		dec;		
	
/* flag indicating the enf of decoding. 'volatile' option is necessary.
 :フレームの最後になると 1 になる。 volatile 指定は必須
 */
static volatile int	isEndOfFlame;	

/*
 * background operation. This function is called when DecDCTout is finished
 : バックグラウンドプロセス 
 * (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
void out_callback()
{
	/* transfer decoded data to frame buffer
	 :loadデコード結果をフレームバッファに転送
	 */
	LoadImage(&dec.slice, dec.imgbuf);
	
	/* update sliced image area: 短柵矩形領域をひとつ右に更新 */
	dec.slice.x += 16;

	/* if decoding is not complete: まだ足りなければ、*/
	if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
		/* get next slice image from MDEC: 次の短柵を受信 */
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	}
	/* when all frame image is decompressed: １フレーム分終ったら、*/
	else {
		/* notify the end of decompression: 終ったことを通知 */
		isEndOfFlame = 1;
		
		/* update ID of each buffer: ID を更新 */
		dec.rectid = dec.rectid? 0: 1;
		dec.slice.x = dec.rect[dec.rectid].x;
		dec.slice.y = dec.rect[dec.rectid].y;
	}
}		

/*
 * foreground operation: フォアグラウンドプロセス
 */	
/* following work buffer size should be dynamically alocated using malloc().
 * the size of buffer can be known by DecDCTvlcBufSize()
 : 以下の作業領域は malloc() を使用して動的に割り付けるべきです。
 * バッファのサイズは DecDCTvlcBufSize() で獲得できます。
 */	
static u_long	vlcbuf0[256*256];	
static u_long	vlcbuf1[256*256];	
static u_long	imgbuf[16*240/2];	/* 1 slice size: 短柵１個 */

static void StrRewind(void);
static u_long *StrNext(void);

/*
 * VLC maximum count of decoding 
 * DecDCTvlc() is foreground and if it occupies CPU for a long time,
 * set maximum VLC decode size here. DecDCTvlc returns after
 * decompression VLC_SIZE words. You can restart VLC decoding again
 * by calling DecDCTvlc() again
 *
 : VLC のデコードワード数:
 * VLC デコードが長時間他の処理をブロックすると不都合な場合は、一回の
 * デコードワードの最大値をここで指定する。
 * DecDCTvlc() は VLC_SIZEワードの VLC をデコードすると一旦制御をアプリ
 * ケーションに戻す。
 */
#define VLC_SIZE	1024	

main()
{
	DISPENV	disp;			/* display environment: 表示環境 */
	DRAWENV	draw;			/* drawing environment: 描画環境 */
	RECT	rect;
	void	out_callback();		/* callback: コールバック */
	int	id;
	u_long	padd;
	u_long	*next, *StrNext();	/* CD-ROM simulator: CD-ROM の代わり */
	int	isvlcLeft;		/* flag indicating end of VLC decode
					 :VLC デコード終了フラグ
					 */
	
	PadInit(0);		/* reset controler: PAD をリセット */
	ResetGraph(0);		/* reset GPU: GPU をリセット */
	DecDCTReset(0);		/* reset MDEC: MDEC をリセット */
	SetDispMask(1);		/* start display: 表示許可 */
	isEndOfFlame = 0;	/* clear flag: 旗を下げる */
	
	/* clear frame buffer: フレームバッファをクリア */
	setRECT(&rect, 0, 0, 640,480);
	ClearImage(&rect, 0, 0, 0);

	/* initialize font environment: フォントロード */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 16, 1, 512));
	
	/* intialize decoding environment: デコード構造体に値を設定 */
	dec.vlcbuf[0] = vlcbuf0;
	dec.vlcbuf[1] = vlcbuf1;
	dec.vlcid     = 0;
	dec.imgbuf    = imgbuf;
	dec.rectid    = 0;
	
	setRECT(&dec.rect[0], 0,  32, 256, 176);
	setRECT(&dec.rect[1], 0, 272, 256, 176);
	setRECT(&dec.slice,   0,  32,  16, 176);
		
	/* define callback: コールバックを定義する */
	DecDCToutCallback(out_callback);
	
	/* rewind CD-ROM simulator: フレームを巻き戻す */
	StrRewind();
	
	/* decode the first VLC: まず最初の VLC を解く */
	DecDCTvlcSize(0);
	DecDCTvlc(StrNext(), dec.vlcbuf[dec.vlcid]);

	/* main loop: メインループ */
	while (1) {
		/* send run-level (result of VLC decoding) to MDEC
		 : VLC の結果（ランレベル）を送信する
		 */
		DecDCTin(dec.vlcbuf[dec.vlcid], 0);	
	
		/* swap VLC ID: ID をスワップ */
		dec.vlcid = dec.vlcid? 0: 1;		

		/* start recieving the first decoded slice image.
		 * next one is called in the callback.
		 : 最初の短柵の受信の準備をする 
		 * ２発目からは、callback() 内で行なう
		 */
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	
		/* read next BS from CD-ROM simulator:
		 * 次の BS を疑似 CD-ROM より読み出す。*/
		if ((next = StrNext()) == 0)	
			break;
		
		/* decode first VLC_SIZE words of VLC buffer
		 : vlc の最初の VLC_SIZE ワードをデコードする
		 */ 
		DecDCTvlcSize(VLC_SIZE);
		isvlcLeft = DecDCTvlc(next, dec.vlcbuf[dec.vlcid]);	

		/* display consumed time: 経過時間を表示 */
		FntPrint("slice=%d,", VSync(1));
		
		/* wait for end of decoding: データが出来上がるのを待つ */
		do {
			/* decode next VLC_SIZE words of VLC buffer
			 : vlc の残りの VLC_SIZE ワードをデコードする
			 */ 
			if (isvlcLeft) {
				isvlcLeft = DecDCTvlc(0, 0);
				FntPrint("%d,", VSync(1));
			}

			/* watch controler: コントローラを監視する */
			if ((padd = PadRead(1)) & PADk) {
				PadStop();
				StopCallback();
				return(0);
			}
		} while (isvlcLeft || isEndOfFlame == 0);
		FntPrint("%d\n", VSync(1));
		isEndOfFlame = 0;
			
		/* wait for V-BLNK: V-BLNK を待つ */
		VSync(0);
		
		/* swap disply buffer.
		 * Notice that drawing buffer (tranfering buffer) is
		 * always opposite one of displaying buffer.
		 * 表示バッファをスワップ 
		 * 表示バッファは、描画バッファの反対側なことに注意
		 */
		id = dec.rectid? 0: 1;
		
		PutDispEnv(SetDefDispEnv(&disp, 0, id==0? 0:240, 256, 240));
		PutDrawEnv(SetDefDrawEnv(&draw, 0, id==0? 0:240, 256, 240));
		FntFlush(-1);
	}
}

/*
 *	read next bitstream data (pseudo CD-ROM simulator)	
 :	次のビットストリームを読み込む（本当は CD-ROM から来る）
 */
static int frame_no = 0;
static void StrRewind(void)
{
	frame_no = 0;
}

static u_long *StrNext(void)
{
	extern	u_long	*mdec_frame[];

	FntPrint("%4d: %4d byte\n",
		 frame_no,
		 mdec_frame[frame_no+1]-mdec_frame[frame_no]);
	
	if (mdec_frame[frame_no] == 0) 
		return(mdec_frame[frame_no = 0]);
	else
		return(mdec_frame[frame_no++]);
}

