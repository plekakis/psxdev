/* $PSLibId: Runtime Library Release 3.6$ */
/*				init
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		      Initialize graphic environment
 :		 グラフィック環境を初期化する
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* int x, y;	GTE offset		: GTE オフセット	*/
/* int z;	distance to the screen	: スクリーンまでの距離	*/
/* int level;	debug level		: デバッグレベル	*/

void init_system(int x, int y, int z, int level)
{
	/* initialize controller: コントローラのリセット */
	PadInit(0);             
	
	/* reset graphic subsystem: 描画・表示環境のリセット */
	ResetGraph(0);		
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
	
	/* set debug mode (0:off, 1:monitor, 2:dump) 
	 : グラフィックシステムのデバッグ (0:なし, 1:チェック, 2:プリント) */
	SetGraphDebug(level);	

	/* initialize geometry subsystem: ＧＴＥの初期化 */
	InitGeom();			
	
	/* set geometry origin as (160, 120): オフセットの設定 */
	SetGeomOffset(x, y);	
	
	/* distance to veiwing-screen: 視点からスクリーンまでの距離の設定 */
	SetGeomScreen(z);		
}
