#多线程

QThread 是实现`多线程`的核心类，一般从 QThread 继承定义**自己的线程类**。线程之间的同步是线程交互的主要问题，Qt 提供了 QMutex、QWaitCondition、QSemaphore 等多个类用于实现线程同步。



[Qt多线程---零声教育](https://zhuanlan.zhihu.com/p/634432390)



# 1. 使用 QThread 创建多线程程序

QThread 类提供不依赖平台的管理线程的方法，如果要设计多线程程序，一般是从 QThread **继承定义一个线程类**，在自定义的类里进行任务处理。

## 1.1 QThread 类简介

一个 QThread 类的对象管理一个线程。在设计多线程程序的时候，需要从 QThread 继承定义线程类，并重定义 QThread 的虚函数 run()，**在函数 run() 里处理线程的事件循环。**

函数 run() 的程序体一般是一个*无限循环*，可以在函数 run() 里调用函数 exit() 或 quit() 结束线程的事件循环，或主线程里调用函数 terminate() 强制结束线程。

* 常用的公共成员函数

  ```c++
  // QThread 类常用 API
  // 构造函数
  QThread::QThread(QObject *parent = Q_NULLPTR);
  // 判断线程中的任务是不是处理完毕了
  bool QThread::isFinished() const;
  // 判断子线程是不是在执行任务
  bool QThread::isRunning() const;
  
  // Qt中的线程可以设置优先级
  // 得到当前线程的优先级
  Priority QThread::priority() const;
  void QThread::setPriority(Priority priority);
  优先级:
      QThread::IdlePriority         --> 最低的优先级
      QThread::LowestPriority
      QThread::LowPriority
      QThread::NormalPriority
      QThread::HighPriority
      QThread::HighestPriority
      QThread::TimeCriticalPriority --> 最高的优先级
      QThread::InheritPriority      --> 子线程和其父线程的优先级相同, 默认是这个
  // 退出线程, 停止底层的事件循环
  // 退出线程的工作函数
  void QThread::exit(int returnCode = 0);
  // 调用线程退出函数之后, 线程不会马上退出因为当前任务有可能还没有完成, 调回用这个函数是
  // 等待任务完成, 然后退出线程, 一般情况下会在 exit() 后边调用这个函数
  bool QThread::wait(unsigned long time = ULONG_MAX);
  ```

* 信号槽

  ```c++
  // 和调用 exit() 效果是一样的
  // 调用这个函数之后, 再调用 wait() 函数
  [slot] void QThread::quit();
  // 启动子线程
  [slot] void QThread::start(Priority priority = InheritPriority);
  // 线程退出, 可能是会马上终止线程, 一般情况下不使用这个函数
  [slot] void QThread::terminate();
  
  // 线程中执行的任务完成了, 发出该信号
  // 任务函数中的处理逻辑执行完毕了
  [signal] void QThread::finished();
  // 开始工作之前发出这个信号, 一般不使用
  [signal] void QThread::started();
  ```

* 静态函数

  ```c++
  // 返回一个指向管理当前执行线程的QThread的指针
  [static] QThread *QThread::currentThread();
  // 返回可以在系统上运行的理想线程数 == 和当前电脑的 CPU 核心数相同
  [static] int QThread::idealThreadCount();
  // 线程休眠函数
  [static] void QThread::msleep(unsigned long msecs);	// 单位: 毫秒
  [static] void QThread::sleep(unsigned long secs);	// 单位: 秒
  [static] void QThread::usleep(unsigned long usecs);	// 单位: 微秒
  ```

* 任务处理函数

  ```c++
  // 子线程要处理什么任务, 需要写到 run() 中
  [virtual protected] void QThread::run();
  ```

  [Qt多线程---苏丙榅]([Qt中多线程的使用 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/thread/))



## 1.2 掷骰子的多线程应用程序

项目图片

![image-20230905225821967](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309052258067.png)

创建项目后，创建一个新的 C++ 类，继承 QThread：

```c++
/* tdicethread.h */
class TDiceThread : public QThread
{
    Q_OBJECT
public:
    explicit TDiceThread(QObject *parent = nullptr);
    void diceBegin();      // 开始掷骰子
    void dicePause();      // 暂停
    void stopThread();     // 结束线程

signals:
    void newValue(int seq, int diceValue);   // 产生新点数的信号

private:
    int m_seq = 0;      // 掷骰子次数的序号
    int m_diceValue;    // 骰子点数
    bool m_paused = true;  // 暂停掷骰子
    bool m_stop = false;   // 停止线程，也就是退出 run() 函数的运行

protected:
    void run() override;    // 线程的任务
};
```

```c++
/* tdicethread.cpp */
TDiceThread::TDiceThread(QObject *parent) : QThread{parent}
{

}

void TDiceThread::diceBegin()
{
    m_paused = false;
}

void TDiceThread::dicePause()
{
    m_paused = true;
}

void TDiceThread::stopThread()
{
    m_stop = true;
}

void TDiceThread::run()
{
    m_stop = false;
    m_paused = true;       // 启动线程后先暂停掷骰子
    m_seq = 0;
    while (!m_stop) {
        if (!m_paused) {
            m_diceValue = QRandomGenerator::global()->bounded(1, 7);     // 生成1-7随机数
            m_seq++;
            emit newValue(m_seq, m_diceValue);   // 每次新掷骰子后，发射信号
        }
        msleep(500);     // 线程休眠 500 ms
    }
    quit();        // 在 m_stop = true 时，退出线程
}
```

```c++
/* mainwindow.h */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TDiceThread *threadA;   // 工作线程

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void do_threadA_started();
    void do_threadA_finished();
    void do_threadA_newValue(int seq, int diceValue);
    
    void on_startAction_triggered();
    void on_endAction_triggered();
    void on_beginAction_triggered();
    void on_suspendAction_triggered();
};
```

```c++
/* mainwindow.cpp */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    threadA = new TDiceThread(this);

    connect(threadA, &TDiceThread::started, this, &MainWindow::do_threadA_started);
    connect(threadA, &TDiceThread::finished, this, &MainWindow::do_threadA_finished);
    connect(threadA, &TDiceThread::newValue, this, &MainWindow::do_threadA_newValue);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 确保窗口被关闭时，线程被停止
    if (threadA->isRunning()) {
        threadA->terminate();   // 强制终止线程
        threadA->wait();    // 等待线程结束
    }
    event->accept();
}

void MainWindow::do_threadA_started()
{
    ui->statusbar->showMessage("线程状态：Thread started");
    ui->startAction->setEnabled(false);
    ui->endAction->setEnabled(true);
    ui->beginAction->setEnabled(true);
}

void MainWindow::do_threadA_finished()
{
    ui->statusbar->showMessage("线程状态：Thread finished");
    ui->startAction->setEnabled(true);
    ui->endAction->setEnabled(false);
    ui->beginAction->setEnabled(false);
    ui->suspendAction->setEnabled(false);
}

void MainWindow::do_threadA_newValue(int seq, int diceValue)
{
    QString str = QString("第%1 次掷骰子，点数为%2").arg(seq).arg(diceValue);
    ui->textBrowser->append(str);
    QString fileName = QString::asprintf(":/images/d%d.jpg", diceValue);
    QPixmap pic(fileName);
    ui->label->setPixmap(pic);
}


void MainWindow::on_startAction_triggered()
{
    threadA->start();
}


void MainWindow::on_endAction_triggered()
{
    threadA->stopThread();
}


void MainWindow::on_beginAction_triggered()
{
    // 开始掷骰子
    threadA->diceBegin();
    ui->beginAction->setEnabled(false);
    ui->suspendAction->setEnabled(true);
}


void MainWindow::on_suspendAction_triggered()
{
    // 暂停掷骰子
    threadA->dicePause();
    ui->beginAction->setEnabled(true);
    ui->suspendAction->setEnabled(false);
}
```



# 2. 线程同步

[[线程同步]]

在多线程程序中，由于存在多个线程，**线程之间可能需要访问同一个变量，或一个线程需要等待另一个线程完成某个操作后才产生相应的动作。**

例如，工作线程生成随机的骰子点数，主线程读取骰子点数并显示，主线程需要等待工作线程生成一个新的骰子点数后再读取数据。程序中使用了信号与槽机制，在生成新的点数之后通过信号通知主线程读取新的数据。

如果不使用信号与槽且改变了计算变量 m_diceValue 的值的方法，TDiceThread 的函数 run() 的代码如下：

```c++
void TDiceThread::run()
{
    m_stop = false;
    m_paused = true;       // 启动线程后先暂停掷骰子
    m_seq = 0;
    while (!m_stop) {
        if (!m_paused) {
            m_dicaValue = 0;
            for (int i = 0; i < 5; ++i) {
                // 生成1-7随机数
                m_diceValue += QRandomGenerator::global()->bounded(1,7);
            }
            m_diceValue = m_diceValue/5;
            m_seq++;
        }
        msleep(500);     // 线程休眠 500 ms
    }
    quit();        // 在 m_stop = true 时，退出线程
}
```

TDiceThread 类需要定义一个公有函数返回 m_diceValue 的值，以便在主线程中调用此函数读取骰子的点数，如：

```c++
int TDiceThread::diceValue() { return m_diceValue; }
```

**由于没有信号与槽的机制，主线程只能采用不断主动查询的方式是否有新数据，并在有新数据时读取新数据。但是主线程调用函数 diceValue() 读取数据的同时，工作线程可能正在运行 run() 函数中修改 m_diceValue 值的语句，即可能运行下面的<u>关键代码</u>**：

```c++
m_dicaValue = 0;
for (int i = 0; i < 5; ++i) {
    // 生成1-7随机数
    m_diceValue += QRandomGenerator::global()->bounded(1,7);
}
m_diceValue = m_diceValue/5;
m_seq++;
```

此时这段数据需要被保护。

## 2.1 基于互斥量的线程同步

### 2.1.1 互斥量的原理和功能

QMutex 和 QMutexLocker 是基于互斥量的线程同步类，QMutex 定义的实例是互斥量，QMutex 主要有以下几个函数：

```c++
void QMutex::lock();    // 锁定互斥量，一直等待
void QMutex::unlock();  // 解锁互斥量
void QMutex::trylock(); // 尝试锁定互斥量，不等待
void QMutex::tryLock(int timeout);  // 尝试锁定互斥量，最多等待 timeout 毫秒
```

函数 lock() 锁定互斥量，如果另一个线程锁定了这个互斥量，它将被阻塞运行知道其他线程解锁这个互斥量。函数 unlock() 解锁互斥量，需要与 lock() 配对使用。

**互斥量相当于一把钥匙，如果两个线程要访问同一个共享资源，需要通过 lock() 或 tryLock() 拿到这把钥匙，然后才可以访问该共享资源，访问完之后还要通过 unlock() 还回钥匙，这样别的线程才有机会拿到钥匙**。

改写上述掷骰子项目，定义一个 QMutex 变量，删除自定义信号 newValue()，增加一个函数 readValue()。

```c++
bool TDiceThread::readValue(int *seq, int *diceValue)
{
    if (mutex.tryLock(100)) {
        // 尝试解锁互斥量，等待 100 ms
        *seq = m_seq;
        *diceValue = m_diceValue;
        mutex.unlock();   // 解锁互斥量
        return true;
    }
    return false;
}

void TDiceThread::run()
{
    m_stop = false;
    m_paused = true;       // 启动线程后先暂停掷骰子
    m_seq = 0;
    while (!m_stop) {
        if (!m_paused) {
            mutex.lock();   // 锁定互斥量
            m_diceValue = 0;
            for (int i = 0; i < 5; ++i) {
                // 生成1-7随机数
                m_diceValue += QRandomGenerator::global()->bounded(1,7);
            }
            m_diceValue = m_diceValue/5;
            m_seq++;
            mutex.unlock();  // 解锁互斥量
        }
        msleep(500);     // 线程休眠 500 ms
    }
    quit();        // 在 m_stop = true 时，退出线程
}
```

### 2.1.2 QMutexLocker 类

QMutex 需要函数 lock() 和 unlock() 配对使用来实现代码片段的保护，在一些逻辑复杂的代码片段或可能发生异常的代码中，配对可能出错。

> QMutexLocker 是另一个简化了互斥量处理的类。QMutexLocker 的构造函数接受互斥量作为参数并将其锁定，QMutexLocker 的析构函数则将此互斥量解锁，所以在 QMutexLocker 实例变量的生存期内的代码片段会得到保护，**自动进行互斥量的锁定和解锁**。

```c++
QMutexLocker locker(&mutex);   // 锁定 mutex ，超出 if 语句范围就解锁，因为 locker 对象不存在了
m_diceValue = 0;
for (int i = 0; i < 5; ++i) {
    // 生成1-7随机数
    m_diceValue += QRandomGenerator::global()->bounded(1,7);
}
m_diceValue = m_diceValue/5;
m_seq++;
```



## 2.2 基于读写锁的线程同步

使用互斥量时存在一个问题，即每次只能有一个线程获得互斥量的使用权限。如果在一个程序中有多个线程读取某个变量，使用互斥量时必须排队。**而实际上如果只是读取一个变量，可以让多个线程同时访问，这种情况下使用互斥量就会降低程序的性能**。

例如，假设有一个数据采集程序，一个线程负责采集数据到缓冲区，一个线程负责读取缓冲区的数据并显示，还有一个线程负责读取缓冲区的数据并保存到文件：

```c++
int buffer[100];
QMutex mutex;
void ThreadDAQ::run() {
    ...
    mutex.lock();
    get_data_and_write_in_buffer();   // 数据写入 buffer
    mutex.unlock();
    ...
}

void ThreadShow::run() {
    ...
    mutex.lock();
    show_buffer();
    mutex.unlock();
    ...
}

void ThreadSaveFile::run() {
    ...
    mutex.lock();
    save_buffer_toFile();
    mutex.lock();
    ...
}
```

数据缓冲区 buffer 和互斥量 mutex 都是全局变量，ThreadDAQ 线程将数据写入 buffer，ThreadShow 和 ThreadSaveFile 线程只是读取 buffer 的数据，但是由于使用了互斥量，这 3 个线程在任何时候都只能只有一个线程可以访问 buffer。**而实际上，ThreadShow 和 ThreadSaveFile 都只是读取 buffer 的数据，它们同时访问 buffer 时是不会冲突的。**

Qt 提供了读写锁类 QReadWriteLock，它是基于读或写的方式进行代码片段锁定的，**它可以允许多个线程以只读的方式同步，但是只要有一个线程在以写入方式访问资源，其他线程就必须等待，直到写操作结束。**

```C++
void lockForRead();   // 以只读的方式锁定资源，如果有其他线程以写入方式锁定资源，这个函数会被阻塞
void lockForWrite();  // 以写入的方式锁定资源，如果有其他线程以读或写方式锁定资源，这个函数会被阻塞
void unlock();
void tryLockForRead();  // 尝试以只读的方式锁定资源，不等待
void tryLockForRead(int timeout);
void tryLockForWrite();
void tryLockForWrite(int timeout);
```

```C++
int buffer[100];
QReadWriteLock lock;
void ThreadDAQ::run() {
    ...
    Lock.lockForWrite();
    get_data_and_write_in_buffer();   // 数据写入 buffer
    Lock.unlock();
    ...
}

void ThreadShow::run() {
    ...
    Lock.lockForRead();
    show_buffer();
    Lock.unlock();
    ...
}

void ThreadSaveFile::run() {
    ...
    Lock.lockForRead();
    save_buffer_toFile();
    Lock.unlock();
    ...
}
```

QReadLocker 和 QWriteLocker 是 QReadWriteLock 的简便形式，无须与 unlock() 配对使用。

```C++
int buffer[100];
QReadWriteLock Lock;
void ThreadDAQ::run() {
    ...
    QWriteLocker locker(&Lock);
    get_data_and_write_in_buffer();   // 数据写入 buffer
    ...
}

void ThreadShow::run() {
    ...
    QReadLocker locker(&Lock);
    show_buffer();
    ...
}

void ThreadSaveFile::run() {
    ...
    QReadLocker locker(&Lock);
    save_buffer_toFile();
    ...
}
```



## 2.3 基于条件等待的线程同步

### 2.3.1 QWaitCondition 原理和功能

QWaitCondition 允许线程在某些情况发生时唤醒另外的线程。一个或多个线程可以阻塞等待一 QWaitCondition ,用wakeOne() 或wakeAll() 设置一个条件。wakeOne() 随机唤醒一个，wakeAll() 唤醒所有。

![image-20230906210417678](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309062104761.png)

> QWaitCondition 一般用于生产者/消费者模型，生产者产生数据，消费者使用数据。

### 2.3.2 线程类设计

```C++
/* tdicethread.h */
class TDiceThread : public QThread
{
    Q_OBJECT
public:
    explicit TDiceThread(QObject *parent = nullptr);

private:

protected:
    void run() override;    // 线程的任务
};

// 获取骰子点数
class TValueThread : public QThread {
    Q_OBJECT
    
protected:
    void run() override;
    
public:
    explicit TValueThread(QObject* parent = nullptr);
    
signals:
    void newValue(int seq, int diceValue);
};

// 获取骰子点数，生成相应的图片文件名
class TPictureThread :public QThread {
    Q_OBJECT
    
protected:
    void run() override;
    
public:
    explicit TPictureThread(QObject * parent = nullptr);
    
signals:
    void newPicture(QString &picName);
};
```

```C++
/* tdicethread.cpp */
// 文件类的全局变量
QReadWriteLock rwLocker;    // 读写锁
QWaitCondition waiter;   // 等待条件
int seq =0, diceValue=0;

void TDiceThread::run()
{
     seq = 0;
    while (1) {
        rwLocker.lockForWrite();  // 以写入的方式锁定
        diceValue = QRandomGenerator::global()->bounded(1,7);
        seq++;
        rwLocker.unlock();  // 解锁
        waiter.wakeAll();   // 唤醒其他等待的线程
        msleep(500);     // 线程休眠 500 ms
    }
}

void TPictureThread::run()
{
    while(1) {
        rwLocker.lockForRead();   // 以只读的方式锁定
        waiter.wait(&rwLocker);   // 等待被唤醒
        QString filename = QString::asprintf(":/images/d%d.jpg", diceValue);
        emit newPicture(filename);
        rwLocker.unlock();  // 解锁
    }
}

void TValueThread::run()
{
    while(1) {
        rwLocker.lockForRead();  // 以只读的方式锁定
        waiter.wait(&rwLocker);  // 等待被唤醒
        emit newValue(seq, diceValue);
        rwLocker.unlock();   // 解锁
    }
}
```

> 在设计多线程程序时，应优先考虑**信号与槽**机制，无法使用信号与槽时再考虑其他 ITC 技术。



## 2.4 基于信号量的线程同步

QSemaphore 是 Qt 框架中提供的一个用于多个线程同步和数据存取的信号量类。与互斥锁（QMutex）和条件变量（QWaitCondition）一样，QSemaphore 也是一个跨平台通用的同步机制。

QSemaphore 类提供了一种通用计数信号量。信号量是互斥锁的一种泛化应用，与互斥锁不同的是，信号量可以被多次获取。信号量通常用于保护一定数量的相同资源，而且这些资源可以被多个线程共享。

QSemaphore 更加灵活，它并不需要像互斥锁那样使用 lock() 和 unlock()、tryLock() 等方法来实现加锁解锁操作。QSemaphore 主要通过其 count() 方法控制在某时刻同时运行的等待线程数量和可执行的线程数量，可以用来实现资源池管理、限流等功能。

```C++
QSemaphore(int n = 0);   // 构造函数，设置资源数为 n，默认情况下 n = 0
void acquire(int n = 1); // 尝试获得 n 个资源，阻塞等待
int available();         // 返回当前信号量可用资源个数
void release(int n = 1); // 释放 n 个资源
bool tryAcquire(int n = 1); // 尝试获取 n 个资源，不等待
bool tryAcquire(int n, int timeout);  // 尝试获取 n 个资源，最多等待 timeout 毫秒
```

创建一个 QSemaphore 对象时可以设置一个初始值，表示可用资源的个数。函数 acquire() 以阻塞等待方式获得一个资源，信号量的资源数减 1；函数 release() 释放一个资源，信号量的资源数加 1；函数 available() 返回信号量当前可用的资源个数。

```C++
QSemaphore sem(5);      // sem.available() == 5  

sem.acquire(3);         // sem.available() == 2  // 成功进去 3 个
sem.acquire(2);         // sem.available() == 0  // 又进去 2 个
sem.release(5);         // sem.available() == 5   // 出来了 5 个
sem.release(5);         // sem.available() == 10  // 增加了 5 个

sem.tryAcquire(1);      // sem.available() == 9, returns true
sem.tryAcquire(250);    // sem.available() == 9, returns false
```

---

双缓冲区数据采集程序：

```C++
/* mythread.h */
class TDaqThread : public QThread
{
    Q_OBJECT
protected:
    bool    m_stop=false;
    void    run();          //线程的任务函数
public:
    explicit TDaqThread(QObject *parent = nullptr);
    void    stopThread();   //结束线程run()函数的运行
};

//TProcessThread 数据处理线程
class TProcessThread : public QThread
{
    Q_OBJECT
protected:
    bool    m_stop=false;
    void    run();          //线程的任务函数
public:
    explicit TProcessThread(QObject *parent = nullptr);
    void    stopThread();   //结束线程run()函数的运行
signals:
    void    dataAvailable(int bufferSeq, int* bufferData, int pointCount);
};
```

```C++
/* mythread.cpp */
#include    <QSemaphore>
#define  BUF_SIZE  10   //缓冲区数据点个数

int buffer1[BUF_SIZE];
int buffer2[BUF_SIZE];
int curBufNum=1;        //当前正在写入的缓冲区编号
int bufSeq=0;           //缓冲区序号

QSemaphore emptyBufs(2);  //信号量，空的缓冲区个数，初始资源个数为2
QSemaphore fullBufs;      //信号量，满的缓冲区个数，初始资源为0

TDaqThread::TDaqThread(QObject *parent): QThread{parent}
{

}

void TDaqThread::stopThread()
{
    m_stop =true;
}

void TDaqThread::run()
{
    curBufNum=1;     //当前正在写入的缓冲区编号
    bufSeq=0;        //缓冲区序号
    int counter=0;   //模拟数据
    int n=emptyBufs.available();
    if (n<2)         //保证线程启动时emptyBufs.available()==2
        emptyBufs.release(2-n);

    m_stop =false;
    while(!m_stop)
    {
        emptyBufs.acquire();    //获取一个空的缓冲区
        int *buf = curBufNum==1? buffer1:buffer2;   //设置当前缓冲区指针
        for(int i=0; i<BUF_SIZE; i++)   //产生一个缓冲区的数据
        {
            *buf=counter;
            buf++;
            counter++;
            msleep(10);
        }
        bufSeq++;        //缓冲区序号
        curBufNum = curBufNum==1? 2:1;   //切换当前写入缓冲区编号
        fullBufs.release();     //fullBufs释放1个资源，有了一个满的缓冲区
    }
}


void TProcessThread::run()
{
    int n=fullBufs.available();
    if (n>0)
        fullBufs.acquire(n);  //将fullBufs可用资源个数初始化为0

    int bufData[BUF_SIZE];
    m_stop =false;
    while(!m_stop)
        if (fullBufs.tryAcquire(1,50))     //尝试获取1个资源，最多等待50ms
        {
            int  *bufferFull= curBufNum==1? buffer2:buffer1;    //获取已满缓冲区的指针
            for(int i=0; i<BUF_SIZE; i++, bufferFull++)         //模拟数据处理
                bufData[i]=*bufferFull;
            emptyBufs.release();    //emptyBufs释放一个资源，可用空缓冲区个数加1

            int  pointCount=BUF_SIZE;
            emit dataAvailable(bufSeq, bufData,pointCount);  //发射信号
        }
}

TProcessThread::TProcessThread(QObject *parent):QThread{parent}
{

}

void TProcessThread::stopThread()
{
    m_stop =true;
}
```

```C++
/* mainwindow.h */
#include    <QMainWindow>
#include    <QTimer>
#include    "mythread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    TDaqThread        *threadDAQ;    //数据采集线程
    TProcessThread    *threadShow;   //数据处理线程
protected:
    void    closeEvent(QCloseEvent *event);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void    do_threadA_started();
    void    do_threadA_finished();

    void    do_readBuffer(int bufferSeq, int* bufferData, int pointCount);
};
```

```C++
/* mainwindow.cpp */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (threadDAQ->isRunning())
        threadDAQ->stopThread();   //结束线程

    if (threadShow->isRunning())
        threadShow->stopThread();   //结束线程

    event->accept();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->plainTextEdit);

    threadDAQ= new TDaqThread(this);        //数据采集线程
    threadShow= new TProcessThread(this);   //数据处理线程

    connect(threadDAQ,&TDaqThread::started, this, &MainWindow::do_threadA_started);
    connect(threadDAQ,&TDaqThread::finished,this, &MainWindow::do_threadA_finished);
    connect(threadShow,&TProcessThread::dataAvailable, this, &MainWindow::do_readBuffer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::do_threadA_started()
{//与线程的started()信号关联
    ui->statusbar->showMessage("Thread状态：thread started");
    ui->actThread_Run->setEnabled(false);
    ui->actThread_Quit->setEnabled(true);
}

void MainWindow::do_threadA_finished()
{//与线程的finished()信号关联
    ui->statusbar->showMessage("Thread状态：thread finished");
    ui->actThread_Run->setEnabled(true);
    ui->actThread_Quit->setEnabled(false);
}


void MainWindow::do_readBuffer(int bufferSeq, int *bufferData, int pointCount)
{
    QString  str=QString::asprintf("第 %d 个缓冲区：",bufferSeq);
    for (int i=0; i<pointCount; i++)
    {
        str=str+QString::asprintf("%d, ",*bufferData);
        bufferData++;
    }
    str=str+'\n';

    ui->plainTextEdit->appendPlainText(str);
}

void MainWindow::on_actClear_triggered()
{
    ui->plainTextEdit->clear();
}

void MainWindow::on_actThread_Run_triggered()
{//"启动线程"按钮
    threadShow->start();
    threadDAQ->start();
}

void MainWindow::on_actThread_Quit_triggered()
{//"结束线程"按钮
    threadShow->stopThread();
    threadDAQ->stopThread();
}
```



# 3. 线程池

[线程池---苏丙温](https://subingwen.cn/qt/threadpool/)

在并发编程时，通常会把一些相对比较耗时的操作放进线程中运行，例如写聊天室程序时，多个用户登录服务器，如果没有并发处理的话，多个用户同时收发信息，可能会出现收不到信息或者收信息延迟很久的情况，所以每个用户登录服务器，就为他单独开辟一个线程，这样虽然一定程度上解决了问题，但是大量线程的创建和销毁需要和系统交互，会产生很大的开销。

若需要频繁的创建线程，建议使用线程池。线程池可以维护一定数量的线程，只需要向线程池中丢任务就行，线程池会根据自己可使用线程数进行任务调度。

>可以理解为多个窗口服务，只排一个队列，当有窗口空闲时，队列第一个人过去办理事务；线程池就是这个办事单位，它维护的线程数量就是窗口数量，任务就是排队的人。

## 3.1 QThreadPool

此类为Qt提供的线程池函数，使用此类只需要配置线程池的最大线程数量、线程长时间不使用的过期时间等参数，不需要进行QThread相关的操作。此类有两种使用方式：**全局线程池**和**局部线程池**。

* 全局线程池

  QThreadPool提供了一个静态函数，`globalInstance()`，使用此方法可获取一个当前进程的全局线程池，可在多个类中共同使用一个线程池。

* 局部线程池

  和常规类的使用相同，可以通过`QThreadPool pool;`的方式建立一个局部线程池，并由当前类维护，可保证此线程池仅供当前类应用。

```C++
int activeThreadCount() const //当前的活动线程数量

void clear()//清除所有当前排队但未开始运行的任务

int expiryTimeout() const//线程长时间未使用将会自动退出节约资源，此函数返回等待时间

int maxThreadCount() const//线程池可维护的最大线程数量

void releaseThread()//释放被保留的线程

void reserveThread()//保留线程，此线程将不会占用最大线程数量，从而可能会引起当前活动线程数量大于最大线程数量的情况

void setExpiryTimeout(int expiryTimeout)//设置线程回收的等待时间

void setMaxThreadCount(int maxThreadCount)//设置最大线程数量

void setStackSize(uint stackSize)//此属性包含线程池工作线程的堆栈大小。

uint stackSize() const//堆大小

void start(QRunnable *runnable, int priority = 0)//加入一个运算到队列，注意start不一定立刻启动，只是插入到队列，排到了才会开始运行。需要传入QRunnable ，后续介绍

bool tryStart(QRunnable *runnable)//尝试启动一个

bool tryTake(QRunnable *runnable)//删除队列中的一个QRunnable，若当前QRunnable 未启动则返回成功，正在运行则返回失败

bool waitForDone(int?<i>msecs</i>?=?-1)//等待所有线程运行结束并退出，参数为等待时间-1表示一直等待到最后一个线程退出
```



## 3.2 QRunable

QRunable 类在 Qt 中是所有可运行对象的基类，代表了由 run() 函数表示的一个任务或一段要执行的代码，我们可以将 QRunable 理解为线程池中的任务；

QRunable 类有 run()、autoDelete()、setAutoDelete() 这三个关键函数，如何 autoDelete() 设为 true，QThreadPool 会在 run() 运行结束后自动删除该对象。



## 3.3 线程池实例

```C++
#ifndef MYRUNABLE_H
#define MYRUNABLE_H

#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>

class MyRunable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit MyRunable(QObject *parent = nullptr);
    ~MyRunable();
protected:
    void run();
};

#endif // MYRUNABLE_H
```

```C++
#include "myrunable.h"

MyRunable::MyRunable(QObject *parent) : QObject(parent)
{
}

MyRunable::~MyRunable()
{
    qDebug()<<"delete Task";
}

void MyRunable::run()
{
    int i = 10;
    while(i--)
    {
        qDebug() << "线程id："<< QThread::currentThreadId()<< QString(":剩余%1").arg(i);
        QThread::sleep(1);
    }
}
```

```C++
#include "mainwindow.h"
#include <QApplication>

#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include"myrunable.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    qDebug() << "main thread id: " << QThread::currentThreadId();

    QThreadPool::globalInstance()->setMaxThreadCount(10);   //设置线程池最大线程数量

    MyRunable* task = new MyRunable();
    task->setAutoDelete(true);          //autoDelete属性默认为true   QThreadPool会在run（）函数运行结束后，自动删除了MyTask对象
    QThreadPool::globalInstance()->start(task);             //任务放进线程池
    QThread::sleep(1);

    MyRunable* task1 = new MyRunable();
    task1->setAutoDelete(true);
    QThreadPool::globalInstance()->start(task1);

    MyRunable* task2 = new MyRunable();
    task2->setAutoDelete(true);
    QThreadPool::globalInstance()->start(task2);

    QThreadPool::globalInstance()->waitForDone();           //等待任务结束
    qDebug() << "end";

    return a.exec();
}
```

