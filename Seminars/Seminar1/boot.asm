bits 16

start:
  mov ax, 0x07C0
  add ax, 0x20
  mov ss, ax      ; stack segment to ax
  mov sp, 0x1000  ; stack pointer set (relative to ss)
  mov ax, 0x07C0
  mov ds, ax      ; data segment to ax

  mov si, msg     ; pointer to the message in source index
  mov ah, 0x0E    ; print char BIOS procedure

  .next:
    lodsb         ; next byte to AL, increment source index
    cmp al, 0     ; if the byte is zero
    je .done      ;   jump to done
    int 0x10      ; invoke BIOS syscall
    jmp .next     ; loop

    .done:
      jmp $         ; loop forever

  msg: db 'Hello', 0; string to print

; $=IP, $$=startOfSegment, $-$$ = how much is used
  times 510-($-$$) db 0 ; fill up to 510 bytes
  dw 0xAA55             ; master boot record signature
