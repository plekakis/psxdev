/* $PSLibId: Runtime Library Release 3.6$ */
#ifndef _EMOUSE_H_
#define _EMOUSE_H_

/*****************************************************************
 *
 * file: emouse.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

/*
 * マウスカーソル表示フック
 *	flag:	0	マウスカーソルの表示／非表示に従う
 *		1	マウスカーソルの強制表示
 */
extern void EMouseDisp( int flag, DB *cdb) ;

/*
 * マウスカーソル変更
 *	type:	マウスカーソルタイプ
 */
extern void EMouseType(int type) ;

/*
 * マウスカーソル表示
 */
extern void EMouseON(void) ;

/*
 * マウスカーソル非表示
 */
extern void EMouseOff(void) ;

/*
 * マウスカーソル移動
 *	x,y:	移動先
 */
extern void EMouseMove( int x, int y) ;

/*
 * マウスカーソルのＸ位置
 */
extern int EMouseX( void) ;

/*
 * マウスカーソルのＹ位置
 */
extern int EMouseY( void) ;

/*
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
extern int EMousePadStat( int ch, int *dx, int *dy, u_long *bt) ;

/*
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
extern int EMouseStat( int *x, int *y, u_long *bt) ;

/*
 * マウスの表示準備
 */
extern int EMouseInitDisp( void ) ;


/*
 * マウス感度設定
 */
/*extern void EMouseSense( float sensx, float sensy) ;*/

/*
 * マウス移動領域設定
 *	x,y:	矩形領域の左上位置
 *	dx,dy:	矩形領域のサイズ
 */
extern void EMouseRange( int x, int y, int dx, int dy) ;

/*
 * マウスモジュール初期化
 */
extern int EMouseInit( void) ;

/*
 * マウス移動にともない、カレントグループを移動する
 */
extern int EMouseMoveMenu( int mx, int my, SPRTX **sprt, u_long padd, u_long *padret) ;

/*
 *サラウンドボタンの押し込み状況表示
 *      RM4のid=61..65を使用
 *      id:0..表示するｉｄ
 */
extern void EMouse_dispreverb( int id );

/*
 *ウェイト付きでコマンドアイコンの向きを変えるためのフック
 */
extern void EMouseRevCardEXECON( int count) ;


/*
 *カードの移動範囲限定
 */
extern void EMouseSetFree();
extern void EMouseSetCardHome();
extern void EMouseSetCardConfirm();
extern void EMouseSetCardIcon();

#endif /* _EMOUSE_H_ */
/* DON'T ADD STUFF AFTER THIS */
