$PSDocId: Document Release 1.0 for Run-time Library Release 4.3$


	*** 漢字コードビューアプログラム ***

【概要】

	PlayStation に内蔵されている漢字データを 16bit のテクスチャと
	して四角形のポリゴンに張り付けるサンプルです。
	内蔵フォントの Shift-JIS Code を一覧することができます。特に、
	外字フォントコードの参照にご利用ください。
	

【操作方法】
	Left Up:	上方向スクロール	
	Left Down:	下方向スクロール	
	L1:		文字色 +
	L2:		文字色 -
	R1:		背景色 +
	R2:		背景色 -

	Key を押し続けることにより、標準速、倍速、4 倍速と
	3 段階にスピードが変化します。


【プログラムのロード】

	> psymake

	で codeview.cpe を生成します。コンパイルを行うためには、
	krom2tim.obj, kprintf.obj, ktrans.obj が必要です。
	これら *.obj は最終的に libgpu.lib に組み込まれる予定です。

	> run patchw
	> run codeview

	で実行してください。
	
	なお DTL-H2000 には、漢字フォントが内蔵されていませんので、
	codeview を実行する前に、別途配布の patchw を実行してください。

	＊注意＊
		patchx には、外字フォントが含まれておりませんので、
		新たにリリースしました patchw をご利用ください。
以上
