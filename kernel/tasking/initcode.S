# Initial process execs /init.
# This code runs in user space.

#include "syscall.h"
#include "traps.h"


# exec(init, argv)
.globl start
start:
  pushl $argv
  pushl $init
  pushl $0  # where caller pc would be 用户态封装的系统调用函数在执行后（即call指令后）,最后在用户栈中保存eip的值。这里因为没有进行正常的系统调用，所以就模拟占位了一个。
  movl $SYS_exec, %eax
  int $T_SYSCALL # should not return

# for(;;) exit();
exit:
  movl $SYS_exit, %eax
  int $T_SYSCALL
  jmp exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .long init
  .long 0

