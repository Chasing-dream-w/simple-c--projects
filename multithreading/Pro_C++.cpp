// Pro_C++.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 线程池的手搓

#include <iostream>  
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <functional>
#include <future>
#include <type_traits>

class ThreadPool {
protected:
	std::vector <std::thread> workers;                       //可用的工作线程
	std::queue <std::function<void()>> tasks;                //待处理的任务队列，存储任务的函数对象
	std::mutex mtx;                                          //互斥锁
	std::condition_variable cv;                              //条件变量，唤醒睡觉的线程
	bool stop = false;                                       //停止标志

public:
	ThreadPool(int numThreads = 2) {
		//创建numThread个工作线程
		std::cout << "线程数为："<<numThreads<< std::endl;
		for (int i = 0; i < numThreads; i++) {
			workers.emplace_back([this, i]() {
				while (true) {
					std::function<void()> task;
					//临界区
					{
						// unique_lock用于自动加锁解锁
						std::unique_lock<std::mutex> lock(mtx);
						cv.wait(lock, [this] {
							return !tasks.empty() || stop; //等待条件：任务队列不空或停止标志为true
						});
						if (stop && tasks.empty()) break;
						task = std::move(tasks.front());
						tasks.pop();
						
					}
					task(); //执行任务
				}
				});
		}
	}

	// 添加任务到线程池
	void addTask(std::function<void()> task) {
		{
			std::unique_lock<std::mutex> lock(mtx);
			tasks.push(task);
		}
		cv.notify_one(); //唤醒一个等待的线程
	}

	// 停止线程池
	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(mtx);
			stop = true;
		}
		cv.notify_all();
		for (auto& t : workers) t.join();
		std::cout << "所有任务已全部完成" << std::endl;
	}

};

class CompletedThreadPool : public ThreadPool {

public:
	// 提交任务并获取结果
	template<typename F, typename... Args>
	auto submits(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
	{
		using return_type = std::invoke_result_t<F, Args...>;

		auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
			[f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable -> return_type {
				return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
			}
		);

		std::future<return_type> result = task_ptr->get_future();
		{
			std::unique_lock<std::mutex> lock(mtx);
			tasks.emplace([task_ptr]() {(*task_ptr)();});
		}

		cv.notify_one();
		return result;
	}

};


static void test() {
	std::cout << "这是一个测试函数1" << std::endl;
}


int main()
{

	//ThreadPool pool(3);  //创建一个包含3个工作线程的线程池
	//// 添加不同的任务
	//pool.addTask([]() { std::cout << "打印消息" << std::endl; });

	//pool.addTask([]() {
	//	for (int i = 1; i <= 3; i++) std::cout << i << " ";
	//	std::cout << std::endl;
	//	});

	//pool.addTask([]() { 
	//	std::cout << 100 + 200 << std::endl; 
	//	});

	//pool.addTask([]() {
	//	std::cout << "任务4：空" << std::endl;
	//	test();
	//	});
	//


	// 继承类的使用
	CompletedThreadPool pool1(3);
	// 提交带返回值的任务
	int x, y;
	std::cout << "请输入两个整数: " << std::endl;
	std::cin >> x >> y;
	auto future = pool1.submits([](int a, int b) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		return a + b;
		}, x, y);

	std::cout << "等待计算结果..." << std::endl;
	int result = future.get();
	std::cout << "结果: " << result << std::endl;

	// 也可以提交无返回值任务（仍可用 addTask）
	pool1.addTask([]() {
		std::cout << "普通任务执行完毕" << std::endl;
		});

	std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}
