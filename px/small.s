movl $0, %eax
movl $5, %edx
jmp L2
L1:
addl %edx,%eax
subl $1,%edx
L2:
cmpl $1,%edx
jge L1
