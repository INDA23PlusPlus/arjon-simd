//
// Created by Arvid Jonasson on 2024-01-25.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H


#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <type_traits>
#include <optional>
#include <stdexcept>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::optional<std::future<std::invoke_result_t<F, Args...>>> {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared< std::packaged_task<return_type()> >(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if(stop || !has_idle_threads())
                return std::nullopt;
            tasks.emplace([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    bool has_idle_threads() const {
        return idle_threads > 0;
    }

private:
    std::vector<std::thread> workers;
    std::queue< std::function<void()> > tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    std::atomic<int> idle_threads;
};

#endif //THREAD_POOL_H
