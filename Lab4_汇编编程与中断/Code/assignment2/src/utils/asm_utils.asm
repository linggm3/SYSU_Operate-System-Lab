[bits 32]

global asm_hello_world

asm_hello_world:
    push eax
    xor eax, eax
    ;mov ax, 0xb800
    ;mov gs, ax
    mov ah, 0x07
    mov al, '2'
    mov [gs:2 * (80 *2 + 0)], ax

    mov al, '1'
    mov [gs:2 * (80 *2 + 1)], ax

    mov al, '3'
    mov [gs:2 * (80 *2 + 2)], ax

    mov al, '0'
    mov [gs:2 * (80 *2 + 3)], ax

    mov al, '7'
    mov [gs:2 * (80 *2 + 4)], ax

    mov al, '0'
    mov [gs:2 * (80 *2 + 5)], ax

    mov al, '7'
    mov [gs:2 * (80 *2 + 6)], ax

    mov al, '7'
    mov [gs:2 * (80 *2 + 7)], ax

    pop eax
    ret
