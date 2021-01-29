#ifndef STEALING_QUEUE_H
#define STEALING_QUEUE_H

#include "function_wapper.h"
#include <deque>
#include <mutex>

namespace concurrency::pool
{
    class StealingQueue
    {
        using Data = FuncWapper;

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