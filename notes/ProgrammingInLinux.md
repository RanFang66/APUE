## Linux 应用开发

### 1. Linux 文件操作

#### 1. 不带缓冲的的I/O unbuffered I/O

- 文件描述符

  对内核而言，所有打开的文件都通过文件描述符引用，文件描述符是系统分配给文件的一个非负整数标识。按照惯例有以下特殊的文件描述符：

  | 文件     | POSIX 宏定义（<unistd.h>） | 值   |
  | -------- | -------------------------- | ---- |
  | 标准输入 | STDIN_FILENO               | 0    |
  | 标准输出 | STDOUT_FILENO              | 1    |
  | 标准错误 | STDERR_FILENO              | 2    |

  文件描述符的变化范围是0～OPEN_MAX-1，即每个进程允许打开的文件最多为OPEN_MAX个。

- open函数

  open函数可以打开或创建一个文件。

  ```c
  #include <fcntl.h>
  int open(const char *path, int oflag, .../* mode_t mode */);
  int openat(int fd, const char *path, int oflag, .../* mode_t mode* /);
  ```

  函数参数的最后一个 ...表示余下的参数的数量和类型是可变的，对于open函数而言，只有当创建文件时才会使用到注视中的这个参数。

  - path 参数表示要打开或创建的文件的名字
  - oflag 参数设置打开文件的一些选项，常用的一些选项如下：

  | oflag    | 含义                                                  |
  | -------- | ----------------------------------------------------- |
  | O_RDONLY | 只读方式打开                                          |
  | O_WRONLY | 只写方式打开                                          |
  | O_RDWR   | 可读，写方式打开                                      |
  | O_EXEC   | 只执行打开                                            |
  | O_APPEND | 每次写文件都追加到文件末尾                            |
  | O_CREAT  | 如果文件不存在，则创建它。                            |
  | O_EXCL   | 如果同时指定了O_CREAT，而文件已存在，则出错           |
  | O_TRUNC  | 如果文件存在，且用写或读写方式打开，则将长度截断为0。 |

  多个选项之间可以用“或”预算构成oflag参数，但是表中前四个选项必须指定一个且只能指定一个。

  - mode 参数在新建一个文件时指定该新文件的访问权限位。
  - 返回值：若打开或创建成功，则返回文件描述符，出错则返回-1

- creat函数

  调用creat函数可以创建一个新的文件。

  ```c
  #include <fcntl.h>
  int creat(const char *path, mode_t mode);
  ```

  这个函数等效于

  ```c
  open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
  ```

  因此，一般使用中不再需要这个函数了，直接使用open函数就行。

- close函数

  调用close函数可以关闭一个已经打开的文件。

  ```c
  #include <unistd.h>
  int close(int fd);
  ```

  - fd: 要关闭的文件的文件描述符
  - 返回值： 关闭成功则返回0， 失败则返回-1
  - 关闭一个文件还会释放该进程加在该文件上的所有记录锁

- lseek函数

  每个打开的文件都会有一个与其关联的“当前文件偏移量”(current file offset)。它通常是一个非负整数，表示从文件开始处计算的字节数，而文件的读写操作都是从该偏移量开始，并增加所读写的字节数。在没有指定O_APPEND选项时，打开文件时的偏移量为0。调用lseek函数可以显示的为打开的文件设置偏移量。

  ```c
  #include <unistd.h>
  off_t lseek(int fd, off_t offset, int whence);
  ```

  - fd： 表示要操作的文件描述符

  - offset：表示相对于某个基准的偏移量，这个基准由第三个参数whence确定。

  - whence：定义偏移量的基准，它有如下取值：

    | whence取值 | 含义                                                   |
    | ---------- | ------------------------------------------------------ |
    | SEEK_SET   | 将文件偏移量设置为距文件开始处offset个字节             |
    | SEEK_CUR   | 将文件偏移量设置为当前值加offset，offset可正可负       |
    | SEEK_END   | 将文件偏移量设置为文件的长度加offset，offset可正可负。 |

  - 返回值：若设置成功则返回新的文件偏移量，若失败则返回-1。

  - *Notice*：如果这里fd描述的是一个pipe，fifo或者socket，那lseek会返回-1，并且errno设置为ESPIPE。

  - *Notice*:  如果偏移量超出了文件当前的长度，这是允许的，这时对于新写入的数据会分配磁盘块，但是对于原文件尾端和新开始写的位置之间（空洞）则不会分配磁盘块，位于文件中但没有写过的字节都被读为0。

  

- read函数

  read函数从打开的文件中读取数据。

  ```c
  #include <unistd.h>
  ssize_t read(int fd, void *buf, size_t nbytes);
  ```

  - fd: 要操作的文件描述符。
  - *buf: 读取内容存在的buffer。
  - nbytes：要读取的字节数。
  - 返回值：实际读到的字节数，若已经到文件末尾则返回0，若出错则返回-1。
  - *Notice*：很多种情况会导致实际读到字节数小于要求读的，如文件的大小小于给定的读取字节数，从终端设备读取时一次最多读一行等等。

- write函数

  调用write函数向打开的文件写入数据。

  ```c
  #include <unistd.h>
  ssize_t write(int fd, const void *buf, size_t nbytes);
  ```

  - fd：要操作的文件描述符
  - *buf：指向要写入内容的指针
  - nbytes：要写入的字节数
  - 返回值：若成功返回已写的字节数，若出错返回-1。



####2. 标准文件I/O bufferd I/O



#### 3. 目录和文件的其他操作

- 文件描述符的复制（函数dup和dup2）

  函数dup和dup2都可以用来复制一个现有的文件描述符：

  ```c
  #include <unistd.h>
  int dup(int fd);
  int dup2(int fd, int fd2);
  ```

  两个函数在成功时都会返回新的描述符，不同的是dup返回的新描述符是当前系统中可用描述符的最小值，而dup2则可以通过第二个参数指定新描述符的值，如果指定的fd2已经被打开，则会先将其关闭。

###2. Linux 多进程

### 3. Linux 进程间通信

###4. Linux 多线程

### 5. Linux线程间通信

### 6. Linux网络编程

