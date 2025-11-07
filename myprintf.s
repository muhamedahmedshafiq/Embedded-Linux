section .text
global myprintf


myprintf:
  
    mov rsi, rdi      

    
    mov rax, 1     
    mov rdi, 1        

    xor rdx, rdx      

count_loop:
    cmp byte [rsi + rdx], 0   
    je do_write
    inc rdx
    jmp count_loop

do_write:
    syscall          
    ret
