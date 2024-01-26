//
// Created by Arvid Jonasson on 2024-01-25.
//

#include "thread_pool.h"

#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threads) : stop(false), idle_threads(threads) {
    for(size_t i = 0; i<threads; ++i)
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                    if(this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                --idle_threads;
                task();
                ++idle_threads;
            }
        });
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}