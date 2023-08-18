// Memory layout

#define EXTMEM  0x100000            // Start of extended memory   物理地址
#define PHYSTOP 0xE000000           // Top physical memory        物理地址, 不过这个地址是如何决定的？我查了很多specs都没有规定。
#define DEVSPACE 0xFE000000         // Other devices are at high addresses  虚拟地址

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000         // First kernel virtual address   虚拟地址
#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked  同vmlinux.ld中一致  虚拟地址

#define V2P(a) (((uint) (a)) - KERNBASE)  // 为什么这里又不需要(void *) 强制转换？
#define P2V(a) ((void *)(((char *) (a)) + KERNBASE))  // 为什么这里需要(void *)强制转换？

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts
