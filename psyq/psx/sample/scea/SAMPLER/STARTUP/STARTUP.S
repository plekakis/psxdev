;	SN Systems replacement for main module
;	in API lib

	opt	c+

	section	.rdata
	section	.text
	section	.ctors
	section	.dtors
	section	.data
	section	.sdata
	section	.sbss
	section	.bss

	xdef	__SN_ENTRY_POINT,__main
	xdef _stacksize,_ramsize
	xdef	__heapbase,__heapsize
	xdef	__text,__textlen
	xdef	__data,__datalen
	xdef	__bss,__bsslen

	xref	main,InitHeap

	section	.text

__SN_ENTRY_POINT:
stup1:
stup2:
	la	t0,sect(.sbss)	; t0 is the bottom of sbss
	la	t1,sectend(.bss); t1 is the top of bss

@clrit:
	opt	at-             ;  clear bss
	sw	zero,0(t0)      ; write to bottom of area to clear
	addiu	t0,t0,4         ; increment bottom
	sltu	at,t0,t1        ; at = bottom - top
	bne	at,zero,@clrit  ; if at is nonzero, loop
	nop
	opt	at+
	
	lw	t0,_ramsize	;this is the new way; because there 
	nop		        ; are no switches on new hardware.

	la	t2,sectend(.bss)	; t2 = heap base
	sll	t2,t2,3
	srl	t2,t2,3	                ;remove mem seg bits
	lw	t1,_stacksize		; t1 = stacksize
	nop

	lui	t4,$8000	;(32k mem seg for kernel cached RAM) t4 = 32k

	subu	t3,t0,t1	;calc t3 = ramsize - stacksize = top of heap
	subu	t3,t3,t2	; t3 = ramsize - stacksize - heap base =
				;    = size of heap
	sw	t3,__heapsize   ; store heapsize in __heapsize
	or	t2,t2,t4	; heapbase = memsegbits (kseg0)  | heapbase;
	sw	t2,__heapbase   ; store heapbase in __heapbase

	la	gp,sect(.sdata) ; set gp to

stup0:

        j       main		; jump to main (note: not jal)
        nop     

__main:
     	jr      ra
       	nop     


	section	.data

	cnop	0,4	;longword align

_stacksize	dw	$00008000	;default stack is 32k
_ramsize	dw	$00200000	; 2 Megabytes 

__heapbase	dw	0
__heapsize	dw	0
__text	dw	sect(.text)
__textlen	dw	sectend(.text)-sect(.text)
__data	dw	sect(.data)
__datalen	dw	sectend(.data)-sect(.data)
__bss	dw	sect(.bss)
__bsslen	dw	sectend(.bss)-sect(.bss)

	section	.sbss

__ra_temp	dsw	1

	end
