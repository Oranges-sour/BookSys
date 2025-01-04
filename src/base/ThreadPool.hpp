#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <functional>
#include <mutex>
#include <queue>
#include <thread>

using TaskFunc = std::function<void()>;

class TaskQueue {
   public:
    void push(const TaskFunc& _func);

    bool empty();

    TaskFunc get();

   private:
    std::mutex mu;
    std::condition_variable cv;
    std::queue<TaskFunc> que;
};

class ThreadPool {
   public:
    ThreadPool();
    ~ThreadPool();
    void push(const TaskFunc& _func);
    void stop();

   private:
    TaskQueue que;
    std::atomic<bool> _run;
    std::vector<std::shared_ptr<std::thread>> worker;
};

ThreadPool& thread_pool();

#endif