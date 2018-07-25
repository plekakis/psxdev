/* $PSLibId: Runtime Library Release 3.6$ */
/*****************************************************************
 *
 * file: emouse.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
/*
 *EMOUSE.Cは拡張機能の寄せ集めのため、各関数は機能ヘッダで種別わけします
 * MAIN:	MAX.Cのメインルーチン関係の関数
 * CDP:		CDLIB.C関係
 * CARD:	CARD.C関係
 * MOUSE:	マウス入力実現のための関数
 */

#include "libps.h"

#include "maxtypes.h"
#include "psxload.h"
#include "spritex.h"
#include "menux.h"
#include "const.h"
#include "maxstat.h"

#include "menuset2.h"

#ifdef EMOUSE

#include "emouse.h"

/*#define DEBUG_EMOUSE       /* debug print ON */

/*これを生かすと１側が生きている時でも２側パッドの入力を許可する */
/*#define ENABLE_2P /* */

/*-----------------------定義とグローバル変数---------------------------------*/
extern void ReverseSprite( SPRTX* sprt );

extern void EMouseVclear(void) ;
static void EMouseAnimeON(void) ;

/*メニューアイテムの最大数（card\menux.cよりコピー） */
#define ITEMMAX		96

/*EXITのサイズ */
#define EXIT (DELTA-LINEW*2)

/*フルスクリーン移動範囲 */
#define FULL_X		0
#define FULL_Y		8
#define FULL_DX		(640-DELTA/2-8)
#define FULL_DY		(480-48)

/*CARD:カードアイコン領域 */
#define	CARDA_X		(XC1-12)
#define	CARDB_X		(XC2-36)
#define	CARD_DX		(DC*3+24)
#define	CARD_Y		(YC-12)
#define	CARD_DY		(DC*5)

/*CARD:カードコマンド領域 */
#define	CARDCOM_X		(320-86-12)
#define	CARDCOM_DX		172
#define	CARDCOM_Y		(138-25-12)
#define	CARDCOM_DY		263

/*CARD:カードconfirm領域 */
#define	CARDCF_X		(320-130-12)
#define	CARDCF_DX		260
#define	CARDCF_Y		(240-60-12)
#define	CARDCF_DY		120

/*メニューステータス pad.cからコピー */
#define MAIN_MENU    0
#define MEMORY_CARD  1
#define CD_PLAYER    2
#define CD_PLAY_MODE 3
#define MESSAGE_VIEW 4

/*外部参照 */
extern int  ScreenMode;
extern u_long	padd, opadd;
extern MAXSTAT	maxstat;

/*マウスカーソル */
#include "mouse.h"

#define	MOUSETYPEMAX	1

#define	MAXPAD		2
#define	RECVLEN		32

/*メニュー項目実行ならば、ボタン情報がゼロになるまでボタン入力を抑制する */
/*また、ON/OFFステータスとしても BT_ON/BT_OFFを使用 */
#define	BT_ON	1
#define	BT_OFF	0
static int BTcontrol = BT_OFF ;

/*カードのメニュー ONでカードメニュー上 */
static int cardmenu = BT_ON ;

/*メニューディレィ */
static	int	menudelay = 0 ;

/*CARD:
 *カードメニューにてコマンドの矢印などを適切な方向に表示する
 *そのために、現在の方向を保存する。
 * カードコマンドリバースフラグ
 *	リバースすると１
 *	[0]:COPY
 *	[1]:ALLCOPY
 *	[2]:DELETE
 */
static int	revflag[] = {0,0,0} ;
static int	revcard = 0 ;		/*リバースするコマンド番号 0..2 */
static int	revexecflag = 0 ;

/*
 * EXITの背景
 * このスプライトを変化させる事によってｅｘｉｔアニメを行う
 */
static SPRTX	sprt_exit ;

/*MOUSE:
 *ＴＩＭデータエントリ
 *　ＴＩＭ型のデータポインタをマウスカーソルのタイプとして登録する
 *　マウスカーソルの種類を増やすならば、MOUSETYPEMAXを増やして、
 *　ここにＴＩＭデータへのポインタを追加すれば良い。
 */
static unsigned long *mousecursor[MOUSETYPEMAX] = {
	mouse_cursor
} ;

/*MOUSE:
 * マウスカーソルを表示型に登録しなおす
 */
static SPRTX	CursorSprites[MOUSETYPEMAX];

static int	display = 1 ;		/*マウスカーソル表示ｏｎ／ｏｆｆ */
static int	cursorN = 0 ;		/*マウスカーソル何番を表示するか */
static int	mouseRSX = 0 ;		/*マウスの移動領域 */
static int	mouseRSY = 0 ;
static int	mouseREX = 640 ;
static int	mouseREY = 480 ;
static int	mouseX = 10 ;		/*マウスの現在の位置 */
static int	mouseY = 10 ;
static int	mouseBT = 0 ;		/*マウスボタンの状況 */

static int      sencible=1;

/*割り込みプログラムによって、このバッファにデータが入る */
/*EMouseの下位ルーチンでデコードを行うので使う側では気にする必要はない */
static unsigned char	PADbuf[MAXPAD][RECVLEN] ;	/*受信バッファ　*/

/*-----------------------低レベルルーチン------------------------------------*/
/*///////////////////////////
// FT4型ポリゴンを反転させる。ただし入力変数はSPRTX型
///////////////////////////*/
void	ReverseSprite( SPRTX* sprt )
{
	POLY_FT4	*poly ;
	int	x0, y0 ;
	int	x1, y1 ;
	int	x2, y2 ;
	int	x3, y3 ;

	poly = &sprt->poly;
	x0 = poly->u0 ;
	x1 = poly->u1 ;
	x2 = poly->u2 ;
	x3 = poly->u3 ;
	y0 = poly->v0 ;
	y1 = poly->v1 ;
	y2 = poly->v2 ;
	y3 = poly->v3 ;
	setUV4(poly,
	 x1, y1,
	 x0, y0,
	 x3, y3,
	 x2, y2
	) ;
}

/*MOUSE:
 * マウスカーソル変更
 *	type:	マウスカーソルタイプ
 */
void EMouseType(int type)
{
	cursorN = type ;
}

/*MOUSE:
 * マウスカーソル表示
 */
void EMouseON(void)
{
	display = 1 ;
}

/*MOUSE:
 * マウスカーソル非表示
 */
void EMouseOff(void)
{
	display = 0 ;
}


/*MOUSE:
 * マウスカーソル移動
 *	x,y:	移動先
 */
void EMouseMove( int x, int y)
{
	if( x < mouseRSX) x = mouseRSX ;
		else if( x > mouseREX) x = mouseREX ;
	if( y < mouseRSY) y = mouseRSY ;
		else if( y > mouseREY) y = mouseREY ;
	mouseX = x ;
	mouseY = y ;
}

/*MOUSE:
 * マウスカーソルのＸ位置
 */
int EMouseX( void)
{
	return mouseX ;
}

/*MOUSE:
 * マウスカーソルのＹ位置
 */
int EMouseY( void)
{
	return mouseY ;
}

/*MOUSE:
 * パッド情報のポーリング
 *	ch:	判定する　PADbufのチャネル番号
 *	dx,dy:	マウス移動量
 *	bt:	ボタン情報
 *
 *	return:		-1	データ取得失敗
 *			上位4ビット：端末種別
 *                                   0x10:マウス
 *		                     0x20:16ボタンアナログ
 *			             0x40:16ボタン
 *			             0x80:マルチタップ
 */
int EMousePadStat( int ch, int *dx, int *dy, u_long *bt)
{
        static u_long bt_latest=0;
	int	err;
	u_long  tbt ;

	*dx = 0 ;
	*dy = 0 ;
	*bt = 0 ;

#ifdef DEBUG_EMOUSE
	printf("ch=%d:%02x, %02x, %02x, %02x, %02x, %02x ",
	       ch, PADbuf[ch][0], PADbuf[ch][1], PADbuf[ch][2],
	       PADbuf[ch][3], PADbuf[ch][4], PADbuf[ch][5]);
	printf("%02x, %02x, %02x ",
	       PADbuf[ch][6], PADbuf[ch][7], PADbuf[ch][8]) ;
	printf("\r");
#endif
	if( PADbuf[ch][0] != 0x00) {
		return -1 ;	/*受信失敗 */
	}

	switch( PADbuf[ch][1] & 0xf0) {
#ifdef NOMOUSE
	case 0x10:	/*XY MOUSE */
	        err = -1;
		break;
#else
	case 0x10:	/*XY MOUSE */
		if( PADbuf[ch][1] != 0x12) {
			err = -1 ;
			break ;
		}
		*dx = (signed char)PADbuf[ch][4] ;
		*dy = (signed char)PADbuf[ch][5] ;
		*bt = 0 ;
		tbt = 0;

		/*１６ボタンｐａｄとの対応。左ボタンを丸、右ボタンをスタートにする。 */
		if( (~PADbuf[ch][3]) & 8) *bt |= _PADRright ;	/*丸 */
		if( ScreenMode == MEMORY_CARD ) {
		    if( (~PADbuf[ch][3]) & 4) *bt |= _PADh ;	/*start */
		    if( (*bt&0xffff) == 0x0820) {
			*bt = _PADl | _PADm | _PADn | _PADo ;
		    }
		}
		err = PADbuf[ch][1] & 0xf0 ;
		/*
		 *マウス入力をマスクするしかけ
		 *カードメニューでｏｎ状態になりっぱなしになるのを抑制する
		 *（もしこの関数を汎用で使用するのならば、この仕掛けは必要ない）
		 */
		if( BTcontrol == BT_ON) {
			if( *bt == 0) BTcontrol = BT_OFF ;
			else {
				*bt = 0 ;
				*dx = 0 ;
				*dy = 0 ;
			}
		}
		break ;
#endif /* NOMOUSE */
	case 0x20:	/*16BT ANALOG */
	case 0x40:	/*16BT */
		*bt = (PADbuf[ch][2] << 8) | PADbuf[ch][3] ;
		*bt = ~*bt & 0xffff ;
		err = PADbuf[ch][1] & 0xf0 ;
		break ;
	default:
		err = -1 ;
		break ;
	}
	return err ;
}

extern int funcokflag;
/*MOUSE:
 * マウスカーソルの状況を取得する
 *	x,y:	マウス座標
 *	bt:	ボタン情報
 *
 *	return:		-1	データ取得失敗
 *			上位4ビット：端末種別
 *                                   0x10:マウス
 *		                     0x20:16ボタンアナログ
 *			             0x40:16ボタン
 *			             0x80:マルチタップ
 */
int EMouseStat( int *x, int *y, u_long *bt)
{
	int	dx, dy ;
	int	_dx, _dy ;
	u_long  rbt, _rbt ;
	int	err, _err ;

	err = EMousePadStat( 0, &dx, &dy, &rbt) ;
	if( err == -1) {
		/*ファーストｐａｄが未接続ならセカンドｐａｄのポーリング */
		err = EMousePadStat( 1, &dx, &dy, &rbt) ;
	}
#ifdef ENABLE_2P
	else if( err != 0x10 && rbt == 0) {
		/*ファーストｐａｄが入力無しならセカンドｐａｄのポーリング */
		/*ただし、ファーストｐａｄがマウス以外の時に動作 */
		_dx = dx ;
		_dy = dy ;
		_rbt = rbt ;
		_err = err ;
		err = EMousePadStat( 1, &dx, &dy, &rbt) ;
		if( (err == -1 || err == 0x10) && _err != -1) {
			/*もしコントローラ２異常ならコントローラ１情報 */
			/*ただしコントローラ１異常ならば
			コントローラ２の異常情報をそのまま返す */
			/*またコントローラ２がマウスかつ
			コントローラ１が生きていればコントローラ１情報 */
			dx = _dx ;
			dy = _dy ;
			rbt = _rbt ;
			err = _err ;
		}
	}
#endif
	if( !funcokflag ) {
	    EMouseMove( mouseX + dx, mouseY + dy) ;
	    *x = mouseX ;
	    *y = mouseY ;
	    *bt = rbt ;
	}

#ifdef DEBUG_EMOUSE
	printf("err=%02x:%d, %d, %04x\n", err, *x, *y, *bt) ;
#endif
	return err ;
}


/*-----------------------アニメーション＆コントロール-------------------------*/
/*CDP:へこますボタンの位置 */
static int OFFbt_x = 0 ;
static int OFFbt_y = 0 ;
static int OFFbt2_x = 0 ;
static int OFFbt2_y = 0 ;

/*CARD:カードアイコンの表示データ .. EMouseCardConfirmで持ってくる */
extern	int	Target ;	/*ターゲットアイコンのｉｄ */
extern	int	Event ;		/*confirm時のイベントｉｄ */
static	int CHGiconflag = 0;	/*アイコン選択したか？yes=BT_ON */
static	SPRTX	ICONsprt ;	/*選択したアイコンスプライトを保持する */
static	SPRTX	BLUEsprt ;	/*アイコン未選択時のスプライト */

static void EMouseTARGETdisp( DB *cdb) {}
static void EMouseOFFbt( DB *cdb) {}
void EMouseCDbt( int id) {}
void EMouseCDbt2( int id) {}
static void chgclut( int id, int flag) {}
void EMouse_dispreverb( int id) {}
static void EMouseCLUT() {}
static int EMouseInitOFFbt( void) {}
static int EMouseInitEXITbt( void) {}
static void EMouseAnimeDisp( DB *cdb) {}
static void EMouseInitCard() {}
void EMouseRevCard(int com, int direc) {}
void EMouseRevCardEXECON( int count) {}
void EMouseRevCardEXEC(int direc) {}
void EMouseRevCardEXECall() {}

/*MAIN:
 * マウスカーソル表示フック
 *	flag:	0	マウスカーソルの表示／非表示に従う
 *		1	マウスカーソルの強制表示
 */
void EMouseDisp( int flag, DB *cdb)
{
	int	x, y ;

	EMouseMove( mouseX, mouseY) ;	/*リミットをかける */
	x = mouseX ;
	y = mouseY ;
	if( (flag == 0 && display == 1) || flag == 1) {
	    SetSpritePosition( &CursorSprites[cursorN],
		 x+CursorSprites[cursorN].mx, y+CursorSprites[cursorN].my );
	    AddSprite2( cdb, &CursorSprites[cursorN]) ;
	}
}

/*MAIN:
 * マウスの表示準備
 */
int EMouseInitDisp( void)
{
	int	i ;
	SPRTX *sprt ;

	EMouseInitEXITbt() ;
	EMouseInitOFFbt() ;
	EMouseInitCard() ;
	EMouseCLUT() ;

	sprt = CursorSprites ;
	for( i = 0; i < MOUSETYPEMAX; i++) {
              /* 95/03/24 sprt -> CursorSprites */
              MakeSpriteData( &CursorSprites[i], mousecursor[i], 0 );
              SetSpritePosition( &CursorSprites[i], 200, 100 );
              SetSpriteTrans( &CursorSprites[i], 0 );
              SetSpriteShadeTex( &CursorSprites[i], 1 );
              CursorSprites[i].priority = 1 ;         /*プライオリティ最大 */
	}
	if( ScreenMode != MEMORY_CARD )
	  funcokflag = 0;

	return 0;
}

/*MOUSE:
 * マウス移動領域設定
 *	x,y:	矩形領域の左上位置
 *	dx,dy:	矩形領域のサイズ
 */
void EMouseRange( int x, int y, int dx, int dy)
{
	mouseRSX = x ;
	mouseRSY = y ;
	mouseREX = mouseRSX + dx ;
	mouseREY = mouseRSY + dy ;
}

/*MAIN:
 * マウスモジュール初期化
 */
int EMouseInit(void)
{
	EMouseVclear() ;
        InitPAD(PADbuf[0], MAXPAD, PADbuf[1], MAXPAD);
       	EMouseMove( 80, 100);
        StartPAD();
	ChangeClearPAD(0);

	return 0;
}

/*MAIN:
 * マウス移動にともない、カレントｉｄを移動する
 * return: 0	マウスはメニュー項目の上にいない
 *	   -1	マウスはメニュー項目の上にいる
 *	   -2	ｅｘｉｔアニメ処理あり
 *	   1	ｃａｒｄ移動処理
 *			padretにｐａｄ値を格納
 */
int EMouseMoveMenu( int mx, int my, SPRTX **sprt, u_long padd, u_long *padret)
{
    int	x, y, dx, dy, i, current_id, group  ;
    SPRTX	*sprite ;
    extern void	funcIcon( long pad );

    if( revexecflag) {
	revexecflag = 0 ;
	EMouseRevCardEXEC( 0) ;
    }

    group = 0 ;
    current_id = MENUgetCurrentItem();
    /*カレントのグループを取得する */
    for( i = 0;; i++) {
	if( MENU[i].RM.id == 0) continue ;  /* break -> continue 95/04/05 */
	if( current_id == MENU[i].RM.id) {
	    group = MENU[i].RM.group ;
	    break ;
	}
    }

    /*メモリーカードアイコンのための特殊処理。
      左右のマウス位置によってｐａｄ移動を発行する */
    /*グループ番号を直接みているので、
      もしカードのメニューが変更された場合には注意！ */
    if ( ScreenMode == MEMORY_CARD && cardmenu == BT_OFF) {
	if( mx > CARDA_X+CARD_DX-15 && group == 3) {
	    *padret = _PADLright ;
	    return 1 ;
	}
	if( mx < CARDB_X+15 && group == 4) {
	    *padret = _PADLleft ;
	    return 1 ;
	}
    }

    /*マウス指示位置の修正 */
    mx = mx + CursorSprites[cursorN].h / 2 ;
    my = my + CursorSprites[cursorN].w / 2 ;

    /*メニュー移動処理 */
    for( i = 0; i < ITEMMAX; i++) {
	if( MENU[i].RM.id == 0) continue ;
	if( MENU[i].RM.execFunc == NULL) continue ;
	if( MENU[i].Sprites[fot].priority == HIDE ) continue;

	sprite = &(MENU[i].Sprites[fot]) ;

	dx = MENU[i].RM.mw ;
	dy = MENU[i].RM.mh ;
	x = sprite->x - dx / 2 ;
	y = sprite->y - dy / 2 ;

	/*マウスがスプライトの上にいたらメニュー移動 */
	if( (x < mx) && ((x+dx) > mx) && (y < my) && ((y+dy) > my) ) {
	    if( MENU[i].RM.id != current_id ) {
		/*effectが0x20のもの（移動中）はメニューセットしません */
		if( MENU[i].RM.effect&0x20 ) ;
		else {
		    if( ScreenMode == CD_PLAY_MODE && MENU[i].RM.id == 22 ) {
			current_id = 10;
			MENUsetCurrentItem( current_id );	/*移動 */
		    }
		    else {
			MENUsetCurrentItem( MENU[i].RM.id );	/*移動 */
		    }
		}
	    }

	    *sprt = sprite ;
	    /*
	      * カードのアイコンの名称を表示する
	      *	アイコンエリアかどうかは メニューに
	      *      funcIconが設定されている
	      *	かどうかによって判定する
	     */
	    if( MENU[i].RM.execFunc == funcIcon) {
		/*effectが0x20のもの（移動中）はガイド表示をしません */
		if( MENU[i].RM.effect&0x20 ) ;
		else {
		    SetStringUVclr(1);
		    SetStringUV( MENU[i].RM.id) ;
		}
	    } else if( sencible ) {
		if( MENU[i].RM.execFunc == funcExit && (padd & OKKey)) {
		    /*ドア開けアニメ＆終了 */
		    EMouseAnimeON() ;
		    return -2 ;
		}
	    }
	    return -1 ;
	}
    }

    if ( ScreenMode == MEMORY_CARD && cardmenu == BT_OFF) {
	if ( my > CARD_Y && my < CARD_Y+CARD_DY ) {
	    if ( mx > CARDA_X && mx < CARDA_X+CARD_DX-15 ) {
		SetStringUV(0);
	    } else if ( mx > CARDB_X && mx < CARDB_X+CARD_DX-15 ) {
		SetStringUV(1);
	    }
	}
    }

    return 0 ;
}

/*MAIN:
 * ＰＡＤ移動時のメニュー解釈処理
 * return: 0	特殊処理なし
 *	   -2	ｅｘｉｔアニメ処理
 */
static int EMouseCheckMenu( u_long padd)
{
	int	i, id, group  ;

	if( revexecflag) {
		revexecflag = 0 ;
		EMouseRevCardEXEC( 0) ;
	}

	group = 0 ;
	id = MENUgetCurrentItem();
	/*カレントのグループを取得する */
	for( i = 0;; i++) {
		if( MENU[i].RM.id == 0) break ;
		if( id == MENU[i].RM.id) {
			group = MENU[i].RM.group ;
			break ;
		}
	}
	for( i = 0;; i++) {
		if( MENU[i].RM.id == 0) break ;
		if( MENU[i].RM.id != id) continue ;
		if( MENU[i].RM.execFunc == funcExit && (padd & OKKey)) {
			/*ドア開けアニメ＆終了 */
			EMouseAnimeON() ;
			return -2 ;
		}
	}
	return 0 ;
}

/*CARD:
 *カードの移動範囲を全画面に
 */
void EMouseSetFree()
{
	EMouseRange( FULL_X, FULL_Y, FULL_DX, FULL_DY);
	EMouseMove( 100, 100);
	menudelay = 2 ;
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/*CARD:
 *カードの移動範囲をコマンド範囲に
 */
void EMouseSetCardHome()
{
	SPRTX	*sprite ;

	if( CHGiconflag == BT_ON) {
		CHGiconflag = BT_OFF ;
		sprite = &ICONsprt ;
		SetSpritePriority( sprite, (u_long)HIDE );
		SetSpritePosition( sprite, BLUEsprt.x, BLUEsprt.y) ;
	}

	EMouseRevCardEXECall() ;

	EMouseRange( CARDCOM_X, CARDCOM_Y, CARDCOM_DX, CARDCOM_DY);

        sprite = MENUgetSprtxPtr(MENUgetCurrentItem());
        EMouseMove( sprite->x-DELTA/4, sprite->y-DELTA/4);
	BTcontrol = BT_ON ;
	cardmenu = BT_ON ;
}

/*CARD:
 *カードの移動範囲を確認範囲に
 */
void EMouseSetCardConfirm()
{
	EMouseRange( CARDCF_X, CARDCF_Y, CARDCF_DX, CARDCF_DY);

        if( Event == _ID_DELETE )
          EMouseMove( CARDCF_X+CARDCF_DX/2+DELTA, CARDCF_Y+CARDCF_DY/2);
        else
          EMouseMove( CARDCF_X+CARDCF_DX/2-DELTA, CARDCF_Y+CARDCF_DY/2);

	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}


/*CARD:
 *カードの移動範囲をＡ側アイコン範囲に
 */
void EMouseSetCardIconA()
{
	EMouseRevCardEXEC( 0) ;

	EMouseRange( CARDA_X, CARD_Y, CARD_DX, CARD_DY);
       	EMouseMove( CARDA_X+CARD_DX/2, CARD_Y+CARD_DY/2);
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/*CARD:
 *カードの移動範囲をＢ側範囲に
 */
void EMouseSetCardIconB()
{
	EMouseRevCardEXEC( 1) ;

	EMouseRange( CARDB_X, CARD_Y, CARD_DX, CARD_DY);
       	EMouseMove( CARDB_X+CARD_DX/2, CARD_Y+CARD_DY/2);
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/*終了ボタンのアニメ処理 */
int exitanime = 0 ;
static SPRTX	*animesprt ;

/*MAIN:
 *アニメ処理中かどうかのフラグ、max.cにて参照します
 */
int EMouse_get_exitanime()
{
	return exitanime ;
}

/*MAIN:
 * 終了アニメ処理（カード＆ＣＤＰ兼用)
 * 終了関数直前で乗っ取ります。アニメ後呼ぶようにします
 */
static void EMouseAnimeON(void)
{
	int	item ;
	POLY_FT4	*poly ;

	item = MENUgetCurrentItem();
	animesprt = &sprt_exit ;
	exitanime = 8*10 ;
	poly = &(animesprt->poly) ;
}

/*MAIN:
 * メインメニュー時専用の動作
 *	パッドカーソルをずらすのはメインメニューの２項目のみなので、
 *	間に合わせの関数にて対応
 */
static void MoveCursorMain( DB *cdb, SPRTX *cursor )
{
    SPRTX *current;

    if( ScreenMode != MESSAGE_VIEW ) { /* Warning 画面にはカーソル不要 */
	current = MENUgetSprtxPtr( MENUgetCurrentItem() );

	SetSpritePosition( cursor,
			  current->x+cursor->w/2, current->y+cursor->h*5/2 );
	AddSprite2( cdb, cursor );
    }
}

/**********************************
MAIN:
  Mouse / PAD 共存部分　入力部

以下のexternを使用しています
	extern u_long	padd;
	extern MAXSTAT	maxstat;
	extern int fot ;
**********************************/

static	int	delay = 0 ;		/*メインメニューブリンク用のカウンタ */
static	int	delaya = 64 ;		/*メインメニューブリンク用のカウンタ */
static	int	delayb = 64 ;		/*メインメニューブリンク用のカウンタ */
static	SPRTX	*sprite ;		/*マウスがヒットしたスプライトX */
static	int	ctype ;			/*デバイスタイプ */

void EMouseIN( DB *cdb)
{
	int		mx, my, i ;
	u_long          bt, cardpad ;
	int		col_b, col_g, col_r ;	/*ブリンク用のワーク変数 */
	POLY_FT4	*poly ;

	if( exitanime > 0 ) {
	    /*終了ボタンのアニメ処理 */
	    return ;
	} else if ( delay ) {
	    /*メイン画面のボタンブリンク処理 */
	    return ;
	}
	/*通常時のマウス／ｐａｄ処理 */
        ctype = EMouseStat( &mx, &my, &bt) ;
        padd = bt ;
        if( ctype == 0x10) {
	    /* マウス動作 */
	    i = EMouseMoveMenu( mx, my, &sprite, padd, &cardpad) ;

	    if( padd == 0 ) {
		sencible = 1;
	    }

	    if( i == 0) {
		if( !(padd & _PADh) ) {   /*右ボタンは領域外でも有効 */
		    if( padd != 0 ) {
			sencible = 0;
		    }
		    padd = 0 ;	/* メニュー項目上でない場合には強制的に無行動 */
		}
	    }
	    else if( i == -2) {
		exitanime = 0;
		delay = 64;
		padd = 0;
	    }
	    else {
		if( i == 1 && padd == 0) {
		    padd = cardpad ;	/*カードアイコンの移動処理 */
		}
	    }

	    if( !sencible ) padd = 0;
	
	    if ( ScreenMode == MAIN_MENU && (padd & OKKey) ) {
		delay = 64 ;
		padd = 0 ;
	    }
        } else if( ctype != -1) {
	    /*マウス以外ならば、１６ボタンｐａｄとして扱ってしまう */
	    i = EMouseCheckMenu( padd) ;
	    if( i == -2) {
		padd = 0 ;	/* アニメ処理 */
		exitanime = 0;
		delay = 64;
		sprite = MENUgetSprtxPtr( MENUgetCurrentItem() );
	    }
	    if ( ScreenMode == MAIN_MENU && (padd & OKKey) ) {
		/*メイン画面でのブリンク処理 */
		delay = 64 ;
		padd = 0 ;
		sprite = MENUgetSprtxPtr( MENUgetCurrentItem() );
	    }
        }
}

/**********************************
MAIN:
  Mouse / PAD 共存部分　表示

以下のexternを使用しています
	extern u_long	padd;
	extern MAXSTAT	maxstat;
	extern int fot ;
**********************************/
void EMouseDISP( DB *cdb)
{
	int		mx, my, i ;
	int		col ;	/*ブリンク用のワーク変数 */
	POLY_FT4	*poly ;

	if ( ScreenMode == MESSAGE_VIEW ) {
		return ;
	}

	if( exitanime > 0) {
		/* 終了ボタンのアニメ処理 */
		poly = (POLY_FT4 *)&(animesprt->poly) ;
		exitanime-- ;
		if( exitanime == 0) {
			padd = OKKey ;		/*EXIT 実行 */
		} else if( (exitanime % 10) == 0) {
			setXY4(poly, poly->x0, poly->y0, poly->x1 - 1, poly->y1 + 1,
			       poly->x2, poly->y2, poly->x3 - 1, poly->y3 - 1 );
			EMouseAnimeDisp( cdb) ;
		} else {
			EMouseAnimeDisp( cdb) ;
		}
	} else if ( delay) {
		/*メイン画面のボタンブリンク処理 */
		delay -= 2 ;
		col = ((delay-4)*2) & 0x7f ;
		SetSpriteRGB( sprite, col, col, col );
		if( delay <= 0) {
			padd = OKKey ;
			SetSpriteRGB( sprite, 128, 128, 128 );
		}
	} else {
		EMouseAnimeDisp( cdb) ;
	        if( ctype == 0x10) {
			/* マウス動作 */
			EMouseDisp( 0, cdb) ;
	        } else if( ctype != -1) {
		    MoveCursor( cdb, &maxstat.cursor[fot] );
	        }
	}
}

/*MAIN:
 * ｓｔａｔｉｃ変数の初期化
 */
void EMouseVclear(void)
{
	BTcontrol = BT_OFF ;
	cardmenu = BT_ON ;
	menudelay = 0 ;
	revflag[0] = 0 ;
	revflag[1] = 0 ;
	revflag[2] = 0 ;
	revcard = 0 ;
	revexecflag = 0 ;
	mousecursor[0] = mouse_cursor ;
	display = 1 ;		/*マウスカーソル表示ｏｎ／ｏｆｆ */
	cursorN = 0 ;		/*マウスカーソル何番を表示するか */
	mouseRSX = 0 ;		/*マウスの移動領域 */
	mouseRSY = 0 ;
	mouseREX = 640 ;
	mouseREY = 480 ;
	mouseX = 10 ;		/*マウスの現在の位置 */
	mouseY = 10 ;
	mouseBT = 0 ;		/*マウスボタンの状況 */
	/*へこますボタンの位置 */
	OFFbt_x = 0 ;
	OFFbt_y = 0 ;
	OFFbt2_x = 0 ;
	OFFbt2_y = 0 ;
	/*カードアイコンの表示データ .. EMouseCardConfirmで持ってくる */
	CHGiconflag = 0;
	/*終了ボタンのアニメ処理 */
	exitanime = 0 ;
	delay = 0 ;			/*メインメニューブリンク用のカウンタ */
	delaya = 64 ;			/*メインメニューブリンク用のカウンタ */
	delayb = 64 ;			/*メインメニューブリンク用のカウンタ */

	ICONsprt.priority = HIDE;

	sencible = 1;
}

#endif /* EMOUSE */
