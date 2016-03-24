org 0x7c00

[bits 16]
MAX_FAILURE EQU 5

jmp entry
    db 0x90 ;nop
    db "kaiosipl" ;maker name
    dw 0x0200 ;size per sector
    db 0x01 ;sector per cluster
    dw 0x01 ;ipl occupies sector
    db 0x02 ;number of fat table
    dw 0x00e0 ;max root directory
    dw 0x0b40 ;number of sector
    db 0x00f0 ;media descriptor
    dw 0x0009 ;fat table size
    dw 0x0012 ;sectors per cylinder
    dw 0x0002 ;number of header
    dd 0x00 ;hidden sectors
    dd 0x00 ;null
    db 0x00 ;int13 driver number
    db 0x00 ;null
    db 0x29 ;extended boot stamp
    dd 0x00 ;volume number
    db "kaimingveos" ;volume label
    db "fat12   " ;file system type

entry:
    mov	ax, cs
    mov	ss, ax
    mov	ds, ax
    mov	sp, 0x7c00
    mov ch, 0x00 ;cylinder number
    mov cl, 0x02 ;sector number
    mov dh, 0x00 ;header number
    mov dl, 0x00 ;driver number
    mov ax, 0x07e0 ;os entry address 0x7e00
    mov es, ax ;io buffer sector
    mov al, 1 ;sector number
init:
    mov si, 0
copyloop:
    mov ah, 0x02 ;read driver
    mov al, 1
    int 0x13
    cmp ah, 0
    je next

    mov dl, 0x00
    mov ah, 0x00
    int 0x13
    inc si
    cmp si, MAX_FAILURE
    jb copyloop
    jmp fail

next:
    mov ax, es
    add ax, 0x0020
    mov es, ax ;next 512 bytes
    inc cl ;sector number
    cmp cl, 18
    jbe init
    mov cl, 1
    inc dh
    cmp dh, 1 ;header number
    jbe init
    mov dh, 0
    inc ch
    cmp ch, 9 ;cylinder number
    jbe init

    jmp 0xbe00 ; head address of asmhead.bin

fail:
    mov si, msg
printloop:
    mov bl, 0x15
    mov al, [si]
    cmp al, 0
    je fin
    inc si
    mov ah, 0x0e
    int 0x10
    jmp printloop
msg:
    db "load fail"
    db 0x00, 0x00

fin:
    hlt
    jmp fin

    resb 510-($ - $$)
    dw 0xaa55
