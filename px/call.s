jmp main
target1:
addl $0x01, %eax
call target2
ret
target2:
addl $0x2, %eax
ret
main:
cmp %eax, %ecx
call target1
addl $0x03, %eax

