%include "sseutils64.nasm"
section .data
  sedici dd 4
section .text

K equ     16
V equ     24

STEP equ 4
DIMREG equ 8
PROCESSATI equ 1

global vettoreDistanze
vettoreDistanze:
push		rbp				; salva il Base Pointer
mov		rbp, rsp			; il Base Pointer punta al Record di Attivazione corrente
pushaq

pushaq
getmem 4,[rbp+K]
popaq
mov r10,rax ; salvo l'indirizzo del vettore in r10
mov rax,r10
mov r11,[rbp+K] ; i=k
forK: cmp r11, 0 ;i>=0?
      je fine
      mov r12,rdx
      vmovaps ymm0,[rdi+r12*4]; salvo il primi 8 elementi di x in ymm0
      mov r13,r8 ; mi ricordo quanto è lungo un singolo vettore
      mov r14,rcx ; mi ricordo da dove parte questo punto
      forD:
        cmp r13,0; fino a quando non ho esaurito le dimensioni del singolo punto
        je salva
        vmovaps ymm6,[rsi+r14*4]; mi salvo 8 componenti di un punto di y
        vsubps ymm6,ymm0;x-y
        vmulps ymm6,ymm6; (x-y)^2
        vhaddps ymm6,ymm6
        vhaddps ymm6,ymm6
        vperm2f128 ymm2,ymm6,ymm6,1
        vaddss xmm2,xmm6 ; a questo punto in xmm2 ho il float della distanza di cui fare la radice
        sub r13, DIMREG; tolgo le dimensioni considerate
        add r14, DIMREG
        vaddss xmm3,xmm2
        jmp forD
      salva:; se sono arrivato qui significa che ho considerato tutte le componenti di un sinfolo punto
      ;  add rcx,r9;passo al punto successivo incrementando di D
        ;vsqrtps ymm2
        vmovss [r10+r15*4],xmm3
        sub r11,PROCESSATI
        jmp forK
fine:
  mov rax,r10
popaq						; ripristina i registri generali
mov		rsp, rbp			; ripristina lo Stack Pointer
pop		rbp				; ripristina il Base Pointer
ret
