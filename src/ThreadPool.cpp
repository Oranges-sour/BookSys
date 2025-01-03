#include "ThreadPool.hpp"

#include <iostream>
#include <thread>

constexpr int THREAD_NUM = 5;

ThreadPool& thread_pool() {
    static ThreadPool pool;
    return pool;
}

void TaskQueue::push(const TaskFunc& _task) {
    std::unique_lock lock(mu);
    que.push(_task);
    cv.notify_one();
}

bool TaskQueue::empty() {
    std::unique_lock lock(mu);
    return que.empty();
}

TaskFunc TaskQueue::get() {
    std::unique_lock lock(mu);
    cv.wait(lock, [this]() { return !que.empty(); });
    TaskFunc task = que.front();
    que.pop();
    return task;
}

ThreadPool::ThreadPool() {
    _run = true;
    for (int i = 1; i <= THREAD_NUM; ++i) {
        worker.push_back(std::make_shared<std::thread>([this, i]() {
            while (_run) {
                TaskFunc func = que.get();
                func();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    _run = false;
    // 保证worker被唤起一次
    for (int i = 1; i <= THREAD_NUM; ++i) {
        que.push([]() {});
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        worker[i]->join();
    }
}
void ThreadPool::stop() { _run = false; }

void ThreadPool::push(const TaskFunc& _func) { que.push(_func); }