

# 1. 组播的特点



组播也可以称之为多播这也是 UDP 的特性之一。`组播是主机间一对多的通讯模式，是一种允许一个或多个组播源发送同一报文到多个接收者的技术。`组播源将一份报文发送到特定的组播地址，组播地址不同于单播地址，它并不属于特定某个主机，而是属于一组主机。一个组播地址表示一个群组，需要接收组播报文的接收者都加入这个群组。

* 广播只能在局域网访问内使用，组播既可以在局域网中使用，也可以用于广域网
* 在发送广播消息的时候，连接到局域网的客户端不管想不想都会接收到广播数据，组播可以控制发送端的消息能够被哪些接收端接收，更灵活和人性化。
* 广播使用的是广播地址，组播需要使用组播地址。
* 广播和组播属性默认都是关闭的，如果使用需要通过 setsockopt() 函数进行设置。



组播需要使用组播地址，在 IPv4 中它的范围从 224.0.0.0 到 239.255.255.255，并被划分为局部链接多播地址、预留多播地址和管理权限多播地址三类:

| IP 地址                     | 说明                                                         |
| --------------------------- | ------------------------------------------------------------ |
| `224.0.0.0~224.0.0.255`     | 局部链接多播地址：是为路由协议和其它用途保留的地址，<br/>只能用于局域网中，路由器是不会转发的地址 224.0.0.0不能用，是保留地址 |
| `224.0.1.0~224.0.1.255`     | 为用户可用的组播地址（临时组地址），可以用于 Internet 上的。 |
| `224.0.2.0~238.255.255.255` | 用户可用的组播地址（临时组地址），全网范围内有效             |
| `239.0.0.0~239.255.255.255` | 为本地管理组播地址，仅在特定的本地范围内有效                 |

组播地址不属于任何服务器或个人，它有点类似一个微信群号，任何成员（**组播源**）往微信群（**组播IP**）发送消息（**组播数据**），这个群里的成员（**组播接收者**）都会接收到此消息。



# 2. 设置组播属性



如果使用组播进行数据的传输，不管是消息发送端还是接收端，都需要进行相关的属性设置，设置函数使用的是同一个，即：`setsockopt()。`



## 2.1 发送端

发送组播消息的一端需要设置组播属性，具体的设置方式如下：

```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

* 参数：

  * sockfd：用于 UDP 通信的套接字
  * level：套接字级别，设置组播属性需要将该参数指定为：`IPPTOTO_IP`
  * optname: 套接字选项名，设置组播属性需要将该参数指定为：`IP_MULTICAST_IF`
  * optval：设置组播属性，这个指针需要指向一个 `struct in_addr{}` 类型的结构体地址，这个结构体地址用于存储组播地址，并且组播IP地址的存储方式是大端的。

  ```c
  struct in_addr
  {
      in_addr_t s_addr;	// unsigned int
  }; 
  ```

  * optlen：optval指针指向的内存大小，即：`sizeof(struct in_addr)`

* 返回值：函数调用成功返回 0，调用失败返回 -1



## 2.2 接收端

因为一个组播地址表示一个群组，所以需要接收组播报文的接收者都加入这个群组，和想要接收群消息就必须要先入群是一个道理。加入到这个组播群组的方式如下：

```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

* 参数：

  * sockfd：基于 udp 的通信的套接字
  * level：套接字级别，加入到多播组该参数需要指定为：`IPPTOTO_IP`
  * optname：套接字选项名，加入到多播组该参数需要指定为：`IP_ADD_MEMBERSHIP`
  * optval：加入到多播组，这个指针应该指向一个 `struct ip_mreqn{}` 类型的结构体地址

  ```c
  typedef unsigned int  uint32_t;
  typedef uint32_t in_addr_t;
  struct sockaddr_in addr;
  
  struct in_addr
  {
      in_addr_t s_addr;	// unsigned int
  };
  
  struct ip_mreqn
  {
      struct in_addr imr_multiaddr;   // 组播地址/多播地址
      struct in_addr imr_address;     // 本地地址
      int   imr_ifindex;              // 网卡的编号, 每个网卡都有一个编号
  };
  // 必须通过网卡名字才能得到网卡的编号: 可以通过 ifconfig 命令查看网卡名字
  #include <net/if.h>
  // 将网卡名转换为网卡的编号, 参数是网卡的名字, 比如: "ens33"
  // 返回值就是网卡的编号
  unsigned int if_nametoindex(const char *ifname);
  ```

  * optlen：optval指向的内存大小，即：`sizeof(struct ip_mreqn)`



# 3. 组播通信流程

发送组播消息的一端需要将数据发送到组播地址和固定的端口上，想要接收组播消息的终端需要绑定对应的固定端口然后加入到组播的群组，最终就可以实现数据的共享。



![image-20230916145326166](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309161453227.png)



# 4. 通信代码



## 4.1 发送端

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

    // 2. 设置组播属性
    struct in_addr opt;
    // 将组播地址初始化到这个结构体成员中即可
    inet_pton(AF_INET, "239.0.1.10", &opt.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &opt, sizeof(opt));

    char buf[1024];
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999); // 接收端需要绑定9999端口
    // 发送组播消息, 需要使用组播地址, 和设置组播属性使用的组播地址一致就可以
    inet_pton(AF_INET, "239.0.1.10", &cliaddr.sin_addr.s_addr);
    // 3. 通信
    int num = 0;
    while(1)
    {
        sprintf(buf, "hello, client...%d\n", num++);
        // 数据广播
        sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&cliaddr, len);
        printf("发送的组播的数据: %s\n", buf);
        sleep(1);
    }

    close(fd);

    return 0;
}
```

**注意事项：在组播数据的发送端，需要先设置组播属性，发送的数据是通过 sendto() 函数发送到某一个组播地址上，并且在程序中数据发送到了接收端的 9999 端口，因此接收端程序必须要绑定这个端口才能收到组播消息。**



## 4.2 接收端

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

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

    // 3. 加入到多播组
    struct ip_mreqn opt;
    // 要加入到哪个多播组, 通过组播地址来区分
    inet_pton(AF_INET, "239.0.1.10", &opt.imr_multiaddr.s_addr);
    opt.imr_address.s_addr = INADDR_ANY;
    opt.imr_ifindex = if_nametoindex("ens33");
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &opt, sizeof(opt));

    char buf[1024];
    // 3. 通信
    while(1)
    {
        // 接收广播消息
        memset(buf, 0, sizeof(buf));
        // 阻塞等待数据达到
        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("接收到的组播消息: %s\n", buf);
    }

    close(fd);

    return 0;
}
```

**注意事项：作为组播消息的接收端，必须要先绑定一个固定端口（发送端就可以把数据发送到这个固定的端口上了），然后加入到组播的群组中（一个组播地址可以看做是一个群组），这样就可以接收到组播消息了。**