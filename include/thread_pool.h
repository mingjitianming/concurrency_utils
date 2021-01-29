#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "function_wapper.h"
#include "stealing_queue.h"
#include "threadsafe_queue.h"

namespace concurrency::pool
{
    class ThreadPool
    {
        using TaskType = FuncWapper;

    public:
        ThreadPool();
        ~ThreadPool();

        template <typename Func, typename... Args>
        auto submit(Func f, Args... args);

    private:
        void threadWorker(const unsigned thread_id);

    private:
        int thread_count_;
        std::atomic<bool> done_;
        static thread_local unsigned thread_id_;
        static thread_local StealingQueue *local_queue_;
        std::unique_ptr<containers::TSQueue<TaskType>> global_queue_;
        std::vector<std::unique_ptr<StealingQueue>> queues_;
        std::vector<std::jthread> threads_;
    };

    thread_local unsigned ThreadPool::thread_id_(
        std::numeric_limits<unsigned>::max());
    thread_local StealingQueue *ThreadPool::local_queue_(nullptr);

    ThreadPool::ThreadPool()
        : thread_count_(std::thread::hardware_concurrency()), done_(false)
    {
        try
        {
            queues_.reserve(thread_count_);
            threads_.reserve(thread_count_);
            std::cout << "nnnnn" << std::endl;
            for (unsigned int i = 0; i < thread_count_; ++i)
            {
                queues_.emplace_back(std::make_unique<StealingQueue>());
                threads_.emplace_back(std::jthread(&ThreadPool::threadWorker, this, i));
            }
        }
        catch (const std::exception &e)
        {
            done_ = true;
            std::cerr << e.what() << '\n';
        }
    }

    ThreadPool::~ThreadPool() {}

    void ThreadPool::threadWorker(const unsigned thread_id)
    {
        thread_id_ = thread_id;
        local_queue_ = queues_[thread_id_].get();
        while (!done_)
        {
            TaskType task;
            if (0)
            {
                task.call();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    template <typename Func, typename... Args>
    auto ThreadPool::submit(Func f, Args... args)
    {
        using R = std::invoke_result_t<Func, Args...>;
        std::packaged_task<R(Args...)> task(f);
        auto fut = task.get_future();
        if (local_queue_)
        {
            local_queue_->push(std::move(task));
        }
        else
        {
            global_queue_->push(std::move(task));
        }
        // XXX: cv.notify_one ?
        return fut;
    }

} // namespace concurrency::pool

#endif