section .data
  sedici dd 4
section .text
%macro	pushaq	0
  push  rax
	push	rbx
	push	rcx
	push	rdx
  push	rbp
	push	rsi
	push	rdi
	push	rsp
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
	pop	rsp
	pop	rdi
	pop	rsi
	pop	rbp
	pop	rdx
	pop	rcx
	pop	rbx
  pop rax
%endmacro

S1 equ     8
S2 equ     12
I1 equ     16
I2 equ     20
STEP equ 4
DIMREG equ 8

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

   xor rax,rax
  vzeroall
   cicloQuoziente:
     cmp r8,8
     jl cicloResto
    sub r8,DIMREG
    vmovups ymm1,[rdi+rdx*STEP]
    vmovups ymm2,[rsi+rcx*STEP]
    vsubps  ymm1,ymm2
    add rdx,DIMREG
    add rcx,DIMREG
    vmulps  ymm1,ymm1
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
     jmp  cicloQuoziente
  cicloResto:
    cmp r8,0
    je fine
    vmovss xmm1,[rdi+rdx*STEP]
    vmovss xmm2,[rsi+rcx*STEP]
    subss xmm1,xmm2
    mulss xmm1,xmm1
    sub r8,1
    add rdx,1
    add rcx, 1
    addss xmm0,xmm1
    jmp cicloResto
  fine:
  vsqrtps ymm0,ymm0
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------

popaq						; ripristina i registri generali
mov		rsp, rbp			; ripristina lo Stack Pointer
pop		rbp				; ripristina il Base Pointer
ret
