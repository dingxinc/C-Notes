

udp 是一个面向无连接的，不安全的，报式传输层协议，**udp 的通信过程默认也是阻塞的**。

* `UDP 通信不需要建立连接` ，因此不需要进行 connect() 操作
* `UDP 通信过程中，每次都需要指定数据接收端的 IP 和端口`，和发快递差不多
* `UDP 不对收到的数据进行排序，在 UDP 报文的首部中并没有关于数据顺序的信息`
* `UDP 对接收到的数据报不回复确认信息，发送端不知道数据是否被正确接收，也不会重发数据。`
* `如果发生了数据丢失，不存在丢一半的情况，如果丢当前这个数据包就全部丢失了`



# 1. 通信流程

使用 UDP 进行通信，服务器和客户端的处理步骤比 TCP 要简单很多，并且两端是对等的 （通信的处理流程几乎是一样的），也就是说并没有严格意义上的客户端和服务器端。UDP 的通信流程如下：

![image-20230916141307461](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309161413521.png)



## 1.1 服务器端

假设服务器端是**接收数据**的角色：

1. 创建通信的套接字
2. 使用通信的套接字和本地的 IP 和端口绑定，IP 和端口需要转换为大端(可选)
3. 通信
4. 关闭套接字（文件描述符）



## 1.2 客户端

假设客户端是**发送数据**的角色：

1. 创建通信的套接字
2. 通信
3. 关闭套接字（文件描述符）

在 UDP 通信过程中，`哪一端是接收数据的角色，那么这个接收端就必须绑定一个固定的端口`，如果某一端不需要接收数据，这个绑定操作就可以省略不写了，通信的套接字会自动绑定一个随机端口。



# 2. 通信函数



基于 UDP 进行套接字通信，创建套接字的函数还是 `socket()` 但是第二个参数的值需要指定为 `SOCK_DGRAM`，通过该参数指定要创建一个基于报式传输协议的套接字，最后一个参数指定为 0 表示使用报式协议中的 UDP 协议。

```c
int socket(int domain, int type, int protocol);
```

* 参数：
  * domain：地址族协议，AF_INET -> IPv4，AF_INET6-> IPv6
  * type：使用的传输协议类型，报式传输协议需要指定为 SOCK_DGRAM
  * protocol：指定为 0，表示使用的默认报式传输协议为 UDP
* 返回值：函数调用成功返回一个可用的文件描述符（大于0），调用失败返回 -1



另外进行 UDP 通信，通信过程虽然默认还是阻塞的，但是通信函数和 TCP 不同，操作函数原型如下：

```c
// 接收数据, 如果没有数据,该函数阻塞
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

* 参数：
  * sockfd: 基于 udp 的通信的文件描述符
  * buf: 指针指向的地址用来存储接收的数据
  * len: buf 指针指向的内存的容量, 最多能存储多少字节
  * flags: 设置套接字属性，一般使用默认属性，指定为 0 即可
  * src_addr: 发送数据的一端的地址信息，IP 和端口都存储在这里边,  是大端存储的
    * 如果这个参数中的信息对当前业务处理没有用处, 可以指定为 NULL, 不保存这些信息
  * addrlen:  类似于 accept() 函数的最后一个参数,  是一个传入传出参数
    * 传入的是 src_addr 参数指向的内存的大小,  传出的也是这块内存的大小
    * 如果 src_addr 参数指定为 NULL, 这个参数也指定为 NULL 即可
* 返回值：成功返回接收的字节数，失败返回 -1



```c
// 发送数据函数
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

* 参数：
  * sockfd:  基于 udp 的通信的文件描述符
  * buf: 这个指针指向的内存中存储了要发送的数据
  * len: 要发送的数据的实际长度
  * flags: 设置套接字属性，一般使用默认属性，指定为 0 即可
  * dest_addr: 接收数据的一端对应的地址信息,  大端的 IP 和端口
  * addrlen: 参数 dest_addr 指向的内存大小
* 返回值：函数调用成功返回实际发送的字节数，调用失败返回 -1



# 3. 通信代码



在 UDP 通信过程中，服务器和客户端都可以作为数据的发送端和数据接收端，假设服务器端是被动接收数据，客户端是主动发送数据，那么在服务器端就必须绑定固定的端口了。



## 3.1 服务器端

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 通信的套接字和本地的IP与端口绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);    // 大端
    addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    char buf[1024];
    char ipbuf[64];
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    // 3. 通信
    while(1)
    {
        // 接收数据
        memset(buf, 0, sizeof(buf));
        int rlen = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, &len);
        printf("客户端的IP地址: %s, 端口: %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ipbuf, sizeof(ipbuf)),
               ntohs(cliaddr.sin_port));
        printf("客户端say: %s\n", buf);

        // 回复数据
        // 数据回复给了发送数据的客户端
        sendto(fd, buf, rlen, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    }

    close(fd);

    return 0;
}
```

作为数据接收端，服务器端通过 `bind()` 函数绑定了固定的端口，然后基于这个固定的端口通过 `recvfrom()` 函数接收客户端发送的数据，同时通过这个函数也得到了数据发送端的地址信息（recvfrom的第三个参数），这样就可以通过得到的地址信息通过 `sendto()` 函数给客户端回复数据了。



## 3.2 客户端

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }
    
    // 初始化服务器地址信息
    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);    // 大端
    inet_pton(AF_INET, "192.168.1.100", &seraddr.sin_addr.s_addr);

    char buf[1024];
    char ipbuf[64];
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    int num = 0;
    // 2. 通信
    while(1)
    {
        sprintf(buf, "hello, udp %d....\n", num++);
        // 发送数据, 数据发送给了服务器
        sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&seraddr, sizeof(seraddr));

        // 接收数据
        memset(buf, 0, sizeof(buf));
        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("服务器say: %s\n", buf);
        sleep(1);
    }

    close(fd);

    return 0;
}
```

作为数据发送端，客户端不需要绑定固定端口，客户端使用的端口是随机绑定的（也可以调用 bind() 函数手动进行绑定）。客户端在接收服务器端回复的数据的时候需要调用 `recvfrom()` 函数，因为客户端在发送数据之前就已经知道服务器绑定的固定的IP和端口信息了，所以接收服务器数据的时候就可以不保存服务器端的地址信息，直接将函数的最后两个参数指定为 NULL 即可。

