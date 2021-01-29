#ifndef STEALING_QUEUE_H
#define STEALING_QUEUE_H

// #include "function_wapper.h"
#include <deque>
#include <functional>
#include <mutex>

namespace concurrency::containers
{
    class StealingQueue
    {
        using Data = std::function<void()>;

    public:
        StealingQueue() = default;
        // StealingQueue(StealingQueue &&rhs);
        // StealingQueue &operator=(StealingQueue &&rhs);

        StealingQueue(StealingQueue &rhs) = delete;
        StealingQueue &operator=(StealingQueue &rhs) = delete;
        ~StealingQueue() = default;

        void push(Data &&data);
        bool try_pop(Data &data);
        bool try_steal(Data &data);
        bool empty();

    private:
        std::deque<Data> queue_;
        mutable std::mutex mut_;
    };

} // namespace concurrency::pool

#endif