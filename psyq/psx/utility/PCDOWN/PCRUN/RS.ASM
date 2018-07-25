;
;

DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
_DATA	ends
_BSS	segment word public 'BSS'
_BSS	ends

_TEXT	segment byte public 'CODE'
	assume	cs:_TEXT

	PUBLIC	_out_port
	PUBLIC	_in_port

_out_port	proc	near
	push	bp
	mov	bp,sp

	push	ax
	push	dx
	mov	dx,[bp+4]
	mov	ax,[bp+6]
	out	dx,al
	pop	dx
	pop	ax

	pop	bp
	ret
_out_port	endp

_in_port	proc	near
	push	bp
	mov	bp,sp

	push	dx
	mov	dx,[bp+4]
	xor	ax,ax
	in	al,dx
	pop	dx

	pop	bp
	ret
_in_port	endp

_TEXT	ends
	end
