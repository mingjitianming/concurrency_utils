#include "containers/stealing_queue.h"
#include <iostream>
#include <thread>

namespace concurrency::containers
{
    // StealingQueue::StealingQueue(StealingQueue &&rhs)
    // {
    //     std::lock_guard lk(rhs.mut_);
    //     queue_ = std::move(rhs.queue_);
    // }

    // StealingQueue::StealingQueue &operator=(StealingQueue &&rhs)
    // {

    //     queue_ = std::move(rhs.queue_);
    //     return *this;
    // }

    void StealingQueue::push(Data &&data)
    {
        std::lock_guard lk(mut_);
        queue_.push_front(std::move(data));
    }

    bool StealingQueue::try_pop(Data &data)
    {
        std::lock_guard lk(mut_);
        if (queue_.empty())
            return false;
        data = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    bool StealingQueue::try_steal(Data &data)
    {
        std::lock_guard lk(mut_);
        if (queue_.empty())
            return false;
        data = std::move(queue_.back());
        queue_.pop_back();
        return true;
    }

    bool StealingQueue::empty()
    {
        std::lock_guard lk(mut_);
        return queue_.empty();
    }

} // namespace concurrency::pool
