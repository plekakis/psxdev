$PSDocId: Document Release 1.0 for Run-time Library Release 4.3$

							95. 7. 7
漢字フォントデータアクセスサンプルプログラム	ver2.0

概要

　これは、サイズ毎の漢字データファイルを使用するサンプルプログラムです。
漢字データファイルには、あらかじめ次のようなラベルが付けられていますので、
漢字データファイルをアプリケーションで呼び出し、ラベルで各漢字データに
アクセスします。

	11dot 非漢字	unsigned long got11j0b[]
	11dot 第１水準	unsigned long got11j1b[]
	11dot 第２水準	unsigned long got11j2b[]
	11dot 縦書き	unsigned long got11tat[]
	11dot 半角	unsigned long got11hlf[]

	13dot 非漢字	unsigned long got13j0b[]
	13dot 第１水準	unsigned long got13j1b[]
	13dot 第２水準	unsigned long got13j2b[]
	13dot 縦書き	unsigned long got13tat[]
	13dot 半角	unsigned long got13hlf[]

	15dot 非漢字	unsigned long got15j0b[]
	15dot 第１水準	unsigned long got15j1b[]
	15dot 第２水準	unsigned long got15j2b[]
	15dot 縦書き	unsigned long got15tat[]
	15dot 半角	unsigned long got15hlf[]

	15dot 外字	unsigned long got15gai[]

  ＊注 漢字フォントデータは、"..\fontdata\"のファイルを参照しています。


使用方法

　はじめに　psymake でコンパイルを行い、run main で起動します。

以上
