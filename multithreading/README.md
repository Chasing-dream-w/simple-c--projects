Thread Pool (C++11 Implementation)
A lightweight C++11 thread pool that supports submitting tasks with or without return values, automatically managing worker threads and a task queue.

Core Features:

Fixed-size pool: Creates a specified number of worker threads that loop executing tasks from the queue.

Task submission:

addTask(): Submit a void task (std::function<void()>).

submits(): Submit a task that returns a value, getting a std::future to retrieve the result.

Graceful shutdown: Waits for all tasks to finish in the destructor before exiting.

Design Highlights:

std::mutex + std::condition_variable for thread-safe task queue.

std::packaged_task and std::future to support asynchronous return values.

Perfect forwarding and variadic templates for flexible task arguments.




线程池（C++11 实现）
一个轻量级的 C++11 线程池，支持提交无返回值或有返回值的任务，自动管理工作线程和任务队列。

核心功能：

固定大小线程池：创建指定数量的工作线程，循环执行任务队列中的任务。

任务提交：

addTask()：提交无返回值的任务（std::function<void()>）。

submits()：提交带返回值的任务，返回 std::future 用于获取结果。

优雅停止：析构时等待所有任务完成，安全退出线程。

设计亮点：

使用 std::mutex + std::condition_variable 实现线程安全的任务队列。

通过 std::packaged_task 和 std::future 支持异步返回值。

支持完美转发和可变参数模板，任务参数灵活传递。
