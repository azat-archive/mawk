
;**************************************************
;  rand.asm
;  PC versions of the rand48 family
;  1988 Michael D. Brennan
;  
;
;  rand.asm  and rand0.c  are the source files
;     for rand.lib  (see also rand48.h)
;**************************************************


public   _srand48, _seed48 , _lcong48
public	 _mrand48, _jrand48 ; uniform on [0,2^32)
public   _lrand48, _nrand48 ; uniform on [0,2^31)
public   _urand48, _vrand48 ; uniform on [0,2^16)

;  rand.asm

standard_scale0 = 0e66dh
standard_scale1 = 0deech
standard_scale2 = 5h
standard_shift  = 0bh

.model  small

.data
prod   dw   3 dup(?)  ;  build  scale*seed here
seed   dw   3 dup(?)  ;  default seed storage for drand, lrand etc
scale  dw   standard_scale0
       dw   standard_scale1
       dw   standard_scale2
shift  dw   standard_shift
seedbuf	dw	3 dup(?)  ;  place old seed  for seed48()

.code


;****************************************
;  rgen -- the basic linear congruence
;  call with  bx->the seed
;*************************************

rgen	proc	near
     ;   seed <- scale * seed + shift  with overflow

     ;  first get  scale*seed into prod

	mov	ax, scale   ;  0,0
	mul	word ptr [bx]
	mov	prod, ax
	mov	cx, dx     ;   save the overflow in cx

	mov	ax, scale+2    ; 1,0
	mul	word ptr [bx]
	add	ax, cx
	adc	dx, 0
	mov	prod+2, ax
	mov	cx, dx

	mov	ax, scale+4   ;  2,0
	mul	word ptr [bx]
	add	ax, cx
	mov	prod+4, ax

	mov	ax, scale     ;  0,1
	mul	word ptr [bx+2]
	add	ax, prod+2
	adc	dx, 0
	mov	prod+2, ax
	mov	cx, dx

	mov	ax, scale+2   ;   1,1
	mul	word ptr [bx+2]
	add	ax, cx
	add	prod+4, ax

	mov	ax, scale   ;  0,2
	mul	word ptr [bx+4]
	add     ax, prod+4
	                 ;  move product to seed
	mov	word ptr [bx+4], ax
	mov	ax, prod+2
	mov	word ptr [bx+2], ax
	mov	ax, prod
	mov	word ptr [bx], ax
			  ;   add shift to seed
	mov	ax, shift
	add	word ptr [bx], ax
	adc	word ptr [bx+2], 0
	adc	word ptr [bx+4], 0

	ret
rgen	endp

set_scale	proc   near ;  set scale and shift to standard
	mov	scale, standard_scale0
	mov	scale+2, standard_scale1
	mov	scale+4, standard_scale2
	mov	shift, standard_shift
	ret
set_scale	endp

;  void  srand48(long seedval)

_srand48	proc	near
	push	bp
	mov	bp, sp
	mov	seed, 330eh ;
	mov	ax, [bp+4]
	mov	seed+2, ax
	mov	ax, [bp+6]
	mov	seed+4, ax
	call	set_scale
	pop	bp
	ret
_srand48	endp


;  short  *seed48( short  newseed[3] )

_seed48		proc   near
	push	bp
	mov	bp, sp
	push	si
	push	di
	push	ds
	pop	es
	cld
	mov	di, offset seedbuf  ;  save old seed
	mov	si, offset seed
	mov	cx, 3
	rep	movsw
	mov	si, [bp+4]   ;  load the new seed
	mov	di, offset seed
	mov	cx, 3
	rep	movsw
	call	set_scale
	mov	ax, offset seedbuf
	pop	di
	pop	si
	pop	bp
	ret
_seed48	endp


;  long  mrand48()
;  long  jrand48(short seed[3])

_mrand48	proc	near
	mov	bx, offset seed
	call	rgen
	mov	dx, seed+4
	mov	ax, seed+2
	ret
_mrand48	endp

_jrand48	proc	near
	push	bp
	mov	bp, sp
	mov	bx, [bp+4]
	call	rgen
	mov	dx, word ptr [bx+4]
	mov	ax, word ptr [bx+2]
	pop	bp
	ret
_jrand48	endp

_nrand48	proc 	near
	push	bp
	mov	bp, sp
	mov	bx, [bp+4]
	call	rgen
	mov	dx, [bx+4]
	mov	ax, [bx+2]
	shr	dx, 1
	rcr	ax, 1
	pop	bp
	ret
_nrand48	endp

_lrand48	proc	near
	mov	bx, offset seed
	call	rgen
	mov	dx, seed+4
	mov	ax, seed+2
	shr     dx, 1
	rcr     ax, 1
	ret
_lrand48	endp

_vrand48	proc   near
	push	bp
	mov	bp, sp
	mov	bx, [bp+4]
	call    rgen
	mov	ax, [bx+4]
	pop	bp
	ret
_vrand48	endp

_urand48	proc	near
	mov	bx, offset seed
	call	rgen
	mov	ax, seed+4
	ret
_urand48	endp

_lcong48	proc 	near
	push	bp
	mov	bp, sp
	push	si
	push	di
	push	ds
	pop	es
	cld
	mov	si, [bp+4]
	mov	di, offset seed
	mov	cx, 7
	rep	movsw
	pop	di
	pop	si
	pop	bp
	ret
_lcong48	endp

end
