#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "containers/stealing_queue.h"
#include "containers/threadsafe_queue.h"

namespace concurrency::pool
{
    class ThreadPool
    {
        using TaskType = std::function<void()>;

    public:
        ThreadPool(const int thread_count = std::thread::hardware_concurrency());
        ~ThreadPool();

        template <typename Func, typename... Args>
        auto submit(Func f, Args... args);

    private:
        void threadWorker(const unsigned thread_id, std::shared_future<void> &ready);
        bool popFromLocal(TaskType &task);
        bool popFromQueues(TaskType &task);
        bool popFromOtherThread(TaskType &task);

    private:
        int thread_count_;
        std::atomic<bool> done_;
        static thread_local unsigned thread_id_;
        static thread_local containers::StealingQueue *local_queue_;
        std::unique_ptr<containers::TSQueue<TaskType>> global_queue_;
        std::vector<std::unique_ptr<containers::StealingQueue>> queues_;
        std::vector<std::jthread> threads_;
    };

    // thread_local unsigned ThreadPool::thread_id_(std::numeric_limits<unsigned>::max());
    // thread_local StealingQueue *ThreadPool::local_queue_(nullptr);

    template <typename Func, typename... Args>
    auto ThreadPool::submit(Func f, Args... args)
    {
        using R = std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<R(Args...)>>(f);
        auto fut = task->get_future();

        if (local_queue_)
        {
            local_queue_->push([task, args...]() { (*task)(args...); });
        }
        else
        {
            global_queue_->push([task, args...]() { (*task)(args...); });
        }
        // XXX: cv.notify_one ?
        return fut;
    }

} // namespace concurrency::pool

#endif