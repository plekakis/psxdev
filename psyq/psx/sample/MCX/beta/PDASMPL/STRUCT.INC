;
; STRUCT.S	構造体メンバ定義
;

; key_params で使用(KEY.S)
	^	0, r7
key_times	#	1
key_timescnt	#	1
key_buf		#	1
key_buf_old	#	1
key_abort_val	#	1
key_abort_cnt	#	1

; timer_params で使用(Timer.S)
	^	0, r7
timer0_buf	#	1
timer0_buf_old	#	1
timer1_buf	#	1
timer1_buf_old	#	1
timer2_buf	#	1
timer2_buf_old	#	1

;-----------------------------------------------------------------
	END

