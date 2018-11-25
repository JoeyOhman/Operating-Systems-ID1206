global start
extern myos_main

bits 32

section .text

start:
  cli
  mov esp, stack
  call myos_main
  hlt

section .bss

resb 8192
stack:
  
