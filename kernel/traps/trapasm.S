#include "mmu.h"

  # vectors.S sends all traps here.
.globl alltraps          # 此时已经切换至当前进程的内核栈并在这之前已经分别 push 了错误码(不是所有中断都有)和中断号了。 
alltraps:                # alltraps 子程序为 trap() 函数做预备工作。还要注意的是此时已经通过 TSS 自动切换到当前进程的内核栈了。
  # Build trap frame.    push用户态的寄存器状态到进程的内核栈中。
  pushl %ds
  pushl %es
  pushl %fs
  pushl %gs
  pushal
  
  # Set up data segments. 为什么只设置 ds 和 es ?
  movw $(SEG_KDATA<<3), %ax # 同时由原先的用户段选择子切换到内核的段选择子。
  movw %ax, %ds
  movw %ax, %es

  # Call trap(tf), where tf=%esp
  pushl %esp   # 这里压入esp的值，是因为trap函数的参数是trapframe指针类型，而此时esp刚好是trapframe的地址。
  call trap     
  addl $4, %esp  # 忽略压入的trapframe的地址。

  # Return falls through to trapret...
.globl trapret  # 恢复用户态的寄存器状态。
trapret:
  popal
  popl %gs
  popl %fs
  popl %es
  popl %ds
  addl $0x8, %esp  # trapno and errcode
  iret
