#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace concurrency
{
    namespace containers
    {
        template <typename T>
        class TSQueue
        {
        public:
            TSQueue() = default;

            TSQueue(const TSQueue &other)
            {
                std::lock_guard lk(other.mutex);
                queue = other.queue;
            }

            ~TSQueue() = default;

            void push(T&& data)
            {
                std::lock_guard lk(mutex);
                queue.emplace(std::forward<T>(data));
                cv.notify_one();
            }

            void wait_and_pop(T &data)
            {
                std::unique_lock lk(mutex);
                cv.wait(lk, [this]() { return !queue.empty(); });
                data = queue.front();
                queue.pop();
            }

            auto wait_and_pop()
            {
                std::unique_lock lk(mutex);
                cv.wait(lk, [this]() { return !queue.empty(); });
                auto data = std::make_shared<T>(queue.front());
                queue.pop();
                return data;
            }

            bool try_pop(T &data)
            {
                std::lock_guard lk(mutex);
                if (queue.empty())
                    return false;
                data = queue.front();
                queue.pop();
                return true;
            }

            auto try_pop()
            {
                std::lock_guard lk(mutex);
                if (queue.empty)
                    return std::make_shared<T>();
                auto res = std::make_shared<T>(queue.front());
                queue.pop();
                return res;
            }

            bool empty() const
            {
                std::lock_guard lk(mutex);
                return queue.empty();
            }

        private:
            std::queue<T> queue;
            mutable std::mutex mutex;
            std::condition_variable cv;
        };
    } // namespace containers
} // namespace concurrency

#endif