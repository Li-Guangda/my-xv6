// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock 为什么这里需要锁呢？
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

