$PSDocId: Document Release 1.0 for Run-time Library Release 4.3$

		PS Tool Manual

名称
	kanjidiv - 漢字フォントからデータを切り出す

日付
	1995/Jul/5 Version 1.2

形式
	kanjidiv [options] input-file

解説
	漢字フォントのデータ（非漢字、第一水準、第二水準、外字）から、
	input-file の漢字データを切り出し、ファイルに出力します。

	＊ご注意＊
		半角、縦書きには対応していません。
		外字は 15x15 フォントの場合だけです。

	[options]

	-o output-file	切り出した漢字データを出力するファイル名を指定します。
			デフォルトは、kanji.out です。

	-font 11|13|15	出力する漢字データのフォントを指定します。
			11x11、13x13、15x15 の中から選択します。
			デフォルトでは 15 になります。

	-path font-path	漢字フォントのデータファイルのパス名を指定します。
			デフォルトは、"..\fontdata\\" です。

	-v		詳細な情報を表示します。

補足
	漢字フォントのデータは、..\fontdata\got1[1,3,5]j[0,1,2]b.h および
	.\fontdata\got15gai.h にあり、kanjidiv の起動時に読み込まれます。
	データをこれ以外の場所に置いた場合は、-path オプションでパス名を
	指定してください。

使用例
	kanjidiv -o kanji.out -font 15 test.knj
