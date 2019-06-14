%include "sseutils64.nasm"
section .data
  sedici dd 4
section .tex

STEP equ 4
DIMREG equ 8




global diff2

diff2 :
; ------------------------------------------------------------
; Sequenza di ingresso nella funzione
; ------------------------------------------------------------
push		rbp				; salva il Base Pointer
mov		rbp, rsp			; il Base Pointer punta al Record di Attivazione corrente
pushaq						; salva i registri generali


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

xor rax,rax
vzeroall
cicloQuoziente:
  cmp r9,DIMREG*3
  jl cicloResto
  sub r9,DIMREG*3
  vmovups ymm1,[rsi+rcx*STEP]
  vmovups ymm2,[rdx+r8*STEP]
  add rcx,DIMREG
  add r8,DIMREG
  vsubps  ymm1,ymm2
  vmovups[rdi+rcx*STEP],ymm1

  vmovups ymm3,[rsi+rcx*STEP]
  vmovups ymm4,[rdx+r8*STEP]
  add rcx,DIMREG
  add r8,DIMREG
  vsubps  ymm3,ymm4
  vmovups[rdi+rcx*STEP],ymm3

  vmovups ymm5,[rsi+rcx*STEP]
  vmovups ymm6,[rdx+r8*STEP]
  add rcx,DIMREG
  add r8,DIMREG
  vsubps  ymm5,ymm6
  vmovups[rdi+rcx*STEP],ymm5

  jmp  cicloQuoziente
cicloResto:
  cmp r9,0
  je fine
  vmovups ymm1,[rsi+rcx*STEP]
  vmovups ymm2,[rdx+r8*STEP]
  subss xmm1,xmm2
  sub r9,1
  add rcx,1
  add r8, 1
  movss [rdi+rcx*STEP],xmm1
  jmp cicloResto
fine:
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------

popaq						; ripristina i registri generali
mov		rsp, rbp			; ripristina lo Stack Pointer
pop		rbp				; ripristina il Base Pointer
ret
