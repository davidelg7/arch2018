%include "sseutils64.nasm"
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
mov		ebp, esp			; il Base Pointer punta al Record di Attivazione corrente
pushaq						; salva i registri generali


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  xor eax,eax
  mov edi,[ebp+4]
  mov esi,[ebp+8]
  mov edx,[ebp+12]
  mov ecx,[ebp+16]
  vzeroall
      cicloQuoziente6:
      cmp [ebp+20],3*DIMREG
      jl cicloQuoziente4
      sub [ebp+20],DIMREG*3
      vmovaps xmm1,[edi+edx*STEP]
      vmovaps xmm2,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps  xmm1,xmm2
      vmulps xmm1,xmm1; ^2
      vmovaps xmm3,[edi+edx*STEP]
      vmovaps xmm4,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps xmm3,xmm4
      vmulps  xmm3,xmm3; ^2
      vmovaps xmm5,[edi+edx*STEP]
      vmovaps xmm6,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vaddps xmm1,xmm3
      vsubps xmm5,xmm6
      vmulps xmm5,xmm5
      vaddps xmm1,xmm5
      vhaddps xmm1,xmm1
      vhaddps xmm1,xmm1
      vperm2f128 xmm2,xmm1,xmm1,1
      vaddss xmm1,xmm2
      addss xmm0,xmm1
      jmp  cicloQuoziente6
  cicloQuoziente4:
     cmp [ebp+20],2*DIMREG
     jl cicloResto
     sub [ebp+20],DIMREG*2
     vmovaps xmm1,[edi+edx*STEP]
     vmovaps xmm2,[esi+ecx*STEP]
     add edx,DIMREG
     add ecx,DIMREG
     vsubps  xmm1,xmm2
     vmulps xmm1,xmm1; ^2
     vmovaps xmm3,[edi+edx*STEP]
     vmovaps xmm4,[esi+ecx*STEP]
     vsubps xmm3,xmm4
     vmulps  xmm3,xmm3; ^2
     vaddps xmm1,xmm3
     vhaddps xmm1,xmm1
     vhaddps xmm1,xmm1
     vperm2f128 xmm2,xmm1,xmm1,1
     vaddss xmm1,xmm2
     addss xmm0,xmm1
     jmp  cicloQuoziente4
   cicloQuoziente2:
    cmp [ebp+20],DIMREG
    jl cicloResto
    vmovaps xmm1,[edi+edx*STEP]
    add rdx,DIMREG
    vmovaps xmm2,[esi+ecx*STEP]
    vsubps  xmm1,xmm2
    vmulps  xmm1,xmm1
    vhaddps xmm1,xmm1
    vhaddps xmm1,xmm1
    add ecx,DIMREG
    vperm2f128 xmm2,xmm1,xmm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    sub [ebp+20],DIMREG
    jmp  cicloQuoziente2
  cicloResto:
    cmp [ebp+20],0
    je fine
    vmovss xmm1,[edi+edx*STEP]
    vmovss xmm2,[esi+ecx*STEP]
    subss xmm1,xmm2
    mulss xmm1,xmm1
    sub [ebp+20],1
    add edx,1
    add ecx, 1
    addss xmm0,xmm1
    jmp cicloResto
  fine:
  sqrtps xmm0,xmm0
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------

popaq						; ripristina i registri generali
mov		esp, ebp			; ripristina lo Stack Pointer
pop		ebp				; ripristina il Base Pointer
ret
