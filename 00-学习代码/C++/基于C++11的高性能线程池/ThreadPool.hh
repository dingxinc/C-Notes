/**
 * 因为程序边运行边创建线程是比较耗时的，所以我们通过池化的思想：在程序开始运行前创建多个线程这样，
 * 程序在运行时，只需要从线程池中拿来用就可以了．大大提高了程序运行效率。
*/

/**
 * 线程池的组成：
 * （1）线程池管理器

创建一定数量的线程，启动线程，调配任务，管理着线程池。
线程池目前只需要启动(start())，停止方法(stop())，及任务添加方法(addTask).
start()创建一定数量的线程池,进行线程循环.
stop()停止所有线程循环，回收所有资源.
addTask()添加任务.

（2）工作线程

线程池中线程，在线程池中等待并执行分配的任务，选用条件变量实现等待与通知机制.

（3）任务接口

添加任务的接口,以供工作线程调度任务的执行。

（4）任务队列

用于存放没有处理的任务。提供一种缓冲机制，同时任务队列具有调度功能，高优先级的任务放在任务队列前面

* 线程池的分类：分为半同步半异步线程池和领导者追随者线程池。
*/

#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
// 使用#pragma once  和 #ifndef THREAD_POOL_H 是为了避免同一个文件被include多次
#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>
namespace std
{
//线程池最大容量,应尽量设小一点
#define  THREADPOOL_MAX_NUM 16
//#define  THREADPOOL_AUTO_GROW
 
//线程池, 可以提交变参函数或拉姆达表达式的匿名函数执行, 可以获取执行返回值
//不直接支持类成员函数, 支持类静态成员函数或全局函数, Opteron()函数等
	class threadpool
	{
		using Task = function<void()>;	//定义类型
		vector<thread> _pool;     //线程池
		queue<Task> _tasks;            //任务队列
		mutex _lock;                   //同步
		condition_variable _task_cv;   //条件阻塞
		atomic<bool> _run{ true };     //线程池是否执行
		atomic<int>  _idlThrNum{ 0 };  //空闲线程数量
 
	public:
		inline threadpool(unsigned short size = 4) { addThread(size); }
		inline ~threadpool()
		{
			_run = false;
			_task_cv.notify_all(); // 唤醒所有线程执行
			for (thread& thread : _pool) {
				//thread.detach(); // 让线程“自生自灭”
				if (thread.joinable())
					thread.join(); // 等待任务结束， 前提：线程一定会执行完
			}
		}
 
	public:
		// 提交一个任务
		// 调用.get()获取返回值会等待任务执行完,获取返回值
		// 有两种方法可以实现调用类成员，
		// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
		// 一种是用   mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
		template<class F, class... Args>
		auto commit(F&& f, Args&&... args) -> future<decltype(f(args...))>
		{
			if (!_run)    // stoped ??
				throw runtime_error("commit on ThreadPool is stopped.");
 
			using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
			auto task = make_shared<packaged_task<RetType()>>(  // auto 的类型是：shared_ptr<package_task<RetType()> > 
				bind(forward<F>(f), forward<Args>(args)...)  // forward 完美转发，将函数以 “原样” 的方式转发给另一个函数
				); // 把函数入口及参数,打包(绑定)
			future<RetType> future = task->get_future();
			{    // 添加任务到队列
				lock_guard<mutex> lock{ _lock };//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
				_tasks.emplace([task]() { // push(Task{...}) 放到队列后面
					(*task)();
					});
			}
#ifdef THREADPOOL_AUTO_GROW
			if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
				addThread(1);
#endif // !THREADPOOL_AUTO_GROW
			_task_cv.notify_one(); // 唤醒一个线程执行
 
			return future;
		}
 
		//空闲线程数量
		int idlCount() { return _idlThrNum; }
		//线程数量
		int thrCount() { return _pool.size(); }
#ifndef THREADPOOL_AUTO_GROW
	private:
#endif // !THREADPOOL_AUTO_GROW
		//添加指定数量的线程
		void addThread(unsigned short size)
		{
			for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
			{   //增加线程数量,但不超过 预定义数量 THREADPOOL_MAX_NUM
				_pool.emplace_back([this] { //工作线程函数
					while (_run)
					{
						Task task; // 获取一个待执行的 task
						{
							// unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
							unique_lock<mutex> lock{ _lock };
							_task_cv.wait(lock, [this] {
								return !_run || !_tasks.empty();
							}); // wait 直到有 task
							if (!_run && _tasks.empty())
								return;
							task = move(_tasks.front()); // 按先进先出从队列取一个 task
							_tasks.pop();
						}
						_idlThrNum--;
						task();//执行任务
						_idlThrNum++;
					}
					});
				_idlThrNum++;
			}
		}
	};
 
}
 
 
#endif