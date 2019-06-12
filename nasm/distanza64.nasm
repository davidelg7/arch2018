%include "sseutils64.nasm"
section .data
  sedici dd 4
section .text

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


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  xor rax,rax
  vzeroall
  cicloQuoziente8:
    cmp r8,4*DIMREG
    jl cicloQuoziente6
    sub r8,DIMREG*4
    vmovaps ymm1,[rdi+rdx*STEP]
    vmovaps ymm2,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vsubps  ymm1,ymm2
    vmulps ymm1,ymm1; ^2
    vmovaps ymm3,[rdi+rdx*STEP]
    vmovaps ymm4,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vsubps ymm3,ymm4
    vmulps  ymm3,ymm3; ^2
    vmovaps ymm5,[rdi+rdx*STEP]
    vmovaps ymm6,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vmovaps ymm7,[rdi+rdx*STEP]
    vmovaps ymm8,[rsi+rcx*STEP]
    vaddps ymm1,ymm3
    vsubps ymm5,ymm6
    vmulps ymm5,ymm5
    vsubps ymm7,ymm8
    vmulps ymm7,ymm7
    vaddps ymm1,ymm5
    vaddps ymm1,ymm7
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    jmp  cicloQuoziente8
    cicloQuoziente6:
      cmp r8,3*DIMREG
      jl cicloQuoziente4
      sub r8,DIMREG*3
      vmovaps ymm1,[rdi+rdx*STEP]
      vmovaps ymm2,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vsubps  ymm1,ymm2
      vmulps ymm1,ymm1; ^2
      vmovaps ymm3,[rdi+rdx*STEP]
      vmovaps ymm4,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vsubps ymm3,ymm4
      vmulps  ymm3,ymm3; ^2
      vmovaps ymm5,[rdi+rdx*STEP]
      vmovaps ymm6,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vaddps ymm1,ymm3
      vsubps ymm5,ymm6
      vmulps ymm5,ymm5
      vaddps ymm1,ymm5
      vhaddps ymm1,ymm1
      vhaddps ymm1,ymm1
      vperm2f128 ymm2,ymm1,ymm1,1
      vaddss xmm1,xmm2
      addss xmm0,xmm1
      jmp  cicloQuoziente6
  cicloQuoziente4:
     cmp r8,2*DIMREG
     jl cicloResto
     sub r8,DIMREG*2
     vmovaps ymm1,[rdi+rdx*STEP]
     vmovaps ymm2,[rsi+rcx*STEP]
     add rdx,DIMREG
     add rcx,DIMREG
     vsubps  ymm1,ymm2
     vmulps ymm1,ymm1; ^2
     vmovaps ymm3,[rdi+rdx*STEP]
     vmovaps ymm4,[rsi+rcx*STEP]
     vsubps ymm3,ymm4
     vmulps  ymm3,ymm3; ^2
     vaddps ymm1,ymm3
     vhaddps ymm1,ymm1
     vhaddps ymm1,ymm1
     vperm2f128 ymm2,ymm1,ymm1,1
     vaddss xmm1,xmm2
     addss xmm0,xmm1
     jmp  cicloQuoziente4
   cicloQuoziente2:
    cmp r8,DIMREG
    jl cicloResto
    vmovaps ymm1,[rdi+rdx*STEP]
    add rdx,DIMREG
    vmovaps ymm2,[rsi+rcx*STEP]
    vsubps  ymm1,ymm2
    vmulps  ymm1,ymm1
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    add rcx,DIMREG
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    sub r8,DIMREG
    jmp  cicloQuoziente2
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

global dista2

dista2 :
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
  cicloQuoziente8_2:
    cmp r8,4*DIMREG
    jl cicloQuoziente6_2
    sub r8,DIMREG*4
    vmovaps ymm1,[rdi+rdx*STEP]
    vmovaps ymm2,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vsubps  ymm1,ymm2
    vmulps ymm1,ymm1; ^2
    vmovaps ymm3,[rdi+rdx*STEP]
    vmovaps ymm4,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vsubps ymm3,ymm4
    vmulps  ymm3,ymm3; ^2
    vmovaps ymm5,[rdi+rdx*STEP]
    vmovaps ymm6,[rsi+rcx*STEP]
    add rdx,DIMREG
    add rcx,DIMREG
    vmovaps ymm7,[rdi+rdx*STEP]
    vmovaps ymm8,[rsi+rcx*STEP]
    vaddps ymm1,ymm3
    vsubps ymm5,ymm6
    vmulps ymm5,ymm5
    vsubps ymm7,ymm8
    vmulps ymm7,ymm7
    vaddps ymm1,ymm5
    vaddps ymm1,ymm7
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    jmp  cicloQuoziente8_2
    cicloQuoziente6_2:
      cmp r8,3*DIMREG
      jl cicloQuoziente4_2
      sub r8,DIMREG*3
      vmovaps ymm1,[rdi+rdx*STEP]
      vmovaps ymm2,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vsubps  ymm1,ymm2
      vmulps ymm1,ymm1; ^2
      vmovaps ymm3,[rdi+rdx*STEP]
      vmovaps ymm4,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vsubps ymm3,ymm4
      vmulps  ymm3,ymm3; ^2
      vmovaps ymm5,[rdi+rdx*STEP]
      vmovaps ymm6,[rsi+rcx*STEP]
      add rdx,DIMREG
      add rcx,DIMREG
      vaddps ymm1,ymm3
      vsubps ymm5,ymm6
      vmulps ymm5,ymm5
      vaddps ymm1,ymm5
      vhaddps ymm1,ymm1
      vhaddps ymm1,ymm1
      vperm2f128 ymm2,ymm1,ymm1,1
      vaddss xmm1,xmm2
      addss xmm0,xmm1
      jmp  cicloQuoziente6_2
  cicloQuoziente4_2:
     cmp r8,2*DIMREG
     jl cicloResto_2
     sub r8,DIMREG*2
     vmovaps ymm1,[rdi+rdx*STEP]
     vmovaps ymm2,[rsi+rcx*STEP]
     add rdx,DIMREG
     add rcx,DIMREG
     vsubps  ymm1,ymm2
     vmulps ymm1,ymm1; ^2
     vmovaps ymm3,[rdi+rdx*STEP]
     vmovaps ymm4,[rsi+rcx*STEP]
     vsubps ymm3,ymm4
     vmulps  ymm3,ymm3; ^2
     vaddps ymm1,ymm3
     vhaddps ymm1,ymm1
     vhaddps ymm1,ymm1
     vperm2f128 ymm2,ymm1,ymm1,1
     vaddss xmm1,xmm2
     addss xmm0,xmm1
     jmp  cicloQuoziente4_2
   cicloQuoziente2_2:
    cmp r8,DIMREG
    jl cicloResto_2
    vmovaps ymm1,[rdi+rdx*STEP]
    add rdx,DIMREG
    vmovaps ymm2,[rsi+rcx*STEP]
    vsubps  ymm1,ymm2
    vmulps  ymm1,ymm1
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    add rcx,DIMREG
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    sub r8,DIMREG
    jmp  cicloQuoziente2_2
  cicloResto_2:
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
    jmp cicloResto_2
  fine_2:
; ------------------------------------------------------------
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------

popaq						; ripristina i registri generali
mov		rsp, rbp			; ripristina lo Stack Pointer
pop		rbp				; ripristina il Base Pointer
ret
