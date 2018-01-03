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

	xdef	__heapbase,__heapsize
	xdef	__text,__textlen
	xdef	__data,__datalen
	xdef	__bss,__bsslen

	xref	main,InitHeap

	section	.text

__SN_ENTRY_POINT:
stup1:
stup2:
	la	t0,sect(.sbss)
	la	t1,sectend(.bss)

	lw	t0,_ramsize	;this is the new way; because there
	nop		        ; are no switches on new hardware.

	la	t2,sectend(.bss)	; t2 = heap base
	sll	t2,t2,3
	srl	t2,t2,3	;remove mem seg bits
	lw	t1,_stacksize
	nop

	lui	t4,$8000	;(mem seg for kernel cached RAM)

	subu	t3,t0,t1	;calc t3 = top of heap
	subu	t3,t3,t2	; -heap base, => t3 = size of heap
	sw	t3,__heapsize
	or	t2,t2,t4	;heap in kseg0
	sw	t2,__heapbase

	la	gp,sect(.sdata)

stup0:

        j       main
        nop     

__main:
     	jr      ra
       	nop     


	section	.data

	cnop	0,4	;longword align

_stacksize	dw	$00008000	;default stack is 32k
_ramsize	dw	$00800000	; 8 Megabytes ** default for MW 3.0

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

