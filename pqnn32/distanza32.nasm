%include "sseutils32.nasm"
section .bss
res: resd 1
section .text

STEP equ 4
DIMREG equ 8




global dista

dista :
; ------------------------------------------------------------
; Sequenza di ingresso nella funzione
; ------------------------------------------------------------
push	ebp				; salva il Base Pointer
mov		ebp, esp
pushaq						; salva i registri generali


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  xor eax,eax
  mov edi,[ebp+8]
  mov esi,[ebp+12]
  mov edx,[ebp+16];terzo parametro
  mov ecx,[ebp+20];quarto parametro
  mov ebx,[ebp+24]
  vzeroall
      cicloQuoziente6:
      cmp ebx,3*DIMREG
      jl cicloQuoziente4
      sub ebx,DIMREG*3
      vmovaps ymm1,[edi+edx*STEP]
      vmovaps ymm2,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps  ymm1,ymm2
      vmulps ymm1,ymm1; ^2
      vmovaps ymm3,[edi+edx*STEP]
      vmovaps ymm4,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps ymm3,ymm4
      vmulps  ymm3,ymm3; ^2
      vmovaps ymm5,[edi+edx*STEP]
      vmovaps ymm6,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
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
     cmp ebx,2*DIMREG
     jl cicloResto
     sub ebx,DIMREG*2
     vmovaps ymm1,[edi+edx*STEP]
     vmovaps ymm2,[esi+ecx*STEP]
     add edx,DIMREG
     add ecx,DIMREG
     vsubps  ymm1,ymm2
     vmulps ymm1,ymm1; ^2
     vmovaps ymm3,[edi+edx*STEP]
     vmovaps ymm4,[esi+ecx*STEP]
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
    cmp ebx,DIMREG
    jl cicloResto
    vmovaps ymm1,[edi+edx*STEP]
    add edx,DIMREG
    vmovaps ymm2,[esi+ecx*STEP]
    vsubps  ymm1,ymm2
    vmulps  ymm1,ymm1
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    add ecx,DIMREG
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    sub ebx,DIMREG
    jmp  cicloQuoziente2
  cicloResto:
    cmp ebx,0
    je fine
    vmovss xmm1,[edi+edx*STEP]
    vmovss xmm2,[esi+ecx*STEP]
    subss xmm1,xmm2
    mulss xmm1,xmm1
    sub ebx,1
    add edx,1
    add ecx, 1
    addss xmm0,xmm1
    jmp cicloResto
  fine:
   vsqrtps ymm0,ymm0
   movss [res],xmm0

   fld dword [res]
  ; mov eax, res
; Sequenza di uscita dalla funzione
; ------------------------------------------------------------
popaq						; ripristina i registri generali
mov		esp, ebp			; ripristina lo Stack Pointer
pop		ebp				; ripristina il Base Pointer
ret

global dista2

dista2 :
; ------------------------------------------------------------
; Sequenza di ingresso nella funzione
; ------------------------------------------------------------
push	ebp				; salva il Base Pointer
mov		ebp, esp
pushaq						; salva i registri generali


; I paramentri sono passati nei registri
; ------------------------------------------------------------
; rdi = indirizzo della struct input

  xor eax,eax
  mov edi,[ebp+8]
  mov esi,[ebp+12]
  mov edx,[ebp+16];terzo parametro
  mov ecx,[ebp+20];quarto parametro
  mov ebx,[ebp+24]
  vzeroall
      cicloQuoziente6_2:
      cmp ebx,3*DIMREG
      jl cicloQuoziente4_2
      sub ebx,DIMREG*3
      vmovaps ymm1,[edi+edx*STEP]
      vmovaps ymm2,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps  ymm1,ymm2
      vmulps ymm1,ymm1; ^2
      vmovaps ymm3,[edi+edx*STEP]
      vmovaps ymm4,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
      vsubps ymm3,ymm4
      vmulps  ymm3,ymm3; ^2
      vmovaps ymm5,[edi+edx*STEP]
      vmovaps ymm6,[esi+ecx*STEP]
      add edx,DIMREG
      add ecx,DIMREG
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
     cmp ebx,2*DIMREG
     jl cicloResto
     sub ebx,DIMREG*2
     vmovaps ymm1,[edi+edx*STEP]
     vmovaps ymm2,[esi+ecx*STEP]
     add edx,DIMREG
     add ecx,DIMREG
     vsubps  ymm1,ymm2
     vmulps ymm1,ymm1; ^2
     vmovaps ymm3,[edi+edx*STEP]
     vmovaps ymm4,[esi+ecx*STEP]
     vsubps ymm3,ymm4
     vmulps  ymm3,ymm3; ^2
     vaddps ymm1,ymm3
     vhaddps ymm1,ymm1
     vhaddps ymm1,ymm1
     vperm2f128 ymm2,ymm1,ymm1,1
     vaddss xmm1,xmm2
     addss xmm0,xmm1
     jmp  cicloQuoziente4
   cicloQuoziente2_2:
    cmp ebx,DIMREG
    jl cicloResto_2
    vmovaps ymm1,[edi+edx*STEP]
    add edx,DIMREG
    vmovaps ymm2,[esi+ecx*STEP]
    vsubps  ymm1,ymm2
    vmulps  ymm1,ymm1
    vhaddps ymm1,ymm1
    vhaddps ymm1,ymm1
    add ecx,DIMREG
    vperm2f128 ymm2,ymm1,ymm1,1
    vaddss xmm1,xmm2
    addss xmm0,xmm1
    sub ebx,DIMREG
    jmp  cicloQuoziente2_2
  cicloResto_2:
    cmp ebx,0
    je fine
    vmovss xmm1,[edi+edx*STEP]
    vmovss xmm2,[esi+ecx*STEP]
    subss xmm1,xmm2
    mulss xmm1,xmm1
    sub ebx,1
    add edx,1
    add ecx, 1
    addss xmm0,xmm1
    jmp cicloResto_2
  fine_2:
   movss [res],xmm0

   fld dword [res]
