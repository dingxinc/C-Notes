

# 1. 广播的特点



广播的 UDP 的特性之一，`通过广播可以向子网中多台计算机发送消息，并且子网中所有的计算机都可以接收到发送方发送的消息`，每个广播消息都包含一个特殊的 IP 地址，这个 IP 中子网内主机标志部分的二进制全部为1 （即点分十进制IP的最后一部分是255）。点分十进制的 IP 地址每一部分是 1 字节，最大值为 255，比如：`192.168.1.100`

* 前两部分 `192.168` 表示当前网络是局域网
* 第三部分 `1` 表示局域网中的某一个网段，最大值为 255
* 第四部分 `100` 用于标记当前网段中的某一台主机，最大值为 255



广播分为两端，即数据发送端和数据接收端，通过广播的方式发送数据，发送端和接收端的关系是 1:N

* `发送广播消息的一端，通过广播地址，可以将消息同时发送到局域网的多台主机上（数据接收端）`
* `在发送广播消息的时候，必须要把数据发送到广播地址上`
* `广播只能在局域网内使用，广域网是无法使用 UDP 进行广播的`
* 只要发送端在发送广播消息，数据接收端就能收到广播消息，消息的接收是无法拒绝的，除非将接收端的进程关闭，就接收不到了。

UDP 的广播和日常生活中的广播是一样的，都是一种快速传播消息的方式，`因此广播的开销很小`，发送端使用一个广播地址，就可以将数据发送到多个接收数据的终端上，如果不使用广播，就需要进行多次发送才能将数据分别发送到不同的主机上。



# 2. 设置广播属性

基于 UDP 虽然可以进行数据的广播，但是这个属性默认是关闭的，如果需要对数据进行广播，那么需要在广播端代码中开启广播属性，需要通过套接字选项函数进行设置，该函数原型为：

```c
int setsockopt(int sockfd, int level, int optname, 	const void *optval, socklen_t optlen);
```

* 参数：
  * sockfd：进行 UDP 通信的文件描述符
  * level: 套接字级别，需要设置为 `SOL_SOCKET`
  * optname：选项名，此处要设置 udp 的广播属性，该参数需要指定为：`SO_BROADCAST`
  * optval：如果是设置广播属性，该指针实际指向一块 `int` 类型的内存
    * 该整型值为 0：关闭广播属性
    * 该整形值为 1 ：打开广播属性
  * optlen：optval 指针指向的内存大小，即：`sizeof(int)`
* 返回值：函数调用成功返回 0，失败返回 -1



# 3. 广播通信流程



如果使用 UDP 在局域网范围内进行消息的广播，一般情况下广播端只发送数据，接收端只接受广播消息。因此在数据接收端需要绑定固定的端口，广播端则不需要手动绑定固定端口，自动随机绑定即可。

![image-20230916143215346](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309161432409.png)



# 4. 通信代码



广播端

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

    // 2. 设置广播属性
    int opt  = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    char buf[1024];
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999); // 接收端需要绑定9999端口
    // 只要主机在237网段, 并且绑定了9999端口, 这个接收端就能收到广播消息
    inet_pton(AF_INET, "192.168.237.255", &cliaddr.sin_addr.s_addr);
    // 3. 通信
    int num = 0;
    while(1)
    {
        sprintf(buf, "hello, client...%d\n", num++);
        // 数据广播
        sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&cliaddr, len);
        printf("发送的广播的数据: %s\n", buf);
        sleep(1);
    }

    close(fd);

    return 0;
}
```

**注意事项：发送广播消息一端必须要开启 UDP 的广播属性，并且发送消息的地址必须是当前发送端所在网段的广播地址，这样才能通过调用一个消息发送函数将消息同时发送 N 台接收端主机上。**



接受端

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
    // 3. 通信
    while(1)
    {
        // 接收广播消息
        memset(buf, 0, sizeof(buf));
        // 阻塞等待数据达到
        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("接收到的广播消息: %s\n", buf);
    }

    close(fd);

    return 0;
}
```

**对于接收广播消息的一端，必须要绑定固定的端口，并由广播端将广播消息发送到这个端口上，因此所有接收端都应绑定相同的端口，这样才能同时收到广播数据。**