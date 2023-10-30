
#网络编程  #TCP  #UDP  #HTTP  #SOCKET
# 1. 主机信息查询

## 1.1 QHostInfo 类和 QNetworkInterface 类

QHostInfo 类可以根据主机名获取主机的 IP 地址，或者通过 IP 地址获取主机名。QHostInfo 类的静态函数 localHostName() 可获取本机的主机名，静态函数 fromName() 可以通过主机名获取 IP 地址，静态函数 lookupHost() 可以通过一个主机名以异步方式查找这个主机的 IP 地址。

```C++
// 公共函数
QList<QHostAddress> address();  // 返回与 hostName() 对应主机关联的 IP 地址列表
HostInfoError error();  // 如果主机查找失败，返回失败类型
QString errorString();  // 如果主机查找失败，返回错误描述字符串
QString hostName();  // 返回通过 IP 地址查找到的主机名
int lookupId();   // 返回本次查找到的 ID

// 静态函数
void abortHostLookup(int id);  // 中断主机查找
QHostInfo fromName(QString &name);  // 返回指定主机名的 IP 地址
QString localDomainName();  // 返回本机域名系统（DNS）域名
QString localHostName();   // 返回本机主机名
int lookupHost(QString &name, QObject *reciver, char* member);// 以异步方式根据主机名查找主机的IP地址，并返回一个表示本次查找的 ID
```



QNetworkInterface 类可以获得运行程序的主机的所有 IP 地址和网络接口列表。


```C++
// 公共函数
QList<QNetworkAddressEntry> addressEntries();  // 返回网络接口的 IP 地址列表，包括子网掩码和广播地址
QString hardwareAddress();  // 返回接口的低级硬件地址，以太网里就是 MAC 地址
QString name();  // 返回网络接口名称
bool isValid(); // 如果接口信息有效就返回 true

// 静态函数
QList<QHostAddress> allAddresses();  // 返回主机上所有 IP 地址的列表
QList<QNetworkInterface> allInterfaces();  // 返回主机上所有网路接口的列表
```



## 1.2 网络主机信息显示功能实现

程序界面

![image-20230907210725584](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309072107636.png)

### 1.2.1 QHostInfo 类的使用

1. 显示本机地址信息

   “获取本机主机名和 IP 地址” 按钮的槽函数代码如下：

   ```C++
   void MainWindow::on_btnGetHostInfo_clicked()
   {//"获取本机主机名和IP地址" 按钮
       ui->textEdit->clear();
       QString hostName=QHostInfo::localHostName();    //本地主机名
       ui->textEdit->appendPlainText("本机主机名："+hostName+"\n");
   
       QHostInfo   hostInfo=QHostInfo::fromName(hostName);  //本机IP地址
       QList<QHostAddress> addrList=hostInfo.addresses();    //IP地址列表
       if (addrList.isEmpty())
           return;
   
       foreach (QHostAddress host, addrList)
       {
           bool show=ui->chkBox_OnlyIPv4->isChecked();   //只显示IPv4
           show= show? (host.protocol()==QAbstractSocket::IPv4Protocol):true;
           if (show)
           {
               ui->textEdit->appendPlainText("协 议："+protocolName(host.protocol())); //协议类型
               ui->textEdit->appendPlainText("本机IP地址："+host.toString());           //IP地址
               ui->textEdit->appendPlainText(QString("isGlobal()=%1\n").arg(host.isGlobal()));
   //            ui->textEdit->appendPlainText(QString("isBroadcast()=%1").arg(host.isBroadcast()));
   //            ui->textEdit->appendPlainText(QString("isLoopback()=%1").arg(host.isLoopback()));
   //            ui->textEdit->appendPlainText(QString("isMulticast()=%1\n").arg(host.isMulticast()));
           }
       }
   }
   ```

   在 MainWindow 类中定义了一个私有函数 protocolName()，代码如下：

   ```C++
   QString MainWindow::protocolName(QAbstractSocket::NetworkLayerProtocol protocol)
   {//通过协议类型返回协议名称
       switch(protocol)
       {
       case QAbstractSocket::IPv4Protocol:
           return "IPv4";
       case QAbstractSocket::IPv6Protocol:
           return "IPv6";
       case QAbstractSocket::AnyIPProtocol:
           return "Any Internet Protocol";
       default:
           return "Unknown Network Layer Protocol";
       }
   }
   ```

   

2. 查找主机地址信息

   “查找域名的 IP 地址”按钮的槽函数代码如下：

   ```C++
   void MainWindow::on_btnLookup_clicked()
   {//"查找域名的IP地址" 按钮
       ui->textEdit->clear();
       QString hostname=ui->comboBox->currentText();     //读取主机名
       ui->textEdit->appendPlainText("正在查找查找主机信息："+hostname);
       QHostInfo::lookupHost(hostname,this,SLOT(do_lookedUpHostInfo(QHostInfo)));
   }
   ```

   ```C++
   // do_lookedUpHostInfo 函数代码如下
   void MainWindow::do_lookedUpHostInfo(const QHostInfo &host)
   {//查找主机信息的槽函数
       QList<QHostAddress> addrList=host.addresses();   //获取主机的地址列表
       if (addrList.isEmpty())
           return;
       foreach(QHostAddress host, addrList)
       {
           bool show=ui->chkBox_OnlyIPv4->isChecked();   //只显示IPv4
           show= show? (host.protocol()==QAbstractSocket::IPv4Protocol):true;
           if (show)
           {
               ui->textEdit->appendPlainText("协 议："+protocolName(host.protocol()));
               ui->textEdit->appendPlainText(host.toString());
               ui->textEdit->appendPlainText(QString("isGlobal()=%1\n").arg(host.isGlobal()));
           }
       }
   }
   ```



### 1.2.2 QNetworkInterface 类的使用

QNetworkInterface 类可以获得应用程序所在主机 的所有网络接口的信息，包括子网掩码和广播地址。其静态函数 allInterfaces() 获取所有网络接口的列表，返回结果是一个 QNetworkInterface 类型的列表。

“allInterfaces()” 按钮的槽函数代码如下：

```C++
void MainWindow::on_btnAllInterface_clicked()
{//"allInterfaces()"按钮
    ui->textEdit->clear();
    QList<QNetworkInterface> list=QNetworkInterface::allInterfaces();   //网络接口列表
    foreach(QNetworkInterface interface,list)
    {
        if (!interface.isValid())
           continue;

        ui->textEdit->appendPlainText("设备名称："+interface.humanReadableName());
        ui->textEdit->appendPlainText("硬件地址："+interface.hardwareAddress());
        ui->textEdit->appendPlainText("接口类型："+interfaceType(interface.type()));
        QList<QNetworkAddressEntry> entryList=interface.addressEntries();   //地址列表
        foreach(QNetworkAddressEntry entry, entryList)
        {
            ui->textEdit->appendPlainText("   IP 地址："+entry.ip().toString());
            ui->textEdit->appendPlainText("   子网掩码："+entry.netmask().toString());
            ui->textEdit->appendPlainText("   广播地址："+entry.broadcast().toString()+"\n");
        }
    }
}
```

```C++
// interfaceType() 函数
QString MainWindow::interfaceType(QNetworkInterface::InterfaceType type)
{//根据枚举值返回字符串
    switch(type)
    {
    case QNetworkInterface::Unknown:
        return "Unknown";
    case QNetworkInterface::Loopback:
        return "Loopback";
    case QNetworkInterface::Ethernet:
        return "Ethernet";
    case QNetworkInterface::Wifi:
        return "Wifi";
    default:
        return "Other type";
    }
}
```

函数 `QNetworkInterface::allInterfaces()` 返回的网络接口的信息比较多，如果无需知道子网掩码和广播地址等信息，可以使用静态函数 `QNetworkInterface::allAddress()` 只获取 IP 地址。



# 2. TCP 通信

[基于TCP的Qt网络通信 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/socket-tcp/)

传输控制协议（TCP）是一种被大多数 Internet 用于数据传输的底层网路协议，它是可靠的、面向连接的传输协议，特别适合用于连续数据传输。

TCP 通信必须首先建立连接，通信端分为客户端和服务器端。

## 2.1 TCP 通信相关的类

```C++
/* QTcpServer */
QTcpServer::QTcpServer(QObject *parent = Q_NULLPTR);

bool QTcpServer::listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
// 判断当前对象是否在监听, 是返回true，没有监听返回false
bool QTcpServer::isListening() const;
// 如果当前对象正在监听返回监听的服务器地址信息, 否则返回 QHostAddress::Null
QHostAddress QTcpServer::serverAddress() const;
// 如果服务器正在侦听连接，则返回服务器的端口; 否则返回0
quint16 QTcpServer::serverPort() const;

QTcpSocket *QTcpServer::nextPendingConnection();

// 阻塞等待客户端发起的连接请求
bool QTcpServer::waitForNewConnection(int msec = 0, bool *timedOut = Q_NULLPTR);

[signal] void QTcpServer::acceptError(QAbstractSocket::SocketError socketError);
[signal] void QTcpServer::newConnection();

/* QTcpSocket*/ 
QTcpSocket::QTcpSocket(QObject *parent = Q_NULLPTR);

[virtual] void QAbstractSocket::connectToHost(const QString &hostName, quint16 port, OpenMode openMode = ReadWrite, NetworkLayerProtocol protocol = AnyIPProtocol);

[virtual] void QAbstractSocket::connectToHost(const QHostAddress &address, quint16 port, OpenMode openMode = ReadWrite);

// 指定可接收的最大字节数 maxSize 的数据到指针 data 指向的内存中
qint64 QIODevice::read(char *data, qint64 maxSize);
// 指定可接收的最大字节数 maxSize，返回接收的字符串
QByteArray QIODevice::read(qint64 maxSize);
// 将当前可用操作数据全部读出，通过返回值返回读出的字符串
QByteArray QIODevice::readAll();

// 发送指针 data 指向的内存中的 maxSize 个字节的数据
qint64 QIODevice::write(const char *data, qint64 maxSize);
// 发送指针 data 指向的内存中的数据，字符串以 \0 作为结束标记
qint64 QIODevice::write(const char *data);
// 发送参数指定的字符串
qint64 QIODevice::write(const QByteArray &byteArray);

[signal] void QIODevice::readyRead();
[signal] void QAbstractSocket::connected();
[signal] void QAbstractSocket::disconnected();
```

> **通信流程**
>
> 服务器端端程序首先需要使用函数 listen() 开始服务器端监听，可以设置监听的 IP 地址和端口，一般一个服务器程序只监听某个端口的网络连接。
>
> 当有新客户端接入时，QTcpServer 内部的函数 incomingConnection() 会创建一个与客户端连接的 QTcpSocket 对象，然后发射 newConnect() 信号。在与 newConnecting() 信号关联的槽函数中，我们可以用 nextPendingConnection() 接受客户端的连接，然后使用 QTcpSocket 对象与客户端通信。
>
> 因此，在客户端和服务器端建立连接后具体的数据通信是通过 QTcpSocket 对象完成的。QTcpSocket 类提供了 TCP 接口，可以用 QTcpSocket 类实现标准的网络通信协议，也可以设计自定义协议。
>
> **由于 QTcpSocket 是从 QIODevice 间接继承的类，因此 QTcpSocket 是一种 I/O 设备类，它具有流数据读写能力。**



## 2.2 TCP 通信程序设计

程序界面

![image-20230907215256424](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309072152469.png)

**TCP Server 程序具有如下功能：**

* 根据 IP 地址（本机地址）和端口开始网络监听，有客户端连接时创建 socket 连接
* 采用基于行的数据通信协议，可以接受客户端发来的消息，也可以向客户端发送消息
* 在状态栏上显示服务器监听状态和 socket 状态

**TCP Client 程序具有如下功能**

* 通过 IP  地址和端口连接到服务器端
* 采用基于行的数据通信协议，与服务器端进行消息的收发
* 处理 QTcpSocket 的 stateChange() 信号，在状态栏上显示 socket 信息

### 2.2.1 主窗口类定义和初始化

```C++
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLabel  *labListen;         //状态栏标签
    QLabel  *labSocketState;    //状态栏标签

    QTcpServer *tcpServer;      //TCP服务器
    QTcpSocket *tcpSocket=nullptr;      //TCP通讯的Socket
    QString getLocalIP();       //获取本机IP地址

//protected:
//    void    closeEvent(QCloseEvent *event);   //close事件处理函数
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();      //析构函数里需要做一些处理，以保证断开与客户端的连接，并停止网络监听

private slots:
    //自定义槽函数
    void    do_newConnection();         //关联QTcpServer的newConnection()信号
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_clientConnected();       //客户端 socket 已连接
    void    do_clientDisconnected();    //客户端 socket 已断开
    void    do_socketReadyRead();       //读取socket传入的数据

    //UI生成的
    void on_actStart_triggered();

    void on_actStop_triggered();

    void on_actClear_triggered();

    void on_btnSend_clicked();

    void on_actHostInfo_triggered();
private:
    Ui::MainWindow *ui;
};
```

MainWindow 类中定义了变量 tcpServer 用于建立 TCP 服务器，定义了 tcpSocket 用于与客户进行 socket 通信。MainWindow 类还定义了几个槽函数，用于与 tcpServer 和 tcpSocket 的相关信号连接。MainWindow 的构造函数和 getLocalIP() 的代码如下：

```C++
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labListen=new QLabel("监听状态:");
    labListen->setMinimumWidth(150);
    ui->statusBar->addWidget(labListen);

    labSocketState=new QLabel("Socket状态：");
    labSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(labSocketState);

    QString localIP=getLocalIP();   //获取本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);
    ui->comboIP->addItem(localIP);

    tcpServer=new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(do_newConnection()));
}
```

```C++
QString MainWindow::getLocalIP()
{//获取本机IPv4地址
    QString hostName=QHostInfo::localHostName();    //本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();  //本机IP地址列表
    if (addList.isEmpty())
        return localIP;

    foreach(QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    return localIP;
}
```

我们在析构函数添加了代码，确保窗口关闭时断开与 TCP 客户端的 socket 连接，停止 TCP 服务器的网络监听。

```C++
//void MainWindow::closeEvent(QCloseEvent *event)
//{//关闭窗口时停止监听
//    if (tcpServer->isListening())
//        tcpServer->close();;//停止网络监听
//    event->accept();
//}

MainWindow::~MainWindow()
{//析构函数
    if (tcpSocket != nullptr)
    {
        if (tcpSocket->state()==QAbstractSocket::ConnectedState)
            tcpSocket->disconnectFromHost();    //断开与客户端的连接
    }

    if (tcpServer->isListening())
        tcpServer->close();     //停止网络监听

    delete ui;
}
```

### 2.2.2 网路监听与 socket 连接的建立

```C++
void MainWindow::on_actStart_triggered()
{//"开始监听"按钮
    QString  IP=ui->comboIP->currentText();  //IP地址字符串,如"127.0.0.1"
    quint16  port=ui->spinPort->value();     //端口
    QHostAddress   address(IP);
    tcpServer->listen(address,port);    //开始监听
    //    tcpServer->listen(QHostAddress::LocalHost,port);// Equivalent to QHostAddress("127.0.0.1").
    ui->textEdit->appendPlainText("**开始监听...");
    ui->textEdit->appendPlainText("**服务器地址："+tcpServer->serverAddress().toString());
    ui->textEdit->appendPlainText("**服务器端口："+QString::number(tcpServer->serverPort()));

    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);
    labListen->setText("监听状态：正在监听");
}
```

> 可以用表示本机地址的 127.0.0.1， 也可以用本机的**实际地址**，也可以使用常量 QHostAddress::LocalHost。
>
> 因此，在本机上监听某个端口的对应代码也可以写成下面这种形式：
>
> `tcpServer->listen(QHostAddress::LocalHost, port)`;

当有客户端接入时，tcpServer 会发射 newConnect() 信号，此信号关联到槽函数 do_newConnection()。

```C++
void MainWindow::do_newConnection()
{
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(tcpSocket, SIGNAL(connected()),this, SLOT(do_clientConnected()));
    do_clientConnected();   //执行一次槽函数，显示状态

    connect(tcpSocket, SIGNAL(disconnected()),this, SLOT(do_clientDisconnected()));

    connect(tcpSocket,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);
    do_socketStateChange(tcpSocket->state());   //执行一次槽函数，显示状态

    connect(tcpSocket,SIGNAL(readyRead()),  this,SLOT(do_socketReadyRead()));
}
```

```C++
/* do_clientConnected */
void MainWindow::do_clientConnected()
{//客户端接入时
    ui->textEdit->appendPlainText("**client socket connected");
    ui->textEdit->appendPlainText("**peer address:"+
                                 tcpSocket->peerAddress().toString());
    ui->textEdit->appendPlainText("**peer port:"+
                                 QString::number(tcpSocket->peerPort()));
}

/* do_clientDisconnected */
void MainWindow::do_clientDisconnected()
{//客户端断开连接时
    ui->textEdit->appendPlainText("**client socket disconnected");
    tcpSocket->deleteLater();
}

/* do_socketStateChange */
void MainWindow::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//socket状态变化时
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        labSocketState->setText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        labSocketState->setText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        labSocketState->setText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        labSocketState->setText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        labSocketState->setText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        labSocketState->setText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        labSocketState->setText("socket状态：ListeningState");
    }
}

// 停止监听槽函数
void MainWindow::on_actStop_triggered()
{//"停止监听"按钮
    if (tcpServer->isListening())   //tcpServer正在监听
    {
        if (tcpSocket != nullptr)
            if (tcpSocket->state()==QAbstractSocket::ConnectedState)
                tcpSocket->disconnectFromHost();
        tcpServer->close();         //停止监听
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        labListen->setText("监听状态：已停止监听");
    }
}
```

### 2.2.3 与 TCP Client 的数据通信

> **socket 之间的数据通信协议一般有两种---基于行或基于块的。** 基于行的数据通信协议一般用于纯文本数据的通信，每一行数据以一个换行符结束。函数 canReadLine() 判断是否有新的一行数据需要读取，如果有就可以用函数 readLine() 读取一行数据。基于块的数据通信协议一般用于*二进制* 数据的传输，需要自定义具体的格式。

```C++
void MainWindow::on_btnSend_clicked()
{//"发送消息"按钮，发送一行字符串，以换行符结束
    QString  msg=ui->editMsg->text();
    ui->textEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');       //添加一个换行符
    tcpSocket->write(str);
}
```

程序将 QString 类型的字符串转换为 QByteArray 类型的数组 str，然后在 str 最后面添加一个换行符，用 QIODevice::writer() 函数将 str 写入缓冲区，这样就能向客户端发送一行文字。

QTcpSocket 对象接受到数据后会发射 readyRead() 信号，在槽函数 do_newConnection() 中已经将这个信号与槽函数 do_socketReadyRead() 连接。

```C++
void MainWindow::do_socketReadyRead()
{//读取缓冲区的行文本
    while(tcpSocket->canReadLine())
        ui->textEdit->appendPlainText("[in] "+tcpSocket->readLine());
}
```



**TCP 客户端程序设计**

```C++
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QTcpSocket  *tcpClient;   //socket
    QLabel  *labSocketState;  //状态栏显示标签

    QString  getLocalIP();    //获取本机IP地址
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
//自定义槽函数
    void    do_connected();
    void    do_disconnected();
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_socketReadyRead();//读取socket传入的数据
//
    void on_actConnect_triggered();

    void on_actDisconnect_triggered();

    void on_actClear_triggered();

    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;
};
```

```C++
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpClient=new QTcpSocket(this);    //创建socket变量

    labSocketState=new QLabel("Socket状态：");  //状态栏标签
    labSocketState->setMinimumWidth(250);
    ui->statusBar->addWidget(labSocketState);

    QString localIP=getLocalIP();      //获取本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);
    ui->comboServer->addItem(localIP);


    connect(tcpClient,SIGNAL(connected()),   this,SLOT(do_connected()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(do_disconnected()));

    connect(tcpClient,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);

    connect(tcpClient,SIGNAL(readyRead()),   this,SLOT(do_socketReadyRead()));
}
```

自定义函数 `getLocalIP()` 与 TCP Server 中完全一样。

```C++
void MainWindow::on_actConnect_triggered()
{//“连接服务器”按钮
    QString   addr=ui->comboServer->currentText();
    quint16   port=ui->spinPort->value();
    tcpClient->connectToHost(addr,port);
    //    tcpClient->connectToHost(QHostAddress::LocalHost,port);
}

void MainWindow::on_actDisconnect_triggered()
{//“断开连接”按钮
    if (tcpClient->state()==QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
}

void MainWindow::on_actClear_triggered()
{
    ui->textEdit->clear();
}

void MainWindow::do_connected()
{ //connected()信号的槽函数
    ui->textEdit->appendPlainText("**已连接到服务器");
    ui->textEdit->appendPlainText("**peer address:"+
                                   tcpClient->peerAddress().toString());
    ui->textEdit->appendPlainText("**peer port:"+
                                   QString::number(tcpClient->peerPort()));
    ui->actConnect->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
}

void MainWindow::do_disconnected()
{//disConnected()信号的槽函数
    ui->textEdit->appendPlainText("**已断开与服务器的连接");
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}

void MainWindow::do_socketReadyRead()
{//readyRead()信号的槽函数
    while(tcpClient->canReadLine())
        ui->textEdit->appendPlainText("[in] "+tcpClient->readLine());
}
```

槽函数 do_socketStateChange() 的功能和代码与 TCP Server 中完全一样。

```C++
void MainWindow::on_btnSend_clicked()
{//“发送数据”按钮
    QString  msg=ui->editMsg->text();
    ui->textEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');
    tcpClient->write(str);
}
```



# 3. UDP 通信

用户数据报协议（UDP）是轻量的、不可靠的、面向数据报的、无连接的协议，它可以用于对可靠性要求不高的场合。与 TCP 不同，**UDP 通信不区分客户端与服务器** ， **UDP 程序都是客户端程序。** 无需预先建立持久的 socket 连接，**UDP 客户端每次发送数据都需要指定目标地址和端口。**

![image-20230908091410557](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309080914631.png)

UDP 消息传送有三种模式，分别是单播、广播和组播三种模式。

1. 单播： 单播用于两个主机之间的端对端通信，需要知道对方的 IP 地址与端口
2. 广播： 广播 UDP 与单播 UDP 的区别就是 IP 地址不同，广播一般使用广播地址 255.255.255.255，将消息发送到在同一广播（也就是局域网内同一网段） 网络上的每个主机
3. 组播：组播（多点广播），也称为多播，将网络中同一业务类型主机进行了逻辑上的分组，进行数据收发的时候其数据仅仅在同一分组中进行，其他的主机没有加入此分组不能收发对应的数据

Qt 中 Socket 类之间的关系：

![image-20230908091810415](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309080918519.png)

## 3.1 QUdpSocket 常用 API

```C++
QUdpSocket::QUdpSocket(QObject *parent = Q_NULLPTR);   // 构造函数

bool QUdpSocket::hasPendingDatagrams() const;   // 如果至少有一个数据报在等待被读取，则返回true，否则返回false。

qint64 QUdpSocket::pendingDatagramSize() const;  // 返回第一个待处理的UDP数据报的大小Byte。如果没有可用的数据报，该函数返回-1。

qint64 QUdpSocket::readDatagram(char *data, qint64 maxSize, QHostAddress *address = Q_NULLPTR, quint16 *port = Q_NULLPTR);  // 接收一个不大于maxSize字节的数据报并将其存储在data中。发送者的主机地址和端口存储在*address和*port中（除非指针为0）。成功时返回数据报的大小；否则返回-1。如果maxSize太小，数据报的其余部分将被丢失。为了避免数据丢失，在试图读取数据报之前，应调用pendingDatagramSize()来确定未决数据报的大小。如果maxSize为0，数据报将被丢弃。

qint64 QUdpSocket::writeDatagram(const char *data, qint64 size, const QHostAddress &address, quint16 port);  // 将数据报以大小的方式发送到端口端口的主机地址。成功时返回发送的字节数，否则返回-1。
/* 数据报总是被写成一个块。数据报的最大尺寸与平台高度相关，但可以低至8192字节。如果数据报太大，这个函数将返回-1，error()将返回DatagramTooLargeError。
一般来说，发送大于512字节的数据报是不利的，因为即使它们被成功发送，在到达最终目的地之前，它们很可能被IP层分割开来。 */
```



## 3.2 UDP 通信程序

程序界面

![image-20230908093010295](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309080930375.png)

### 3.2.1 UDP 服务端

```C++
#ifndef UDPSERVER_H
#define UDPSERVER_H
 
#include <QWidget>
#include <QtNetwork>
 
QT_BEGIN_NAMESPACE
namespace Ui { class UdpServer; }
QT_END_NAMESPACE
 
class UdpServer : public QWidget
{
    Q_OBJECT
 
public:
    UdpServer(QWidget *parent = nullptr);
    ~UdpServer();
 
private slots:
    void on_pushButton_start_clicked();
 
    void on_pushButton_send_clicked();
    void readPendingDatagrams();
 
private:
    Ui::UdpServer *ui;
    //Udp服务器
    QUdpSocket *mSocket;
    //通信的ip和端口，用于获取发送者的 IP 和端口
    QHostAddress addr;
    quint16 port;
};
#endif // UDPSERVER_H
```

```C++
#include "udpserver.h"
#include "ui_udpserver.h"
 
UdpServer::UdpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UdpServer)
{
    ui->setupUi(this);
}
 
UdpServer::~UdpServer()
{
    delete ui;
}
 
//启动
void UdpServer::on_pushButton_start_clicked()
{
    //1.创建QUdpSocket对象
    mSocket = new QUdpSocket(this);
 
    //2.连接接收数据信号和槽
    QObject::connect(mSocket,&QUdpSocket::readyRead,this,&UdpServer::readPendingDatagrams);
 
 
    //3.绑定
    mSocket->bind(QHostAddress::Any,ui->spinBox->value());
 
    //连接回车发送的信号和槽
    QObject::connect(ui->lineEdit,&QLineEdit::returnPressed,this,&UdpServer::on_pushButton_send_clicked);
 
    //禁止端口号和启动按钮
    ui->spinBox->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
}
 
void UdpServer::on_pushButton_send_clicked()
{
    //获取发送的数据
    QByteArray arr = ui->lineEdit->text().toUtf8();
 
    //发送
    mSocket->writeDatagram(arr,addr,port);
    //显示发送的内容
    ui->textBrowser->insertPlainText("send:"+QString(arr)+"\n");
    //情况lineEdit
    ui->lineEdit->clear();
}
 
void UdpServer::readPendingDatagrams()
{
    //数据缓冲区
    QByteArray arr;
    while(mSocket->hasPendingDatagrams())
    {
 
        //调整缓冲区的大小和收到的数据大小一致
        arr.resize(mSocket->bytesAvailable());
 
        //接收数据
        mSocket->readDatagram(arr.data(),arr.size(),&addr,&port);
        //显示
        ui->textBrowser->insertPlainText(addr.toString()+":"+QString(arr)+"\n");
 
        //使能发送按钮和编辑框
        ui->lineEdit->setEnabled(true);
        ui->pushButton_send->setEnabled(true);
    }
}
```



[Qt中的基础数据类型 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/qt-basetype/)

将 QByteArray() 类型的字符串，转换为 char* 。使用 `QByteArray::data()` 



### 3.2.2 UDP 客户端

```C++
#ifndef UDPCILENT_H
#define UDPCILENT_H
 
#include <QWidget>
#include <QtNetwork>
 
QT_BEGIN_NAMESPACE
namespace Ui { class UdpCilent; }
QT_END_NAMESPACE
 
class UdpCilent : public QWidget
{
    Q_OBJECT
 
public:
    UdpCilent(QWidget *parent = nullptr);
    ~UdpCilent();
 
private slots:
    void on_pushButton_send_clicked();
    void readPendingDatagrams();
 
private:
    Ui::UdpCilent *ui;
    //UDP客户端
    QUdpSocket *mSocket;
};
#endif // UDPCILENT_H
```

```C++
#include "udpcilent.h"
#include "ui_udpcilent.h"
 
UdpCilent::UdpCilent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UdpCilent)
{
    ui->setupUi(this);
    //1.创建QUdpSocket
    mSocket = new QUdpSocket(this);
    //2.通信(接收)
    QObject::connect(mSocket,&QUdpSocket::readyRead,this,&UdpCilent::readPendingDatagrams);
 
    //连接回车发送的信号和槽
    QObject::connect(ui->lineEdit_send,&QLineEdit::returnPressed,this,&UdpCilent::on_pushButton_send_clicked);
 
}
 
UdpCilent::~UdpCilent()
{
    delete ui;
}
 
//发送
void UdpCilent::on_pushButton_send_clicked()
{
    //获取发送的数据
    QByteArray arr = ui->lineEdit_send->text().toUtf8();
 
    //发送
    //mSocket->writeDatagram(arr,QHostAddress(ui->lineEdit_ip->text()),ui->spinBox->value());
    mSocket->writeDatagram(arr,QHostAddress::Broadcast,ui->spinBox->value());
    //显示发送的内容
    ui->textBrowser->insertPlainText("send:"+QString(arr)+"\n");
    //清空lineEdit
    ui->lineEdit_send->clear();
}
 
void UdpCilent::readPendingDatagrams()
{
    QHostAddress addr; //用于获取发送者的 IP 和端口
    quint16 port;
    //数据缓冲区
    QByteArray arr;
    while(mSocket->hasPendingDatagrams())
    {
        //调整缓冲区的大小和收到的数据大小一致
        arr.resize(mSocket->bytesAvailable());
 
        //接收数据
        mSocket->readDatagram(arr.data(),arr.size(),&addr,&port);
        //显示
        ui->textBrowser->insertPlainText(addr.toString()+":"+QString(arr)+"\n");
    }
}
```



[如何正确使用QTcpSocket的readyRead信号？_readyread()信号_燕山暮雪的博客-CSDN博客](https://blog.csdn.net/thebulesky/article/details/119605065?ops_request_misc=%7B%22request%5Fid%22%3A%22169413802816800182171978%22%2C%22scm%22%3A%2220140713.130102334..%22%7D&request_id=169413802816800182171978&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-119605065-null-null.142^v93^insert_down28v1&utm_term=readyread信号&spm=1018.2226.3001.4187)



# 4. 基于 HTTP 的网络应用程序

Qt 网络模块提供一些类来实现 OSI 七层网络模型中高层的网络协议，如 HTTP、FTP、SNMP 等，这些类主要是 `QNetworkRequest、 QNetworkAccessManager、 QNetworkReply` 。

QNetworkRequest 类通过 URL 发起网络协议请求，其也保存网络请求的信息，目前支持 HTTP、FTP 和本地文件 URL 的下载或上传。

QNetworkAccessManager 类用于协调网络操作，在 QNetworkRequest 发起网络请求后，QNetworkAccessManager 负责发送网络请求，以及创建网络响应。

QNetworkReply 类表示网络请求的响应，由 QNetworkAccessManager 在发送网络请求后创建网络响应。QNetwrokReply 提供信号 **finished()、readyRead()、downloadProgress()** 可用于监测网络响应的执行情况，从而进行相应的操作。**QNetworkReply 是 QIODevice 的子类，所以 QNetworkReply 支持流数据读写功能。**



程序界面

![image-20230908222752107](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309082227170.png)



```C++
#include <QMainWindow>

class QNetworkAccessManager;
class QNetworkReply;
class QFile;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;  // 网络管理
    QNetworkReply *reply;   // 网络响应
    QFile *downloadFile;

private slots:
    void do_finished();
    void do_readyRead();
    void do_downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void on_btnDownload_clicked();
    void on_btnDefaultPath_clicked();
};
```



```C++
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::do_finished()
{// 网络响应结束
    QFileInfo fileInfo(downloadFile->fileName());   // 获取下载的文件的文件名
    downloadFile->close();
    delete downloadFile;   // 删除临时文件对象
    reply->deleteLater();  // 由主事件循环删除此对象
    if (ui->checkBox->isChecked()) QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));  // 打开下载的文件
    ui->btnDownload->setEnabled(true);
}

void MainWindow::do_readyRead()
{// 读取下载的数据
    downloadFile->write(reply->readAll());
}

void MainWindow::do_downloadProgress(qint64 bytesRead, qint64 totalBytes)
{// 下载进度
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}


void MainWindow::on_btnDownload_clicked()
{// “下载按钮” ，开始下载
    QString urlSpace = ui->editURL->text().trimmed();  // 去掉路径中的空格
    if (urlSpace.isEmpty()) {
        QMessageBox::information(this, "错误", "请指定需要下载的 URL ");
        return;
    }
    QUrl newUrl = QUrl::fromUserInput(urlSpace);   // URL
    if (!newUrl.isValid()) {
        QString info = "无效 URL：" + urlSpace +"\n错误信息：" + newUrl.errorString();
        QMessageBox::information(this, "错误", info);
        return;
    }
    QString tempDir = ui->editSaveFile->text().trimmed();   // 临时目录
    if (tempDir.isEmpty()) {
        QMessageBox::information(this, "错误", "请指定保存下载文件的目录");
        return;
    }

    QString fullFileName = tempDir + newUrl.fileName();  // 文件名
    if (QFile::exists(fullFileName)) QFile::remove(fullFileName);
    downloadFile = new QFile(fullFileName);   // 创建临时文件
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, "错误", "临时文件打开错误");
        return;
    }
    ui->btnDownload->setEnabled(false);
    // 发送网络请求，创建网络响应
    reply = networkManager->get(QNetworkRequest(newUrl));
    connect(reply, SIGNAL(readyRead()), this, SLOT(do_readyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(do_downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(do_finished()));
}


void MainWindow::on_btnDefaultPath_clicked()
{// “默认路径” 按钮
    QString curPath = QDir::currentPath();
    QDir dir(curPath);
    QString sub = "temp";
    ui->editSaveFile->setText(curPath + "/" + sub + "/");
}
```

