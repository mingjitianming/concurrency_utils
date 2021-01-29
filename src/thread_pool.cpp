#include "thread_pool.h"

namespace concurrency::pool
{
    thread_local unsigned ThreadPool::thread_id_(std::numeric_limits<unsigned>::max());
    thread_local containers::StealingQueue *ThreadPool::local_queue_(nullptr);

    ThreadPool::ThreadPool(const int thread_count)
        : thread_count_(thread_count < std::thread::hardware_concurrency() ? thread_count : std::thread::hardware_concurrency()),
          done_(false), global_queue_(std::make_unique<containers::TSQueue<TaskType>>())
    {
        try
        {
            queues_.reserve(thread_count_);
            threads_.reserve(thread_count_);
            auto is_ready = std::promise<void>();
            std::shared_future<void> ready = is_ready.get_future();
            for (unsigned int i = 0; i < thread_count_; ++i)
            {
                queues_.emplace_back(std::make_unique<containers::StealingQueue>());
                threads_.emplace_back(std::jthread(&ThreadPool::threadWorker, this, i, std::ref(ready)));
            }
            is_ready.set_value();
        }
        catch (const std::exception &e)
        {
            done_ = true;
            std::cerr << e.what() << '\n';
        }
    }

    ThreadPool::~ThreadPool()
    {
        done_ = true;
    }

    void ThreadPool::threadWorker(const unsigned thread_id, std::shared_future<void> &ready)
    {
        thread_id_ = thread_id;
        local_queue_ = queues_[thread_id_].get();
        ready.wait();
        while (!done_)
        {
            // XXX: cv.wait()
            TaskType task;
            if (popFromLocal(task) || popFromQueues(task) || popFromOtherThread(task))
            {
                task();
            }
            else
            {
                // TODO:使用yield，无任务时会将cpu占满，考虑是否使用条件变量
                // std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        }
    }

    bool ThreadPool::popFromLocal(TaskType &task)
    {
        return local_queue_ && local_queue_->try_pop(task);
    }
    bool ThreadPool::popFromQueues(TaskType &task)
    {
        return global_queue_->try_pop(task);
    }
    bool ThreadPool::popFromOtherThread(TaskType &task)
    {
        for (unsigned i = 1; i < thread_count_; ++i)
        {
            unsigned id = (thread_id_ + i) % thread_count_;
            if (queues_[id]->try_steal(task))
                return true;
        }
        return false;
    }

} // namespace concurrency::pool
