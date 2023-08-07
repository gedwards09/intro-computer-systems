cmpl %eax, %ecx
jmp target
addl $0x03, %eax
target:
addl $0x01, %eax
