$PSDocId: Document Release 1.0 for Run-time Library Release 4.3$

関数説明

ascii2sjis	ASCIIコードからShift-JISコードへの変換

	形式	int ascii2sjis( ascii_code )
		unsigned char ascii_code;

	引数	ascii_code	変換するASCIIコード
		
	解説	指定されたASCIIコードをShift-JISコードに変換する。
		ただし、変換できるASCIIコードの範囲は、0x20～0x7E
		とする。
				
	返り値	成功するとShift_JISコードを、失敗すると0を返す。

	備考	
