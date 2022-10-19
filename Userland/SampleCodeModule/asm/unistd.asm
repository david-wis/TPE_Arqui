GLOBAL write
GLOBAL read
GLOBAL clear

write:
    mov rax, 1
    int 80h
    ret

read:
    mov rax, 0
    int 80h
    ret

clear:
    mov rax, 2
    int 80h
    ret