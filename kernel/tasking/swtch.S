# Context switch
#
#   void swtch(struct context **old, struct context *new);
# 
# Save the current registers on the stack, creating
# a struct context, and save its address in *old.
# Switch stacks to new and pop previously-saved registers.

.globl swtch
swtch:
  movl 4(%esp), %eax  # 参数 old
  movl 8(%esp), %edx  # 参数 new

  # Save old callee-saved registers
  pushl %ebp # eip 也压栈了。在ebp之前。
  pushl %ebx
  pushl %esi
  pushl %edi  # 保存context在kernel的内核栈。

  # Switch stacks
  movl %esp, (%eax)   # 注意了，这里有个括号，表示存储到eax所指向的地址。平坦模型下，ss在任何情况下都是一样的，stack不同只在于sp,而不在于ss。
  movl %edx, %esp     # 切换到新的esp,即切换到进程的内核栈。

  # Load new callee-saved registers
  popl %edi
  popl %esi
  popl %ebx
  popl %ebp
  ret
