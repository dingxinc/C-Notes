#互斥锁 #条件变量 #原子变量 #call_once #future #async #promise #生产者消费者 #mutex #yield

C++11 之前，C++ 语言没有对并发编程提供语言级别的支持，这使得我们在编写可移植的并发程序时，存在诸多的不便。现在 C++11 中增加了线程以及线程相关的类，很方便地支持了并发编程，使得编写的多线程程序的可移植性得到了很大的提高。

C++11中提供的线程类叫做 `std::thread`，基于这个类创建一个新的线程非常的简单，只需要提供线程函数或者函数对象即可，并且可以同时指定线程函数的参数。



# 1. C++ 线程的使用



## 1.1 构造函数

```c++
// ①
thread() noexcept;
// ②
thread( thread&& other ) noexcept;
// ③
template< class Function, class... Args >
explicit thread( Function&& f, Args&&... args );
// ④
thread( const thread& ) = delete;
```

* 构造函数①：默认构造函，构造一个线程对象，在这个线程中不执行任何处理动作
* 构造函数②：移动构造函数，将 `other` 的线程**所有权转移**给新的 `thread` 对象。之后 `other` 不再表示执行线程。
* 构造函数③：创建线程对象，并在该线程中执行函数 `f` 中的业务逻辑，`args` 是要传递给函数 `f` 的参数
  * 任务函数 `f` 的可选类型有很多，具体如下：
    * `普通函数，类成员函数，匿名函数，仿函数`（这些都是可调用对象类型）
    * 可以是可调用对象包装器类型，也可以是使用绑定器绑定之后得到的类型（仿函数）
* 构造函数④：使用 [[默认函数控制 =default 与 =delete#2.2 =delete | =delete]] 显式删除拷贝构造, 不允许线程对象之间的拷贝



## 1.2 公共成员函数

### 1.2.1 get_id()

应用程序启动之后默认只有一个线程，这个线程一般称之为 `主线程或父线程`，通过线程类创建出的线程一般称之为 `子线程`，每个被创建出的线程实例都对应一个线程 ID，这个 ID 是唯一的，可以通过这个 ID 来区分和识别各个已经存在的线程实例，这个获取线程 ID 的函数叫做 `get_id()`，函数原型如下：

```c++
std::thread::id get_id() const noexcept;
```

示例程序如下：

```c++
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void func(int num, string str)
{
    for (int i = 0; i < 10; ++i)
    {
        cout << "子线程: i = " << i << "num: " 
             << num << ", str: " << str << endl;
    }
}

void func1()
{
    for (int i = 0; i < 10; ++i)
    {
        cout << "子线程: i = " << i << endl;
    }
}

int main()
{
    cout << "主线程的线程ID: " << this_thread::get_id() << endl;
    thread t(func, 520, "i love you");
    thread t1(func1);
    cout << "线程t 的线程ID: " << t.get_id() << endl;
    cout << "线程t1的线程ID: " << t1.get_id() << endl;
}
```

* `thread t(func, 520, "i love you");`：创建了子线程对象 `t`，`func()` 函数会在这个子线程中运行
  * `func()` 是一个回调函数，线程启动之后就会执行这个任务函数，程序猿只需要实现即可
  * `func()` 的参数是通过 `thread` 的参数进行传递的，`520,i love you` 都是调用 `func()` 需要的实参
  * 线程类的构造函数③是一个变参函数，因此无需担心线程任务函数的参数个数问题
  * 任务函数 `func()` 一般返回值指定为 `void`，因为子线程在调用这个函数的时候不会处理其返回值
* `thread t1(func1);`：子线程对象 `t1` 中的任务函数 `func1()`，没有参数，因此在线程构造函数中就无需指定了
* 通过线程对象调用 `get_id()` 就可以知道这个子线程的线程 ID 了，`t.get_id()，t1.get_id()。`

**在上面的示例程序中有一个 bug，在主线程中依次创建出两个子线程，打印两个子线程的线程 ID，最后主线程执行完毕就退出了（主线程就是执行main()函数的那个线程）。默认情况下，主线程销毁时会将与其关联的两个子线程也一并销毁，但是这时有可能子线程中的任务还没有执行完毕，最后也就得不到我们想要的结果了。**

当启动了一个线程（创建了一个thread对象）之后，在这个线程结束的时候（std::terminate()），我们如何去回收线程所使用的资源呢？thread库给我们两种选择：

* `加入式（join()）`
* `分离式（detach()）`

另外，我们必须要在线程对象销毁之前在二者之间作出选择，否则程序运行期间就会有 bug 产生。



### 1.2.2 join()

`join()` 字面意思是连接一个线程，意味着主动地等待线程的终止（线程阻塞）。在某个线程中通过子线程对象调用 `join()` 函数，调用这个函数的线程被阻塞，但是子线程对象中的任务函数会继续执行，当任务执行完毕之后 `join()` 会清理当前子线程中的相关资源然后返回，同时，调用该函数的线程解除阻塞继续向下执行。

再次强调，我们一定要搞清楚这个函数阻塞的是哪一个线程，**函数在哪个线程中被执行，那么函数就阻塞哪个线程**。该函数的函数原型如下：

```c++
void join();
```

有了这样一个线程阻塞函数之后，就可以解决在上面测试程序中的 bug 了，`如果要阻塞主线程的执行，只需要在主线程中通过子线程对象调用这个方法即可，当调用这个方法的子线程对象中的任务函数执行完毕之后，主线程的阻塞也就随之解除了`。修改之后的示例代码如下：

```c++
int main()
{
    cout << "主线程的线程ID: " << this_thread::get_id() << endl;
    thread t(func, 520, "i love you");
    thread t1(func1);
    cout << "线程t 的线程ID: " << t.get_id() << endl;
    cout << "线程t1的线程ID: " << t1.get_id() << endl;
    t.join();
    t1.join();
}
```

当主线程运行到第八行 `t.join();`，根据子线程对象 `t` 的任务函数 `func()` 的执行情况，主线程会做如下处理：

* 如果任务函数 `func()` 还没执行完毕，主线程阻塞，直到任务执行完毕，主线程解除阻塞，继续向下运行
* 如果任务函数 `func()` 已经执行完毕，主线程不会阻塞，继续向下运行

同样，第 9 行的代码亦如此。

> 为了更好的理解 `join()` 的使用，场景如下：
>
> 程序中一共有三个线程，其中两个子线程负责分段下载同一个文件，下载完毕之后，由主线程对这个文件进行下一步处理，那么示例程序就应该这么写：

```c++
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void download1()
{
    // 模拟下载, 总共耗时500ms，阻塞线程500ms
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "子线程1: " << this_thread::get_id() << ", 找到历史正文...." << endl;
}

void download2()
{
    // 模拟下载, 总共耗时300ms，阻塞线程300ms
    this_thread::sleep_for(chrono::milliseconds(300));
    cout << "子线程2: " << this_thread::get_id() << ", 找到历史正文...." << endl;
}

void doSomething()
{
    cout << "集齐历史正文, 呼叫罗宾...." << endl;
    cout << "历史正文解析中...." << endl;
    cout << "起航，前往拉夫德尔...." << endl;
    cout << "找到OnePiece, 成为海贼王, 哈哈哈!!!" << endl;
    cout << "若干年后，草帽全员卒...." << endl;
    cout << "大海贼时代再次被开启...." << endl;
}

int main()
{
    thread t1(download1);
    thread t2(download2);
    // 阻塞主线程，等待所有子线程任务执行完毕再继续向下执行
    t1.join();
    t2.join();
    doSomething();
}
```

示例程序输出的结果：

```c++
子线程2: 72540, 找到历史正文....
子线程1: 79776, 找到历史正文....
集齐历史正文, 呼叫罗宾....
历史正文解析中....
起航，前往拉夫德尔....
找到OnePiece, 成为海贼王, 哈哈哈!!!
若干年后，草帽全员卒....
大海贼时代再次被开启....
```

在上面示例程序中最核心的处理是在主线程调用 `doSomething();` 之前在第 `35、36 行`通过子线程对象调用了 `join()`方法，这样就能够保证两个子线程的任务都执行完毕了，也就是文件内容已经全部下载完成，主线程再对文件进行后续处理，如果子线程的文件没有下载完毕，主线程就去处理文件，很显然从逻辑上讲是有问题的。



### 1.2.3 detach()

`detach()` 函数的作用是进行线程分离，分离主线程和创建出的子线程。`在线程分离之后，主线程退出也会一并销毁创建出的所有子线程，在主线程退出之前，它可以脱离主线程继续独立的运行，任务执行完毕之后，这个子线程会自动释放自己占用的系统资源。`（其实就是孩子翅膀硬了，和家里断绝关系，自己外出闯荡了，如果家里被诛九族还是会受牵连）。该函数函数原型如下：

```c++
void detach();
```

线程分离函数没有参数也没有返回值，只需要在线程成功之后，通过线程对象调用该函数即可，继续将上面的测试程序修改一下：

```c++
int main()
{
    cout << "主线程的线程ID: " << this_thread::get_id() << endl;
    thread t(func, 520, "i love you");
    thread t1(func1);
    cout << "线程t 的线程ID: " << t.get_id() << endl;
    cout << "线程t1的线程ID: " << t1.get_id() << endl;
    t.detach();
    t1.detach();
    // 让主线程休眠, 等待子线程执行完毕
    this_thread::sleep_for(chrono::seconds(5));
}
```

> **注意事项：线程分离函数 detach() 不会阻塞线程，子线程和主线程分离之后，在主线程中就不能再对这个子线程做任何控制了，比如：通过 join() 阻塞主线程等待子线程中的任务执行完毕，或者调用 get_id() 获取子线程的线程 ID。有利就有弊，鱼和熊掌不可兼得，建议使用 join()。**



### 1.2.4 joinable()

`joinable()` 函数用于判断主线程和子线程是否处理关联（连接）状态，一般情况下，二者之间的关系处于关联状态，该函数返回一个布尔类型：

* 返回值为 `true：`主线程和子线程之间有关联（连接）关系
* 返回值为 `false：`主线程和子线程之间没有关联（连接）关系

```c++
bool joinable() const noexcept;
```

示例代码如下：

```c++
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void foo()
{
    this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
    thread t;
    cout << "before starting, joinable: " << t.joinable() << endl;

    t = thread(foo);
    cout << "after starting, joinable: " << t.joinable() << endl;

    t.join();
    cout << "after joining, joinable: " << t.joinable() << endl;

    thread t1(foo);
    cout << "after starting, joinable: " << t1.joinable() << endl;
    t1.detach();
    cout << "after detaching, joinable: " << t1.joinable() << endl;
}
```

示例代码打印的结果如下：

```c++
before starting, joinable: 0
after starting, joinable: 1
after joining, joinable: 0
after starting, joinable: 1
after detaching, joinable: 0
```

基于示例代码打印的结果可以得到以下结论：

* 在创建的子线程对象的时候，如果没有指定任务函数，那么子线程不会启动，主线程和这个子线程也不会进行连接
* 在创建的子线程对象的时候，如果指定了任务函数，子线程启动并执行任务，主线程和这个子线程自动连接成功
* 子线程调用了`detach()` 函数之后，父子线程分离，同时二者的连接断开，调用 `joinable() 返回 false`

* 在子线程调用了 `join()` 函数，子线程中的任务函数继续执行，直到任务处理完毕，这时 `join()` 会清理（回收）当前子线程的相关资源，所以这个子线程和主线程的连接也就断开了，因此，调用 `join()` 之后再调用 `joinable()` 会返回 `false`。



### 1.2.5 operator=

`线程中的资源是不能被复制的`，因此通过 `=` 操作符进行赋值操作最终并不会得到两个完全相同的对象。

```c++
// move (1)	
thread& operator= (thread&& other) noexcept;
// copy [deleted] (2)	
thread& operator= (const other&) = delete;
```

通过以上 `=` 操作符的重载声明可以得知：

* 如果 `other` 是一个右值，会进行资源所有权的转移
* 如果 `other` 不是右值，禁止拷贝，该函数被显示删除（`=delete`），不可用



## 1.3 静态函数

`thread` 线程类还提供了一个静态方法，用于获取当前计算机的 CPU 核心数，根据这个结果在程序中创建出数量相等的线程，每个线程独自占有一个 CPU 核心，这些线程就不用分时复用 CPU 时间片，此时程序的并发效率是最高的。

```c++
static unsigned hardware_concurrency() noexcept;
```

示例代码如下：

```c++
#include <iostream>
#include <thread>
using namespace std;

int main()
{
    int num = thread::hardware_concurrency();
    cout << "CPU number: " << num << endl;
}
```



## 1.4 C 线程库

C 语言提供的线程库不论在 window 还是 Linux 操作系统中都是可以使用的，看明白了这些 C 语言中的线程函数之后会发现它和上面的 C++ 线程类使用很类似（其实就是基于面向对象的思想进行了封装），但 C++ 的线程类用起来更简单一些。

[[线程 | C 线程库函数的使用]]



# 2.  命名空间 - this_thread



在 C++11 中不仅添加了线程类，还添加了一个关于线程的命名空间 `std::this_thread`，在这个命名空间中提供了四个公共的成员函数，通过这些成员函数就可以对当前线程进行相关的操作了。



## 2.1 get_id()

调用命名空间 `std::this_thread` 中的 `get_id()` 方法可以得到当前线程的线程 ID，函数原型如下：

```c++
thread::id get_id() noexcept;
```

关于函数使用对应的示例代码如下：

```c++
#include <iostream>
#include <thread>
using namespace std;

void func()
{
    cout << "子线程: " << this_thread::get_id() << endl;
}

int main()
{
    cout << "主线程: " << this_thread::get_id() << endl;
    thread t(func);
    t.join();
}
```

程序启动，开始执行 `main()` 函数，此时只有一个线程也就是主线程。当创建了子线程对象 `t` 之后，指定的函数 `func()` 会在子线程中执行，这时通过调用 `this_thread::get_id()` 就可以得到当前线程的线程 ID 了。



## 2.2 sleep_for()

[[进程控制#1.3 进程状态 | 进程被创建后的五种状态]]

同样地线程被创建后也有这五种状态：`创建态，就绪态，运行态，阻塞态(挂起态)，退出态(终止态)` ，关于状态之间的转换是一样的，请参考进程，在此不再过多的赘述。

线程和进程的执行有很多相似之处，在计算机中启动的多个线程都需要占用 CPU 资源，但是 CPU 的个数是有限的并且每个 CPU 在同一时间点不能同时处理多个任务。`为了能够实现并发处理，多个线程都是分时复用 CPU 时间片，快速的交替处理各个线程中的任务。因此多个线程之间需要争抢 CPU 时间片，抢到了就执行，抢不到则无法执行`（因为默认所有的线程优先级都相同，内核也会从中调度，不会出现某个线程永远抢不到 CPU 时间片的情况）。

命名空间 `this_thread` 中提供了一个休眠函数 `sleep_for()`，调用这个函数的线程会马上从**运行态变成阻塞态**并在这种状态下休眠一定的时长，因为阻塞态的线程已经让出了 CPU 资源，代码也不会被执行，所以线程休眠过程中对 CPU 来说没有任何负担。这个函数是函数原型如下，参数需要指定一个休眠时长，是一个时间段：

[[处理日期和时间的 chrono 库#1.2 类的使用 | chrono 库中的时间间隔类 duration 的使用]]

```c++
template <class Rep, class Period>
  void sleep_for (const chrono::duration<Rep,Period>& rel_time);
```

示例程序如下：

```c++
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void func()
{
    for (int i = 0; i < 10; ++i)
    {
        this_thread::sleep_for(chrono::seconds(1));
        cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
    }
}

int main()
{
    thread t(func);
    t.join();
}
```

在 `func()` 函数的 `for` 循环中使用了 `this_thread::sleep_for(chrono::seconds(1));` 之后，每循环一次程序都会阻塞 1 秒钟，也就是说每隔 1 秒才会进行一次输出。需要注意的是：**程序休眠完成之后，会从阻塞态重新变成就绪态，就绪态的线程需要再次争抢 CPU 时间片，抢到之后才会变成运行态，这时候程序才会继续向下运行。**



## 2.3 sleep_until();

命名空间 `this_thread` 中提供了另一个休眠函数 `sleep_until()`，和 `sleep_for()` 不同的是它的参数类型不一样

* `sleep_until()`：指定线程阻塞到某一个指定的时间点 `time_point` 类型，之后解除阻塞
* `sleep_for()`：指定线程阻塞一定的时间长度 `duration` 类型，之后解除阻塞


[[处理日期和时间的 chrono 库#2. 时间点 time point | chrono 库中的时间点类 time_point 的使用]]


该函数的函数原型如下：

```c++
template <class Clock, class Duration>
  void sleep_until (const chrono::time_point<Clock,Duration>& abs_time);
```

示例程序如下：

```c++
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void func()
{
    for (int i = 0; i < 10; ++i)
    {
        // 获取当前系统时间点
        auto now = chrono::system_clock::now();
        // 时间间隔为2s
        chrono::seconds sec(2);
        // 当前时间点之后休眠两秒
        this_thread::sleep_until(now + sec);
        cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
    }
}

int main()
{
    thread t(func);
    t.join();
}
```

`sleep_until()` 和 `sleep_for()` 函数的功能是一样的，只不过前者是基于时间点去阻塞线程，后者是基于时间段去阻塞线程，项目开发过程中根据实际情况选择最优的解决方案即可。



## 2.4 yield()

命名空间 `this_thread` 中提供了一个非常绅士的函数 `yield()`，在线程中调用这个函数之后，处于运行态的线程会主动让出自己已经抢到的  CPU 时间片，最终变为就绪态，这样其它的线程就有更大的概率能够抢到 CPU时间片了。使用这个函数的时候需要注意一点，**线程调用了 yield() 之后会主动放弃 CPU 资源，但是这个变为就绪态的线程会马上参与到下一轮 CPU 的抢夺战中，不排除它能继续抢到 CPU 时间片的情况，这是概率问题。**

```c++
void yield() noexcept;
```

函数对应的示例程序如下：

```c++
#include <iostream>
#include <thread>
using namespace std;

void func()
{
    for (int i = 0; i < 100000000000; ++i)
    {
        cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
        this_thread::yield();
    }
}

int main()
{
    thread t(func);
    thread t1(func);
    t.join();
    t1.join();
}
```

在上面的程序中，执行 `func()` 中的 `for` 循环会占用大量的时间，在极端情况下，如果当前线程占用 CPU 资源不释放就会导致其他线程中的任务无法被处理，或者该线程每次都能抢到 CPU 时间片，导致其他线程中的任务没有机会被执行。解决方案就是每执行一次循环，让该线程主动放弃 CPU 资源，重新和其他线程再次抢夺 CPU 时间片，如果其他线程抢到了 CPU 时间片就可以执行相应的任务了。

> 结论：
>
> 1. `std::this_thread::yield()` 的目的是避免一个线程长时间占用 CPU 资源，从而导致多线程处理性能下降
> 2. `std::this_thread::yield()` 是让当前线程主动放弃了当前自己抢到的 CPU 资源，但是在下一轮还会继续抢



# 3. 线程同步之互斥锁



进行多线程编程，如果多个线程需要对同一块内存进行操作，比如：`同时读、同时写、同时读写`对于后两种情况来说，如果不做任何的人为干涉就会出现各种各样的错误数据。这是因为线程在运行的时候需要先得到 CPU 时间片，时间片用完之后需要放弃已获得的 CPU 资源，就这样线程频繁地在就绪态和运行态之间切换，更复杂一点还可以在就绪态、运行态、挂起态之间切换，这样就会导致线程的执行顺序并不是有序的，而是随机的混乱的，就如同下图中的这个例子一样，理想很丰满现实却很残酷。

![image-20230919135426861](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191354990.png)

解决多线程数据混乱的方案就是进行线程同步，最常用的就是互斥锁，在 C++11 中一共提供了四种互斥锁：

1. `std::mutex`：独占的互斥锁，不能递归使用
2. `std::timed_mutex`：带超时的独占互斥锁，不能递归使用
3. `std::recursive_mutex`：递归互斥锁，不带超时功能
4. `std::recursive_timed_mutex`：带超时的递归互斥锁

互斥锁在有些资料中也被称之为互斥量，二者是一个东西。

[[线程同步]]



## 3.1 std::mutex

不论是在 C 还是 C++ 中，进行线程同步的处理流程基本上是一致的，C++ 的 mutex 类提供了相关的 API 函数：

### 3.1.1 成员函数

`lock()` 函数用于给临界区加锁，并且只能有一个线程获得锁的所有权，它有阻塞线程的作用，函数原型如下：

```c++
void lock();
```

独占互斥锁对象有两种状态：`锁定`和`未锁定`。如果互斥锁是打开的，调用 `lock()` 函数的线程会得到互斥锁的所有权，并将其上锁，其它线程再调用该函数的时候由于得不到互斥锁的所有权，就会被 `lock()` 函数阻塞。当拥有互斥锁所有权的线程将互斥锁解锁，此时被 `lock()` 阻塞的线程解除阻塞，抢到互斥锁所有权的线程加锁并继续运行，没抢到互斥锁所有权的线程继续阻塞。

除了使用 `lock()` 还可以使用 `try_lock()` 获取互斥锁的所有权并对互斥锁加锁，函数原型如下：

```c++
bool try_lock();
```

二者的区别在于 `try_lock()` 不会阻塞线程，`lock()` 会阻塞线程：

* 如果互斥锁是未锁定状态，得到了互斥锁所有权并加锁成功，函数返回 true
* 如果互斥锁是锁定状态，无法得到互斥锁所有权加锁失败，函数返回 false

当互斥锁被锁定之后可以通过 `unlock()` 进行解锁，但是需要注意的是**只有拥有互斥锁所有权的线程也就是对互斥锁上锁的线程才能将其解锁，其它线程是没有权限做这件事情的**。该函数的函数原型如下：

```c++
void unlock();
```

通过介绍以上三个函数，使用互斥锁进行线程同步的大致思路差不多就能搞清楚了，主要分为以下几步：

1. 找到多个线程操作的共享资源（全局变量、堆内存、类成员变量等），也可以称之为临界资源

2. 找到和共享资源有关的上下文代码，也就是临界区（下图中的黄色代码部分）
3. 在临界区的上边调用互斥锁类的 `lock()` 方法
4. 在临界区的下边调用互斥锁的 `unlock()` 方法

**线程同步的目的是让多线程按照顺序依次执行临界区代码，这样做线程对共享资源的访问就从并行访问变为了线性访问，访问效率降低了，但是保证了数据的正确性。**

![image-20230919140314168](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191403224.png)

> **当线程对互斥锁对象加锁，并且执行完临界区代码之后，一定要使用这个线程对互斥锁解锁，否则最终会造成线程的死锁。死锁之后当前应用程序中的所有线程都会被阻塞，并且阻塞无法解除，应用程序也无法继续运行。**



### 3.1.2 线程同步

我们让两个线程共同操作同一个全局变量，二者交替数数，将数值存储到这个全局变量里边并打印出来。

```c++
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
using namespace std;

int g_num = 0;  // 为 g_num_mutex 所保护
mutex g_num_mutex;

void slow_increment(int id)
{
    for (int i = 0; i < 3; ++i) 
    {
        g_num_mutex.lock();
        ++g_num;
        cout << id << " => " << g_num << endl;
        g_num_mutex.unlock();

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main()
{
    thread t1(slow_increment, 0);
    thread t2(slow_increment, 1);
    t1.join();
    t2.join();
}
```

在上面的示例程序中，两个子线程执行的任务的一样的（其实也可以不一样，不同的任务中也可以对共享资源进行读写操作），在任务函数中把与全局变量相关的代码加了锁，两个线程只能顺序访问这部分代码（如果不进行线程同步打印出的数据是混乱且无序的）。另外需要强调一点：

1. **在所有线程的任务函数执行完毕之前，互斥锁对象是不能被析构的，一定要在程序中保证这个对象的可用性。**
2. **互斥锁的个数和共享资源的个数相等，也就是说每一个共享资源都应该对应一个互斥锁对象。互斥锁对象的个数和线程的个数没有关系。**



## 3.2 std::lock_guard

`lock_guard` 是 C++11 新增的一个模板类，使用这个类，可以简化互斥锁 `lock()` 和 `unlock()` 的写法，同时也更安全。这个模板类的定义和常用的构造函数原型如下：

```c++
// 类的定义，定义于头文件 <mutex>
template< class Mutex >
class lock_guard;

// 常用构造函数
explicit lock_guard( mutex_type& m );
```

`lock_guard` 在使用上面提供的这个构造函数构造对象时，会自动锁定互斥量，而在退出作用域后进行析构时就会自动解锁，从而保证了互斥量的正确操作，避免忘记 `unlock()` 操作而导致线程死锁。`lock_guard` 使用了 RAII 技术，就是在类构造函数中分配资源，在析构函数中释放资源，保证资源出了作用域就释放。

使用 `lock_guard` 对上面的例子进行修改，代码如下：

```c++
void slow_increment(int id)
{
    for (int i = 0; i < 3; ++i) 
    {
        // 使用哨兵锁管理互斥锁
        lock_guard<mutex> lock(g_num_mutex);
        ++g_num;
        cout << id << " => " << g_num << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}
```

通过修改发现代码被精简了，而且不用担心因为忘记解锁而造成程序的死锁，但是`这种方式也有弊端，在上面的示例程序中整个 for 循环体都被当做了临界区，多个线程是线性的执行临界区代码的，因此临界区越大程序效率越低`，还是需要根据实际情况选择最优的解决方案。



## 3.3 std::recursive_mutex

递归互斥锁 `std::recursive_mutex` 允许同一线程多次获得互斥锁，可以用来解决同一线程需要多次获取互斥量时死锁的问题，在下面的例子中使用独占非递归互斥量会发生死锁：

```c++
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

struct Calculate
{
    Calculate() : m_i(6) {}

    void mul(int x)
    {
        lock_guard<mutex> locker(m_mutex);
        m_i *= x;
    }

    void div(int x)
    {
        lock_guard<mutex> locker(m_mutex);
        m_i /= x;
    }

    void both(int x, int y)
    {
        lock_guard<mutex> locker(m_mutex);
        mul(x);
        div(y);
    }

    int m_i;
    mutex m_mutex;
};

int main()
{
    Calculate cal;
    cal.both(6, 3);
    return 0;
}
```

上面的程序中执行了 `cal.both(6, 3);` 调用之后，程序就会发生死锁，在 `both()` 中已经对互斥锁加锁了，继续调用 `mult()` 函数，已经得到互斥锁所有权的线程再次获取这个互斥锁的所有权就会造成死锁（在 C++ 中程序会异常退出，使用 C 库函数会导致这个互斥锁永远无法被解锁，最终阻塞所有的线程）。要解决这个死锁的问题，一个简单的办法就是使用递归互斥锁

`std::recursive_mutex`，它允许一个线程多次获得互斥锁的所有权。修改之后的代码如下：

```c++
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

struct Calculate
{
    Calculate() : m_i(6) {}

    void mul(int x)
    {
        lock_guard<recursive_mutex> locker(m_mutex);
        m_i *= x;
    }

    void div(int x)
    {
        lock_guard<recursive_mutex> locker(m_mutex);
        m_i /= x;
    }

    void both(int x, int y)
    {
        lock_guard<recursive_mutex> locker(m_mutex);
        mul(x);
        div(y);
    }

    int m_i;
    recursive_mutex m_mutex;
};

int main()
{
    Calculate cal;
    cal.both(6, 3);
    cout << "cal.m_i = " << cal.m_i << endl;
    return 0;
}
```

虽然递归互斥锁可以解决同一个互斥锁频繁获取互斥锁资源的问题，但是还是建议少用，主要原因如下：

1. **使用递归互斥锁的场景往往都是可以简化的，使用递归互斥锁很容易放纵复杂逻辑的产生，从而导致 bug 的产生**
2. **递归互斥锁比非递归互斥锁效率要低一些。**
3. **递归互斥锁虽然允许同一个线程多次获得同一个互斥锁的所有权，但最大次数并未具体说明，一旦超过一定的次数，就会抛出 std::system 错误。**



## 3.4 std::timed_mutex

`std::timed_mutex` 是超时独占互斥锁，主要是在获取互斥锁资源时增加了超时等待功能，因为不知道获取锁资源需要等待多长时间，为了保证不一直等待下去，设置了一个超时时长，超时后线程就可以解除阻塞去做其他事情了。

`std::timed_mutex` 比 `std::_mutex` 多了两个成员函数：`try_lock_for()` 和 `try_lock_until()`：

```c++
void lock();
bool try_lock();
void unlock();

// std::timed_mutex比std::_mutex多出的两个成员函数
template <class Rep, class Period>
  bool try_lock_for (const chrono::duration<Rep,Period>& rel_time);

template <class Clock, class Duration>
  bool try_lock_until (const chrono::time_point<Clock,Duration>& abs_time);
```

* `try_lock_for` 函数是当线程获取不到互斥锁资源的时候，让线程阻塞一定的时间长度
* `try_lock_until` 函数是当线程获取不到互斥锁资源的时候，让线程阻塞到某一个指定的时间点
* 关于两个函数的返回值：当得到互斥锁的所有权之后，函数会马上解除阻塞，返回 true，如果阻塞的时长用完或者到达指定的时间点之后，函数也会解除阻塞，返回 false

下面的示例程序中演示了`std::timed_mutex` 的使用：

```c++
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

timed_mutex g_mutex;

void work()
{
    chrono::seconds timeout(1);
    while (true)
    {
        // 通过阻塞一定的时长来争取得到互斥锁所有权
        if (g_mutex.try_lock_for(timeout))
        {
            cout << "当前线程ID: " << this_thread::get_id() 
                << ", 得到互斥锁所有权..." << endl;
            // 模拟处理任务用了一定的时长
            this_thread::sleep_for(chrono::seconds(10));
            // 互斥锁解锁
            g_mutex.unlock();
            break;
        }
        else
        {
            cout << "当前线程ID: " << this_thread::get_id() 
                << ", 没有得到互斥锁所有权..." << endl;
            // 模拟处理其他任务用了一定的时长
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}

int main()
{
    thread t1(work);
    thread t2(work);

    t1.join();
    t2.join();

    return 0;
}
```

示例代码输出的结果：

```c++
当前线程ID: 125776, 得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 没有得到互斥锁所有权...
当前线程ID: 112324, 得到互斥锁所有权...
```

在上面的例子中，通过一个 `while` 循环不停的去获取超时互斥锁的所有权，如果得不到就阻塞 1 秒钟，1 秒之后如果还是得不到阻塞 50 毫秒，然后再次继续尝试，直到获得互斥锁的所有权，跳出循环体。

关于递归超时互斥锁 `std::recursive_timed_mutex` 的使用方式和 `std::timed_mutex` 是一样的，只不过它可以允许一个线程多次获得互斥锁所有权，而 `std::timed_mutex` 只 允许线程获取一次互斥锁所有权。另外，递归超时互斥锁 `std::recursive_timed_mutex` 也拥有和 `std::recursive_mutex` 一样的弊端，不建议频繁使用。

[[Qt多线程]]



# 4.  线程同步之条件变量

条件变量是 C++11 提供的另外一种用于等待的同步机制，它能阻塞一个或多个线程，直到收到另外一个线程发出的通知或者超时时，才会唤醒当前阻塞的线程。**条件变量需要和互斥量配合起来使用**，C++11 提供了两种条件变量：

* `condition_variable`：需要配合 `std::unique_lock<std::mutex >`进行 `wait` 操作，也就是阻塞线程的操作。
* `condition_variable_any`：可以和任意带有 `lock()、unlock()` 语义的 `mutex` 搭配使用，也就是说有四种：
  * `std::mutex`：独占的非递归互斥锁
  * `std::timed_mutex`：带超时的独占非递归互斥锁
  * `std::recursive_mutex`：不带超时功能的递归互斥锁
  * `std::recursive_timed_mutex`：带超时的递归互斥锁

**条件变量通常用于生产者和消费者模型**，大致使用过程如下：

1. 拥有条件变量的线程获取互斥量

2. 循环检查某个条件，如果条件不满足阻塞当前线程，否则线程继续向下执行
   * 产品的数量达到上限，生产者阻塞，否则生产者一直生产。。
   * 产品的数量为零，消费者阻塞，否则消费者一直消费。。。
3. 条件满足之后，可以调用 `notify_one()` 或者 `notify_all()` 唤醒一个或者所有被阻塞的线程
   * 由消费者唤醒被阻塞的生产者，生产者解除阻塞继续生产。。。
   * 由生产者唤醒被阻塞的消费者，消费者解除阻塞继续消费。。。



## 4.1 condition_variable

### 4.1.1 成员函数

`condition_variable` 的成员函数主要分为两部分：`线程等待（阻塞）函数` 和 `线程通知（唤醒）函数`，这些函数被定义于头文件 `<condition_variable>`。

* 等待函数

  调用 `wait()` 函数的线程会被阻塞

  ```c++
  // ①
  void wait (unique_lock<mutex>& lck);
  // ②
  template <class Predicate>
  void wait (unique_lock<mutex>& lck, Predicate pred);
  ```

  * 函数①：调用该函数的线程直接被阻塞
  * 函数②：该函数的第二个参数是一个判断条件，是一个返回值为布尔类型的函数
    * `该参数可以传递一个有名函数的地址，也可以直接指定一个匿名函数`
    * `该参数可以传递一个有名函数的地址，也可以直接指定一个匿名函数`
  * 独占的互斥锁对象不能直接传递给 `wait()` 函数，需要通过模板类 `unique_lock` 进行二次处理，通过得到的对象仍然可以对独占的互斥锁对象做如下操作，使用起来更灵活。

  | 公共成员函数   | 说明                                                         |
  | -------------- | ------------------------------------------------------------ |
  | lock           | 锁定关联的互斥锁                                             |
  | try_lock       | 尝试锁定关联的互斥锁，若无法锁定，函数直接返回               |
  | try_lock_for   | 试图锁定关联的可定时锁定互斥锁，若互斥锁在给定时长中仍不能被锁定，函数返回 |
  | try_lock_until | 试图锁定关联的可定时锁定互斥锁，若互斥锁在给定的时间点后仍不能被锁定，函数返回 |
  | unlock         | 将互斥锁解锁                                                 |

  * `如果线程被该函数阻塞，这个线程会释放占有的互斥锁的所有权，当阻塞解除之后这个线程会重新得到互斥锁的所有权，继续向下执行`（这个过程是在函数内部完成的，了解这个过程即可，其目的是为了避免线程的死锁）。

  `wait_for()` 函数和 `wait()` 的功能是一样的，只不过多了一个阻塞时长，假设阻塞的线程没有被其他线程唤醒，当阻塞时长用完之后，线程就会自动解除阻塞，继续向下执行。

  ```c++
  template <class Rep, class Period>
  cv_status wait_for (unique_lock<mutex>& lck,
                      const chrono::duration<Rep,Period>& rel_time);
  	
  template <class Rep, class Period, class Predicate>
  bool wait_for(unique_lock<mutex>& lck,
                 const chrono::duration<Rep,Period>& rel_time, Predicate pred);
  ```

  `wait_until()` 函数和 `wait_for()` 的功能是一样的，它是指定让线程阻塞到某一个时间点，假设阻塞的线程没有被其他线程唤醒，当到达指定的时间点之后，线程就会自动解除阻塞，继续向下执行。

  ```c++
  template <class Clock, class Duration>
  cv_status wait_until (unique_lock<mutex>& lck,
                        const chrono::time_point<Clock,Duration>& abs_time);
  
  template <class Clock, class Duration, class Predicate>
  bool wait_until (unique_lock<mutex>& lck,
                   const chrono::time_point<Clock,Duration>& abs_time, Predicate pred);
  ```

* 通知函数

  ```c++
  void notify_one() noexcept;      // 唤醒一个被当前条件变量阻塞的线程
  void notify_all() noexcept;      // 唤醒全部被当前条件变量阻塞的线程
  ```



### 4.1.2 生产者和消费者模型

我们可以使用条件变量来实现一个同步队列，这个队列作为生产者线程和消费者线程的共享资源，示例代码如下：

```c++
#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <condition_variable>
using namespace std;

class SyncQueue
{
public:
    SyncQueue(int maxSize) : m_maxSize(maxSize) {}
    
    void put(const int& x)
    {
        unique_lock<mutex> locker(m_mutex);
        // 判断任务队列是不是已经满了
        while (m_queue.size() == m_maxSize)
        {
            cout << "任务队列已满, 请耐心等待..." << endl;
            // 阻塞线程
            m_notFull.wait(locker);
        }
        // 将任务放入到任务队列中
        m_queue.push_back(x);
        cout << x << " 被生产" << endl; 
        // 通知消费者去消费
        m_notEmpty.notify_one();
    }

    int take()
    {
        unique_lock<mutex> locker(m_mutex);
        while (m_queue.empty())
        {
            cout << "任务队列已空，请耐心等待。。。" << endl;
            m_notEmpty.wait(locker);
        }
        // 从任务队列中取出任务(消费)
        int x = m_queue.front();
        m_queue.pop_front();
        // 通知生产者去生产
        m_notFull.notify_one();
        cout << x << " 被消费" << endl;
        return x;
    }

    bool empty()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.empty();
    }

    bool full()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.size() == m_maxSize;
    }

    int size()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.size();
    }

private:
    list<int> m_queue;     // 存储队列数据
    mutex m_mutex;         // 互斥锁
    condition_variable m_notEmpty;   // 不为空的条件变量
    condition_variable m_notFull;    // 没有满的条件变量
    int m_maxSize;         // 任务队列的最大任务个数
};

int main()
{
    SyncQueue taskQ(50);
    auto produce = bind(&SyncQueue::put, &taskQ, placeholders::_1);
    auto consume = bind(&SyncQueue::take, &taskQ);
    thread t1[3];
    thread t2[3];
    for (int i = 0; i < 3; ++i)
    {
        t1[i] = thread(produce, i+100);
        t2[i] = thread(consume);
    }

    for (int i = 0; i < 3; ++i)
    {
        t1[i].join();
        t2[i].join();
    }

    return 0;
}
```

条件变量 `condition_variable` 类的 `wait()` 还有一个重载的方法，`可以接受一个条件，这个条件也可以是一个返回值为布尔类型的函数，条件变量会先检查判断这个条件是否满足，如果满足条件（布尔值为 true），则当前线程重新获得互斥锁的所有权，结束阻塞，继续向下执行；如果不满足条件（布尔值为 false），当前线程会释放互斥锁（解锁）同时被阻塞，等待被唤醒。`

上面示例程序中的 `put()、take()` 函数可以做如下修改：

* put() 函数

  ```c++
  void put(const int& x)
  {
      unique_lock<mutex> locker(m_mutex);
      // 根据条件阻塞线程
      m_notFull.wait(locker, [this]() {
          return m_queue.size() != m_maxSize;
      });
      // 将任务放入到任务队列中
      m_queue.push_back(x);
      cout << x << " 被生产" << endl;
      // 通知消费者去消费
      m_notEmpty.notify_one();
  }
  ```

* take() 函数

  ```c++
  int take()
  {
      unique_lock<mutex> locker(m_mutex);
      m_notEmpty.wait(locker, [this]() {
          return !m_queue.empty();
      });
      // 从任务队列中取出任务(消费)
      int x = m_queue.front();
      m_queue.pop_front();
      // 通知生产者去生产
      m_notFull.notify_one();
      cout << x << " 被消费" << endl;
      return x;
  }
  ```

  修改之后可以发现，程序变得更加精简了，而且执行效率更高了，因为在这两个函数中的 `while` 循环被删掉了，但是最终的效果是一样的，推荐使用这种方式的 `wait()` 进行线程的阻塞。



## 4.2 condition_variable_any

### 4.2.1 成员函数

`condition_variable_any` 的成员函数也是分为两部分：线程等待（阻塞）函数 和线程通知（唤醒）函数，这些函数被定义于头文件 `<condition_variable>`。

* 等待函数

  ```c++
  // ①
  template <class Lock> void wait (Lock& lck);
  // ②
  template <class Lock, class Predicate>
  void wait (Lock& lck, Predicate pred);
  ```

  * 函数①：调用该函数的线程直接被阻塞
  * 函数②：该函数的第二个参数是一个判断条件，是一个返回值为布尔类型的函数
    * `该参数可以传递一个有名函数的地址，也可以直接指定一个匿名函数`
    * `表达式返回 false 当前线程被阻塞，表达式返回 true 当前线程不会被阻塞，继续向下执行`
  * 可以直接传递给 `wait()` 函数的互斥锁类型有四种，分别是：
    * `std::mutex、std::timed_mutex、std::recursive_mutex、std::recursive_timed_mutex`
  * `如果线程被该函数阻塞，这个线程会释放占有的互斥锁的所有权，当阻塞解除之后这个线程会重新得到互斥锁的所有权，继续向下执行`（这个过程是在函数内部完成的，了解这个过程即可，其目的是为了避免线程的死锁）。

  `wait_for()` 函数和 `wait()` 的功能是一样的，只不过多了一个阻塞时长，假设阻塞的线程没有被其他线程唤醒，当阻塞时长用完之后，线程就会自动解除阻塞，继续向下执行。

  ```c++
  template <class Lock, class Rep, class Period>
  cv_status wait_for (Lock& lck, const chrono::duration<Rep,Period>& rel_time);
  	
  template <class Lock, class Rep, class Period, class Predicate>
  bool wait_for (Lock& lck, const chrono::duration<Rep,Period>& rel_time, Predicate pred);
  ```

  `wait_until()` 函数和 `wait_for()` 的功能是一样的，它是指定让线程阻塞到某一个时间点，假设阻塞的线程没有被其他线程唤醒，当到达指定的时间点之后，线程就会自动解除阻塞，继续向下执行。

  ```c++
  template <class Lock, class Clock, class Duration>
  cv_status wait_until (Lock& lck, const chrono::time_point<Clock,Duration>& abs_time);
  
  template <class Lock, class Clock, class Duration, class Predicate>
  bool wait_until (Lock& lck, 
                   const chrono::time_point<Clock,Duration>& abs_time, 
                   Predicate pred);
  ```

* 通知函数

  ```c++
  void notify_one() noexcept;    // 唤醒一个被当前条件变量阻塞的线程
  void notify_all() noexcept;    // 唤醒全部被当前条件变量阻塞的线程
  ```



### 4.2.2 生产者和消费者模型

使用条件变量 `condition_variable_any` 同样可以实现上面的生产者和消费者的例子，代码只有个别细节上有所不同：

```c++
#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <condition_variable>
using namespace std;

class SyncQueue
{
public:
    SyncQueue(int maxSize) : m_maxSize(maxSize) {}

    void put(const int& x)
    {
        lock_guard<mutex> locker(m_mutex);
        // 根据条件阻塞线程
        m_notFull.wait(m_mutex, [this]() {
            return m_queue.size() != m_maxSize;
        });
        // 将任务放入到任务队列中
        m_queue.push_back(x);
        cout << x << " 被生产" << endl;
        // 通知消费者去消费
        m_notEmpty.notify_one();
    }

    int take()
    {
        lock_guard<mutex> locker(m_mutex);
        m_notEmpty.wait(m_mutex, [this]() {
            return !m_queue.empty();
        });
        // 从任务队列中取出任务(消费)
        int x = m_queue.front();
        m_queue.pop_front();
        // 通知生产者去生产
        m_notFull.notify_one();
        cout << x << " 被消费" << endl;
        return x;
    }

    bool empty()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.empty();
    }

    bool full()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.size() == m_maxSize;
    }

    int size()
    {
        lock_guard<mutex> locker(m_mutex);
        return m_queue.size();
    }

private:
    list<int> m_queue;     // 存储队列数据
    mutex m_mutex;         // 互斥锁
    condition_variable_any m_notEmpty;   // 不为空的条件变量
    condition_variable_any m_notFull;    // 没有满的条件变量
    int m_maxSize;         // 任务队列的最大任务个数
};

int main()
{
    SyncQueue taskQ(50);
    auto produce = bind(&SyncQueue::put, &taskQ, placeholders::_1);
    auto consume = bind(&SyncQueue::take, &taskQ);
    thread t1[3];
    thread t2[3];
    for (int i = 0; i < 3; ++i)
    {
        t1[i] = thread(produce, i + 100);
        t2[i] = thread(consume);
    }

    for (int i = 0; i < 3; ++i)
    {
        t1[i].join();
        t2[i].join();
    }

    return 0;
}
```

> **总结：以上介绍的两种条件变量各自有各自的特点，condition_variable 配合 unique_lock 使用更灵活一些，可以在在任何时候自由地释放互斥锁，而condition_variable_any 如果和lock_guard 一起使用必须要等到其生命周期结束才能将互斥锁释放。但是，condition_variable_any 可以和多种互斥锁配合使用，应用场景也更广，而 condition_variable 只能和独占的非递归互斥锁（mutex）配合使用，有一定的局限性。**



# 5. 原子变量



C++11 提供了一个原子类型 `std::atomic<T>` ，通过这个原子类型管理的内部变量就可以称之为原子变量，我们可以给原子类型指定 `bool、char、int、long、指针` 等类型作为模板参数（`不支持浮点类型和复合类型`）。

原子指的是一系列不可被 CPU 上下文交换的机器指令，这些指令组合在一起就形成了原子操作。在多核 CPU下，当某个 CPU 核心开始运行原子操作时，会先暂停其它 CPU 内核对内存的操作，以保证原子操作不会被其它CPU 内核所干扰。

由于原子操作是通过指令提供的支持，因此它的性能相比锁和消息传递会好很多。相比较于锁而言，原子类型不需要开发者处理加锁和释放锁的问题，同时支持修改，读取等操作，还具备较高的并发性能，几乎所有的语言都支持原子类型。

可以看出原子类型是无锁类型，但是无锁不代表无需等待，因为原子类型内部使用了 `CAS` 循环，当大量的冲突发生时，该等待还是得等待！但是总归比锁要好。

C++11 内置了整形的原子变量，这样就可以更方便的使用原子变量了。在多线程操作中，使用原子变量之后就不需要再使用互斥量来保护该变量了，用起来更简洁。因为对原子变量进行的操作只能是一个原子操作（`atomic operation`），`原子操作指的是不会被线程调度机制打断的操作，这种操作一旦开始，就一直运行到结束，中间不会有任何的上下文切换`。多线程同时访问共享资源造成数据混乱的原因就是因为 CPU 的上下文切换导致的，使用原子变量解决了这个问题，因此互斥锁的使用也就不再需要了。

> `CAS` 全称是 `Compare and swap`, 它通过一条指令读取指定的内存地址，然后判断其中的值是否等于给定的前置值，如果相等，则将其修改为新的值



## 5.1 atomic 类成员

类定义

```c++
// 定义于头文件 <atomic>
template< class T >
struct atomic;
```

通过定义可得知：`在使用这个模板类的时候，一定要指定模板类型。`

**构造函数**

```c++
// ①
atomic() noexcept = default;
// ②
constexpr atomic( T desired ) noexcept;
// ③
atomic( const atomic& ) = delete;
```

- 构造函数①：默认无参构造函数。
- 构造函数②：使用 `desired` 初始化原子变量的值。
- 构造函数③：使用 `=delete` 显示删除拷贝构造函数, 不允许进行对象之间的拷贝



**公共成员函数**

原子类型在类内部重载了 `=` 操作符，**并且不允许在类的外部使用 `=` 进行对象的拷贝。**

```c++
T operator=( T desired ) noexcept;
T operator=( T desired ) volatile noexcept;

atomic& operator=( const atomic& ) = delete;
atomic& operator=( const atomic& ) volatile = delete;
```

原子地以 `desired` 替换当前值。按照 `order` 的值影响内存

```c++
void store( T desired, std::memory_order order = std::memory_order_seq_cst ) noexcept;
void store( T desired, std::memory_order order = std::memory_order_seq_cst ) volatile noexcept;
```

* **desired**：存储到原子变量中的值
* **order**：强制的内存顺序

原子地加载并返回原子变量的当前值。按照 `order` 的值影响内存。直接访问原子对象也可以得到原子变量的当前值。

```c++
T load( std::memory_order order = std::memory_order_seq_cst ) const noexcept;
T load( std::memory_order order = std::memory_order_seq_cst ) const volatile noexcept;
```



**特化成员函数**

* 复合赋值运算符重载，主要包含以下形式：

  ![image-20230919160411994](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191604121.png)

* 以上各个 operator 都会有对应的 **fetch_*** 操作，详细见下表：

![image-20230919160439161](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191604236.png)



**内存顺序约束**

通过上面的 API 函数我们可以看出，在调用 `atomic` 类提供的 API 函数的时候，需要指定原子顺序，在 C++11 给我们提供的 API 中使用枚举用作执行原子操作的函数的实参，以指定如何同步不同线程上的其他操作。

```c++
typedef enum memory_order {
    memory_order_relaxed,   // relaxed
    memory_order_consume,   // consume
    memory_order_acquire,   // acquire
    memory_order_release,   // release
    memory_order_acq_rel,   // acquire/release
    memory_order_seq_cst    // sequentially consistent
} memory_order;
```

![image-20230919160611616](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191606704.png)



**C++20 新增成员**

在 C++20 版本中添加了新的功能函数，可以通过原子类型来阻塞线程，和条件变量中的等待/通知函数是一样的。

| 公共成员函数      | 说明                                       |
| ----------------- | ------------------------------------------ |
| wait(C++20)       | 阻塞线程直至被提醒且原子值更改             |
| notify_one(C++20) | 通知（唤醒）至少一个在原子对象上阻塞的线程 |
| notify_all(C++20) | 通知（唤醒）所有在原子对象上阻塞的线程     |



## 5.2 原子变量的使用

假设我们要制作一个多线程交替数数的计数器，我们使用互斥锁和原子变量的方式分别进行实现，对比一下二者的差异：

### 5.2.1 互斥锁版本

```c++
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
using namespace std;

struct Counter
{
    void increment()
    {
        for (int i = 0; i < 10; ++i)
        {
            lock_guard<mutex> locker(m_mutex);
            m_value++;
            cout << "increment number: " << m_value 
                << ", theadID: " << this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

    void decrement()
    {
        for (int i = 0; i < 10; ++i)
        {
            lock_guard<mutex> locker(m_mutex);
            m_value--;
            cout << "decrement number: " << m_value 
                << ", theadID: " << this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

    int m_value = 0;
    mutex m_mutex;
};

int main()
{
    Counter c;
    auto increment = bind(&Counter::increment, &c);
    auto decrement = bind(&Counter::decrement, &c);
    thread t1(increment);
    thread t2(decrement);

    t1.join();
    t2.join();

    return 0;
}
```



### 5.2.2 原子变量版本

```c++
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
using namespace std;

struct Counter
{
    void increment()
    {
        for (int i = 0; i < 10; ++i)
        {
            m_value++;
            cout << "increment number: " << m_value
                << ", theadID: " << this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    void decrement()
    {
        for (int i = 0; i < 10; ++i)
        {
            m_value--;
            cout << "decrement number: " << m_value
                << ", theadID: " << this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
    // atomic<int> == atomic_int
    atomic_int m_value = 0;
};

int main()
{
    Counter c;
    auto increment = bind(&Counter::increment, &c);
    auto decrement = bind(&Counter::decrement, &c);
    thread t1(increment);
    thread t2(decrement);

    t1.join();
    t2.join();

    return 0;
}
```

通过代码的对比可以看出，使用了原子变量之后，就不需要再定义互斥量了，在使用上更加简便，并且这两种方式都能保证在多线程操作过程中数据的正确性，不会出现数据的混乱。

原子类型 `atomic<T>` 可以封装原始数据最终得到一个原子变量对象，操作原子对象能够得到和操作原始数据一样的效果，当然也可以通过 `store()` 和 `load()` 来读写原子对象内部的原始数据。



# 6. call_once



在某些特定情况下，某些函数只能在多线程环境下调用一次，比如：要初始化某个对象，而这个对象只能被初始化一次，就可以使用 `std::call_once()` 来保证函数在多线程环境下只能被调用一次。使用 `call_once()` 的时候，需要一个 `once_flag` 作为 `call_once()` 的传入参数，该函数的原型如下：

```c++
// 定义于头文件 <mutex>
template< class Callable, class... Args >
void call_once( std::once_flag& flag, Callable&& f, Args&&... args );
```

- flag：`once_flag` 类型的对象，要保证这个对象能够被多个线程同时访问到
- f：回调函数，可以传递一个有名函数地址，也可以指定一个匿名函数
- args：作为实参传递给回调函数

多线程操作过程中，`std::call_once()` 内部的回调函数只会被执行一次，示例代码如下：

```c++
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

once_flag g_flag;
void do_once(int a, string b)
{
    cout << "name: " << b << ", age: " << a << endl;
}

void do_something(int age, string name)
{
    static int num = 1;
    call_once(g_flag, do_once, 19, "luffy");
    cout << "do_something() function num = " << num++ << endl;
}

int main()
{
    thread t1(do_something, 20, "ace");
    thread t2(do_something, 20, "sabo");
    thread t3(do_something, 19, "luffy");
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
```

示例程序输出的结果：

```c++
name: luffy, age: 19
do_something() function num = 1
do_something() function num = 2
do_something() function num = 3
```

通过输出的结果可以看到，虽然运行的三个线程中都执行了任务函数 `do_something()` 但是 `call_once()` 中指定的回调函数只被执行了一次，我们的目的也达到了。



# 7. 多线程异步操作



## 7.1 std::future

C++11 中增加的线程类，使得我们能够非常方便的创建和使用线程，但有时会有些不方便，比如需要获取线程返回的结果，就不能通过 `join()` 得到结果，只能通过一些额外手段获得，比如：定义一个全局变量，在子线程中赋值，在主线程中读这个变量的值，整个过程比较繁琐。C++ 提供的线程库中提供了一些类用于访问异步操作的结果。

那么，什么叫做异步呢？

![image-20230919162825327](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309191628400.png)

我们去星巴克买咖啡，因为都是现磨的，所以需要等待，但是我们付完账后不会站在柜台前死等，而是去找个座位坐下来玩玩手机打发一下时间，当店员把咖啡磨好之后，就会通知我们过去取，这就叫做异步。

* 顾客（主线程）发起一个任务（子线程磨咖啡），磨咖啡的过程中顾客去做别的事情了，有两条时间线（异步）
* 顾客（主线程）发起一个任务（子线程磨咖啡），磨咖啡的过程中顾客没去做别的事情而是死等，这时就只有一条时间线（同步），此时效率相对较低。

因此多线程程序中的任务大都是异步的，主线程和子线程分别执行不同的任务，如果想要在主线中得到某个子线程任务函数返回的结果可以使用 C++11 提供的 `std:future` 类，这个类需要和其他类或函数搭配使用，先来介绍一下这个类的 API 函数：

**类的定义**

通过类的定义可以得知，`future` 是一个模板类，也就是这个类可以存储任意指定类型的数据。

```c++
// 定义于头文件 <future>
template< class T > class future;
template< class T > class future<T&>;
template<>          class future<void>;
```



**构造函数**

```c++
// ①
future() noexcept;
// ②
future( future&& other ) noexcept;
// ③
future( const future& other ) = delete;
```

- 构造函数①：默认无参构造函数
- 构造函数②：移动构造函数，转移资源的所有权
- 构造函数③：使用 `=delete` 显示删除拷贝构造函数, 不允许进行对象之间的拷贝



**常用成员函数（public)**

一般情况下使用 `=` 进行赋值操作就进行对象的拷贝，但是 `future` 对象不可用复制，因此会根据实际情况进行处理：

* 如果 `other` 是右值，那么转移资源的所有权
* 如果 `other` 是非右值，不允许进行对象之间的拷贝（该函数被显示删除禁止使用）

```c++
future& operator=( future&& other ) noexcept;
future& operator=( const future& other ) = delete;
```

取出 `future` 对象内部保存的数据，其中 `void get()` 是为 `future<void>` 准备的，此时对象内部类型就是 `void`，该函数是一个阻塞函数，当子线程的数据就绪后解除阻塞就能得到传出的数值了。

```c++
T get();
T& get();
void get();
```

因为 `future` 对象内部存储的是异步线程任务执行完毕后的结果，是在调用之后的将来得到的，因此可以通过调用 `wait()` 方法，阻塞当前线程，等待这个子线程的任务执行完毕，任务执行完毕当前线程的阻塞也就解除了。

```c++
void wait() const;
```

如果当前线程 `wait()` 方法就会死等，直到子线程任务执行完毕将返回值写入到 `future` 对象中，调用`wait_for()` 只会让线程阻塞一定的时长，但是这样并不能保证对应的那个子线程中的任务已经执行完毕了。

`wait_until()` 和 `wait_for()` 函数功能是差不多，前者是阻塞到某一指定的时间点，后者是阻塞一定的时长。

```c++
template< class Rep, class Period >
std::future_status wait_for( const std::chrono::duration<Rep,Period>& timeout_duration ) const;

template< class Clock, class Duration >
std::future_status wait_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) const;
```

当 `wait_until()` 和 `wait_for()` 函数返回之后，并不能确定子线程当前的状态，因此我们需要判断函数的返回值，这样就能知道子线程当前的状态了：

| 常量                    | 解释                                         |
| ----------------------- | -------------------------------------------- |
| future_status::deferred | 子线程中的任务函仍未启动                     |
| future_status::ready    | 子线程中的任务已经执行完毕，结果已就绪       |
| future_status::timeout  | 子线程中的任务正在执行中，指定等待时长已用完 |



## 7.2 std::promise

`std::promise` 是一个协助线程赋值的类，它能够将数据和 `future` 对象绑定起来，为获取线程函数中的某个值提供便利。



### 7.2.1 类成员函数

**类定义**

通过 `std::promise` 类的定义可以得知，这也是一个模板类，我们要在线程中传递什么类型的数据，模板参数就指定为什么类型。

```c++
// 定义于头文件 <future>
template< class R > class promise;
template< class R > class promise<R&>;
template<>          class promise<void>;
```

**构造函数**

```c++
// ①
promise();
// ②
promise( promise&& other ) noexcept;
// ③
promise( const promise& other ) = delete;
```

- 构造函数①：默认构造函数，得到一个空对象
- 构造函数②：移动构造函数
- 构造函数③：使用 `=delete` 显示删除拷贝构造函数, 不允许进行对象之间的拷贝

**公共成员函数**

在 `std::promise` 类内部管理着一个 `future` 类对象，调用 `get_future()` 就可以得到这个 `future` 对象了

```c++
std::future<T> get_future();
```

存储要传出的 `value` 值，并立即让状态就绪，这样数据被传出其它线程就可以得到这个数据了。重载的第四个函数是为 `promise<void>` 类型的对象准备的。

```c++
void set_value( const R& value );
void set_value( R&& value );
void set_value( R& value );
void set_value();
```

存储要传出的 `value` 值，但是不立即令状态就绪。在当前线程退出时，子线程资源被销毁，再令状态就绪。

```c++
void set_value_at_thread_exit( const R& value );
void set_value_at_thread_exit( R&& value );
void set_value_at_thread_exit( R& value );
void set_value_at_thread_exit();
```



### 7.2.2 promise 的使用

通过 `promise` 传递数据的过程一共分为 5 步：

1. 在主线程中创建 `std::promise` 对象

2. 将这个 `std::promise` 对象通过引用的方式传递给子线程的任务函数
3. 在子线程任务函数中给 `std::promise` 对象赋值
4. 在主线程中通过 `std::promise` 对象取出绑定的 `future` 实例对象
5. 通过得到的 `future` 对象取出子线程任务函数中返回的值。

**子线程任务函数执行期间，让状态就绪**

```c++
#include <iostream>
#include <thread>
#include <future>
using namespace std;

int main()
{
    promise<int> pr;
    thread t1([](promise<int> &p) {
        p.set_value(100);
        this_thread::sleep_for(chrono::seconds(3));
        cout << "睡醒了...." << endl;
    }, ref(pr));

    future<int> f = pr.get_future();
    int value = f.get();
    cout << "value: " << value << endl;

    t1.join();
    return 0;
}
```

示例程序输出的结果：

```c++
value: 100
睡醒了....
```

示例程序的中子线程的任务函数指定的是一个匿名函数，在这个匿名的任务函数执行期间通过 `p.set_value(100);`传出了数据并且激活了状态，数据就绪后，外部主线程中的 `int value = f.get();` 解除阻塞，并将得到的数据打印出来，5秒钟之后子线程休眠结束，匿名的任务函数执行完毕。

**子线程任务函数执行结束，让状态就绪**

```c++
#include <iostream>
#include <thread>
#include <future>
using namespace std;

int main()
{
    promise<int> pr;
    thread t1([](promise<int> &p) {
        p.set_value_at_thread_exit(100);
        this_thread::sleep_for(chrono::seconds(3));
        cout << "睡醒了...." << endl;
    }, ref(pr));

    future<int> f = pr.get_future();
    int value = f.get();
    cout << "value: " << value << endl;

    t1.join();
    return 0;
}
```

示例程序输出的结果：

```c++
睡醒了....
value: 100
```

在示例程序中，子线程的这个匿名的任务函数中通过 `p.set_value_at_thread_exit(100);` 在执行完毕并退出之后才会传出数据并激活状态，数据就绪后，外部主线程中的 `int value = f.get();` 解除阻塞，并将得到的数据打印出来，因此子线程在休眠5秒钟之后主线程中才能得到传出的数据。

另外，在这两个实例程序中有一个知识点需要强调，在外部主线程中创建的 `promise` 对象必须要通过引用的方式传递到子线程的任务函数中，**在实例化子线程对象的时候，如果任务函数的参数是引用类型，那么实参一定要放到 std::ref() 函数中，表示要传递这个实参的引用到任务函数中。**



## 7.3 std::packaged_task

`std::packaged_task` 类包装了一个 [[可调用对象包装器、绑定器 | 可调用对象包装器类]] 对象（可调用对象包装器包装的是可调用对象，可调用对象都可以作为函数来使用）

这个类可以将内部包装的函数和 `future` 类绑定到一起，以便进行后续的异步调用，它和 `std::promise` 有点类似，`std::promise` 内部保存一个共享状态的值，而 `std::packaged_task` 保存的是一个函数。



### 7.3.1 类成员函数

**类的定义**

通过类的定义可以看到这也是一个模板类，模板类型和要在线程中传出的数据类型是一致的。

```c++
// 定义于头文件 <future>
template< class > class packaged_task;
template< class R, class ...Args >
class packaged_task<R(Args...)>;
```



**构造函数**

```c++
// ①
packaged_task() noexcept;
// ②
template <class F>
explicit packaged_task( F&& f );
// ③
packaged_task( const packaged_task& ) = delete;
// ④
packaged_task( packaged_task&& rhs ) noexcept;
```

1. 构造函数①：无参构造，构造一个无任务的空对象
2. 构造函数②：通过一个可调用对象，构造一个任务对象
3. 构造函数③：显示删除，不允许通过拷贝构造函数进行对象的拷贝
4. 构造函数④：移动构造函数



**常用公共成员函数**

通过调用任务对象内部的 `get_future()` 方法就可以得到一个 `future` 对象，基于这个对象就可以得到传出的数据了。

```c++
std::future<R> get_future();
```



### 7.3.2 packaged_task 的使用

`packaged_task` 其实就是对子线程要执行的任务函数进行了包装，和可调用对象包装器的使用方法相同，包装完毕之后直接将包装得到的任务对象传递给线程对象就可以了。

```c++
#include <iostream>
#include <thread>
#include <future>
using namespace std;

int main()
{
    packaged_task<int(int)> task([](int x) {
        return x += 100;
    });

    thread t1(ref(task), 100);

    future<int> f = task.get_future();
    int value = f.get();
    cout << "value: " << value << endl;

    t1.join();
    return 0;
}
```

在上面的示例代码中，通过 `packaged_task` 类包装了一个匿名函数作为子线程的任务函数，最终的得到的这个任务对象需要通过引用的方式传递到子线程内部，这样才能在主线程的最后通过任务对象得到 `future` 对象，再通过这个 `future` 对象取出子线程通过返回值传递出的数据。



## 7.4 std::async

`std::async` 函数比前面提到的 `std::promise 和 packaged_task` **更高级一些**，**因为通过这函数可以直接启动一个子线程并在这个子线程中执行对应的任务函数**，异步任务执行完成返回的结果也是存储到一个 `future` 对象中，当需要获取异步任务的结果时，只需要调用 `future`  类的 `get()` 方法即可，如果不关注异步任务的结果，只是简单地等待任务完成的话，可以调用 `future`  类的 `wait()` 或者 `wait_for()` 方法。该函数的函数原型如下：

```c++
// 定义于头文件 <future>
// ①
template< class Function, class... Args>
std::future<std::result_of_t<std::decay_t<Function>(std::decay_t<Args>...)>>
    async( Function&& f, Args&&... args );

// ②
template< class Function, class... Args >
std::future<std::result_of_t<std::decay_t<Function>(std::decay_t<Args>...)>>
    async( std::launch policy, Function&& f, Args&&... args );
```

可以看到这是一个模板函数，在 C++11 中这个函数有两种调用方式：

- 函数①：直接调用传递到函数体内部的可调用对象，返回一个 `future` 对象
- 函数②：通过指定的策略调用传递到函数内部的可调用对象，返回一个 `future` 对象

函数参数:

- `f`：可调用对象，这个对象在子线程中被作为任务函数使用

- `Args`：传递给 f 的参数（实参）

- `policy`：可调用对象·f的执行策略

  | 策略               | 说明                                                         |
  | ------------------ | ------------------------------------------------------------ |
  | std::launch::async | 调用 async 函数时创建新的线程执行任务函数                    |
  | std::launch::async | 调用async函数时不执行任务函数，直到调用了 `future` 的 `get()` 或者 `wait()` 时才执行任务（这种方式不会创建新的线程） |

关于 `std::async()` 函数的使用，对应的示例代码如下：



### 7.4.1 方式1

**调用async()函数直接创建线程执行任务**

```c++
#include <iostream>
#include <thread>
#include <future>
using namespace std;

int main()
{
    cout << "主线程ID: " << this_thread::get_id() << endl;
    // 调用函数直接创建线程执行任务
    future<int> f = async([](int x) {
        cout << "子线程ID: " << this_thread::get_id() << endl;
        this_thread::sleep_for(chrono::seconds(5));
        return x += 100;
    }, 100);

    future_status status;
    do {
        status = f.wait_for(chrono::seconds(1));
        if (status == future_status::deferred)
        {
            cout << "线程还没有执行..." << endl;
            f.wait();
        }
        else if (status == future_status::ready)
        {
            cout << "子线程返回值: " << f.get() << endl;
        }
        else if (status == future_status::timeout)
        {
            cout << "任务还未执行完毕, 继续等待..." << endl;
        }
    } while (status != future_status::ready);

    return 0;
}
```

示例程序输出的结果为：

```c++
主线程ID: 8904
子线程ID: 25036
任务还未执行完毕, 继续等待...
任务还未执行完毕, 继续等待...
任务还未执行完毕, 继续等待...
任务还未执行完毕, 继续等待...
任务还未执行完毕, 继续等待...
子线程返回值: 200
```

调用 `async()` 函数时不指定策略就是直接创建线程并执行任务，示例代码的主线程中做了如下操作 status = `f.wait_for(chrono::seconds(1));` 其实直接调用 `f.get()` 就能得到子线程的返回值。这里为了演示`wait_for()` 的使用，所以写的复杂了些。



### 7.4.2 方式2

**调用async()函数不创建线程执行任务**

```c++
#include <iostream>
#include <thread>
#include <future>
using namespace std;

int main()
{
    cout << "主线程ID: " << this_thread::get_id() << endl;
    // 调用函数直接创建线程执行任务
    future<int> f = async(launch::deferred, [](int x) {
        cout << "子线程ID: " << this_thread::get_id() << endl;
        return x += 100;
    }, 100);

    this_thread::sleep_for(chrono::seconds(5));
    cout << f.get();

    return 0;
}
```

示例程序输出的结果：

```c++
主线程ID: 24760
主线程开始休眠5秒...
子线程ID: 24760
200
```

由于指定了 `launch::deferred` 策略，因此调用 `async()` 函数并不会创建新的线程执行任务，当使用 `future `类对象调用了 `get()` 或者 `wait()` 方法后才开始执行任务（此处一定要注意调用 `wait_for()` 函数是不行的）。

通过测试程序输出的结果可以看到，两次输出的线程 ID 是相同的，任务函数是在主线程中被延迟（主线程休眠 了 5 秒）调用了。



最终总结：

1. **使用 async() 函数，是多线程操作中最简单的一种方式，不需要自己创建线程对象，并且可以得到子线程函数的返回值。**
2. **使用 std::promise 类，在子线程中可以传出返回值也可以传出其他数据，并且可选择在什么时机将数据从子线程中传递出来，使用起来更灵活。**
3. **使用 std::packaged_task 类，可以将子线程的任务函数进行包装，并且可以得到子线程的返回值。**
