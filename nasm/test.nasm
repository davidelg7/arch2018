section .data
  sedici dd 4
section .text
%macro	pushaq	0
  push  rax
	push	rbx
	push	rcx
	push	rdx
;	push	rbp
	push	rsi
	push	rdi
;	push	rsp
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
%endmacro

%macro	popaq	0
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
;	pop	rsp
	pop	rdi
	pop	rsi
;	pop	rbp
	pop	rdx
	pop	rcx
	pop	rbx
  pop rax
%endmacro

S1      equ     8
S2 equ     12
i1  equ     16
i2      equ     20
dest      equ     24
global dista

dista :
; ------------------------------------------------------------
; Sequenza di ingresso nella funzione
; ------------------------------------------------------------
push		rbp				; salva il Base Pointer
mov		rbp, rsp			; il Base Pointer punta al Record di Attivazione corrente
pushaq						; salva i registri generali

; ------------------------------------------------------------
; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  vmovaps ymm0,[rdi+rdx*4]
  vmovaps ymm1,[rsi+rcx*4]
  vsubps  ymm0, ymm1
  vmulps  ymm0,ymm0
  add rdx,8
  add rcx, 8
  vmovaps ymm2,[rdi+rdx*4]
  vmovaps ymm3,[rsi+rcx*4]
  vsubps  ymm2, ymm3
  vmulps  ymm2,ymm2
  vhaddps ymm0,ymm0
  vhaddps ymm2,ymm2
  vhaddps ymm2,ymm2
  vhaddps ymm0,ymm0
  vperm2f128 ymm1,ymm0,ymm0,1
  vperm2f128 ymm3,ymm2,ymm2,1
  vaddss xmm2,xmm3
  vaddss xmm0,xmm1
  vaddss xmm2,xmm0
  vsqrtps ymm0,ymm0
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------

popaq						; ripristina i registri generali
mov		rsp, rbp			; ripristina lo Stack Pointer
pop		rbp				; ripristina il Base Pointer
  ret
