#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1]; // 用户栈
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0) // 为什么不用原来的页目录呢？
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;  // ~3 : ...11111100    +1 是代表 '\0' 字符, ~3表示4字节对齐, 计算当前字符串所需要的存储空间。
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0) // 复制argv[argc]所指的字符串到用户栈中。
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0; // 3即下面的ustack[0]、ustack[1]、ustack[2]

  ustack[0] = 0xffffffff;  // fake return PC   我的理解是，因为程序是从虚拟地址0处开始执行，这里的0xffffffff是模拟了跳转前的ip地址，当该函数结束时，即return语句处返回则下一条指令刚好就回到了地址0处开始执行了。
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer
  // 注意了，前面仅仅只是把字符串复制到栈中了。
  sp -= (3+argc+1) * 4; // 这里的 4 是4字节的意思吗？ +1是因为argv[]中最后一个元素的地址为0。
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0) // ustack已经准备好了，可以复制到sp所指向的地址了。
    goto bad;
  // 用户栈的最终布局见：https://xiayingp.gitbook.io/build_a_os/traps-and-interrupts/how-exec-works
  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
