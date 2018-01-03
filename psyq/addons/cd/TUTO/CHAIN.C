/* $PSLibId: Runtime Library Release 3.6$ */
/*			chain: chained CdRead()
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.25,1995	suzu
 *
 *			     Chained CdRead
 *
 *	CdReadChain() calls CdRead() automatically when the previous CdRead()
 *	is finished. The reading parameters should be predetermined and
 *	defined in the arrays which is sent cdReadChain() as an argument.
 *	
 *--------------------------------------------------------------------------
 *	
 * CdReadChain		Read many files from CD-ROM in background.
 *
 * SYNOPSIS
 *	int cdReadChain(CdlLOC *postbl, int *scttbl, u_long **buftbl, int ntbl)
 *
 * ARGUMENT
 *	postbl	array stored the position of files to be read.
 *	scttbl	array stored the number of sectors to be read.
 *	buftbl	array stored the address of main memory to be loaded.
 *	ntbl	number of elements of each array
 *
 * DESCRIPTION
 *	cdReadChain reads may files from CD-ROM and loads the contents
 *	on the different address of the main memory.
 *	Each reading operations is kicked automatically in the
 * 	CdReadCallback by The end of the previous reading operation.
 *	Reading position, sector size and buffer adresses are set in
 *	each array. The end of the whole reading can be detected by
 *	CdReadChainSync() .
 *
 * NOTE	
 * 	cdReadChain() uses CdReadCallback().
 *	If any read error is detected, cdReaChain retry from the top
 *	of the file list.
 *	
 * RETURN
 *	always 0
 *--------------------------------------------------------------------------
 *	
 * cdReadChainSync	get status of CdReadChain()
 *
 * SYNOPSIS
 *	int cdReadChainSync(void)
 *
 * ARGUMENT
 *	none
 *
 * DESCRITION
 *	cdReadChainSync returns the number of file to be left.
 *
 * RETURN
 *	plus 	number of file to be read.
 *	0	normally terminated
 *	-1	error detected
 *--------------------------------------------------------------------------
 *
 :	CdRead() が正常終了する際に発生するコールバック CdReadCallbacK()
 *	を使用して、配列に登録されたリード要求を次々に実行する
 *
 *--------------------------------------------------------------------------
 *	
 * CdReadChain		複数のファイルをバックグラウンドで読み込む
 *
 * 形式	int cdReadChain(CdlLOC *postbl, int *scttbl, u_long **buftbl, int ntbl)
 *
 * 引数		postbl	データの CD-ROM 上の位置を格納した配列
 *		scttbl	データサイズを格納した配列
 *		buftbl	データのメインメモリのアドレスを格納した配列
 *		ntbl	各配列の要素数
 *
 * 解説		postbl にあらかじめ設定された位置から scttbl に設定さ
 *		れたセクタ数を読み出して、buftbl 以下のアドレスに順次
 *		格納する。 読み込み終了の検出は、CdReadCallback() が使
 *		用され、ntbl 個のファイルが読み込まれるまでバックグラ
 *		ウンドで処理を行なう。
 *
 * 備考		cdReadChain() は CdReadCallback() を排他的に使用する。
 *		リードに失敗すると、配列の先頭に戻って最初からリトライ
 *		する。
 *	
 * 返り値	常に 0
 *	
 *--------------------------------------------------------------------------
 *	
 * CdReadChainSync	CdReadChain() の実行状態を調べる
 *
 * 形式	int cdReadChainSync(void)
 *
 * 引数		なし
 *
 * 解説		cdReadChain() で未処理のファイルの個数を調べる。 
 *
 * 返り値	正整数	まだ処理が完了していないファイルの数。
 *		0	すべてのファイルが正常に読み込まれた
 *		-1	いずれかのファイルの読み込みに失敗した。
 *--------------------------------------------------------------------------
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

static CdlLOC	*postbl;	/* position table */
static int	*scttbl;	/* sector number table */
static u_long	**buftbl;	/* destination buffer pointer table */
static int	ctbl;		/* current CdRead */
static int	ntbl;		/* total CdReads */

void cbread(u_char intr, u_char *result);
		   
int cdReadChainSync()
{
	return(ntbl - ctbl);
}
	    
int cdReadChain(CdlLOC *_postbl, int *_scttbl, u_long **_buftbl, int _ntbl)
{
	unsigned char com;

	/* save pointers */
	postbl = _postbl;
	scttbl = _scttbl;
	buftbl = _buftbl;
	ntbl   = _ntbl;
	ctbl   = 0;
	
	CdReadCallback(cbread);
	CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
	com = CdlModeSpeed;
	CdControlB( CdlSetmode, &com, 0 );
	VSync( 3 );
	CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
	return(0);
}

void cbread(u_char intr, u_char *result)
{
	/*printf("cbread: (%s)...\n", CdIntstr(intr));*/
	if (intr == CdlComplete) {
		if (++ctbl == ntbl)
			CdReadCallback(0);
		else {
			CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
			CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
		}
	}
	else {
		printf("cdReadChain: data error\n");
		ctbl = 0;
		CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
		CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
	}
}
		
		

