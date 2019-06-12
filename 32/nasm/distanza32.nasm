%include "sseutils32.nasm"
section .data
  sedici dd 4
section .text

S1 equ     8
S2 equ     12
I1 equ     16
I2 equ     20
STEP equ 4
DIMREG equ 4




global dista

dista :
; ------------------------------------------------------------
; Sequenza di ingresso nella funzione
; ------------------------------------------------------------
push		ebp				; salva il Base Pointer
mov		ebp, ebx			; il Base Pointer punta al Record di Attivazione corrente
pushaq						; salva i registri generali


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  xor eax,eax
  mov edi,[ebp+8]
  mov esi,[ebp+12]
  mov edx,[ebp+16];terzo parametro
  mov ecx,[ebp+20];quarto parametro
  ; mov ebx,[ebp+24]
  mov eax, ecx
  ; vzeroall
  ;     cicloQuoziente6:
  ;     cmp ebx,3*DIMREG
  ;     jl cicloQuoziente4
  ;     sub ebx,DIMREG*3
  ;     movaps xmm1,[edi+edx*STEP]
  ;     movaps xmm2,[esi+ecx*STEP]
  ;     add edx,DIMREG
  ;     add ecx,DIMREG
  ;     subps  xmm1,xmm2
  ;     mulps xmm1,xmm1; ^2
  ;     movaps xmm3,[edi+edx*STEP]
  ;     movaps xmm4,[esi+ecx*STEP]
  ;     add edx,DIMREG
  ;     add ecx,DIMREG
  ;     subps xmm3,xmm4
  ;     mulps  xmm3,xmm3; ^2
  ;     movaps xmm5,[edi+edx*STEP]
  ;     movaps xmm6,[esi+ecx*STEP]
  ;     add edx,DIMREG
  ;     add ecx,DIMREG
  ;     addps xmm1,xmm3
  ;     subps xmm5,xmm6
  ;     mulps xmm5,xmm5
  ;     addps xmm1,xmm5
  ;     haddps xmm1,xmm1
  ;     haddps xmm1,xmm1
  ;     movhlps xmm1,xmm2
  ;     addss xmm1,xmm2
  ;     addss xmm0,xmm1
  ;     jmp  cicloQuoziente6
  ; cicloQuoziente4:
  ;    cmp ebx,2*DIMREG
  ;    jl cicloResto
  ;    sub ebx,DIMREG*2
  ;    movaps xmm1,[edi+edx*STEP]
  ;    movaps xmm2,[esi+ecx*STEP]
  ;    add edx,DIMREG
  ;    add ecx,DIMREG
  ;    subps  xmm1,xmm2
  ;    mulps xmm1,xmm1; ^2
  ;    movaps xmm3,[edi+edx*STEP]
  ;    movaps xmm4,[esi+ecx*STEP]
  ;    subps xmm3,xmm4
  ;    mulps  xmm3,xmm3; ^2
  ;    addps xmm1,xmm3
  ;    haddps xmm1,xmm1
  ;    haddps xmm1,xmm1
  ;    movhlps xmm1,xmm2
  ;    addss xmm1,xmm2
  ;    addss xmm0,xmm1
  ;    jmp  cicloQuoziente4
  ;  cicloQuoziente2:
  ;   cmp ebx,DIMREG
  ;   jl cicloResto
  ;   movaps xmm1,[edi+edx*STEP]
  ;   add edx,DIMREG
  ;   movaps xmm2,[esi+ecx*STEP]
  ;   subps  xmm1,xmm2
  ;   mulps  xmm1,xmm1
  ;   haddps xmm1,xmm1
  ;   haddps xmm1,xmm1
  ;   add ecx,DIMREG
  ;   movhlps xmm1,xmm2
  ;   addss xmm1,xmm2
  ;   addss xmm0,xmm1
  ;   sub ebx,DIMREG
  ;   jmp  cicloQuoziente2
  ; cicloResto:
  ;   cmp ebx,0
  ;   je fine
    ; movss xmm1,[edi+edx*STEP]
    ; movss xmm2,[esi+ecx*STEP]
    ; subss xmm1,xmm2
    ; mulss xmm1,xmm1
    ; sub ebx,1
    ; add edx,1
    ; add ecx, 1
    ; addss xmm0,xmm1
    ; jmp cicloResto
  ; fine:
  ; sqrtps xmm0,xmm0
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------
popaq						; ripristina i registri generali
mov		ebx, ebp			; ripristina lo Stack Pointer
pop		ebp				; ripristina il Base Pointer
ret

global dista2

dista2 :
