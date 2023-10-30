

# 1. 虚拟地址空间



> 虚拟地址空间是一个非常抽象的概念，先根据字面意思进行解释：
>
> * 它可以用来加载程序数据（数据可能被加载到物理内存上，空间不够就加载到虚拟内存中）
> * 它对应着一段连续的内存地址，起始位置为 0
> * 之所以说虚拟是因为这个起始的 0 地址是被虚拟出来的， 不是物理内存的 0 地址



虚拟地址空间的大小也由操作系统决定，`32位的操作系统虚拟地址空间的大小为` 2^32 字节，也就是`4G`，64 位的操作系统虚拟地址空间大小为 2^64 字节，这是一个非常大的数。`当我们运行磁盘上一个可执行程序, 就会得到一个进程，内核会给每一个运行的进程创建一块属于自己的虚拟地址空间，并将应用程序数据装载到虚拟地址空间对应的地址上。`

进程在运行过程中，程序内部所有的指令都是通过 CPU 处理完成的，CPU 只进行数据运算并不具备数据存储的能力，其处理的数据都加载自物理内存，那么进程中的数据是如何进出入到物理内存中的呢？其实是通过 CPU 中的**内存管理单元 MMU（Memory Management Unit）从进程的虚拟地址空间中映射过去的**。



![image-20230911214253436](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112142488.png)



## 1.1 存在的意义

为什么操作系统不直接将数据加载到物理内存中而是将数据加载到虚拟地址空间中，在通过CPU的MMU映射到物理内存中呢？

先来看一下如果直接将数据加载到物理内存会发生什么事情：



![image-20230911214406295](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112144328.png)



> 假设计算机的物理内存大小为1G,  进程 A 需要 100M 内存因此直接在物理内存上从 0 地址开始分配 100M, 进程 B 启动需要 250M 内存,  因此继续在物理内存上为其分配 250M 内存,  并且进程 A 和进程 B 占用的内存是连续的。之后再启动其他进程继续按照这种方法进行物理内存的分配。。。
>
> 使用这种方式分配内存会有如下几个问题：

1. `每个进程的地址不隔离，有安全风险`

   由于程序都是直接访问物理内存，所以恶意程序可以通过内存寻址随意修改别的进程对应的内存数据，以达到破坏的目的。虽然有些时候是非恶意的，但是有些存在 bug 的程序可能不小心修改了其它程序的内存数据，就会导致其它程序的运行出现异常。

2. `内存效率低`

   如果直接使用物理内存的话，一个进程对应的内存块就是作为一个整体操作的，如果出现物理内存不够用的时候，我们一般的办法是将不常用的进程拷贝到磁盘的交换分区（虚拟内存）中，以便腾出内存，因此就需要将整个进程一起拷走，如果数据量大，在内存和磁盘之间拷贝时间就会很长，效率低下。

3. `进程中数据的地址不确定，每次都会发生变化`

   由于物理内存的使用情况一直在动态的变化，我们无法确定内存现在使用到哪里了，如果直接将程序数据加载到物理内存，内存中每次存储数据的起始地址都是不一样的，这样数据的加载都需要使用相对地址，加载效率低（静态库是使用绝对地址加载的）。



> 有了虚拟地址空间之后就可以完美的解决上边提到的所有问题了，`虚拟地址空间就是一个中间层，相当于在程序和物理内存之间设置了一个屏障，将二者隔离开来。程序中访问的内存地址不再是实际的物理内存地址，而是一个虚拟地址，然后由操作系统将这个虚拟地址映射到适当的物理内存地址上。`这样，只要操作系统处理好虚拟地址到物理内存地址的映射，就可以保证不同的程序最终访问的内存地址位于不同的区域，彼此没有重叠，就可以达到内存地址空间隔离的效果。



## 1.2 分区

从操作系统层级上看，虚拟地址空间主要分为两个部分`内核区`和`用户区`。

* 内核区：
  * 内核空间为内核保留，`不允许应用程序读写该区域的内容或直接调用内核代码定义的函数。`
  * 内核总是驻留在内存中，是操作系统的一部分。
  * 系统中所有进程对应的虚拟地址空间的内核区都会映射到同一块物理内存上（系统内核只有一个）。
* 用户区：存储用户程序运行中用到的各种数据。

我们先来看一下进程对应的虚拟地址空间的各个分区，再来详细介绍用户区的组成（以32位系统的虚拟地址空间为例）。



![image-20230911214952924](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112149970.png)

每个进程的虚拟地址空间都是从0地址开始的，我们在程序中打印的变量地址也其在虚拟地址空间中的地址，程序是无法直接访问物理内存的。虚拟地址空间中用户区地址范围是 0~3G，里边分为多个区块：

* `保留区：` 位于虚拟地址空间的最底部，未赋予物理地址。任何对它的引用都是非法的，程序中的空指针（NULL）指向的就是这块内存地址。
* `.text段：` 代码段也称正文段或文本段，通常用于存放程序的执行代码(即 CPU 执行的机器指令)，代码段一般情况下是只读的，这是对执行代码的一种保护机制。
* `.data段：` 数据段通常用于存放程序中已初始化且初值不为 0 的全局变量和静态变量。数据段属于静态内存分配(静态存储区)，可读可写。
* `.bss段：` 未初始化以及初始为 0 的全局变量和静态变量，操作系统会将这些未初始化变量初始化为 0。
* `堆(heap)：` 用于存放进程运行时动态分配的内存。
  * 堆中内容是匿名的，**不能按名字直接访问，只能通过指针间接访问。**
  * 堆向高地址扩展(即“向上生长”)，是不连续的内存区域。这是由于系统用链表来存储空闲内存地址，自然不连续，而链表从低地址向高地址遍历。
* `内存映射区(mmap)：` 作为内存映射区加载磁盘文件，或者加载程序运作过程中需要调用的动态库。
* `栈(stack)：` 存储函数内部声明的非静态局部变量，函数参数，函数返回地址等信息，栈内存由编译器自动分配释放。**栈和堆相反地址“向下生长”**，分配的内存是连续的。
* `命令行参数：` 存储进程执行的时候传递给`main()`函数的参数，argc，argv[]
* `环境变量：`  存储和进程相关的环境变量, 比如: 工作路径, 进程所有者等信息



# 2. 文件描述符



## 2.1 文件描述符

在Linux操作系统中的一切都被抽象成了文件，那么一个打开的文件是如何与应用程序进行对应呢？

解决方案是使用`文件描述符（file descriptor，简称fd），当在进程中打开一个现有文件或者创建一个新文件时，内核向该进程返回一个文件描述符，用于对应这个打开/新建的文件。`这些文件描述符都存储在内核为每个进程维护的一个文件描述符表中。

在程序设计中，一些涉及底层的程序编写往往会围绕着文件描述符展开。但是文件描述符这一概念往往只适用于UNIX、Linux这样的操作系统。

> 在Linux系统中一切皆文件，系统中一切都被抽象成了文件。对这些文件的读写都需要通过文件描述符来完成。标准 C 库的文件 IO 函数使用的文件指针 `FILE*` 在 Linux 中也需要通过文件描述符的辅助才能完成读写操作。`FILE `其实是一个结构体，其内部有一个成员就是文件描述符（下面结构体的第25行）。

**FILE 结构体在 Linux 头文件中的定义**

```C
// linux c FILE结构体定义： /usr/include/libio.h
struct _IO_FILE {
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags
 
  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  /* The following fields are used to support backing up and undo. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */
 
  struct _IO_marker *_markers;
 
  struct _IO_FILE *_chain;
 
  int _fileno;			// 文件描述符
#if 0
  int _blksize;
#else
  int _flags2;
#endif
  _IO_off_t _old_offset; /* This used to be _offset but it's too small.  */
 
#define __HAVE_COLUMN /* temporary */
  /* 1+column number of pbase(); 0 is unknown. */
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
 
  /*  char* _save_gptr;  char* _save_egptr; */
 
  _IO_lock_t *_lock;
#ifdef _IO_USE_OLD_IO_FILE
};
```



## 2.2 文件描述符表

前面讲到启动一个进程就会得到一个对应的虚拟地址空间，这个虚拟地址空间分为两大部分，在内核区有专门用于进程管理的模块。Linux 的进程控制块 PCB（process control block）本质是一个叫做 `task_struct` 的结构体，里边包括管理进程所需的各种信息，其中有一个结构体叫做 `file ` ，我们将它叫做文件描述符表，里边有一个整形索引表,用于存储文件描述符。

内核为每一个进程维护了一个文件描述符表，索引表中的值都是从 0 开始的，所以在不同的进程中你会看到相同的文件描述符，但是它们指向的不一定是同一个磁盘文件。



![image-20230911220039299](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112200346.png)



> Linux 中用户操作的每个终端都被视作一个设备文件, 当前操作的终端文件可以使用 `/dev/tty` 表示。



* **打开的最大文件数**

  每一个进程对应的文件描述符表能够存储的打开的文件数是有限制的, 默认为 1024 个，这个默认值是可以修改的，支持打开的最大文件数据取决于操作系统的硬件配置。

* **默认分配的文件描述符**

  当一个进程被启动之后，内核 PCB 的文件描述符表中就已经分配了三个文件描述符，这三个文件描述符对应的都是当前启动这个进程的终端文件（Linux 中一切皆文件，终端就是一个设备文件，在 /dev 目录中）

  * `STDIN_FILENO`：标准输入，可以通过这个文件描述符将数据输入到终端文件中，宏值为 0。
  * `STDOUT_FILENO`：标准输出，可以通过这个文件描述符将数据通过终端输出出来，宏值为 1。
  * `STDERR_FILENO`：标准错误，可以通过这个文件描述符将错误信息通过终端输出出来，宏值为 2。

  这三个默认分配的文件描述符是可以通过 `close()` 函数关闭掉，但是关闭之后当前进程也就不能和当前终端进行输入或者输出的信息交互了。

* **给新打开的文件分配文件描述符**

  * 因为进程启动之后，文件描述符表中的`0`,`1`,`2`就被分配出去了，因此从`3`开始分配
  * 在进程中每打开一个文件，就会给这个文件分配一个新的文件描述符，比如：
    * 通过 `open()` 函数打开  `/hello.txt`，文件描述符 3 被分配给了这个文件，保持这个打开状态，再次通过 `open()` 函数打开 `/hello.txt`，文件描述符 4 被分配给了这个文件，也就是说一个进程中不同的文件描述符打开的磁盘文件可能是同一个。
    * 通过 `open()` 函数打开 `/hello.txt`，文件描述符 3 被分配给了这个文件，将打开的文件关闭，此时文件描述符3就被释放了。再次通过 `open()` 函数打开 `/hello.txt`，文件描述符 3 被分配给了这个文件，也就是说打开的**新文件会关联文件描述符表中最小的没有被占用的文件描述符。**

> 总结:
>
> 1. 每个进程对应的文件描述符表默认支持打开的最大文件数为 1024，可以修改
> 2. 每个进程的文件描述符表中都已经默认分配了三个文件描述符，对应的都是当前终端文件（/dev/tty）
> 3. 每打开新的文件，内核会从进程的文件描述符表中找到一个空闲的没有别占用的文件描述符与其进行关联
> 4. 文件描述符表中不同的文件描述符可以对应同一个磁盘文件
> 5. 每个进程文件描述符表中的文件描述符值是唯一的，不会重复



# 3. open/close

每个系统都有自己的专属函数，我们习惯称其为系统函数。系统函数并不是内核函数，因为内核函数是不允许用户使用的，系统函数就充当了二者之间的桥梁，这样用户就可以间接的完成某些内核操作了。

在前面介绍了文件描述符，在 Linux 系统中必须要使用系统提供的IO函数才能基于这些文件描述符完成对相关文件的读写操作。这些 Linux 系统 IO 函数和标准 C 库的 IO 函数使用方法类似，函数名称也类似。



## 3.1 函数原型

> 通过 open 函数我们即可打开一个磁盘文件，如果磁盘文件不存在还可以创建一个新的的文件，函数原型如下：

```C
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
open是一个系统函数, 只能在linux系统中使用, windows不支持
fopen 是标准c库函数, 一般都可以跨平台使用, 可以这样理解:
		- 在linux中 fopen底层封装了Linux的系统API open
		- 在window中, fopen底层封装的是 window 的 api
*/
// 打开一个已经存在的磁盘文件
int open(const char *pathname, int flags);
// 打开磁盘文件, 如果文件不存在, 就会自动创建
int open(const char *pathname, int flags, mode_t mode);
```



* 参数介绍：

  * pathname: 被打开的文件的文件名

  * flags: 使用什么方式打开指定的文件，这个参数对应一些宏值，需要根据实际需求指定

    * `必须要指定的属性：` 以下三个属性不能同时使用, 只能任选其一
      * `O_RDONLY`: 以只读方式打开文件
      * `O_WRONLY:` 以只写方式打开文件
      * `O_RDWR`: 以读写方式打开文件
      * `可选属性,` 和上边的属性一起使用
        * `O_APPEND`: 新数据追加到文件尾部, 不会覆盖文件的原来内容
        * `O_CREAT`: 如果文件不存在, 创建该文件, 如果文件存在什么也不做
        * `O_EXCL`: 检测文件是否存在, 必须要和 O_CREAT 一起使用, 不能单独使用: `O_CREAT | O_EXCL`
          * 检测到文件不存在, 创建新文件
          * 检测到文件已经存在, 创建失败, 函数直接返回-1（如果不添加这个属性，不会返回-1）

  * mode: 在创建新文件的时候才需要指定这个参数的值，用于指定新文件的权限，这是一个八进制的整数

    * 这个参数的最大值为：0777

    * 创建的新文件对应的最终实际权限, 计算公式: (`mode & ~umask`)

      * umask 掩码可以通过 umask 命令查看

        ```shell
        $ umask
        0002
        ```

      * 假设 mode 参数的值为 0777, 通过计算得到的文件权限为 0775

        ```shell
        # umask(文件掩码):  002(八进制)  = 000000010 (二进制)  
        # ~umask(掩码取反): ~000000010 (二进制) = 111111101 (二进制)  
        # 参数mode指定的权限为: 0777(八进制) = 111111111(二进制)
        # 计算公式: mode & ~umask
                     111111111
               &     111111101
                    ------------------
                     111111101    二进制
                    ------------------
                     mod=0775     八进制  
        ```

* 返回值：
  * 成功:  返回内核分配的文件描述符, 这个值被记录在内核的文件描述符表中，这是一个大于0的整数
  * 失败: -1



## 3.2 close 函数原型

> 通过 open 函数可以让内核给文件分配一个文件描述符,  如果需要释放这个文件描述符就需要关闭文件。对应的这个系统函数叫做 close，函数原型如下：

```C
#include <unistd.h>
int close(int fd);
```

* 函数参数: fd 是文件描述符, 是 open() 函数的返回值
* 函数返回值: 函数调用成功返回值 0, 调用失败返回 -1



## 3.3 打开已存在文件

> 我们可以使用 `open()` 函数打开一个本地已经存在的文件, 假设我们想要读写这个文件, 操作代码如下:

```C
// open.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 打开文件
    int fd = open("abc.txt", O_RDWR);
    if(fd == -1)
    {
        printf("打开文件失败\n");
    }
    else
    {
        printf("fd: %d\n", fd);
    }

    close(fd);
    return 0;
}
```

编译并执行程序

```shell
$ gcc open.c 
$ ./a.out 
fd: 3		# 打开的文件对应的文件描述符值为 3
```



## 3.4 创建新文件

> 如果要创建一个新的文件，还是使用 open 函数，只不过需要添加 `O_CREAT` 属性, 并且给新文件指定操作权限。

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 创建新文件
    int fd = open("./new.txt", O_CREAT|O_RDWR, 0664);
    if(fd == -1)
    {
        printf("打开文件失败\n");
    }
    else
    {
        printf("创建新文件成功, fd: %d\n", fd);
    }

    close(fd);
    return 0;
}
```

```shell
$ gcc open1.c 
$ ./a.out 
创建新文件成功, fd: 3
```

假设在创建新文件的时候,  给 open 指定第三个参数指定新文件的操作权限,  文件也是会被创建出来的,  只不过新的文件的权限可能会有点奇怪,  这个权限会随机分配而且还会出现一些特殊的权限位, 如下:

```shell
$ $ ll new.txt 
-r-x--s--T 1 robin robin 0 Jan 30 16:17 new.txt*   # T 就是一个特殊权限
```



## 3.5 文件状态判断

> 在创建新文件的时候我们还可以通过 `O_EXCL` 进行文件的检测, 具体处理方式如下:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 创建新文件之前, 先检测是否存在
    // 文件存在创建失败, 返回-1, 文件不存在创建成功, 返回分配的文件描述符
    int fd = open("./new.txt", O_CREAT|O_EXCL|O_RDWR);
    if(fd == -1)
    {
        printf("创建文件失败, 已经存在了, fd: %d\n", fd);
    }
    else
    {
        printf("创建新文件成功, fd: %d\n", fd);
    }

    close(fd);
    return 0;
}
```

编译并执行程序：

```shell
$ gcc open1.c 
$ ./a.out 
创建文件失败, 已经存在了, fd: -1
```



# 4. read/write



## 4.1 read

>read 函数用于读取文件内部数据，在通过 open 打开文件的时候需要指定读权限，函数原型如下：

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```



* 参数：
  * fd: 文件描述符, open() 函数的返回值, 通过这个参数定位打开的磁盘文件
  * buf: 是一个传出参数, 指向一块有效的内存, 用于存储从文件中读出的数据
    * 传出参数: 类似于返回值,  将变量地址传递给函数,  函数调用完毕,  地址中就有数据了
  * count: buf 指针指向的内存的大小,  指定可以存储的最大字节数
* 返回值：
  * 大于0: 从文件中读出的字节数，读文件成功
  * 等于0: 代表文件读完了，读文件成功
  * -1: 读文件失败了



## 4.2 write

> write 函数用于将数据写入到文件内部，在通过 open 打开文件的时候需要指定写权限，函数原型如下：

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```



* 参数：
  * fd: 文件描述符, open() 函数的返回值, 通过这个参数定位打开的磁盘文件
  * buf: 指向一块有效的内存地址, 里边有要写入到磁盘文件中的数据
    * count: 要往磁盘文件中写入的字节数, 一般情况下就是buf字符串的长度, strlen(buf)
* 返回值：
  * 大于0: 成功写入到磁盘文件中的字节数
  * -1: 写文件失败了



## 4.3 文件拷贝

> 假设有一个比较大的磁盘文件,  打开这个文件得到文件描述符 fd1，然后在创建一个新的磁盘文件得到文件描述符 fd2,  在程序中通过 fd1 将文件内容读出，并通过 fd2 将读出的数据写入到新文件中。

```c
// 文件的拷贝
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 打开存在的文件english.txt, 读这个文件
    int fd1 = open("./english.txt", O_RDONLY);
    if(fd1 == -1)
    {
        perror("open-readfile");
        return -1;
    }

    // 2. 打开不存在的文件, 将其创建出来, 将从english.txt读出的内容写入这个文件中
    int fd2 = open("copy.txt", O_WRONLY|O_CREAT, 0664);
    if(fd2 == -1)
    {
        perror("open-writefile");
        return -1;
    }

    // 3. 循环读文件, 循环写文件
    char buf[4096];
    int len = -1;
    while( (len = read(fd1, buf, sizeof(buf))) > 0 )
    {
        // 将读到的数据写入到另一个文件中
        write(fd2, buf, len); 
    }
    // 4. 关闭文件
    close(fd1);
    close(fd2);

    return 0;
}
```



# 5. Iseek

> 系统函数 `lseek`  的功能是比较强大的,  我们**既可以通过这个函数移动文件指针,**  也可以通过这个函数**进行文件的拓展**。这个函数的原型如下:

```C
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```



* 参数：
  * fd: 文件描述符, open() 函数的返回值, 通过这个参数定位打开的磁盘文件
  * offset: 偏移量，需要和第三个参数配合使用
  * whence: 通过这个参数指定函数实现什么样的功能
    * SEEK_SET: 从文件头部开始偏移 offset 个字节
    * SEEK_CUR: 从当前文件指针的位置向后偏移offset个字节
    * SEEK_END: 从文件尾部向后偏移offset个字节
* 返回值：
  * 成功: 文件指针从头部开始计算总的偏移量
  * 失败: -1



## 5.1 移动文件指针

> 通过对 lseek 函数第三个参数的设置, 经常使用该函数实现如下几个功能， 如下所示：

* 文件指针移动到文件头部

  ```C
  lseek(fd, 0, SEEK_SET);
  ```

* 得到当前文件指针的位置

  ```C
  lseek(fd, 0, SEEK_CUR); 
  ```

* 得到文件总大小

  ```C
  lseek(fd, 0, SEEK_END);
  ```



## 5.2 文件扩展

假设使用一个下载软件进行一个大文件下载，但是磁盘很紧张，如果不能马上将文件下载到本地，磁盘空间就可能被其他文件占用了，导致下载软件下载的文件无处存放。那么这个文件怎么解决呢？

我们可以在开始下载的时候先进行文件拓展，将一些字符写入到目标文件中，让拓展的文件和即将被下载的文件一样大，这样磁盘空间就被成功抢到手，软件就可以慢悠悠的下载对应的文件了。

> 使用 lseek 函数进行文件拓展必须要满足一下条件：
>
> 1. 文件指针必须要偏移到文件尾部之后， 多出来的就需要被填充的部分。
> 2. 文件拓展之后，必须要使用 `write()` 函数进行一次写操作（写什么都可以，没有字节数要求）。



```C
// lseek.c
// 拓展文件大小
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("hello.txt", O_RDWR);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }

    // 文件拓展, 一共增加了 1001 个字节
    lseek(fd, 1000, SEEK_END);
    write(fd, " ", 1);
        
    close(fd);
    return 0;
}
```

```shell
# 编译程序 
$ gcc lseek.c

# 查看目录文件信息
$ ll
-rwxrwxr-x 1 robin robin 8808 May  6  2019 a.out*
-rwxrwxr-x 1 robin robin 1013 May  6  2019 hello.txt*
-rw-rw-r-- 1 robin robin  299 May  6  2019 lseek.c

# 执行程序, 拓展文件
$ ./a.out 

# 在查看目录文件信息
$ ll
-rwxrwxr-x 1 robin robin 8808 May  6  2019 a.out*
-rwxrwxr-x 1 robin robin 2014 Jan 30 17:39 hello.txt*   # 大小从 1013 -> 2014, 拓展了1001字节
-rw-rw-r-- 1 robin robin  299 May  6  2019 lseek.c
```



# 6. truncate/ftruncate

truncate/ftruncate 这两个函数的功能是一样的，可以对文件进行拓展也可以截断文件。使用这两个函数拓展文件比使用 lseek 要简单。这两个函数的函数原型如下：

```C
// 拓展文件或截断文件
#include <unistd.h>
#include <sys/types.h>

int truncate(const char *path, off_t length);
	- 
int ftruncate(int fd, off_t length);
```



* 参数：
  * path: 要拓展/截断的文件的文件名
  * fd: 文件描述符, open() 得到的
  * length: 文件的最终大小
    * 文件原来size > length，文件被截断, 尾部多余的部分被删除, 文件最终长度为length
    * 文件原来size < length，文件被拓展, 文件最终长度为length
* 返回值：成功返回 0; 失败返回值 -1



> truncate() 和 ftruncate() 两个函数的区别在于一个使用文件名一个使用文件描述符操作文件, 功能相同。
>
> 不管是使用这两个函数还是使用 lseek() 函数拓展文件，文件尾部填充的字符都是 0。
>



# 7. perror

在查看 Linux 系统函数的时候,  我们可以发现一个规律:  大部分系统函数的返回值都是整形，并且通过这个返回值来描述系统函数的状态（调用是否成功了）。在 man 文档中关于系统函数的返回值大部分时候都是这样描述的：

```mak
RETURN VALUE
       On  success,  zero is returned.  On error, -1 is returned, and errno is set
       appropriately.
       
       如果成功，则返回0。出现错误时，返回-1，并给errno设置一个适当的值。
```

> `errno` 是一个全局变量，只要调用的 Linux 系统函数有异常（返回-1）, 错误对应的错误号就会被设置给这个全局变量。这个错误号存储在系统的两个头文件中：
>
> 1. /usr/include/asm-generic/errno-base.h
> 2. /usr/include/asm-generic/errno.h

得到错误号，去查询对应的头文件是非常不方便的，我们可以通过 perror 函数将错误号对应的描述信息打印出来：

```C
#include <stdio.h>
// 参数, 自己指定这个字符串的值就可以, 指定什么就会原样输出, 除此之外还会输出错误号对应的描述信息
void perror(const char *s);	
```

举例：使用 perror 打印错误信息

```C
// open.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("hello.txt", O_RDWR|O_EXCL|O_CREAT, 0777);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }
        
    close(fd);
    return 0;
}
```

```shell
$ gcc open.c
$ $ ./a.out 
open: File exists	# 通过 perror 输出的错误信息
```



# 8. 错误号



## 8.1 part 1

信息来自头文件: /usr/include/asm-generic/errno-base.h

```C
#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */
```



## 8.2 part 2

信息来自头文件: /usr/include/asm-generic/errno.h

```C
#define EDEADLK         35      /* Resource deadlock would occur */
#define ENAMETOOLONG    36      /* File name too long */
#define ENOLCK          37      /* No record locks available */

/*
 * This error code is special: arch syscall entry code will return
 * -ENOSYS if users try to call a syscall that doesn't exist.  To keep
 * failures of syscalls that really do exist distinguishable from
 * failures due to attempts to use a nonexistent syscall, syscall
 * implementations should refrain from returning -ENOSYS.
 */
#define ENOSYS          38      /* Invalid system call number */

#define ENOTEMPTY       39      /* Directory not empty */
#define ELOOP           40      /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          42      /* No message of desired type */
#define EIDRM           43      /* Identifier removed */
#define ECHRNG          44      /* Channel number out of range */
#define EL2NSYNC        45      /* Level 2 not synchronized */
#define EL3HLT          46      /* Level 3 halted */
#define EL3RST          47      /* Level 3 reset */
#define ELNRNG          48      /* Link number out of range */
#define EUNATCH         49      /* Protocol driver not attached */
#define ENOCSI          50      /* No CSI structure available */
#define EL2HLT          51      /* Level 2 halted */
#define EBADE           52      /* Invalid exchange */
#define EBADR           53      /* Invalid request descriptor */
#define EXFULL          54      /* Exchange full */
#define ENOANO          55      /* No anode */
#define EBADRQC         56      /* Invalid request code */
#define EBADSLT         57      /* Invalid slot */

#define EDEADLOCK       EDEADLK

#define EBFONT          59      /* Bad font file format */
#define ENOSTR          60      /* Device not a stream */
#define ENODATA         61      /* No data available */
#define ETIME           62      /* Timer expired */
#define ENOSR           63      /* Out of streams resources */
#define ENONET          64      /* Machine is not on the network */
#define ENOPKG          65      /* Package not installed */
#define EREMOTE         66      /* Object is remote */
#define ENOLINK         67      /* Link has been severed */
#define EADV            68      /* Advertise error */
#define ESRMNT          69      /* Srmount error */
#define ECOMM           70      /* Communication error on send */
#define EPROTO          71      /* Protocol error */
#define EMULTIHOP       72      /* Multihop attempted */
#define EDOTDOT         73      /* RFS specific error */
#define EBADMSG         74      /* Not a data message */
#define EOVERFLOW       75      /* Value too large for defined data type */
#define ENOTUNIQ        76      /* Name not unique on network */
#define EBADFD          77      /* File descriptor in bad state */
#define EREMCHG         78      /* Remote address changed */
#define ELIBACC         79      /* Can not access a needed shared library */
#define ELIBBAD         80      /* Accessing a corrupted shared library */
#define ELIBSCN         81      /* .lib section in a.out corrupted */
#define ELIBMAX         82      /* Attempting to link in too many shared libraries */
#define ELIBEXEC        83      /* Cannot exec a shared library directly */
#define EILSEQ          84      /* Illegal byte sequence */
#define ERESTART        85      /* Interrupted system call should be restarted */
#define ESTRPIPE        86      /* Streams pipe error */
#define EUSERS          87      /* Too many users */
#define ENOTSOCK        88      /* Socket operation on non-socket */
#define EDESTADDRREQ    89      /* Destination address required */
#define EMSGSIZE        90      /* Message too long */
#define EPROTOTYPE      91      /* Protocol wrong type for socket */
#define ENOPROTOOPT     92      /* Protocol not available */
#define EPROTONOSUPPORT 93      /* Protocol not supported */
#define ESOCKTNOSUPPORT 94      /* Socket type not supported */
#define EOPNOTSUPP      95      /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96      /* Protocol family not supported */
#define EAFNOSUPPORT    97      /* Address family not supported by protocol */
#define EADDRINUSE      98      /* Address already in use */
#define EADDRNOTAVAIL   99      /* Cannot assign requested address */
#define ENETDOWN        100     /* Network is down */
#define ENETUNREACH     101     /* Network is unreachable */
#define ENETRESET       102     /* Network dropped connection because of reset */
#define ECONNABORTED    103     /* Software caused connection abort */
#define ECONNRESET      104     /* Connection reset by peer */
#define ENOBUFS         105     /* No buffer space available */
#define EISCONN         106     /* Transport endpoint is already connected */
#define ENOTCONN        107     /* Transport endpoint is not connected */
#define ESHUTDOWN       108     /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109     /* Too many references: cannot splice */
#define ETIMEDOUT       110     /* Connection timed out */
#define ECONNREFUSED    111     /* Connection refused */
#define EHOSTDOWN       112     /* Host is down */
#define EHOSTUNREACH    113     /* No route to host */
#define EALREADY        114     /* Operation already in progress */
#define EINPROGRESS     115     /* Operation now in progress */
#define ESTALE          116     /* Stale file handle */
#define EUCLEAN         117     /* Structure needs cleaning */
#define ENOTNAM         118     /* Not a XENIX named type file */
#define ENAVAIL         119     /* No XENIX semaphores available */
#define EISNAM          120     /* Is a named type file */
#define EREMOTEIO       121     /* Remote I/O error */
#define EDQUOT          122     /* Quota exceeded */

#define ENOMEDIUM       123     /* No medium found */
#define EMEDIUMTYPE     124     /* Wrong medium type */
#define ECANCELED       125     /* Operation Canceled */
#define ENOKEY          126     /* Required key not available */
#define EKEYEXPIRED     127     /* Key has expired */
#define EKEYREVOKED     128     /* Key has been revoked */
#define EKEYREJECTED    129     /* Key was rejected by service */

/* for robust mutexes */
#define EOWNERDEAD      130     /* Owner died */
#define ENOTRECOVERABLE 131     /* State not recoverable */

#define ERFKILL         132     /* Operation not possible due to RF-kill */

#define EHWPOISON       133     /* Memory page has hardware error */
```



# 9. file 命令



Linux 是一个基于文件的操作系统， 因此作为文件本身也就有很多属性，如果想要查看某一个文件的属性有两种方式：`命令`和 `函数`。虽然有两种方式但是它们对应的名字是相同的，叫做 `stat`。另外使用 `file` 命令也可以查看文件的一些属性信息。

> 该命令用来识别文件类型，也可用来辨别一些文件的编码格式。它是通过查看文件的头部信息来获取文件类型，而不是像 Windows 通过扩展名来确定文件类型的。

```shell
# 参数在命令中的位置没有限制
$ file 文件名 [参数] 
```

file 命令的参数是可选项,  可以不加,  常用的参数如下表:

| 参数 | 功能                                   |
| ---- | -------------------------------------- |
| -b   | 只显示文件类型和文件编码, 不显示文件名 |
| -i   | 显示文件的 MIME 类型                   |
| -F   | 设置输出字符串的分隔符                 |
| -L   | 查看软连接文件自身文件属性             |



## 9.1 查看文件类型和编码格式

使用不带任何选项的 file 命令，即可查看指定文件的类型和文件编码信息。

```shell
# 空文件
$ file 11.txt 
11.txt: empty

# 源文件, 编码格式为: ASCII
$ file b.cpp
b.cpp: C source, ASCII text

# 源文件, 编码格式为: UTF-8 
robin@OS:~$ file test.cpp 
test.cpp: C source, UTF-8 Unicode (with BOM) text, with CRLF line terminators

# 可执行程序, Linux中的可执行程序为 ELF 格式
robin@OS:~$ file a.out 
a.out: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/l, for GNU/Linux 2.6.32, BuildID[sha1]=5317ae9fba592bf583c4f680d8cc48a8b58c96a5, not stripped
```



## 9.2 只显示文件格式以及编码

使用  `-b` 参数，可以使 file 命令的输出不出现文件名，只显示文件格式以及编码。

```shell
# 空文件
$ file 11.txt -b
empty

# 源文件, 编码格式为: ASCII
$ file b.cpp -b
C source, ASCII text

# 源文件, 编码格式为: UTF-8 
robin@OS:~$ file test.cpp  -b
C source, UTF-8 Unicode (with BOM) text, with CRLF line terminators

# 可执行程序, Linux中的可执行程序为 ELF 格式
robin@OS:~$ file a.out  -b
ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/l, for GNU/Linux 2.6.32, BuildID[sha1]=5317ae9fba592bf583c4f680d8cc48a8b58c96a5, not stripped
```



## 9.3 显示文件的 MIME 类型

给 file 命令添加 `-i` 参数，可以输出文件对应的 MIME 类型的字符串。

> `MIME(Multipurpose Internet Mail Extensions) 多用途互联网邮件扩展类型`。是设定某种扩展名的文件用一种应用程序来打开的方式类型，当该扩展名文件被访问的时候，`浏览器会自动使用指定应用程序来打开`。多用于指定一些客户端自定义的文件名，以及一些媒体文件打开方式。

```shell
# charset 为该文件的字符编码

# 源文件, MIME类型: text/x-c, 字符编码: utf-8
$ file occi.cpp -i
occi.cpp: text/x-c; charset=utf-8

# 压缩文件, MIME类型: application/gzip, 字符编码: binary
$ file fcgi.tar.gz -i
fcgi.tar.gz: application/gzip; charset=binary

# 文本文件, MIME类型: text/plain, 字符编码: utf-8
$ file english.txt -i
english.txt: text/plain; charset=utf-8

# html文件, MIME类型: text/html, 字符编码: us-ascii
$ file demo.html -i
demo.html: text/html; charset=us-ascii
```



## 9.4 设置输出分隔符

> 在 file 命令中，文件名和后边的属性信息默认使用冒号（:）分隔，我们可以通过 `-F ` 参数修改分隔符，分隔符可以是单字符也可以是一个字符串，如果分隔符是字符串需要将这个参数值写到引号中（单/双引号都可以）。
>

```shell
# 默认格式输出
$ file english.txt 
english.txt: UTF-8 Unicode text, with very long lines, with CRLF line terminators

# 修改分隔符为字符串 “==>"
$ file english.txt -F "==>"
english.txt==> UTF-8 Unicode text, with very long lines, with CRLF line terminators

$ file english.txt -F '==>'
english.txt==> UTF-8 Unicode text, with very long lines, with CRLF line terminators

# 修改分隔符为单字符 '='
$ file english.txt -F = 
english.txt= UTF-8 Unicode text, with very long lines, with CRLF line terminators
```



## 9.5 查看软连接文件

> 软连接文件是一个特殊格式的文件,  查看这种格式的文件可以得到两种结果: 第一种是软连接文件本身的属性信息,  另一种是链接文件指向的那个文件的属性信息。
>
> 直接通过 file 查看文件属性得到的是链接文件指向的文件的信息，如果添加参数 `-L` 得到的链接文件自身的属性信息。
>

```shell
# 使用 ls 查看链接文件属性信息
$ ll link.lnk 
lrwxrwxrwx 1 root root 24 Jan 25 17:27 link.lnk -> /root/luffy/onepiece.txt

# 使用file直接查看链接文件信息: 得到的是链接文件指向的那个文件的名字
$ file link.lnk 
link.lnk: symbolic link to `/root/luffy/onepiece.txt'

# 使用 file 查看链接文件自身属性信息, 添加参数 -L
$ file link.lnk -L
link.lnk: UTF-8 Unicode text
```



# 10. stat 命令

stat 命令显示文件或目录的详细属性信息包括文件系统状态，比 ls 命令输出的信息更详细。语法格式如下:

```shell
# 参数在命令中的位置没有限制
$ stat [参数] 文件或者目录名
```

关于这个命令的可选参数如下表:

| 参数 | 功能                                             |
| ---- | ------------------------------------------------ |
| -f   | 不显示文件本身的信息，显示文件所在文件系统的信息 |
| -L   | 查看软链接文件关联的文件的属性信息               |
| -c   | 查看文件某个单个的属性信息                       |
| -t   | 简洁模式，只显示摘要信息, 不显示属性描述         |



## 10.1 显示所有属性

```shell
$ stat english.txt 
  File: 'english.txt'
  Size: 129567          Blocks: 256        IO Block: 4096   regular file
Device: 801h/2049d      Inode: 526273      Links: 1
Access: (0644/-rw-r--r--)  Uid: ( 1001/   robin)   Gid: ( 1001/   robin)
Access: 2021-01-31 00:00:36.791490304 +0800
Modify: 2021-01-31 00:00:36.791490304 +0800
Change: 2021-01-31 00:00:36.791490304 +0800
 Birth: -
```

在输出的信息中我们可以看到很多属性：



![image-20230911230816596](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112308689.png)



## 10.2 只显示系统信息

给 stat 命令添加 `-f`参数将只显示文件在文件系统中的相关属性信息,  文件自身属性不显示

```shell
$ stat luffy/ -f
  File: "luffy/"
    ID: 47d795d8889d00d3 Namelen: 255     Type: ext2/ext3
Block size: 4096       Fundamental block size: 4096
Blocks: Total: 10288179   Free: 8991208    Available: 8546752
Inodes: Total: 2621440    Free: 2515927
```



## 10.3 软连接文件

使用 stat 查看软链接类型的文件,  默认显示的是这个软链接文件的属性信息,  添加参数  `-L`就可以查看这个软连接文件关联的文件的属性信息了。

```shell
# 查看软件文件属性 -> 使用 ls -l
ls -l link.lnk 
lrwxrwxrwx 1 root root 24 Jan 25 17:27 link.lnk -> /root/luffy/onepiece.txt

# 使用 stat 查看软连接文件属性信息
$ stat link.lnk 
  File: ‘link.lnk’ -> ‘/root/luffy/onepiece.txt’
  Size: 24              Blocks: 0          IO Block: 4096   symbolic link
Device: fd01h/64769d    Inode: 393832      Links: 1
Access: (0777/lrwxrwxrwx)  Uid: (    0/    root)   Gid: (    0/    root)
Access: 2021-01-30 23:46:29.922760178 +0800
Modify: 2021-01-25 17:27:12.057386837 +0800
Change: 2021-01-25 17:27:12.057386837 +0800
 Birth: -

# 使用 stat 查看软连接文件关联的文件的属性信息
$ stat link.lnk -L
  File: ‘link.lnk’
  Size: 3700              Blocks: 8          IO Block: 4096   regular file
Device: fd01h/64769d    Inode: 660353      Links: 2
Access: (0444/-r--r--r--)  Uid: (    0/    root)   Gid: (    0/    root)
Access: 2021-01-30 23:46:53.696723182 +0800
Modify: 2021-01-25 17:54:47.000000000 +0800
Change: 2021-01-26 11:57:00.587658977 +0800
 Birth: -
```



## 10.4 简介输出

使用 stat 进行简洁信息输出的可读性不是太好, 所有的属性描述都别忽略了, 如果只想得到属性值, 可以给该命令添加 `-t` 参数:

```shell
$ stat luffy/ -t
luffy/ 4096 8 41fd 1001 1001 801 662325 8 0 0 1611659086 1580893020 1580893020 0 4096
```



## 10.5 单个属性输出

如果每次只想通过 stat 命令得到某一个文件属性, 可以给名添加 `-c`参数。 不同的文件属性分别对应一些定义好的特殊符号，想得到哪个属性值，将其指定到参数 `-c `后边即可。属性对应的字符如下表：

| 格式化字符 | 功能                                                         |
| ---------- | ------------------------------------------------------------ |
| %a         | 文件的八进制访问权限（`#` 和 `0` 是输出标准）                |
| %A         | 人类可读形式的文件访问权限（ `rwx` ）                        |
| %b         | 已分配的块数量                                               |
| %B         | 报告的每个块的大小(以字节为单位)                             |
| %C         | SELinux 安全上下文字符串                                     |
| %d         | 设备编号 （十进制）                                          |
| %D         | 设备编号 （十六进制）                                        |
| %F         | 文件类型                                                     |
| %g         | 文件所属组组ID                                               |
| %G         | 文件所属组名字                                               |
| %h         | 用连接计数                                                   |
| %i         | inode编号                                                    |
| %m         | 挂载点                                                       |
| %n         | 文件名                                                       |
| %N         | 用引号括起来的文件名，并且会显示软连接文件引用的文件路径     |
| %o         | 最佳I/O传输大小提示                                          |
| %s         | 文件总大小, 单位为字节                                       |
| %t         | 十六进制的主要设备类型，用于字符/块设备特殊文件              |
| %T         | 十六进制的次要设备类型，用于字符/块设备特殊文件              |
| %u         | 文件所有者ID                                                 |
| %U         | 文件所有者名字                                               |
| %w         | 文件生成的日期 ，人类可识别的时间字符串 – 获取不到信息不显示 |
| %W         | 文件生成的日期 ，自纪元以来的秒数 （参考 %X ）– 获取不到信息不显示 |
| %x         | 最后访问文件的时间, 人类可识别的时间字符串                   |
| %X         | 最后访问文件的时间, 自纪元以来的秒数（从1970.1.1开始到最后一次文件访问的总秒数） |
| %y         | 最后修改文件内容的时间, 人类可识别的时间字符串               |
| %Y         | 最后修改文件内容的时间, 自纪元以来的秒数（参考 %X ）         |
| %z         | 最后修改文件状态的时间, 人类可识别的时间字符串               |
| %Z         | 最后修改文件状态的时间, 自纪元以来的秒数（参考 %X ）         |

文件的每一个属性都有一个或者多个与之对应的格式化字符，这样就可以精确定位所需要的属性信息了，下面举了几个例子，可以作为参考：

```shell
$ stat occi.cpp -c %a
644

$ stat occi.cpp -c %A           
-rw-r--r--

# 使用 ls -l 验证权限
$ ll occi.cpp 
-rw-r--r-- 1 robin robin 1406 Jan 31 00:00 occi.cpp		# 0664

$ stat link.lnk -c %N
'link.lnk' -> '/home/robin/english.txt'

$ stat link.lnk -c %y
2021-01-31 10:48:52.317846411 +0800
```



# 11. stat/lstat 函数

stat/lstat 函数的功能和 stat 命令的功能是一样的, 只不过是应用场景不同。这两个函数的区别在于处理软链接文件的方式上：

* lstat(): 得到的是软连接文件本身的属性信息
* stat(): 得到的是软链接文件关联的文件的属性信息

```C
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *pathname, struct stat *buf);
int lstat(const char *pathname, struct stat *buf);
```



* 参数：
  * pathname: 文件名, 要获取这个文件的属性信息
  * buf: 传出参数, 文件的信息被写入到了这块内存中
* 返回值：函数调用成功返回 0，调用失败返回 -1



> 这个函数的第二个参数是一个结构体类型,  这个结构体相对复杂,  通过这个结构体可以存储得到的文件的所有属性信息,  结构体原型如下:

```C
struct stat {
    dev_t          st_dev;        	// 文件的设备编号
    ino_t           st_ino;        	// inode节点
    mode_t      st_mode;      		// 文件的类型和存取的权限, 16位整形数  -> 常用
    nlink_t        st_nlink;     	// 连到该文件的硬连接数目，刚建立的文件值为1
    uid_t           st_uid;       	// 用户ID
    gid_t           st_gid;       	// 组ID
    dev_t          st_rdev;      	// (设备类型)若此文件为设备文件，则为其设备编号
    off_t            st_size;      	// 文件字节数(文件大小)   --> 常用
    blksize_t     st_blksize;   	// 块大小(文件系统的I/O 缓冲区大小)
    blkcnt_t      st_blocks;    	// block的块数
    time_t         st_atime;     	// 最后一次访问时间
    time_t         st_mtime;     	// 最后一次修改时间(文件内容)
    time_t         st_ctime;     	// 最后一次改变时间(指属性)
};
```



## 11. 1 获取文件大小

下面调用 stat() 函数, 以代码的方式演示一下如何得到某个文件的大小:

```C
#include <sys/stat.h>

int main()
{
    // 1. 定义结构体, 存储文件信息
    struct stat myst;
    // 2. 获取文件属性 english.txt
    int ret = stat("./english.txt", &myst);
    if(ret == -1)
    {
        perror("stat");
        return -1;
    }

    printf("文件大小: %d\n", (int)myst.st_size);

    return 0;
}
```



## 11.2 获取文件类型

文件的类型信息存储在 `struct stat`  结构体的 `st_mode` 成员中,  它是一个 `mode_t` 类型,  本质上是一个 16位的整数。Linux API 中为我们提供了相关的宏函数，通过对应的宏函数可以直接判断出文件是不是某种类型，这些信息都可以通过 man 文档（ `man 2 stat` ）查询到。

```C
// 类型是存储在结构体的这个成员中: mode_t  st_mode;  
// 这些宏函数中的m 对应的就是结构体成员  st_mode
// 宏函数返回值: 是对应的类型返回-> 1, 不是对应类型返回0

S_ISREG(m)  is it a regular file?  
	- 普通文件
S_ISDIR(m)  directory?
	- 目录
S_ISCHR(m)  character device?
	- 字符设备
S_ISBLK(m)  block device?
	- 块设备
S_ISFIFO(m) FIFO (named pipe)?
	- 管道
S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
	- 软连接
S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
    - 本地套接字文件
```

在程序中通过宏函数判断文件类型,  实例代码如下:

```C
int main()
{
    // 1. 定义结构体, 存储文件信息
    struct stat myst;
    // 2. 获取文件属性 english.txt
    int ret = stat("./hello", &myst);
    if(ret == -1)
    {
        perror("stat");
        return -1;
    }

    printf("文件大小: %d\n", (int)myst.st_size);

    // 判断文件类型
    if(S_ISREG(myst.st_mode))
    {
        printf("这个文件是一个普通文件...\n");
    }

    if(S_ISDIR(myst.st_mode))
    {
        printf("这个文件是一个目录...\n");
    }
    if(S_ISLNK(myst.st_mode))
    {
        printf("这个文件是一个软连接文件...\n");
    }

    return 0;
}
```



## 11.3 获取文件权限

用户对文件的操作权限也存储在 `struct stat`  结构体的 `st_mode` 成员中,  在这个 16 位的整数中不同用户的权限存储位置如下图，如果想知道有没有相关权限可以通过按位与(&)操作将这个标志位值取出判断即可。

![image-20230911232444490](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112324551.png)

Linux 中为我们提供了用于不同用户不同权限判定使用的宏，具体信息如下：

```shell
关于变量 st_mode: 
- st_mode -- 16位整数
	○ 0-2 bit -- 其他人权限
		- S_IROTH    00004  读权限   100
		- S_IWOTH    00002  写权限   010
		- S_IXOTH    00001  执行权限  001
		- S_IRWXO    00007  掩码, 过滤 st_mode中除其他人权限以外的信息
	○ 3-5 bit -- 所属组权限
		- S_IRGRP    00040  读权限
		- S_IWGRP    00020  写权限
		- S_IXGRP    00010  执行权限
		- S_IRWXG    00070  掩码, 过滤 st_mode中除所属组权限以外的信息
	○ 6-8 bit -- 文件所有者权限
		- S_IRUSR    00400    读权限
		- S_IWUSR    00200    写权限
		- S_IXUSR    00100    执行权限
		- S_IRWXU    00700    掩码, 过滤 st_mode中除文件所有者权限以外的信息
	○ 12-15 bit -- 文件类型
		- S_IFSOCK   0140000 套接字
		- S_IFLNK    0120000 符号链接（软链接）
		- S_IFREG    0100000 普通文件
		- S_IFBLK    0060000 块设备
		- S_IFDIR    0040000 目录
		- S_IFCHR    0020000 字符设备
		- S_IFIFO    0010000 管道
		- S_IFMT     0170000 掩码,过滤 st_mode中除文件类型以外的信息
			
############### 按位与操作举例 ###############			
    1111 1111 1111 1011   # st_mode
    0000 0000 0000 0100   # S_IROTH
&
----------------------------------------
    0000 0000 0000 0000   # 没有任何权限
    
```

通过仔细阅读上边提供的宏信息, 我们可以知道处理使用它们得到用户对文件的操作权限, 还可以用于判断文件的类型（判断文件类型的第二种方式），具体操作方式可以参考如下代码：

```C
#include <sys/stat.h>

int main()
{
    // 1. 定义结构体, 存储文件信息
    struct stat myst;
    // 2. 获取文件属性 english.txt
    int ret = stat("./hello", &myst);
    if(ret == -1)
    {
        perror("stat");
        return -1;
    }

    printf("文件大小: %d\n", (int)myst.st_size);

    // 判断文件类型
    if(S_ISREG(myst.st_mode))
    {
        printf("这个文件是一个普通文件...\n");
    }

    if(S_ISDIR(myst.st_mode))
    {
        printf("这个文件是一个目录...\n");
    }
    if(S_ISLNK(myst.st_mode))
    {
        printf("这个文件是一个软连接文件...\n");
    }

    // 文件所有者对文件的操作权限
    printf("文件所有者对文件的操作权限: ");
    if(myst.st_mode & S_IRUSR)
    {
        printf("r");
    }
    if(myst.st_mode & S_IWUSR)
    {
        printf("w");
    }
    if(myst.st_mode & S_IXUSR)
    {
        printf("x");
    }
    printf("\n");
    return 0;
}
```



## 11.4 使用 stat/lstat 模拟执行命令 ls -l 效果

```C
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("./a.out filename\n");
        exit(1);
    }

    struct stat st;
    int ret = stat(argv[1], &st);
    if(ret == -1)
    {
        perror("stat");
        exit(1);
    }

    // 存储文件类型和访问权限
    char perms[11] = {0};
    // 判断文件类型
    switch(st.st_mode & S_IFMT)
    {
        case S_IFLNK:
            perms[0] = 'l';
            break;
        case S_IFDIR:
            perms[0] = 'd';
            break;
        case S_IFREG:
            perms[0] = '-';
            break;
        case S_IFBLK:
            perms[0] = 'b';
            break;
        case S_IFCHR:
            perms[0] = 'c';
            break;
        case S_IFSOCK:
            perms[0] = 's';
            break;
        case S_IFIFO:
            perms[0] = 'p';
            break;
        default:
            perms[0] = '?';
            break;
    }
    // 判断文件的访问权限
    // 文件所有者
    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
    // 文件所属组
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
    // 其他人
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    // 硬链接计数
    int linkNum = st.st_nlink;
    // 文件所有者
    char* fileUser = getpwuid(st.st_uid)->pw_name;
    // 文件所属组
    char* fileGrp = getgrgid(st.st_gid)->gr_name;
    // 文件大小
    int fileSize = (int)st.st_size;
    // 修改时间
    char* time = ctime(&st.st_mtime);
    char mtime[512] = {0};
    strncpy(mtime, time, strlen(time)-1);

    char buf[1024];
    sprintf(buf, "%s  %d  %s  %s  %d  %s  %s", 
            perms, linkNum, fileUser, fileGrp, fileSize, mtime, argv[1]);

    printf("%s\n", buf);

    return 0;
}
```



# 12. dup

在 Linux 中只要调用 `open()` 函数就可以给被操作的文件分配一个文件描述符，除了使用这种方式 Linux 系统还提供了一些其他的 API 用于文件描述符的分配，相关函数有三个：`dup, dup2, fcntl`。



## 12.1 函数详解

dup 函数的作用是复制文件描述符，这样就有多个文件描述符可以指向同一个文件了。函数原型如下：

```C
#include <unistd.h>
int dup(int oldfd);
```



* 参数： oldfd 是要被复制的文件描述符
* 返回值：函数调用成功返回被复制出的文件描述符，调用失败返回 -1

下图展示了 `dup()` 函数具体行为,  这样不过使用 `fd1` 还是使用 `fd2` 都可以对磁盘文件 A 进行操作了。

![image-20230911232943094](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112329165.png)

> 被复制出的新文件描述符是独立于旧的文件描述符的，二者没有连带关系。也就是说当旧的文件描述符被关闭了，**复制出的新文件描述符还是可以继续使用的。**



## 12.2 示例代码

下面的代码中演示了通过 `dup()` 函数进行文件描述符复制,  并验证了复制之后两个新、旧文件描述符是独立的，二者没有连带关系。

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 创建一个新的磁盘文件
    int fd = open("./mytest.txt", O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 写数据
    const char* pt = "你好, 世界......";
    // 写成功之后, 文件指针在文件尾部
    write(fd, pt, strlen(pt));


    // 复制这个文件描述符 fd
    int newfd = dup(fd);
    printf("newfd: %d\n", newfd);

    // 关闭旧的文件描述符
    close(fd);

    // 使用新的文件描述符继续写文件
    const char* ppt = "((((((((((((((((((((((骚年，你要相信光！！！))))))))))))))))))))))";
    write(newfd, ppt, strlen(ppt));
    close(newfd);

    return 0;
}
```



# 13. dup2



## 13.1 函数详解

dup2() 函数是 dup() 函数的加强版，基于dup2() 既可以进行文件描述符的复制,  也可以进行文件描述符的重定向。文件描述符重定向就是改变已经分配的文件描述符关联的磁盘文件。

dup2() 函数原型如下：

```C
#include <unistd.h>
// 1. 文件描述符的复制, 和dup是一样的
// 2. 能够重定向文件描述符
// 	- 重定向: 改变文件描述符和文件的关联关系, 和新的文件建立关联关系, 和原来的文件断开关联关系
//		1. 首先通过open()打开文件 a.txt , 得到文件描述符 fd
//		2. 然后通过open()打开文件 b.txt , 得到文件描述符 fd1
//		3. 将fd1从定向 到fd上:
//			fd1和b.txt这磁盘文件断开关联, 关联到a.txt上, 以后fd和fd1都对用同一个磁盘文件 a.txt
int dup2(int oldfd, int newfd);
```



* 参数：`oldfd` 和 `newfd`  都是文件描述符
* 返回值：函数调用成功返回新的文件描述符, 调用失败返回 -1

关于这个函数的两个参数虽然都是文件描述符，但是在使用过程中又对应了不同的场景，具体如下：

* 场景1：

  假设参数 `oldfd` 对应磁盘文件 `a.txt`,  `newfd` 对应磁盘文件 `b.txt` 。在这种情况下调用 `dup2` 函数, 是给 `newfd` 做了重定向，`newfd` 和文件  `b.txt ` 断开关联,  相当于关闭了这个文件,  同时 `newfd ` 指向了磁盘上的 `a.txt` 文件，最终 `oldfd`  和 `newfd`  都指向了磁盘文件 `a.txt` 。

![image-20230911233510535](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112335604.png)

* 场景2：

  假设参数 `oldfd`  对应磁盘文件 `a.txt`, `newfd` 不对应任何的磁盘文件（**newfd 必须是一个大于等于0的整数**）。在这种情况下调用 `dup2` 函数,  在这种情况下会进行文件描述符的复制，``newfd 指向了磁盘上的a.txt文件``，最终 oldfd 和 newfd 都指向了磁盘文件 a.txt。

  ![image-20230911233630635](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112336707.png)

* 场景3：

  假设参数 `oldfd` 和 `newfd` 两个文件描述符对应的是同一个磁盘文件 `a.txt` , 在这种情况下调用 `dup2` 函数,  相当于啥也没发生,  不会有任何改变。

  ![image-20230911233720707](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309112337781.png)



## 13.2 示例代码

> 给 dup2() 的第二个参数指定一个空闲的没被占用的文件描述符就可以进行文件描述符的复制了,  示例代码如下:

```C
// 使用dup2 复制文件描述符
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 创建一个新的磁盘文件
    int fd = open("./111.txt", O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 写数据
    const char* pt = "你好, 世界......";
    // 写成功之后, 文件指针在文件尾部
    write(fd, pt, strlen(pt));


    // 2. fd1没有对应任何的磁盘文件, fd1 必须要 >=0
    int fd1 = 1023;

    // fd -> 111.txt
    // 文件描述符复制, fd1指向fd对应的文件 111.txt
    dup2(fd, fd1);

    // 关闭旧的文件描述符
    close(fd);

    // 使用fd1写文件
    const char* ppt = "((((((((((((((((((((((骚年，你要相信光！！！))))))))))))))))))))))";
    write(fd1, ppt, strlen(ppt));
    close(fd1);

    return 0;
}
```

> 将两个有效的文件描述符分别传递给 dup2() 函数，就可以实现文件描述符的重定向了。`将第二个参数的文件描述符重定向到参数1文件描述符指向的文件上。 `示例代码如下:

```C
// 使用dup2 文件描述符重定向
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 创建一个新的磁盘文件
    int fd = open("./111.txt", O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 写数据
    const char* pt = "你好, 世界......";
    // 写成功之后, 文件指针在文件尾部
    write(fd, pt, strlen(pt));


    // 2. 创建第二个磁盘文件 222.txt
    int fd1 = open("./222.txt", O_RDWR|O_CREAT, 0664);
    if(fd1 == -1)
    {
        perror("open1");
        exit(0);
    }

    // fd -> 111.txt, fd1->222.txt
    // 从定向, 将fd1指向fd对应的文件 111.txt
    dup2(fd, fd1);

    // 关闭旧的文件描述符
    close(fd);

    // 使用fd1写文件
    const char* ppt = "((((((((((((((((((((((骚年，你要相信光！！！))))))))))))))))))))))";
    write(fd1, ppt, strlen(ppt));
    close(fd1);

    return 0;
}
```



# 14. fcntl



## 14.1 函数详解

fcntl() 是一个变参函数,  并且是多功能函数，在这里只介绍如何通过这个函数实现 `文件描述符的复制` 和 `获取/设置已打开的文件属性`。该函数的函数原型如下：

```C
#include <unistd.h>
#include <fcntl.h>	// 主要的头文件

int fcntl(int fd, int cmd, ... /* arg */ );
```



* 参数：

  * fd: 要操作的文件描述符
  * cmd: 通过该参数控制函数要实现什么功能

* 返回值：

  * 参数 cmd = F_DUPFD：返回新的被分配的文件描述符
  * 参数 cmd = F_GETFL：返回文件的flag属性信息


fcntl() 函数的 cmd 可使用的参数列表:

| 参数 cmd 的取值 | 功能描述                     |
| --------------- | ---------------------------- |
| F_DUPFD         | 复制一个已经存在的文件描述符 |
| F_GETFL         | 获取文件的状态标志           |
| F_SETFL         | 设置文件的状态标志           |

文件的状态标志指的是在使用 open() 函数打开文件的时候指定的 flags 属性,  也就是第二个参数

```C
int open(const char *pathname, int flags);
```

下表中列出了一些常用的文件状态标志：

| 文件状态标志 | 说明                     |
| ------------ | ------------------------ |
| O_RDONLY     | 只读打开                 |
| O_WRONLY     | 只写打开                 |
| O_RDWR       | 读、写打开               |
| O_APPEND     | 追加写                   |
| O_NONBLOCK   | 非阻塞模式               |
| O_SYNC       | 等待写完成（数据和属性） |
| O_ASYNC      | 异步I/O                  |
| O_RSYNC      | 同步读和写               |



## 14.2 复制文件描述符

使用 fcntl() 函数进行文件描述符复制,  第二个参数 cmd 需要指定为 `F_DUPFD` （这是个变参函数其他参数不需要指定）。

```C
int newfd = fcntl(fd, F_DUPFD);
```

> 使用 fcntl() 复制文件描述符, 函数返回值为新分配的文件描述符，示例代码如下:

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 创建一个新的磁盘文件
    int fd = open("./mytest.txt", O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 写数据
    const char* pt = "你好, 世界......";
    // 写成功之后, 文件指针在文件尾部
    write(fd, pt, strlen(pt));


    // 复制这个文件描述符 fd
    int newfd = fcntl(fd, F_DUPFD);
    printf("newfd: %d\n", newfd);

    // 关闭旧的文件描述符
    close(fd);

    // 使用新的文件描述符继续写文件
    const char* ppt = "((((((((((((((((((((((骚年，你要相信光！！！))))))))))))))))))))))";
    write(newfd, ppt, strlen(ppt));
    close(newfd);

    return 0;
}
```



## 14.3 设置文件状态标志

通过 `open()`  函数打开文件之后,  文件的 flag 属性就已经被确定下来了，如果想要在打开状态下修改这些属性， 可以使用 `fcntl()` 函数实现,  但是有一点需要注意,  不是所有的 flag  属性都能被动态修改,  只能修改如下状态标志: `O_APPEND, O_NONBLOCK, O_SYNC, O_ASYNC, O_RSYNC` 等。

得到已打开的文件的状态标志，需要将 cmd 设置为 F_GETFL，得到的信息在函数的返回值中

```C
int flag = fcntl(fd, F_GETFL);
```

设置已打开的文件的状态标志，需要将 cmd 设置为 F_SETFL，新的flag需要通过第三个参数传递给 fcntl() 函数

```C
// 得到文件的flag属性
int flag = fcntl(fd, F_GETFL);
// 添加新的flag 标志
flag = flag | O_APPEND;
// 将更新后的falg设置给文件
fcntl(fd, F_SETFL, flag);
```

> 举例： 通过 `fcntl()` 函数 `获取/设置已打开的文件属性`，先来描述一下场景：
>
> 如果要往当前文件中写数据,  打开一个新文件,  文件的写指针在文件头部，数据默认也是写到文件开头，如果不想将数据写到文件头部,  可以给文件追加一个 `O_APPEND` 属性。实例代码如下：
>

```C
// 写实例程序, 给文件描述符追加 O_APPEND
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 打开一个已经存在的磁盘文件
    int fd = open("./111.txt", O_RDWR);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 如果不想将数据写到文件头部, 可以给文件描述符追加一个O_APPEND属性
    // 通过fcntl获取文件描述符的 flag属性
    int flag = fcntl(fd, F_GETFL);
    // 给得到的flag追加 O_APPEND属性
    flag = flag | O_APPEND; // flag |= O_APPEND;
    // 重新将flag属性设置给文件描述符
    fcntl(fd, F_SETFL, flag);

    // 使用fd写文件, 添加的数据应该写到文件尾部
    const char* ppp = "((((((((((((((((((((((骚年，你要相信光！！！))))))))))))))))))))))";
    write(fd, ppp, strlen(ppp));
    close(fd);

    return 0;
}
```



# 15. 目录三剑客



Linux 的目录是一个树状结构，了解数据结构都明白，遍历一棵树最简单的方式是递归。在我们已经掌握了递归的使用方法之后，遍历树状目录也不是一件难事儿。

Linux 给我们提供了相关的目录遍历的函数，分别为：`opendir(), readdir(), closedir()`。目录的操作方式和标准 C 库提供的文件操作步骤是类似的。



## 15.1 opendir

> 在目录操作之前必须要先通过 opendir() 函数打开这个目录，函数原型如下：

```C
#include <sys/types.h>
#include <dirent.h>
// 打开目录
DIR *opendir(const char *name);
```

* 参数：name -> 要打开的目录的名字
* 返回值: DIR*, 结构体类型指针。打开成功返回目录的实例，打开失败返回 NULL



## 15.2 readdir

> 目录打开之后，就可以通过 readdir() 函数遍历目录中的文件信息了。每调用一次这个函数就可以得到目录中的一个文件信息，当目录中的文件信息被全部遍历完毕会得到一个空对象。先来看一下这个函数原型：

```C
// 读目录
#include <dirent.h>
struct dirent *readdir(DIR *dirp);
```

* 参数：dirp -> opendir() 函数的返回值
* 返回值：函数调用成功，返回读到的文件的信息, 目录文件被读完了或者函数调用失败返回 NULL

函数返回值 `struct dirent` 结构体原型如下:

```C
struct dirent {
    ino_t          d_ino;       /* 文件对应的inode编号, 定位文件存储在磁盘的那个数据块上 */
    off_t          d_off;       /* 文件在当前目录中的偏移量 */
    unsigned short d_reclen;    /* 文件名字的实际长度 */
    unsigned char  d_type;      /* 文件的类型, linux中有7中文件类型 */
    char           d_name[256]; /* 文件的名字 */
};
```

关于结构体中的文件类型 `d_type`，可使用的宏值如下：

| DT_BLK     | 块设备文件         |
| ---------- | ------------------ |
| DT_CHR     | 字符设备文件       |
| DT_DIR     | 目录文件           |
| DT_FIFO    | 管道文件           |
| DT_LNK     | 软连接文件         |
| DT_REG     | 普通文件           |
| DT_SOCK    | 本地套接字文件     |
| DT_UNKNOWN | 无法识别的文件类型 |

```C
// 打开目录
DIR* dir = opendir("/home/test");
struct dirent* ptr = NULL;
// 遍历目录
while( (ptr=readdir(dir)) != NULL)
{
    .......
}
```



## 15.3 closedir

目录操作完毕之后,  需要通过 closedir() 关闭通过 opendir() 得到的实例，释放资源。函数原型如下：

```C
// 关闭目录, 参数是 opendir() 的返回值
int closedir(DIR *dirp);
```

* 参数：dirp-> opendir() 函数的返回值
* 返回值: 目录关闭成功返回 0,  失败返回  -1



# 16. 遍历目录



## 16.1 遍历单层目录

如果只遍历单层目录是不需要递归的，按照上边介绍的函数的使用方法，依次继续调用即可。假设我们需要得到某个指定目录下 `mp3`  格式文件的个数，示例代码如下：

```C
// filenum.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[])
{
    // 1. 打开目录
    DIR* dir = opendir(argv[1]);
    if(dir == NULL)
    {
        perror("opendir");
        return -1;
    }

    // 2. 遍历当前目录中的文件
    int count = 0;
    while(1)
    {
        struct dirent* ptr = readdir(dir);
        if(ptr == NULL)
        {
            printf("目录读完了...\n");
            break;
        }
        // 读到了一个文件
        // 判断文件类型
        if(ptr->d_type == DT_REG)
        {
            char* p = strstr(ptr->d_name, ".mp3");
            if(p != NULL && *(p+4) == '\0')
            {
                count++;
                printf("file %d: %s\n", count, ptr->d_name);
            }
        }
    }

    printf("%s目录中mp3文件的个数: %d\n", argv[1], count);

    // 关闭目录
    closedir(dir);

    return 0;
}
```

编译并执行程序

```shell
$ gcc filenum.c
# 读当前目录中mp3文件个数
$ ./a.out .
file 1: 1.mp3
目录读完了...
.目录中mp3文件的个数: 1

# 读 ./sub 目录中mp3文件个数
$ ./a.out ./sub/
file 1: 3.mp3
file 2: 1.mp3
file 3: 5.mp3
file 4: 4.mp3
file 5: 2.mp3
目录读完了...
./sub/目录中mp3文件的个数: 5
```



## 16.2 遍历多层目录

Linux 的目录是树状结构，遍历每层目录的方式都是一样的，也就是说最简单的遍历方式是递归。程序的重点就是确定递归结束的条件：``遍历的文件如果不是目录类型就结束递归``。

```c
// filenum.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int getMp3Num(const char* path)
{
    // 1. 打开目录
    DIR* dir = opendir(path);
    if(dir == NULL)
    {
        perror("opendir");
        return 0;
    }
    // 2. 遍历当前目录
    struct dirent* ptr = NULL;
    int count = 0;
    while((ptr = readdir(dir)) != NULL)
    {
        // 如果是目录 . .. 跳过不处理
        if(strcmp(ptr->d_name, ".")==0 ||
           strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        // 假设读到的当前文件是目录
        if(ptr->d_type == DT_DIR)
        {
            // 目录
            char newPath[1024];
            sprintf(newPath, "%s/%s", path, ptr->d_name);
            // 读当前目录的子目录
            count += getMp3Num(newPath);
        }
        else if(ptr->d_type == DT_REG)
        {
            // 普通文件
            char* p = strstr(ptr->d_name, ".mp3");
            // 判断文件后缀是不是 .mp3
            if(p != NULL && *(p+4) == '\0')
            {
                count++;
                printf("%s/%s\n", path, ptr->d_name);
            }
        }
    }

    closedir(dir);
    return count;
}

int main(int argc, char* argv[])
{
    // ./a.out path
    if(argc < 2)
    {
        printf("./a.out path\n");
        return 0;
    }

    int num = getMp3Num(argv[1]);
    printf("%s 目录中mp3文件个数: %d\n", argv[1], num);

    return 0;
}
```

编译并执行程序

```shell
$ gcc filenum.c
# 查看 abc 目录中mp3 文件个数
$ ./a.out abc
abc/sub/3.mp3
abc/sub/1.mp3
abc/sub/5.mp3
abc/sub/4.mp3
abc/sub/2.mp3
abc/sub/music/test2.mp3
abc/sub/music/test3.mp3
abc/sub/music/test1.mp3
abc/hello.mp3
abc 目录中mp3文件个数: 9
```



# 17. scandir 函数



除了使用上边介绍的目录三剑客遍历目录，也可以使用 `scandir()` 函数进行目录的遍历（只遍历指定目录，不进入到子目录中进行递归遍历），它的参数并不简单，涉及到三级指针和回调函数的使用。

其函数原型如下：

```c
// 头文件
#include <dirent.h> 
int scandir(const char *dirp, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **));
int alphasort(const struct dirent **a, const struct dirent **b);
int versionsort(const struct dirent **a, const struct dirent **b);
```



* 参数：
  * dirp: 需要遍历的目录的名字
  * namelist: 三级指针,  传出参数,  需要在指向的地址中存储遍历目录得到的所有文件的信息
    * `在函数内部会给这个指针指向的地址分配内存，要注意在程序中释放内存`
  * filter: 函数指针,  指针指向的函数就是回调函数,  需要在自定义函数中指定如果过滤目录中的文件
    * `如果不对目录中的文件进行过滤, 该函数指针指定为 NULL 即可`
    * `如果自己指定过滤函数, 满足条件要返回 1, 否则返回 0`
  * compar: 函数指针,  对过滤得到的文件进行排序,  可以使用提供的两种排序方式:
    * alphasort: 根据文件名进行排序
    * versionsort: 根据版本进行排序
* 返回值：函数执行成功返回找到的匹配成功的文件的个数，如果失败返回 -1。



## 17.1 文件过滤

`scandir()` 可以让使用者自定义文件的过滤方式,  然后将过滤函数的地址传递给 scandir() 的第三个参数，我们可以得知过滤函数的原型是这样的：

```c
// 函数的参数就是遍历的目录中的子文件对应的结构体
int (*filter)(const struct dirent *);
```

> 基于这个函数指针定义的函数就可以称之为回调函数,  这个函数不是由程序猿调用,  而是通过 scandir() 调用，因此这个函数的实参也是由 scandir() 函数提供的，作为回调函数的编写人员，只需要搞明白这个参数的含义是什么，然后在函数体中直接使用即可。
>

假设还是判断目录中某一个文件是否为 Mp3 格式,  函数实现如下:

```c
int isMp3(const struct dirent *ptr)
{
    if(ptr->d_type == DT_REG)
    {
        char* p = strstr(ptr->d_name, ".mp3");
        if(p != NULL && *(p+4) == '\0')
        {
            return 1;
        }
    }
    return 0;
}
```



## 17.2 遍历目录

了解了 `scandir()`  函数的使用之后,  下边写一个程序,  来搜索指定目录下的 mp3 格式文件个数和文件名,  代码如下:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

// 文件过滤函数
int isMp3(const struct dirent *ptr)
{
    if(ptr->d_type == DT_REG)
    {
        char* p = strstr(ptr->d_name, ".mp3");
        if(p != NULL && *(p+4) == '\0')
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("./a.out path\n");
        return 0;
    }
    struct dirent **namelist = NULL;
    int num = scandir(argv[1], &namelist, isMp3, alphasort);
    for(int i=0; i<num; ++i)
    {
        printf("file %d: %s\n", i, namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);
    return 0;
}
```

最后再解析一下 scandir() 的第二个参数，传递的是一个二级指针的地址:

```c
struct dirent **namelist = NULL;
int num = scandir(argv[1], &namelist, isMp3, alphasort);
```

那么在这个 namelist 中存储的什么类型的数据呢？ 也就是 `struct dirent **namelist`  指向的什么类型的数据?

答案: 指向的是一个指针数组 `struct dirent *namelist[]`

* 数组元素的个数就是遍历的目录中的文件个数
* 数组的每个元素都是指针类型: `struct dirent *`,  指针指向的地址是有 scandir() 函数分配的,  因此在使用完毕之后需要释放内存



