; bits 16

section .multiboot_header

magic equ 0xe85250d6      ; multiboot 2
arch equ 0                ; protected mode i386

header_start: ; dd = define double word, dw = define word, word=2bytes in 32bitsys
  dd magic  ; magice number
  dd arch   ; architecture
  dd header_end - header_start  ; header length
  dd 0x100000000 - (magic + arch + (header_end - header_start)) ; checksum
  dw 0  ; type
  dw 0  ; flags
  dd 8  ; size
header_end:
