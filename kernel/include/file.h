struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count 因为 filedup() 函数会复制文件描述符，所以需要 ref 来记录被引用的次数。
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};


// in-memory copy of an inode
struct inode { // 为什么不直接包含 struct dinode 呢？
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count  作用？ 代表进程引用该节点的次数。
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;        // 作用？ link()相关函数会增加。
  uint size;
  uint addrs[NDIRECT+1];
};

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
